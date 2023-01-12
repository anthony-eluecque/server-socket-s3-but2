#include <stdio.h>
#include <stdlib.h> /* pour exit */
#include <unistd.h> /* pour read, write, close, sleep */
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h> /* pour memset */
#include <netinet/in.h> /* pour struct sockaddr_in */
#include <arpa/inet.h> /* pour htons et inet_aton */
#include "./utils/Utils.c"
#include "./utils/Utils_serveur.c"
#include "./utils/Utils_write_read.c"

#define MAX_LEN 256
#define COLONNES 3
#define LIGNES 3

// --------------------------------//
// 			ELUECQUE ANTHONY       //
// 			CORION GAUTHIER		   //
// --------------------------------//
//   BUT 2 : SAE Système et réseau //

int main(int argc, char *argv[]){
	int descripteurSocket;
	struct sockaddr_in sockaddrDistant;
	socklen_t longueurAdresse;

	char buffer[]="Hello server!"; // buffer stockant le message
	int nb; /* nb d’octets écrits et lus */
    char Recoi[256]; /* Message de réception */
	char ip_dest[16];
    char status[9]; /* Le status de la partie */
    char placement[MAX_LEN];
    int choix,row,col,port_dest;
    /* 
        choix = permet de rentrer dans une boucle while de saisie 
        row =  la ligne (de 0 à 2) de placement
        col = la colonne (de 0 à 2) de placement
    */

    // Préparation du jeu
    char grille[LIGNES][COLONNES]; /* La grille de jeu */
    int choixCol = 10,choixLigne = 10; /* Initialisation de la colonne et la ligne */
    initGrille(grille); /* Initialisation de la grille */

    // Partie gestion de la connexion du client avec ./client IP PORT

	if (argc>1) { // si il y a au moins 2 arguments passés en ligne de commande, récupération ip et port
		strncpy(ip_dest,argv[1],16);
		sscanf(argv[2],"%d",&port_dest);
	}else{
		printf("USAGE : %s ip port\n",argv[0]);
		exit(-1);
	}
	descripteurSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(descripteurSocket < 0){
		perror("Erreur en création de la socket..."); // Affiche le message d’erreur
		exit(-1); // On sort en indiquant un code erreur
	}
	printf("Socket créée! (%d)\n", descripteurSocket);
	longueurAdresse = sizeof(sockaddrDistant);
	memset(&sockaddrDistant, 0x00, longueurAdresse);
	sockaddrDistant.sin_family = AF_INET;
	sockaddrDistant.sin_port = htons(port_dest);
	inet_aton(ip_dest, &sockaddrDistant.sin_addr);
    
	if((connect(descripteurSocket, (struct sockaddr *)&sockaddrDistant,longueurAdresse)) == -1){
		perror("Erreur de connection avec le serveur distant...");
		close(descripteurSocket);
		exit(-2); // On sort en indiquant un code erreur
	}

	printf("Connexion au serveur %s:%d réussie!\n",ip_dest,port_dest);

    /* Initialisation des messages de reçu */
    char Recoi_start[MAX_LEN];
    int choix_2;

    /* Réception de la partie qui start ou non */
    read_char(descripteurSocket,Recoi_start,sizeof(Recoi_start));
    
    /* Condition qui permet de savoir si le jeu commence ou non */
    if (strcmp(Recoi_start,"start")==0){
        printf("Client : Connexion réussi, début de partie...\n");

        /* Variable de symbole actuel du joueur */
        char Color[256];

        /* Variable de symbole de l'adversaire */
        char Color_Adver[2];

        /* Attente d'une seconde pour que la transmission se passe bien */
        sleep(1);

        /* Réception du symbole actuel du joueur */
        read_char(descripteurSocket,Color,sizeof(Color));
        /* Insertion du symbole du joueur et de l'adversaire */
        if (Color[0] == 'X'){
            Color_Adver[0] = 'O';
        } else {
            Color_Adver[0] = 'X';
        }

        printf("\n\nVous êtes la pièce : %c \n et l'adversaire est : %c \n\n", Color[0], Color_Adver[0]);

        /* Initialisation des message de reçu */
        char Recoi_2[MAX_LEN];
        char Recoi_3[MAX_LEN];

        /* Boucle de jeu */
        while(1){

            /* Affichage de la grille */
            afficheGrille(grille);
            printf("\nEn attente d'une instruction du serveur... \n");

            /* Attente d'une seconde pour que la transmission se passe bien */
            sleep(1);

            /* Réception de l'information si le joueur attend ou non */
            read_char(descripteurSocket,Recoi_2,sizeof(Recoi_2));
            /* Variable de condition si c'est bien attente ou nonattente */
            choix_2 = strcmp(Recoi_2,"attente");

            /* Boucle qui permet au client de rester dedans s'il est en attente */
            while (choix_2==0){
                printf("Attente : En attente de l'adversaire\n");
                sleep(1);

                /* Réception des coordonnées que l'adversaire a mis */
                read_char(descripteurSocket,Recoi_3,sizeof(Recoi_3));
                /* Vérification si l'emplacement est vide ou non */
                for (int i=0;i<2;i++){
                    placement[i] = ' ';
                    placement[i] = Recoi_3[i];
                }
                for (int i=2;i<sizeof(Recoi_3);i++){
                    status[i-2] = ' ';
                    status[i-2] = Recoi_3[i];     
                }

                /* Valeur par défaut de la ligne et de la colonne */
                row = 10;
                col = 10;

                /* Initialisation des valeur temporaires de placement */
                char stTemp[2];
                char stTemp_2[2];

                /* Insertion des valeur de placement dans les deux variables temporaires */
                sprintf(stTemp,"%c",placement[0]);
                sprintf(stTemp_2,"%c",placement[1]);

                /* Convertion char en int */
                row = atoi(stTemp);
                col = atoi(stTemp_2);
        
                        
                printf("Attente : Reçu : Voici le placement row : %d et col %d\n",row,col);
                
                /* Condition si l'utilisateur continue ou non */
                if (strcmp(status,"continue")==0){
                    updateGrille(grille,row,col,Color_Adver[0]);
                }
                else{ /* Le client a perdu */
                    updateGrille(grille,row,col,Color_Adver[0]);
                    printf("%s","Nous vous souhaitons une bonne journée.\n");
                    afficheGrille(grille);
                    printf("\n\n\nVous avez perdu.\n\n\n");
                    close(descripteurSocket);
                    exit(0);
                }
                strcpy(Recoi_2,"nonattente\0");
                choix_2 = 1;
                /* Sortie de la boucle */
                
                read_char(descripteurSocket,Recoi_3,sizeof(Recoi_3));
            }
            
                    
            /* Affichage de la grille */
            afficheGrille(grille);

            /* Valeurs par défaut de la ligne et colonne */
            choixCol = 10;
            choixLigne = 10;

            /* Valeur par défaut de notre choix */
            choix = -1;
            while (choix==-1)
            {
                printf("\nChoisissez une colonne: ");
                scanf(" %d", &choixCol);

                printf("\nChoisissez une ligne: ");
                scanf(" %d", &choixLigne);
                choixCol = choixCol -1;
                choixLigne = choixLigne -1;
                /* Condition s'il est dans la grille ou non */
                choix = isInGrille(grille,choixLigne,choixCol);
                if (choix != -1) { /* Si non, alors on vérifie si la grille est vide */
                    choix = isEmpty(grille,choixLigne,choixCol);
                }
            }   

            /* Mise à jour de la grille */
            updateGrille(grille,choixLigne,choixCol,Color[0]);
            char Envoi[2] = {choixLigne,choixCol};
            // Envoi des coordonnées au serveur */
            write_char(descripteurSocket,Envoi,sizeof(Envoi));
            printf("\nColonne envoyé : %d\n",Envoi[0]);
            printf("Ligne envoyé : %d\n\n",Envoi[1]);
            
            sleep(2);
            printf("\nEn attente d'une instruction du serveur... \n");

            /* Réception du résultat du serveur */
            read_char(descripteurSocket,Recoi,sizeof(Recoi));

            /* Vérification s'il n'y a pas eu un bug */
            if (strcmp(Recoi,"nonattente")==0){
                printf("\n\nErreur j'ai reçu le code d'autre chose...\n\n");
            }
            
            /* Vérification si l'emplacement est vide ou non */
            for (int i=0;i<2;i++){
                placement[i] = ' ';
                placement[i] = Recoi[i];
            }
            for (int i=2;i<sizeof(Recoi);i++){
                status[i-2] = ' ';
                status[i-2] = Recoi[i];     
            }
        
            /* Valeur par défaut de la ligne et de la colonne */
            row = 10;
            col = 10;

            /* Initialisation des valeur temporaires de placement */
            char stTemp[2];
            char stTemp_2[2];

            /* Insertion des valeur de placement dans les deux variables temporaires */
            sprintf(stTemp,"%c",placement[0]);
            sprintf(stTemp_2,"%c",placement[1]);

            /* Convertion char en int */
            row = atoi(stTemp);
            col = atoi(stTemp_2);

                    
            printf("\nVoici le placement row : %d et col %d\n",row,col);
            
                /* Condition si l'utilisateur continue ou non */
            if (strcmp(status,"continue")==0){
                updateGrille(grille,row,col,Color[0]);
            }
            else{ /* Le client a gagné ou  perdu */
                printf("%s","Nous vous souhaitons une bonne journée.\n");
                afficheGrille(grille);
                if ((strcmp(status,"Xwins")==0) || (strcmp(status,"Owins")==0)){ 
                    printf("\n\n\nVous avez gané !!!!\n\n\n"); /* Le client a gagné */
                } else {
                    printf("\n\n\nVous avez perdu.\n\n\n"); /* Le client a perdu */
                }
                close(descripteurSocket); /* Fermeture du descripteur */
                exit(0);
            }

            /* Attente d'une seconde pour que la transmission se passe bien */
            sleep(1);
    
        }

    
    }
    /* Fermeture du descripteur */
	close(descripteurSocket);
	return 0;
}