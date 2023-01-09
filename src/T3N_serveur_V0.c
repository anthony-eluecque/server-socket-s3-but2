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
	char messageEnvoi[LG_MESSAGE]; /* le message de la couche Application ! */
	char messageRecu[LG_MESSAGE]; /* le message de la couche Application ! */
	int ecrits, lus; /* nb d’octets ecrits et lus */
	int nb;
	int retour;
	char result[80];

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
	memset(&pointDeRencontreLocal, 0x00, longueurAdresse); pointDeRencontreLocal.sin_family = PF_INET;
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
		

        ecrits = write(socketDialogue, "start", strlen("start"));
        switch(ecrits){
            case -1 : /* une erreur ! */
                perror("write");
                close(socketDialogue);
                exit(-6);
            case 0 :  /* la socket est fermée */
                fprintf(stderr, "La socket a été fermée par le client !\n\n");
                close(socketDialogue);
                return 0;
            default:  /* envoi de n octets */
                printf("Message %s envoyé (%d octets)\n\n", messageEnvoi, ecrits);
                // On ferme la socket de dialogue et on se replace en attente ...
        }








		// On réception les données du client (cf. protocole)
		lus = read(socketDialogue, messageRecu, LG_MESSAGE*sizeof(char)); // ici appel bloquant
		switch(lus) {
			case -1 : /* une erreur ! */ 
				perror("read"); 
				close(socketDialogue); 
				exit(-5);
			case 0  : /* la socket est fermée */
				fprintf(stderr, "La socket a été fermée par le client !\n\n");
   				close(socketDialogue);
   				return 0;
			default:  /* réception de n octets */
				printf("Message reçu : %s (%d octets)\n\n", messageRecu, lus);
				// On envoie des données vers le client (cf. protocole)
                updateGrille(grille,messageRecu[0],messageRecu[1],'O');

                choixLigne = 10;
                choixCol = 10;
                while (isInGrille(grille,choixLigne,choixCol)==-1 && isEmpty(grille,choixLigne,choixCol)==-1)
                {
                    int choixLigne;
                    choixLigne = rand() % 3;
                    int choixCol;
                    choixCol = rand() % 3;
                }   
				updateGrille(grille,choixLigne,choixCol,'X');

                char Envoi[] = {choixCol,choixLigne};

				ecrits = write(socketDialogue, Envoi, strlen(Envoi));
				switch(ecrits){
					case -1 : /* une erreur ! */
						perror("write");
						close(socketDialogue);
						exit(-6);
					case 0 :  /* la socket est fermée */
						fprintf(stderr, "La socket a été fermée par le client !\n\n");
						close(socketDialogue);
						return 0;
					default:  /* envoi de n octets */
						printf("Message %s envoyé (%d octets)\n\n", messageEnvoi, ecrits);
						// On ferme la socket de dialogue et on se replace en attente ...
				}
		}
	

	}
	// On ferme la ressource avant de quitter
   	close(socketEcoute);
	return 0; 
}