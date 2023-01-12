#include <stdio.h>

#define COLONNES 3
#define LIGNES 3

/* Vérification en colonne */
int checkDiag(char grille[3][3],char joueur) 
{
    int nbH = 0;
    int nbB = 0;
    int row,col;
    /* Boucle ligne */
    for (row = 0; row < LIGNES; row++) {
        /* Gauche droit à gauche haut */
        if (grille[row][row] == joueur) {
            nbH = nbH+1;
        }
        /* gauche bas à droite haut */
        if (grille[(LIGNES-row)-1][row] == joueur) {
            nbB = nbB+1;
        }
    }
    if ((nbH == 3) || (nbB == 3)) {
        return 1;
    }
    return -1;
}

/* Vérification en ligne */
int checkLigne(char grille[3][3],char joueur) 
{
    int nb = 0;
    int row,col;
    /* Boucle lignes */
    for (row = 0; row < LIGNES; row++) {
        /* boucle colonne */
        for (col = 0; col < COLONNES; col++) {
            if (grille[row][col]==joueur){
                /* Incrémentaiton si le joueur a une pièce à cette endroit la*/
                nb = nb + 1;
            }
        }
        if (nb == 3){
            /* Le joueur a gagné */
            return 1;
        }
        nb = 0;
    }
    /* Le joueur a perdu */
    return -1;
}

/* Vérification en colonne */
int checkCol(char grille[3][3],char joueur) 
{
    int nb = 0;
    int row,col;
    /* Boucle lignes */
    for (col = 0; col < COLONNES; col++) {
        /* boucle colonne */
        for (row = 0; row < LIGNES; row++) {
            if (grille[row][col]==joueur){
                /* Incrémentaiton si le joueur a une pièce à cette endroit la*/
                nb = nb + 1;
            }
        }
        if (nb == 3){
            /* Le joueur a gagné */
            return 1;
        }
        nb = 0;
    }
    /* Le joueur a perdu */
    return -1;
}

int checkWin(char grille[3][3],char joueur)
{
    /* Si la ligne, la colonne ou la diagonale est rempli alors on retourne 1 */
    if ((checkCol(grille,joueur) == 1) || (checkLigne(grille,joueur) == 1) || (checkDiag(grille,joueur) == 1)){
        return 1;
    } /* Aucun n'a gagné */
    return -1;
}

int grillePleine(char grille[3][3])
{
    /*
    * Si status = -1 => grille pleine
    * Sinon , grille avec un emplacement vide
    */
    int status = -1;
    int row,col;
    for (row = 0; row < LIGNES; row++){
        for (col = 0; col < COLONNES; col++){
            if ((grille[row][col] != 'X') && (grille[row][col] != 'O')){
                status = 1;
            }
        }
    }
    return status;
}