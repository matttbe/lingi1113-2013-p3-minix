/*
 * Project P3
 * 
 * Copyright 2013 UCLouvain / pole INGI
 *
 * Authors:
 *   Matthieu Baerts <matthieu.baerts@student.uclouvain.be>
 *   Helene Verhaeghe <helene.verhaeghe@student.uclouvain.be>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 */

#include "fs.h"
#include "buf.h"
#include "inode.h"
#include "super.h"
#include "proto.h"
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <minix/vfsif.h>
#include <stdio.h>

PRIVATE int get_nb_fragments(pInode)
register struct inode *pInode;
{
	int iNFrags; /* result */
	off_t iPos;  /* current position */
	block_t pCurrBlock, pPrevBlock;

	iNFrags = 0;

	/** TODO: check block... */

	printf ("|");

	for (iPos = 0 ; iPos < pInode->i_size ; iPos += pInode->i_sp->s_block_size)
	{
		pCurrBlock = read_map (pInode, iPos);
		if (pCurrBlock != pPrevBlock)
		{
			/* if the current block is distant of 2 to the previous, we have a fragment */
			if (pCurrBlock - pPrevBlock > 1
				|| pCurrBlock - pPrevBlock < 0) /* can be negative? */
			{
				iNFrags++;
				printf (" ");
			}
			else
				printf ("*");
		}
		else
			printf ("^");
		pPrevBlock = pCurrBlock;
	}

	printf ("|\n");

	return iNFrags;
}

PUBLIC int fs_nfrags()
{
	register struct inode *pInode;
	int iNFrags;

	/* Get the right inode if available */
	pInode = get_inode (fs_dev, (ino_t)fs_m_in.REQ_INODE_NR);
	if (! pInode)
		return EINVAL;

	iNFrags = get_nb_fragments (pInode);

	put_inode (pInode); /* will free the inode, etc. */

	fs_m_out.m9_s2 = iNFrags;

	return OK;
}

/* ******************** *
 * Functions for Defrag *
 * ******************** */

/*
 * check if there is a zone where iNZones consecutive bits are available.
 * inspired by super.c:alloc_bit (but rename variables... hard to have shorter name...
 */
PRIVATE int get_free_zone_start (pSuperBlock, iNZones, iFreeZoneStart)
		 struct super_block* pSuperBlock;
		 int iNZones;
		 bit_t *iFreeZoneStart;
{
	block_t iStartBlock;
	bit_t iNBitsMap, i, iBitId;
	unsigned uNBlockMap, uNewStartBlock, uNewStartWord;
	int iNFreeBits;
	struct buf *pBuf;
	bitchunk_t *pCurrWord, *pWordLimit, pCurrWordConverted;

	/* read-only file system */
	if (pSuperBlock->s_rd_only)
		return EROFS;

	/* from super.c ; considered we have a zmap */
	iStartBlock = START_BLOCK + pSuperBlock->s_imap_blocks; /* for imap: only start_block */
	iNBitsMap = pSuperBlock->s_zones - pSuperBlock->s_firstdatazone - 1;
	uNBlockMap = pSuperBlock->s_zmap_blocks;

	if (pSuperBlock->s_zsearch >= iNBitsMap)
		return ENOSPC; /* no space left on device */
 
	/* get start block and word (also used in super.c:free_bit) */
	uNewStartBlock = pSuperBlock->s_zsearch / FS_BITS_PER_BLOCK(pSuperBlock->s_block_size);
	uNewStartWord = (pSuperBlock->s_zsearch % FS_BITS_PER_BLOCK(pSuperBlock->s_block_size)) / FS_BITCHUNK_BITS;

	iNFreeBits = 0;
	/* Iterate the all block */ 
	while (uNewStartBlock < uNBlockMap)
	{
		/* Get the current block (super.c:free_bit) */
		pBuf = get_block (pSuperBlock->s_dev, iStartBlock + uNewStartBlock, NORMAL);
		pWordLimit = &pBuf->b_bitmap[FS_BITMAP_CHUNKS(pSuperBlock->s_block_size)];
	
		/* Iterate over the words in block. */
		for (pCurrWord = &pBuf->b_bitmap[uNewStartWord]; pCurrWord < pWordLimit; pCurrWord++)
		{
			/* Does this word contain a free bit? */
			if (*pCurrWord == (bitchunk_t) ~0)
			{
				iNFreeBits = 0;
				continue;
			}
			
			/* Find and allocate the free bit. */
			/* Possibly swap a 16-bit word between 8086 and 68000 byte order. */
			pCurrWordConverted = conv2 (pSuperBlock->s_native, (int) *pCurrWord);
			for (i = 0; i < FS_BITCHUNK_BITS; i++) 
			{
				if((pCurrWordConverted & (1 << i)) == 0)
				{
					iNFreeBits++;
					if(iNFreeBits == iNZones) /* size enough */
						break;
				}
				else
					iNFreeBits = 0;
			}

			if(iNFreeBits == iNZones)
			{
				/* Bit number from the start of the bit map. */
				iBitId = ((bit_t) uNewStartBlock * FS_BITS_PER_BLOCK(pSuperBlock->s_block_size))
						+ (pCurrWord - &pBuf->b_bitmap[0]) * FS_BITCHUNK_BITS
						+ i;

				/* Don't allocate bits beyond the end of the map: oversize */
				if (iBitId >= iNBitsMap)
					break;

				/* We found it!: release block and fill in iFreeZoneStart */
				put_block (pBuf, MAP_BLOCK);
				*iFreeZoneStart = iBitId - (iNZones-1);
				return OK;
			}
		}
		put_block (pBuf, MAP_BLOCK);

		uNewStartWord = 0;
		uNewStartBlock++;
	}
 
	return ENOSPC;
}



