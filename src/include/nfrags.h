#ifndef __NFRAGS_H__
#define __NFRAGS_H__

/**
 * entrée: *file* Une chaîne de caractères indiquant le chemin d’accès
 * vers un fichier, absolu ou relatif.
 * valeur de retour: La valeur de retour est le nombre de
 * fragments non contigus qui constituent le fichier file.
 * Un fichier non fragmenté contient un fragment.
 * Si une erreur se produit, -1 est retourné, et errno
 * reçoit le code correspondant à l’erreur.
 */
_PROTOTYPE(int nfrags, (const char *));

#endif /* __NFRAGS_H__ */
