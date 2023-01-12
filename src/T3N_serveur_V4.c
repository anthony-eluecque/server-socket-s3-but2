#include <stdio.h>
#include <stdlib.h> /* pour exit */
#include <unistd.h> /* pour read, write, close, sleep */
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h> /* pour memset */
#include <netinet/in.h> /* pour struct sockaddr_in */
#include <arpa/inet.h> /* pour htons et inet_aton */
#include <sys/ioctl.h>
#include <time.h>
#include "./utils/Utils.c"
#include "./utils/Utils_serveur.c"
#include "./utils/Utils_socket_serveur.c"

#define NB_JOUEURS 2
#define COLONNES 3
#define LIGNES 3
#define PORT 5004 // = 5000 (ports >= 5000 réservés pour usage explicite)
#define LG_MESSAGE 256

// --------------------------------//
// 			ELUECQUE ANTHONY       //
// 			CORION GAUTHIER		   //
// --------------------------------//
//   BUT 2 : SAE Système et réseau //

int main(int argc, char *argv[]){

	struct sockaddr_in pointDeRencontreLocal;
	socklen_t longueurAdresse;
	
	int connectSocket[50];
	struct sockaddr_in pointDeRencontreDistant;

	char messageEnvoi[] = "start\0";  /* le message de la couche Application ! */
	char messageRecu[LG_MESSAGE]; /* le message de la couche Application ! */
	
	int socketEcoute,ecrits,lus,nb,retour,autre; 
	/* ecrits & lus = nb d’octets ecrits et lus */
	/* nb =  Octet lu */
	/* retour Message de retour */
	/* autre = L'autre joueur */
	// Partie création serveur
	longueurAdresse = sizeof(pointDeRencontreLocal);
	// Crée un socket de communication
	socketEcoute = createListenSocket();
	// memset sert à faire une copie d'un octet n fois à partir d'une adresse mémoire donnée
	// ici l'octet 0 est recopié longueurAdresse fois à partir de l'adresse &pointDeRencontreLocal
	memset(&pointDeRencontreLocal, 0x00, longueurAdresse); 
	pointDeRencontreLocal.sin_family = PF_INET;
	pointDeRencontreLocal.sin_addr.s_addr = htonl(INADDR_ANY); // attaché à toutes les interfaces locales disponibles
	pointDeRencontreLocal.sin_port = htons(PORT); // = 5000 ou plus
	attachementLocalSocket(socketEcoute,pointDeRencontreLocal,longueurAdresse);
	fixFileSocket(socketEcoute,5);

	// Paramètre du jeu
    char grille[LIGNES][COLONNES]; /* La grille de jeu */
    int choixCol = 10; /* Initialisé à 10 pour permettre de rentrer dans la futur boucle while*/
    int choixLigne = 10; /* Initialisé à 10 pour permettre de rentrer dans la futur boucle while*/
	int joueur_actuel = 0;
	char MSGCol[11]; /* Le choix de la colonne */
	char MSGLigne[11]; /* Le choix de la ligne */
	char attente[LG_MESSAGE] = "attente\0"; /* Message d'attente */
	char attente_non[LG_MESSAGE] = "nonattente\0"; /* Message de non attente */
	char result[LG_MESSAGE]; /* message de résultat */
	char Message[11]; /* Le message d'envoi */
	strcpy(Message,"continue"); /* Copie par défaut du message continue */
    initGrille(grille);
	
	// boucle d’attente de connexion : en théorie, un serveur attend indéfiniment !
	while(1){
		memset(messageRecu, 0x00, LG_MESSAGE*sizeof(char)); // memset sert à faire une copie d'un octet n fois à partir d'une adresse mémoire donnée
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
			char joueurJouer = 'O'; /* L'un des joueurs prendre le role '0'*/
			char joueurEnFace = 'X'; /* L'autre joueur prendre le role 'X' */
			char temp = 'Z'; /* Variable temporaire utilisé pour inversé les deux roles */
			joueur_actuel = 0; /* Connaître le joueur actuel */
			autre = 1; /* L'autre joueur */
			if (fork()){
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
							/* Initialisation et envoi des X et O en fonction de leur position dans le socket */
							char c1 = 'X'; 
							char c2 = 'O';
							sleep(1);
							/* Envoi au joueur qui ne joue pas qu'il peut joeur */
							write(connectSocket[autre], &messageEnvoi, sizeof(messageEnvoi));

							/* Envoi au joueur numéro 1 le symbole */
							write(connectSocket[0],&c1,sizeof(c1));

							/* Attente d'une seconde pour que la transmission se passe bien */
							sleep(1);

							/* Envoi au joueur numéro 2 le symbole */
							write(connectSocket[1],&c2,sizeof(c2));

							/* Boucle de jeu */
							while (1){
								/* Échange des joueurs de leurs symboles (X et O) et leur index (0 ou 1) */
								temp = joueurEnFace;
								joueurEnFace = joueurJouer;
								joueurJouer = temp;
								if (joueur_actuel == 0){
									autre = 1;
								} else {
									autre = 0;
								}
								
								/* Transfert du message au client pour qu'il n'attend pas*/
								write(connectSocket[joueur_actuel], &attente_non, sizeof(attente_non));
								
								/* Attente d'une seconde pour que la transmission se passe bien */
								sleep(1);
								
								/* Transfert du message au client pour qu'il attend */
								write(connectSocket[autre], &attente, sizeof(attente));
								
								
								printf("\nEn attente d'une coordonnée...\n");
								
								/* Réception des coordonnées du client */
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
										
										
										printf("\nServeur : Message reçu : %d (%d octets)\n\n", messageRecu[0], lus);

										/* Mise à jour de la grille */
										updateGrille(grille,messageRecu[0],messageRecu[1],joueurJouer);

										/* Affichage de la grille */
										afficheGrille(grille);

										/* Vérification si la grille est pleine */
										char fusionJoueur[256] = "";
										if (grillePleine(grille)==-1) {
											strcat(fusionJoueur,&joueurJouer);
											strcat(fusionJoueur,"end");
											strcpy(Message,fusionJoueur);
										} 
										/* Vérification si le joueur est gagnant ou non */
										else if (checkWin(grille,joueurJouer)==1) {
											strcat(fusionJoueur,&joueurJouer);
											strcat(fusionJoueur,"wins");
											strcpy(Message,fusionJoueur);
										}
										
										/* Concaténation des coordonnées et du message de renvoi (Xwins,00continue,Owins, Xend...) */
										MSGCol[0] = ' ';
										MSGLigne[0] = ' ';
										sprintf(MSGCol,"%d",messageRecu[1]);
										sprintf(MSGLigne,"%d",messageRecu[0]);
										strcat(MSGLigne,MSGCol);
										strcat(MSGLigne,Message);
						
									
										printf("Changement : %c %c \n", joueurJouer, joueurEnFace);
										
										/* Envoi au joueur qui passe dans la boucle actuellement le changement */
										switch(ecrits = write(connectSocket[joueur_actuel], &MSGLigne, sizeof(MSGLigne))){
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
										sleep(1); /* Permet d'éviter le moindre bug */

										/* Envoi à l'autre joueur le changement pour qu'il mette à jour sa grille chez lui directement */
										switch(ecrits = write(connectSocket[autre], &MSGLigne, sizeof(MSGLigne))){
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
												printf("Serveur : Message envoyé à %d (%d octets) \nStatus %s \nCol : %c \nLigne : %c\n\n",connectSocket[autre],ecrits,MSGLigne,MSGLigne[0],MSGLigne[1]);
				
										}
									
										/* Condition pour savoir si le jeu ne continue pas pour fermer le serveur */
										if (strcmp(Message,"continue")!=0){
											printf("Le seveur vient d'être fermé\n");
											close(connectSocket[0]);
											close(connectSocket[1]);
											exit(0);
										}

										/* Échange des index des joueurs */
										if (joueur_actuel == 1){
											joueur_actuel = 0;
										} else {
											joueur_actuel = 1;
										}
										printf("\n\nAttente d'une nouvelle manche....\n\n");

										/* Attente de 3 secondes pour que la transmission se passe bien */
										sleep(3);
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