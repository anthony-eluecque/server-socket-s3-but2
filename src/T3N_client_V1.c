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

    int choix,port_dest,row,col;
    /* 
        choix = permet de rentrer dans une boucle while de saisie 
        row =  la ligne (de 0 à 2) de placement
        col = la colonne (de 0 à 2) de placement
    */

    /* Initialisation des reçu */
    char status[9];
    char placement[MAX_LEN];


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


    // Préparation du jeu
    char grille[LIGNES][COLONNES];   /* La grille de jeu */
    int choixCol = 10,choixLigne = 10;  /* Initialisation de la colonne et la ligne */
    initGrille(grille);  /* Initialisation de la grille */
    char Recoi_start[MAX_LEN];

    /* Réception de si le joueur peut commencer ou non */
    switch(nb = read(descripteurSocket, Recoi_start, sizeof(Recoi_start))) {
        case -1 : /* Erreur ! */
            perror("Erreur de lecture...");
            close(descripteurSocket);
            exit(-4);
        case 0 : /* socket fermé */
        fprintf(stderr, "La socket a été fermée par le serveur !\n\n");
            return 0;
        default: /* n octet */

            printf("Client : Lancement Message %s reçu! (%d octets)\n\n", Recoi_start, nb);

            /* Vérification si le joueur peut commencer */
            if (strcmp(Recoi_start,"start")==0){
                printf("Client : Connexion réussi, début de partie...");
                /* Boucle de jeu */
                while(1){
                    
                    /* Affichage de l a grille */
                    afficheGrille(grille);
                    choixCol = 10;
                    choixLigne = 10;
                    choix = -1;
                    /* Choix des coordonnées */
                    while (choix==-1)
                    {
                        printf("Choisissez une colonne: ");
                        scanf(" %d", &choixCol);

                        printf("Choisissez une ligne: ");
                        scanf(" %d", &choixLigne);
                        
                        /* Vérification les coordnnées sont dans la grille */
                        choix = isInGrille(grille,choixLigne,choixCol);
                        if (choix != -1) {
                            /* Vérification si la grille est vide */
                            choix = isEmpty(grille,choixLigne,choixCol);
                        }
                    }   

                    /* Mise à jour de la grille */
                    updateGrille(grille,choixLigne,choixCol,'X');
                    char Envoi[2] = {choixLigne,choixCol};
                    
                    /* Envoi des coordnnées */
                    switch(nb = write(descripteurSocket, Envoi, sizeof(Envoi))){
                        case -1 : /* Une erreur ! */
                                perror("Erreur en écriture...");
                                close(descripteurSocket);
                                exit(-3);
                        case 0 :  /* Socket fermé */
                            fprintf(stderr, "La socket a été fermée par le serveur !\n\n");
                            return 0;
                        default:  /* n octtet */
                            printf("Client : envoyé! (%d octets)\n\n", nb);
                            printf("\nColonne envoyé : %d\n",Envoi[0]);
                            printf("Ligne envoyé : %d\n\n",Envoi[1]);
                    }
                    
                    
                    /* Réception des coordonnées et du message si c'est fini bon ou gagnant */
                    switch(nb = read(descripteurSocket, Recoi, sizeof(Recoi))) {
                        case -1 :
                            perror("Erreur de lecture...");
                            close(descripteurSocket);
                            exit(-4);
                        case 0 : 
                        fprintf(stderr, "La socket a été fermée par le serveur !\n\n");
                            return 0;
                        default:
                            printf("Serveur : Message reçu (%d octets) \nMessage %s \n\n", nb, Recoi);
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
                    
                            
                            printf("Voici le placement row : %d et col %d\n",row,col);
                            printf("Voici le status : %s\n",status);

                            /* Condition si le joueur continue la partie */
                            if (strcmp(status,"continue")==0){
                                /* Mise à jour de la grille */
                                updateGrille(grille,row,col,'O');
                            }
                            else{ /* Le client a fini de joueur */
                                printf("%s","Le client se ferme\n");
                                close(descripteurSocket);
                                exit(0);
                            }
                    }
                }

            }
    }
	close(descripteurSocket);

	return 0;
}