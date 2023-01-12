#include <stdio.h>
#include <stdlib.h> /* pour exit */
#include <unistd.h> /* pour read, write, close, sleep */
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h> /* pour memset */
#include <netinet/in.h> /* pour struct sockaddr_in */
#include <arpa/inet.h> /* pour htons et inet_aton */
#include <time.h>

#define LG_MESSAGE 256

void read_char(int connectSocket[],int joueurActuel,char* msg,int longueurMessage)
{
	int id;
	id = read(connectSocket[joueurActuel],msg,longueurMessage);
	if (id == -1){
		perror("write");
		close(connectSocket[joueurActuel]);
		exit(-6);	
	} else if (id == 0){
		close(connectSocket[joueurActuel]);
	}
	printf("\nServeur : Message reçu : %d (%d octets)\n\n", msg[0], id);
}

void write_char(int connectSocket[],int joueurActuel,char* msg,int longueurMessage)
{
	int id;
	printf("Socket : %d",connectSocket[joueurActuel]);
	id = write(connectSocket[joueurActuel], msg, longueurMessage);
	if (id == -1){
		perror("write");
		close(connectSocket[joueurActuel]);
		exit(-6);	
	} else if (id == 0){
		close(connectSocket[joueurActuel]);
	}
	printf("Serveur : Message %s envoyé (%d octets)\n\n", msg, id);
}

void write_int(int connectSocket[],int joueurActuel,char* messageEnvoi,int longueurMessage)
{
	int id;
	printf("Socket : %d",connectSocket[joueurActuel]);
	id = write(connectSocket[joueurActuel], messageEnvoi, longueurMessage);
	if (id == -1){
		perror("write");
		close(connectSocket[joueurActuel]);
		exit(-6);	
	} else if (id == 0){
		close(connectSocket[joueurActuel]);
	}
	printf("Serveur : Message %s envoyé (%d octets)\n\n", messageEnvoi, id);
	printf("\n Valeur ecrits ====> : %d\n",id);
}