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

#include <lib.h>
#include <unistd.h>
#include <string.h> /* strlen */

PUBLIC int nfrags(cFilePath)
const char *cFilePath;
{
  message m;
  int r;

  m.m1_p1 = (char *)cFilePath;		/* m_in.name1 */
  m.m1_i1 = strlen(cFilePath) + 1;	/* m_in.name1_length, TODO: can maybe be removed => setgroups */
  r = _syscall(VFS_PROC_NR, NFRAGS, &m);
  if (r < 0)
	return -1; /* we want -1 if there is an error */
  return r;
}
