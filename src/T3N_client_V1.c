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
            printf("Client : Lancement Message %s reçu! (%d octets)\n\n", Recoi_start, nb);
            if (strcmp(Recoi_start,"start")==0){
                printf("Client : Connexion réussi, début de partie...");
                while(1){

                    afficheGrille(grille);
                    choixCol = 10;
                    choixLigne = 10;
                    choix = -1;
                    while (choix==-1)
                    {
                        printf("Choisissez une colonne: ");
                        scanf(" %d", &choixCol);

                        printf("Choisissez une ligne: ");
                        scanf(" %d", &choixLigne);
                        
                        choix = isInGrille(grille,choixLigne,choixCol);
                        if (choix != -1) {
                            choix = isEmpty(grille,choixLigne,choixCol);
                        }
                    }   
                    updateGrille(grille,choixLigne,choixCol,'X');
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
                            printf("Client : envoyé! (%d octets)\n\n", nb);
                            printf("\nColonne envoyé : %d\n",Envoi[0]);
                            printf("Ligne envoyé : %d\n\n",Envoi[1]);
                    }
                    // Partie réception
                    // char *Recoi[4]; 
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
                            printf("Voici le placement row : %d et col %d\n",row,col);
                            printf("Voici le status : %s\n",status);
                            if (strcmp(status,"continue")==0){
                                updateGrille(grille,row,col,'O');
                            }
                            else{
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