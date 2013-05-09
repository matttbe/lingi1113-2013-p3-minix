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

	for (iPos = 0 ; iPos < pInode->i_size ; iPos += pInode->i_sp->s_block_size)
	{
		pCurrBlock = read_map (pInode, iPos);
		if (pCurrBlock != pPrevBlock)
		{
			/* if the current block is distant of 2 to the previous, we have a fragment */
			if (pCurrBlock - pPrevBlock > 1
				|| pCurrBlock - pPrevBlock < 0) /* can be negative? */
				iNFrags++; 
		}
		pPrevBlock = pCurrBlock;
	}
	return iNFrags;
}

PUBLIC int fs_nfrags()
{
	register struct inode *pInode;
	int iNFrags;

	pInode = get_inode (fs_dev, (ino_t)fs_m_in.REQ_INODE_NR);
	if (! pInode)
		return EINVAL;

	iNFrags = get_nb_fragments (pInode);

	put_inode (pInode); /* will free the inode, etc. */

	fs_m_out.m9_s2 = iNFrags

	return 0; /* OK */
}


PUBLIC int fs_defrag()
{
	return 11;
}



