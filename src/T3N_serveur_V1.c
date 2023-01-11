#include <stdio.h>
#include <stdlib.h> /* pour exit */
#include <unistd.h> /* pour read, write, close, sleep */
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h> /* pour memset */
#include <netinet/in.h> /* pour struct sockaddr_in */
#include <arpa/inet.h> /* pour htons et inet_aton */
#include <time.h>
#include "Utils.c"
#include "Utils_serveur.c"

#define COLONNES 3
#define LIGNES 3
#define PORT 4500 // = 4500 (ports >= 4500 réservés pour usage explicite)

#define LG_MESSAGE 256

int main(int argc, char *argv[]){
	int socketEcoute;

	struct sockaddr_in pointDeRencontreLocal;
	socklen_t longueurAdresse;

	int socketDialogue;
	struct sockaddr_in pointDeRencontreDistant;
	// char messageEnvoi[LG_MESSAGE]; /* le message de la couche Application ! */
	char messageEnvoi[] = "start\0";
	char messageRecu[LG_MESSAGE]; /* le message de la couche Application ! */
	int ecrits, lus; /* nb d’octets ecrits et lus */

	/* Les octet reçu et envoyé */
	int nb;
	int retour;
    int choix;

	/* La colonne et la ligne */
	char MSGCol[11];
	char MSGLigne[11];

	// char Message[LG_MESSAGE][LG_MESSAGE];

	char result[LG_MESSAGE];
	char Message[11];
	strcpy(Message,"continue");

	// Crée un socket de communication
	socketEcoute = socket(PF_INET, SOCK_STREAM, 0); 
	// Teste la valeur renvoyée par l’appel système socket() 
	if(socketEcoute < 0){
		perror("socket"); // Affiche le message d’erreur 
	exit(-1); // On sort en indiquant un code erreur
	}
	printf("Socket créée avec succès ! (%d)\n", socketEcoute); // On prépare l’adresse d’attachement locale

	// Remplissage de sockaddrDistant (structure sockaddr_in identifiant le point d'écoute local)
	longueurAdresse = sizeof(pointDeRencontreLocal);
	// memset sert à faire une copie d'un octet n fois à partir d'une adresse mémoire donnée
	// ici l'octet 0 est recopié longueurAdresse fois à partir de l'adresse &pointDeRencontreLocal
	memset(&pointDeRencontreLocal, 0x00, longueurAdresse); 
	pointDeRencontreLocal.sin_family = PF_INET;
	pointDeRencontreLocal.sin_addr.s_addr = htonl(INADDR_ANY); // attaché à toutes les interfaces locales disponibles
	pointDeRencontreLocal.sin_port = htons(PORT); // = 5000 ou plus
	
	// On demande l’attachement local de la socket
	if((bind(socketEcoute, (struct sockaddr *)&pointDeRencontreLocal, longueurAdresse)) < 0) {
		perror("bind");
		exit(-2); 
	}
	printf("Socket attachée avec succès !\n");

	// On fixe la taille de la file d’attente à 5 (pour les demandes de connexion non encore traitées)
	if(listen(socketEcoute, 5) < 0){
   		perror("listen");
   		exit(-3);
	}
	printf("Socket placée en écoute passive ...\n");

	// boucle d’attente de connexion : en théorie, un serveur attend indéfiniment !

	/* Initialisaiton de la grille */
    char grille[LIGNES][COLONNES];
    int choixCol = 10;
    int choixLigne = 10;
    initGrille(grille);

	while(1){
		memset(messageRecu, 0x00, LG_MESSAGE*sizeof(char));
		printf("Attente d’une demande de connexion (quitter avec Ctrl-C)\n\n");
		
		// c’est un appel bloquant
		socketDialogue = accept(socketEcoute, (struct sockaddr *)&pointDeRencontreDistant, &longueurAdresse);
		if (socketDialogue < 0) {
   			perror("accept");
			close(socketDialogue);
   			close(socketEcoute);
   			exit(-4);
		}

		/* Écriture de commencement */
        switch(ecrits = write(socketDialogue, &messageEnvoi, sizeof(messageEnvoi))){
            case -1 : /* une erreur ! */
                perror("write");
                close(socketDialogue);
                exit(-6);
            case 0 :  /* la socket est fermée */
                fprintf(stderr, "La socket a été fermée par le client !\n\n");
                close(socketDialogue);
                return 0;
            default:  /* envoi de n octets */
                printf("Serveur : Message %s envoyé (%d octets)\n\n", messageEnvoi, ecrits);
                
				while (1){
                    // On réception les données du client (cf. protocole)
                    switch(lus = read(socketDialogue, messageRecu, LG_MESSAGE*sizeof(char))) {
                        case -1 : /* une erreur ! */ 
                            perror("read"); 
                            close(socketDialogue); 
                            exit(-5);
                        case 0  : /* la socket est fermée */
                            fprintf(stderr, "La socket a été fermée par le client !\n\n");
                            close(socketDialogue);
                            return 0;
                        default:  /* réception de n octets */
							printf("Retour après jeu du joueur\n");
                            printf("Serveur : Message reçu : %d (%d octets)\n\n", messageRecu[0], lus);

							/* Mise à joru de la grille */
                            updateGrille(grille,messageRecu[0],messageRecu[1],'X');

							/* Condition si la grille est pleine ou non */
							if (grillePleine(grille)==-1) {
								/* Message mis sur Xend */
								strcpy(Message,"Xend");
							} else  {
								/* Vérification si X a gagné */
								if (checkWin(grille,'X')==1) {
									strcpy(Message,"Xwins");
								} else {
									/* Mise à joru de la grille */
									choixLigne = 10;
                            		choixCol = 10;
									choix = -1;
									while (choix == -1) {
										choixLigne = rand() % 3;
										choixCol = rand() % 3;
										choix = isInGrille(grille,choixLigne,choixCol);
										if (choix != -1) {
											choix = isEmpty(grille,choixLigne,choixCol);
										}
									}   

									/* Mise à jour de la grille */
									updateGrille(grille,choixLigne,choixCol,'O');

									/* Affichage de la grille */
									afficheGrille(grille);

									/* Vérification si O gagne */	
									if (checkWin(grille,'O')==1) {
										strcpy(Message,"Owins");
									} else {
										if (grillePleine(grille)==-1) {
											strcpy(Message,"Owins");
										}
									}
								}
							}

							/* Concaténation des coordonnées et du message de renvoi (Xwins,00continue,Owins, Xend...) */
							MSGCol[0] = ' ';
							MSGLigne[0] = ' ';
							sprintf(MSGCol,"%d",choixCol);
							sprintf(MSGLigne,"%d",choixLigne);
							strcat(MSGLigne,MSGCol);
							strcat(MSGLigne,Message);
			

							/* Envoi du message s'il peut continuer, s'il a gagné ou s'il arrête. */
                            switch(ecrits = write(socketDialogue, MSGLigne, sizeof(MSGLigne))){
                                case -1 : /* une erreur ! */
                                    perror("write");
                                    close(socketDialogue);
                                    exit(-6);
                                case 0 :  /* la socket est fermée */
                                    fprintf(stderr, "La socket a été fermée par le client !\n\n");
                                    close(socketDialogue);
                                    return 0;
                                default:  /* envoi de n octets */
                                    printf("Serveur : Message envoyé (%d octets) \nStatus %s \nCol : %c \nLigne : %c\n\n",ecrits,MSGLigne,MSGLigne[0],MSGLigne[1]);
                                    // On ferme la socket de dialogue et on se replace en attente ...
							}

							/* condition s'il continue ou non la parite */
							if (strcmp(Message,"continue")!=0){
								printf("JE ME FERME\n");
								close(socketDialogue);
								close(socketEcoute);
								exit(0);
                            }
                        }
                }
        }
	}
	// On ferme la ressource avant de quitter
   	close(socketEcoute);
	return 0; 
}