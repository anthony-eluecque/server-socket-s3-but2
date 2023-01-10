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

#define NB_JOUEURS 2
#define COLONNES 3
#define LIGNES 3
#define PORT 4525 // = 4500 (ports >= 4500 réservés pour usage explicite)

#define LG_MESSAGE 256

int main(int argc, char *argv[]){
	int socketEcoute;

	struct sockaddr_in pointDeRencontreLocal;
	socklen_t longueurAdresse;
	
	int connectSocket[50];

	int socketDialogue_joueur0,socketDialogue_joueurX;
	struct sockaddr_in pointDeRencontreDistant;
	// char messageEnvoi[LG_MESSAGE]; /* le message de la couche Application ! */
	char messageEnvoi[] = "start\0";
	char messageRecu[LG_MESSAGE]; /* le message de la couche Application ! */
	int ecrits, lus; /* nb d’octets ecrits et lus */
	int nb;
	int retour;
    int choix;
	char MSGCol[11];
	char MSGLigne[11];
	int autre;
	char attente[LG_MESSAGE] = "attente\0";
	char attente_non[LG_MESSAGE] = "nonattente\0";

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
	printf("%d",longueurAdresse);
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
	printf("Socket 0 attachée avec succès !\n");


	// if((bind(socketEcoute_joueurX, (struct sockaddr *)&pointDeRencontreLocal, longueurAdresse)) < 0) {
	// 	perror("bind");
	// 	exit(-2); 
	// }
	// printf("Socket X attachée avec succès !\n");

	// On fixe la taille de la file d’attente à 5 (pour les demandes de connexion non encore traitées)
	if(listen(socketEcoute, 5) < 0){
   		perror("listen");
   		exit(-3);
	}
	printf("Socket placée en écoute passive ...\n");

    char grille[LIGNES][COLONNES];
    int choixCol = 10;
    int choixLigne = 10;
    initGrille(grille);

	int joueur_actuel = 0;
	// boucle d’attente de connexion : en théorie, un serveur attend indéfiniment !
	while(1){
		memset(messageRecu, 0x00, LG_MESSAGE*sizeof(char));
		printf("Attente d’une demande de connexion (quitter avec Ctrl-C)\n\n");


		// c’est un appel bloquant
		connectSocket[joueur_actuel] = accept(socketEcoute, (struct sockaddr *)&pointDeRencontreDistant, &longueurAdresse);
		if (connectSocket[joueur_actuel] < 0) {
   			perror("accept");
			close(connectSocket[joueur_actuel]);
   			exit(-4);
		}
		joueur_actuel = joueur_actuel + 1;
		// On commence dès qu'on a deux joueurs
		if (joueur_actuel == 2){
			int tour = 0;
			char joueurJouer = 'O';
			char joueurEnFace = 'X';
			char temp = 'Z';
			joueur_actuel = 0;
			autre = 1;
			while(1) {
				switch(ecrits = write(connectSocket[joueur_actuel], &messageEnvoi, sizeof(messageEnvoi))){
					case -1 : /* une erreur ! */
						perror("write");
						close(connectSocket[joueur_actuel]);
						exit(-6);
					case 0 :  /* la socket est fermée */
						fprintf(stderr, "La socket a été fermée par le client !\n\n");
						close(connectSocket[joueur_actuel]);
						return 0;
					default:  /* envoi de n octets */
						printf("Serveur : Message %s envoyé (%d octets)\n\n", messageEnvoi, ecrits);

						char c1 = 'X';
						char c2 = 'O';
						write(connectSocket[autre], &messageEnvoi, sizeof(messageEnvoi));
						write(connectSocket[0],&c1,sizeof(c1));
						write(connectSocket[1],&c2,sizeof(c2));
						while (1){
							temp = joueurEnFace;
							joueurEnFace = joueurJouer;
							joueurJouer = temp;
							if (joueur_actuel == 0){
								autre = 1;
							} else {
								autre = 0;
							}
							printf("---------->   %d,%d",joueur_actuel,autre);
							write(connectSocket[joueur_actuel], attente_non, sizeof(attente_non));
							write(connectSocket[autre], attente, sizeof(attente));
							// printf("Joueur en non-attente : %d\n",joueur_actuel);
							// printf("Joueur en attente : %d\n",aut²re);
							// On réception les données du client (cf. protocole)
							switch(lus = read(connectSocket[joueur_actuel], messageRecu, LG_MESSAGE*sizeof(char))) {
								case -1 : /* une erreur ! */ 
									perror("read"); 
   									close(connectSocket[0]);
									close(connectSocket[1]);
									exit(-5);
								case 0  : /* la socket est fermée */
									fprintf(stderr, "La socket a été fermée par le client !\n\n");
									close(connectSocket[0]);
									close(connectSocket[1]);
									return 0;
								default:  /* réception de n octets */
									printf("Retour après jeu du joueur\n");
									printf("Serveur : Message reçu : %d (%d octets)\n\n", messageRecu[0], lus);
									// On envoie des données vers le client (cf. protocole)

									updateGrille(grille,messageRecu[0],messageRecu[1],joueurJouer);
									afficheGrille(grille);

									char fusionJoueur[256] = "";
									if (grillePleine(grille)==-1) {
										strcat(fusionJoueur,&joueurJouer);
										strcat(fusionJoueur,"end");
										strcpy(Message,fusionJoueur);
									} 
									else if (checkWin(grille,joueurJouer)==1) {
										strcat(fusionJoueur,&joueurJouer);
										strcat(fusionJoueur,"wins");
										strcpy(Message,fusionJoueur);
									}
									
									MSGCol[0] = ' ';
									MSGLigne[0] = ' ';
									sprintf(MSGCol,"%d",messageRecu[0]);
									sprintf(MSGLigne,"%d",messageRecu[1]);
									strcat(MSGLigne,MSGCol);
									strcat(MSGLigne,Message);
					
								
									printf("Changement : %c %c", joueurJouer, joueurEnFace);
									

									switch(ecrits = write(connectSocket[joueur_actuel], MSGLigne, sizeof(MSGLigne))){
										case -1 : /* une erreur ! */
											perror("write");
											close(connectSocket[0]);
											close(connectSocket[1]);
											exit(-6);
										case 0 :  /* la socket est fermée */
											fprintf(stderr, "La socket a été fermée par le client !\n\n");
											close(connectSocket[0]);
											close(connectSocket[1]);
											return 0;
										default:  /* envoi de n octets */
											printf("Serveur : Message envoyé à %d (%d octets) \nStatus %s \nCol : %c \nLigne : %c\n\n",joueur_actuel,ecrits,MSGLigne,MSGLigne[0],MSGLigne[1]);
											// On ferme la socket de dialogue et on se replace en attente ...
									}
									
									
									switch(ecrits = write(connectSocket[autre], MSGLigne, sizeof(MSGLigne))){
										case -1 : /* une erreur ! */
											perror("write");
											close(connectSocket[0]);
											close(connectSocket[1]);
											exit(-6);
										case 0 :  /* la socket est fermée */
											fprintf(stderr, "La socket a été fermée par le client !\n\n");
											close(connectSocket[0]);
											close(connectSocket[1]);
											return 0;
										default:  /* envoi de n octets */
											printf("Serveur : Message envoyé à %d (%d octets) \nStatus %s \nCol : %c \nLigne : %c\n\n",autre,ecrits,MSGLigne,MSGLigne[0],MSGLigne[1]);
											// On ferme la socket de dialogue et on se replace en attente ...
									}
									if (strcmp(Message,"continue")!=0){
										printf("JE ME FERME\n");
										close(connectSocket[0]);
										close(connectSocket[1]);
										exit(0);
									}
									if (joueur_actuel == 1){
										joueur_actuel = 0;
									} else {
										joueur_actuel = 1;
									}
								}
						}
				}
			}
		}
	}
	// On ferme la ressource avant de quitter
   	close(connectSocket[0]);
	close(connectSocket[1]);
	return 0; 
}