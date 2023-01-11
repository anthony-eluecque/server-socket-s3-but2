#include <stdio.h>
#include <stdlib.h> /* pour exit */
#include <unistd.h> /* pour read, write, close, sleep */
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h> /* pour memset */
#include <netinet/in.h> /* pour struct sockaddr_in */
#include <arpa/inet.h> /* pour htons et inet_aton */
#include "Utils.c"
#include "Utils_serveur.c"

#define MAX_LEN 256
#define COLONNES 3
#define LIGNES 3


int main(int argc, char *argv[]){
	int descripteurSocket;
	struct sockaddr_in sockaddrDistant;
	socklen_t longueurAdresse;

	char buffer[]="Hello server!"; // buffer stockant le message
	int nb; /* nb d’octets écrits et lus */
    char Recoi[256];
	char ip_dest[16];
	int port_dest;
    int choix;
    char status[9];
    char placement[MAX_LEN];
    int row,col;

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






    char grille[LIGNES][COLONNES];
    int choixCol = 10,choixLigne = 10;
    initGrille(grille);
    char Recoi_start[MAX_LEN];
    int choix_2;

    switch(nb = read(descripteurSocket, Recoi_start, sizeof(Recoi_start))) {
        case -1 :
            perror("Erreur de lecture...");
            close(descripteurSocket);
            exit(-4);
        case 0 : 
        fprintf(stderr, "La socket a été fermée par le serveur !\n\n");
            return 0;
        default:
            // Recoi_start[nb]='\0';
            // printf("Client : Lancement Message %s reçu! (%d octets)\n\n", Recoi_start, nb);
            if (strcmp(Recoi_start,"Vous êtes dans la file d'attente")==0){
                while(1) {
                    printf("\nBienvenue en tant que spectateur de la partie !\n");
                    printf("\nVous êtes en attente d'une fin de manche...\n");
                    switch(nb = read(descripteurSocket, Recoi_start, sizeof(Recoi_start))) {
                        case -1 :
                            perror("Erreur de lecture...");
                            close(descripteurSocket);
                            exit(-4);
                        case 0 : 
                        fprintf(stderr, "La socket a été fermée par le serveur !\n\n");
                            return 0;
                        default:
                            printf("%s", Recoi_start);
                    }
                }
            }
            if (strcmp(Recoi_start,"start")==0){
                printf("Client : Connexion réussi, début de partie...\n");
                char Color[256];
                char Color_Adver[2];
                sleep(1);
                 switch(read(descripteurSocket, Color, sizeof(Recoi_start))) {
                    case -1 :
                        return 0;  
                    case 0 : 
                      return 0;  
                    default:
                        if (Color[0] == 'X'){
                            Color_Adver[0] = 'O';
                        } else {
                            Color_Adver[0] = 'X';
                        }
                        printf("\n\nVous êtes la pièce : %c \n et l'adversaire est : %c \n\n", Color[0], Color_Adver[0]);
                        char Recoi_2[MAX_LEN];
                        char Recoi_3[MAX_LEN];
                        while(1){
                            afficheGrille(grille);
                            printf("\nEn attente d'une instruction du serveur... \n");
                            sleep(1);
                            switch(read(descripteurSocket, Recoi_2, sizeof(Recoi_2))) {
                                default:
                                    // printf("----------> %s \n",Recoi_2);
                                    choix_2 = strcmp(Recoi_2,"attente");
                                    while (choix_2==0){
                                        printf("Attente : En attente de l'adversaire\n");
                                        sleep(1);
                                        switch(nb = read(descripteurSocket, Recoi_3, sizeof(Recoi_3))) {
                                            case -1 :
                                                perror("Attente : Erreur de lecture...");
                                                close(descripteurSocket);
                                                exit(-4);
                                            case 0 : 
                                            fprintf(stderr, "Attente : La socket a été fermée par le serveur !\n\n");
                                                return 0;
                                            default:
                                                // printf("Attente : Message reçu (%d octets) \nMessage %s \n\n", nb, Recoi_3);
                                                // Les conditions
                                                for (int i=0;i<2;i++){
                                                    placement[i] = ' ';
                                                    placement[i] = Recoi_3[i];
                                                }
                                                for (int i=2;i<sizeof(Recoi_3);i++){
                                                    status[i-2] = ' ';
                                                    status[i-2] = Recoi_3[i];     
                                                }
                                    
                                                row = 10;
                                                col = 10;

                                                char stTemp[2];
                                                char stTemp_2[2];
                                            
                                                sprintf(stTemp,"%c",placement[0]);
                                                sprintf(stTemp_2,"%c",placement[1]);
                                                
                                                row = atoi(stTemp);
                                                col = atoi(stTemp_2);
                                        
                                                // strcpy
                                                // Recoi_start[MAX_LEN] = "attente\0"
                                                printf("Attente : Reçu : Voici le placement row : %d et col %d\n",row,col);
                                                // printf("Attente : Reçu : Voici le status : %s\n",status);
                                                if (strcmp(status,"continue")==0){
                                                    updateGrille(grille,row,col,Color_Adver[0]);
                                                }
                                                else{
                                                    printf("%s","Nous vous souhaitons une bonne journée.\n");
                                                    afficheGrille(grille);
                                                    printf("\n\n\nVous avez perdu.\n\n\n");
                                                    close(descripteurSocket);
                                                    exit(0);
                                                    close(descripteurSocket);
                                                    exit(0);
                                                }
                                                strcpy(Recoi_2,"nonattente\0");
                                                choix_2 = 1;
                                                // printf("JE SORS DE l'attente\n");
                                                printf("\nEn attente d'une instruction du serveur... \n");
                                                switch(nb = read(descripteurSocket, Recoi_3, sizeof(Recoi_3))) {
                                                    default:
                                                        printf("\nFin de votre attente...\n");
                                                }
                                        }
                                    }
                                    // switch(read(descripteurSocket, Recoi_start, sizeof(Recoi_start))) {
                                    afficheGrille(grille);
                                    choixCol = 10;
                                    choixLigne = 10;
                                    choix = -1;
                                    while (choix==-1)
                                    {
                                        printf("\nChoisissez une colonne: ");
                                        scanf(" %d", &choixCol);

                                        printf("\nChoisissez une ligne: ");
                                        scanf(" %d", &choixLigne);
                                        
                                        choix = isInGrille(grille,choixLigne,choixCol);
                                        if (choix != -1) {
                                            choix = isEmpty(grille,choixLigne,choixCol);
                                        }
                                    }   
                                    updateGrille(grille,choixLigne,choixCol,Color[0]);
                                    char Envoi[2] = {choixLigne,choixCol};
                                    // Partie envoie
                                    switch(nb = write(descripteurSocket, Envoi, sizeof(Envoi))){
                                        case -1 :
                                                perror("Erreur en écriture...");
                                                close(descripteurSocket);
                                                exit(-3);
                                        case 0 : 
                                            fprintf(stderr, "La socket a été fermée par le serveur !\n\n");
                                            return 0;
                                        default: 
                                            // printf("Client : envoyé! (%d octets)\n\n", nb);
                                            printf("\nColonne envoyé : %d\n",Envoi[0]);
                                            printf("Ligne envoyé : %d\n\n",Envoi[1]);
                                    }
                                    // Partie réception
                                    // char *Recoi[4]; 
                                    sleep(2);
                                    printf("\nEn attente d'une instruction du serveur... \n");
                                    switch(nb = read(descripteurSocket, Recoi, sizeof(Recoi))) {
                                        case -1 :
                                            perror("Erreur de lecture...");
                                            close(descripteurSocket);
                                            exit(-4);
                                        case 0 : 
                                        fprintf(stderr, "La socket a été fermée par le serveur !\n\n");
                                            return 0;
                                        default:
                                            // printf("Serveur : Message reçu (%d octets) \n\n", nb, Recoi);
                                            if (strcmp(Recoi,"nonattente")==0){
                                                printf("\n\nErreur j'ai reçu le code d'autre chose...\n\n");
                                            }
                                            // Les conditions
                                            for (int i=0;i<2;i++){
                                                placement[i] = ' ';
                                                placement[i] = Recoi[i];
                                            }
                                            for (int i=2;i<sizeof(Recoi);i++){
                                                status[i-2] = ' ';
                                                status[i-2] = Recoi[i];     
                                            }
                                
                                            row = 10;
                                            col = 10;

                                            char stTemp[2];
                                            char stTemp_2[2];
                                        
                                            sprintf(stTemp,"%c",placement[0]);
                                            sprintf(stTemp_2,"%c",placement[1]);
                                            
                                            row = atoi(stTemp);
                                            col = atoi(stTemp_2);
                                    
                                            // strcpy
                                            printf("\nVoici le placement row : %d et col %d\n",row,col);
                                            // printf("Voici le status : %s\n",status);
                                            if (strcmp(status,"continue")==0){
                                                updateGrille(grille,row,col,Color[0]);
                                            }
                                            else{
                                                printf("%s","Nous vous souhaitons une bonne journée.\n");
                                                afficheGrille(grille);
                                                if ((strcmp(status,"Xwins")==0) || (strcmp(status,"Owins")==0)){
                                                    printf("\n\n\nVous avez gané !!!!\n\n\n");
                                                } else {
                                                    printf("\n\n\nVous avez perdu.\n\n\n");
                                                }
                                                close(descripteurSocket);
                                                exit(0);
                                            }
                                            sleep(1);
                                    }
                            }  
                        }
                }

            }
    }
	close(descripteurSocket);

	return 0;
}