PRIVATE int reserve_zone(pSuperBlock, iFreeZoneStart, iNZones)
		 struct super_block* pSuperBlock;
		 bit_t iFreeZoneStart;
		 int iNZones;
{
	block_t iStartBlock;
	bit_t iNBitsMap, i, iBitId;
	unsigned uNBlockMap, uNewStartBlock, uNewStartWord;
	int iNFreeBits;
	struct buf *pBuf;
	bitchunk_t *pCurrWord, *pWordLimit, pCurrWordConverted;


	/* read-only file system */
	if (pSuperBlock->s_rd_only)
		return EROFS;

	/* from super.c ; considered we have a zmap */
	iStartBlock = START_BLOCK + pSuperBlock->s_imap_blocks; /* for imap: only start_block */
	iNBitsMap = pSuperBlock->s_zones - pSuperBlock->s_firstdatazone - 1;
	uNBlockMap = pSuperBlock->s_zmap_blocks;

	if (iFreeZoneStart >= iNBitsMap)
		return ENOSPC; /* no space left on device */

	/* get start block and word (also used in super.c:free_bit) */
	uNewStartBlock = iFreeZoneStart / FS_BITS_PER_BLOCK (pSuperBlock->s_block_size);
	uNewStartWord = (iFreeZoneStart % FS_BITS_PER_BLOCK (pSuperBlock->s_block_size)) / FS_BITCHUNK_BITS;

	i = iFreeZoneStart % FS_BITCHUNK_BITS;
	iNFreeBits = 0;
	while(uNewStartBlock < uNBlockMap)
	{
		/* Get the current block (super.c:free_bit) */
		pBuf = get_block (pSuperBlock->s_dev, iStartBlock + uNewStartBlock, NORMAL);
		pWordLimit = &pBuf->b_bitmap[FS_BITMAP_CHUNKS(pSuperBlock->s_block_size)];

		/* Iterate over the words in block. */
		for (pCurrWord = &pBuf->b_bitmap[uNewStartWord]; pCurrWord < pWordLimit; pCurrWord++)
		{
			/* Does this word contain a free bit? */
			if (*pCurrWord == (bitchunk_t) ~0) { /* word already full */
				put_block (pBuf, MAP_BLOCK);
				return EBUSY;
			}

			/* Find and allocate the free bit. */
			pCurrWordConverted = conv2 (pSuperBlock->s_native, (int) *pCurrWord);

			while(i < FS_BITCHUNK_BITS)
			{
				if ((pCurrWordConverted & (1 << i)) == 0)
				{
					pCurrWordConverted |= 1 << i;
					*pCurrWord = conv2 (pSuperBlock->s_native, (int) pCurrWordConverted);
					pBuf->b_dirt = DIRTY; /* used !*/
					iNFreeBits++;

					if (iNFreeBits == iNZones) /* all is reserved */
					{
						put_block (pBuf, MAP_BLOCK);
						return OK;
					}
				}
				/* One bit is already used (should not happen) */
				else
					return EBUSY;
				i++;
			}
			i = 0;
		}

		put_block (pBuf, MAP_BLOCK); /* release it */
		uNewStartWord = 0;
		uNewStartBlock++;
	}

	return OK;
}




PRIVATE int move_bits_full(pInode, iFreeZoneStart, iNZones)
		 struct inode *pInode;
		 bit_t iFreeZoneStart;
		 int iNZones;
{


	return OK;
}

PUBLIC int fs_defrag()
{
	int iZoneSize, iNZones, iReturn;
	struct inode *pInode;
	bit_t iFreeZoneStart;

	/* Get the right inode if available */
	pInode = get_inode (fs_dev, (ino_t)fs_m_in.REQ_INODE_NR);
	if (!pInode)
		return EINVAL;

	/* we need the size of the zone: link between the block and the zone
	   --> read_map from read.c */
	iZoneSize = (1 << pInode->i_sp->s_log_zone_size) * pInode->i_sp->s_block_size;
	iNZones = pInode->i_size / iZoneSize;
	
	/* we need to find somewhere to place the new file */
	iReturn = get_free_zone_start (pInode->i_sp, iNZones, &iFreeZoneStart);
	if (iReturn != OK)
	{
		put_inode (pInode); /* release the inode */
		return iReturn;
	}

	printf ("Find a zone: %d\n", iFreeZoneStart); /** TODO: remove */

	/* We have a zone, reserve this space */
	iReturn = reserve_zone (pInode->i_sp, iFreeZoneStart, iNZones);
	if (iReturn != OK)
	{
		put_inode (pInode); /* release the inode */
		return iReturn;
	}

	printf ("Zone reserved\n"); /** TODO: remove */

	/* Move bits and update all things linked to the inode */
	iReturn = move_bits_full (iNZones, pInode, iFreeZoneStart);
	if (iReturn != OK)
	{
		put_inode (pInode); /* release the inode */
		return iReturn;
	}

	printf ("Zone reserved\n"); /** TODO: remove */

	put_inode (pInode); /* release the inode */

	return OK;
}



