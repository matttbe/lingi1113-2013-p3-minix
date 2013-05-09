/*
 * Project P3
 * 
 * Copyright 2013 UCLouvain / pole INGI
 *
 * Authors:
 *   Matthieu Baerts <matthieu.baerts@student.uclouvain.be>
 *   Hélène Verhaeghe <helene.verhaeghe@student.uclouvain.be>
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


#include <nfrags.h>
#include <defrag.h>
#include <stdio.h>  /* FILE */
#include <assert.h> /* assert() */
#include <errno.h>  /* EFTYPE, EBUSY, etc. */

#define TRUE 1
#define FALSE 0

#define _cFilePath 				"/mnt/disk/test.txt"
#define _cWrongFilePathDir 		"/mnt/disk/"
#define _cWrongFilePathError 	"/mnt/disk/error.txt"

static test_wrong_file_dir (int bNFrags)
{
	int iReturn;

	printf ("Test Wrong file: dir: %s\n", _cWrongFilePathDir);

	if (bNFrags)
		iReturn = nfrags (_cWrongFilePathDir);
	else
		iReturn = defrag (_cWrongFilePathDir);
	assert (iReturn == -1);
	assert (errno == EFTYPE);

	printf ("Return: %d, error: %d\n", iReturn, errno);
}

static test_wrong_file_unavailable (int bNFrags)
{
	int iReturn;

	printf ("Test Wrong file: unavailable: %s\n", _cWrongFilePathError);

	if (bNFrags)
		iReturn = nfrags (_cWrongFilePathError);
	else
		iReturn = defrag (_cWrongFilePathError);
	assert (iReturn == -1);
	/* assert (errno == EFTYPE); */

	printf ("Return: %d, error: %d\n", iReturn, errno);
}

static test_file_busy (int bNFrags)
{
	int iReturn;
	FILE *fd; /* To have a busy file */

	printf ("Test file: busy: %s\n", _cFilePath);

	fd = fopen (_cFilePath, "r");

	if (bNFrags)
		iReturn = nfrags (_cFilePath);
	else
		iReturn = defrag (_cFilePath);
	assert (iReturn == -1);
	assert (errno == EBUSY);

	printf ("Return: %d, error: %d\n", iReturn, errno);

	fclose (fd);
}

int main(int argc, char **argv)
{
	int iReturn;

	test_wrong_file_dir (TRUE);
	test_wrong_file_unavailable (TRUE);
	test_file_busy (TRUE);

	iReturn = nfrags (_cFilePath);
	printf ("Test NFrags: %d\n", iReturn);

	test_wrong_file_dir (FALSE);
	test_wrong_file_unavailable (FALSE);
	test_file_busy (FALSE);

	iReturn = defrag (_cFilePath);
	printf ("Test DeFrag: %d\n", iReturn);

	return 0;
}

