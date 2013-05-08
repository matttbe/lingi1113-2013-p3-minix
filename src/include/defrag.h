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
_PROTOTYPE(int defrag, (const char*));

#endif /* __DEFRAG_H__ */
