#include <stdio.h>
// #include "Utils.h"
#define COLONNES 3
#define LIGNES 3

void initGrille(char grille[3][3])
{
    /* Initalise les caractères de la grille.*/
    int i,j;
    for (i = 0; i < LIGNES; i++)
        for (j = 0; j < COLONNES; j++)
            grille[i][j] = ' ';
}

void afficheGrille(char grille[3][3])
{
    /*
     * Affiche la grille pour le ou les joueurs.
     */
    printf("\n");
    int col,ligne;
    for (ligne = 0 ; ligne<LIGNES ; ligne++){
        for (col = 0 ; col< COLONNES ; col++){
            printf("%c", grille[ligne][col]);
            if (col != 2) {
                printf("%c",'|');
            }
        }
        printf("\n");
    }
}

int isInGrille(char grille[3][3], int ligne, int colonne)
{
    printf("%d,%d",ligne,colonne);
    if ((ligne<0) || (ligne>=LIGNES)){
        printf("%s","En dehors du tab");
        return -1;
    }
    if ((colonne<0) || (colonne>=COLONNES)){
        printf("%s","En dehors du tab");
        return -1;
    }
    return 1;
}

int isEmpty(char grille[3][3],int ligne, int colonne){
    if ((ligne == 10) && (colonne == 10)){
        return -1;
    } 
    if ((grille[ligne][colonne] != 'X') && (grille[ligne][colonne] != 'O')){
        return 1;
    }
    return -1;
}

void updateGrille(char grille[3][3],int ligne,int colonne,char type) {
    grille[ligne][colonne] = type;
}

