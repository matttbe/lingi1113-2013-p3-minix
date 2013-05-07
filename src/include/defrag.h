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


#ifndef __DEFRAG_H__
#define __DEFRAG_H__

/**
 * entrée: *file* Une chaîne de caractères indiquant le chemin d’accès
 * vers un fichier, absolu ou relatif.
 * valeur de retour: La valeur de retour est le nombre de
 * fragments qui constituaient le fichier file avant
 * la défragmentation.
 * Si defrag retourne une valeur positive, alors
 * l’exécution de nfrags sur le même fichier donnera 1.
 * Si une erreur se produit, -1 est retourné, et errno
 * reçoit le code correspondant à l’erreur.
 */
_PROTOTYPE(int defrag, (const char *file));

#endif /* __DEFRAG_H__ */