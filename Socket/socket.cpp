#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring>


#include "socket.h"

/*struct sockaddr_in {
 sa_family_t sin_family; // Famille d'adresses (AF_INET)
 in_port_t sin_port; // Numéro de port
 struct in_addr sin_addr; // Adresse IP
 char sin_zero[8]; // Remplissage pour alignement
};

struct in_addr {
 in_addr_t s_addr; // Adresse IP en format réseau
};

struct addrinfo {
 int ai_flags;
 int ai_family;
 int ai_socktype;
 int ai_protocol;
 socklen_t ai_addrlen;
 struct sockaddr ai_addr;
 charai_canonname;
 struct addrinfo *ai_next;
};*/

int ServerSocket(int port)
{
	//création socket
	int s;
	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("Erreur de creation de socket.");
		exit(1);
	}
	printf("Création de socket réussie : %d", s); 

	//cheat//

    int option = 1;

    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(int)) < 0)
        perror("setsockopt(SO_REUSEADDR) failed");

    /////////

	//construction de l'addresse réseau de la socket
 	struct addrinfo hints;
	struct addrinfo *results;
	memset(&hints,0,sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE | AI_NUMERICSERV; // pour une connexion passive
	if (getaddrinfo(NULL,"50000",&hints,&results) != 0)
	{
		close(s);
		perror("Echec de la création de l'adr reseau.");
		exit(1);
	} 

	// Liaison de la socket à l'adresse
	if (bind(s,results->ai_addr,results->ai_addrlen) < 0)
	{
		perror("Erreur de bind()");
		exit(1);
	}

	//libération ? ? 
	freeaddrinfo(results);
	printf("bind() reussi !\n");
	return s;
}

int Accept(int socket,char *ipClient)
{

	// Mise à l'écoute de la socket
	if (listen(socket,SOMAXCONN) == -1)
	{
		perror("Erreur de listen()");
		exit(1);
	}
	printf("listen() reussi !\n");
	struct sockaddr adrClient;
	
	socklen_t adrClientLen =sizeof(adrClient); 

	// Attente d'une connexion BLOQUANT
	int socketServ;
	if ((socketServ = accept(socket,&adrClient,&adrClientLen)) == -1)
	{
		perror("Erreur de accept()");
		exit(1);
	}

	printf("accept() reussi !\n");
	printf("socket de service = %d\n",socketServ);

	// Recuperation d'information sur le client connecte

	//char host[NI_MAXHOST];
	char port[NI_MAXSERV];
	//struct sockaddr_in adrClient;
	//struct sockaddr adrClient;
	
	//socklen_t adrClientLen =sizeof(adrClient); 
	
	//getpeername(socketServ,(struct sockaddr*)&adrClient,&adrClientLen);
	
	getnameinfo(&adrClient,adrClientLen,ipClient,NI_MAXHOST,port,NI_MAXSERV,NI_NUMERICSERV | NI_NUMERICHOST);
	/*getnameinfo((struct sockaddr*)&adrClient,adrClientLen,
				host,NI_MAXHOST,
				port,NI_MAXSERV,
				NI_NUMERICSERV | NI_NUMERICHOST);*/

	
	//printf("host : %s\n",*host);
	//strcpy(ipClient, host);
	
	

	printf("Client connecte --> Adresse IP: %s -- Port: %s\n",ipClient, port);

	return socketServ;
}

int ClientSocket(char* ipServeur,int portServeur)
{
	int s;
	if((s = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("Erreur de creation de socket.");
		exit(1);
	}
	printf("Création de socket réussie : %d", s); 

	    //cheat///

    int option = 1;
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(int)) < 0)
        perror("setsockopt(SO_REUSEADDR) failed");


    /////////////
	// Construction de l'adresse du serveur
	struct addrinfo myAddrInfo; // Déclare une variable de type struct addrinfo
	struct addrinfo *results;
	memset(&myAddrInfo,0,sizeof(struct addrinfo));
	myAddrInfo.ai_family = AF_INET;
	myAddrInfo.ai_socktype = SOCK_STREAM;
	myAddrInfo.ai_flags = AI_NUMERICSERV;

	if (getaddrinfo(NULL,"50000",&myAddrInfo,&results) != 0)
	{
		perror("Echec de la création de l'adr du serveur.");
		exit(1);
	}

	// Demande de connexion
	if (connect(s,results->ai_addr,results->ai_addrlen) == -1)
	{
		perror("Erreur de connect()");
		exit(1);
	}
	printf("connect() reussi !\n");
	return s;

}

int Send(int sSocket,char data[],int taille)
{
	if (taille > TAILLE_MAX_DATA)
		return -1;

	//printf("\n TRAME B4 : %s", data);

	//printf("\n TAILLE : %d", taille);

	// Preparation de la charge utile
	char trame[TAILLE_MAX_DATA+2];
	strcpy(trame, data);
	//printf("\n TAILLE : %d", taille);
	//printf("\n TRAME DATA : %s\n\n\n\n", trame);
	strcat(trame, "#");
	strcat(trame, ")");



	// Ecriture sur la socket

	printf("\n TRAME : %s", trame);
	return write(sSocket,trame,taille+2)-2;
}

int Receive(int sSocket,char* data)
{
	bool fini = false;
	int nbLus, i = 0;
	char lu1,lu2;
	
	while(!fini)
	{
		if ((nbLus = read(sSocket,&lu1,1)) == -1)
		return -1;
		if (nbLus == 0) return i; // connexion fermee par client
		if (lu1 == '#')
		{
			if ((nbLus = read(sSocket,&lu2,1)) == -1)
			return -1;
			if (nbLus == 0) 
				return i; // connexion fermee par client

			if (lu2 == ')') fini = true;
			else
			{
				data[i] = lu1;
				data[i+1] = lu2;
				i += 2;
			}
		}
		else
		{
			data[i] = lu1;
			i++;
		}
	}
	data[i] = '\0';
	return i;
}


