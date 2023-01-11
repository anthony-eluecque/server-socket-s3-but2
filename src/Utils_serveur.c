#include <stdio.h>

#define COLONNES 3
#define LIGNES 3

int checkDiag(char grille[3][3],char joueur) 
{
    int nbH = 0;
    int nbB = 0;
    int row,col;
    for (row = 0; row < LIGNES; row++) {
        if (grille[row][row] == joueur) {
            nbH = nbH+1;
        }
        if (grille[(LIGNES-row)-1][row] == joueur) {
            nbB = nbB+1;
        }
    }
    if ((nbH == 3) || (nbB == 3)) {
        return 1;
    }
    return -1;
}

int checkLigne(char grille[3][3],char joueur) 
{
    int nb = 0;
    int row,col;
    for (row = 0; row < LIGNES; row++) {
        for (col = 0; col < COLONNES; col++) {
            if (grille[row][col]==joueur){
                nb = nb + 1;
            }
        }
        if (nb == 3){
            return 1;
        }
        nb = 0;
    }
    return -1;
}

int checkCol(char grille[3][3],char joueur) 
{
    int nb = 0;
    int row,col;
    for (col = 0; col < COLONNES; col++) {
        for (row = 0; row < LIGNES; row++) {
            if (grille[row][col]==joueur){
                nb = nb + 1;
            }
        }
        if (nb == 3){
            return 1;
        }
        nb = 0;
    }
    return -1;
}

int checkWin(char grille[3][3],char joueur)
{
    if ((checkCol(grille,joueur) == 1) || (checkLigne(grille,joueur) == 1) || (checkDiag(grille,joueur) == 1)){
        return 1;
    }
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