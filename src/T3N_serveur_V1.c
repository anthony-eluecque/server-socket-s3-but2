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
#define PORT 4501 // = 4500 (ports >= 4500 réservés pour usage explicite)

#define LG_MESSAGE 256

struct Message {
	int col;
	int ligne;
	char *status;
} Msg;

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
	int nb;
	int retour;


	char result[LG_MESSAGE];

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
    char grille[LIGNES][COLONNES];
    int choixCol;
    int choixLigne;
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
                            // On envoie des données vers le client (cf. protocole)
                            updateGrille(grille,messageRecu[0],messageRecu[1],'O');


							if (grillePleine(grille)==-1) {
								Msg.status = "Xend";
							} else  {
								if (checkWin(grille,'X')==1) {
									Msg.status = "Xwins";
								} else {
									choixLigne = 10;
                            		choixCol = 10;
									while ((isInGrille(grille,choixLigne,choixCol)==-1) && (isEmpty(grille,choixLigne,choixCol)==-1)) {
										choixLigne = rand() % 3;
										choixCol = rand() % 3;
									}   
									updateGrille(grille,choixLigne,choixCol,'X');

									afficheGrille(grille);

									if (checkWin(grille,'O')==1) {
										Msg.status = "Owins";
									} else {
										if (grillePleine(grille)==-1) {
											Msg.status = "Oend";
										} else {
											Msg.status = "continue";
										}
									}
								}
							}
							Msg.col = choixCol;
							Msg.ligne = choixLigne;

                            switch(ecrits = write(socketDialogue, &Msg, sizeof(Msg))){
                                case -1 : /* une erreur ! */
                                    perror("write");
                                    close(socketDialogue);
                                    exit(-6);
                                case 0 :  /* la socket est fermée */
                                    fprintf(stderr, "La socket a été fermée par le client !\n\n");
                                    close(socketDialogue);
                                    return 0;
                                default:  /* envoi de n octets */
                                    printf("Serveur : Message envoyé (%d octets) \nStatus %s \nCol : %d \nLigne : %d: \n\n",ecrits, Msg.status,Msg.col,Msg.ligne);
                                    // On ferme la socket de dialogue et on se replace en attente ...
							}
							if (strcmp(Msg.status,"continue")!=0){
								printf("JE ME FERME");
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