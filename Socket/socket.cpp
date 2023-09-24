#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>


#include "LibraireSocket.h"


int ServerSocket(int port)
{
	//création socket
	int socket;
	if((socket = socket(AS_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("Erreur de creation de socket.");
		exit(1);
	}
	printf("Création de socket réussie : %d", socket); 

	//construction de l'addresse réseau de la socket
 	struct addrinfo hints;
	struct addrinfo *results;
	memset(&hints,0,sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE | AI_NUMERICSERV; // pour une connexion passive
	if (getaddrinfo(NULL,"50000",&hints,&results) != 0)
	{
		close(socket);
		perror("Echec de la création de l'adr reseau.");
		exit(1);
	} 

	// Liaison de la socket à l'adresse
	if (bind(socket,results->ai_addr,results->ai_addrlen) < 0)
	{
		perror("Erreur de bind()");
		exit(1);
	}

	//libération ? ? 
	freeaddrinfo(results);
	printf("bind() reussi !\n");
	return socket;
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

	// Attente d'une connexion BLOQUANT
	int socketServ;
	if ((socketServ = accept(socket,NULL,NULL)) == -1)
	{
		perror("Erreur de accept()");
		exit(1);
	}

	printf("accept() reussi !");
	printf("socket de service = %d\n",socketServ);

	return socketServ;
}

int ClientSocket(char* ipServeur,int portServeur)
{
	int socket;
	if((socketC = socket(AS_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("Erreur de creation de socket.");
		exit(1);
	}
	printf("Création de socket réussie : %d", socketC); 

	// Construction de l'adresse du serveur
	struct addrinfo hints;
	struct addrinfo *results;
	memset(&hints,0,sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_NUMERICSERV;

	if (getaddrinfo(argv[1],argv[2],&hints,&results) != 0)
	{
		perror("Echec de la création de l'adr du serveur.")
		exit(1);
	}

	// Demande de connexion
	if (connect(socketC,results->ai_addr,results->ai_addrlen) == -1)
	{
		perror("Erreur de connect()");
		exit(1);
	}
	printf("connect() reussi !\n");
	return socketC;

}

int Send(int sSocket,char* data,int taille)
{
	if (taille > TAILLE_MAX_DATA)
		return -1;

	// Preparation de la charge utile
	char trame[TAILLE_MAX_DATA+2];
	memcpy(trame,data,taille);
	trame[taille] = '#';
	trame[taille+1] = ')';

	// Ecriture sur la socket
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
			if (nbLus == 0) return i; // connexion fermee par client

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
	
	return i;
}


