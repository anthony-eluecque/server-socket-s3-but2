#include <stdio.h>
// #include "Utils.h"
#define COLONNES 3
#define LIGNES 3

void initGrille(char grille[3][3])
{
    /* Initalise les caractères de la grille.*/
    int i,j;
    /* La ligne */
    for (i = 0; i < LIGNES; i++)
        /* La colonne */
        for (j = 0; j < COLONNES; j++)
            grille[i][j] = ' ';
}

void afficheGrille(char grille[3][3])
{
    /*
     * Affiche la grille pour le ou les joueurs.
     */
    printf("\n");
    printf("  | 1 | 2 | 3 |\n---------------\n");
    int col,ligne;
    /* La ligne */
    for (ligne = 0 ; ligne<LIGNES ; ligne++){
        /* La colonne */
        for (col = 0 ; col< COLONNES ; col++){
            if (col==0){
                printf("%d | ",ligne+1);
            }
            printf("%c%s",grille[ligne][col]," | ");

        }
        printf("\n---------------\n");
    }
}

int isInGrille(char grille[3][3], int ligne, int colonne)
{
    printf("%d,%d",ligne,colonne);
    if ((ligne<0) || (ligne>=LIGNES)){
        /* Il dépasse la ligne*/
        return -1;
    }
    if ((colonne<0) || (colonne>=COLONNES)){
        /* Il dépasse la colonne */
        return -1;
    }
    /* Oui il peut poser */
    return 1;
}

int isEmpty(char grille[3][3],int ligne, int colonne){
    if ((ligne == 10) && (colonne == 10)){
        /* Nombre par défaut */
        return -1;
    } 
    if ((grille[ligne][colonne] != 'X') && (grille[ligne][colonne] != 'O')){
        /* Il y a quelqu'un a cette emplacement */
        return 1;
    }
    /* Oui il peut poser */
    return -1;
}

void updateGrille(char grille[3][3],int ligne,int colonne,char type) {
    /* Mise à jour de la grille en par un nouveau type */
    grille[ligne][colonne] = type;
}
