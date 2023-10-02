#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <mysql.h>
#include "OVESP.h"

int clients[NB_MAX_CLIENTS];
int nbClients = 0;


//***** Parsing de la requete et creation de la reponse *************
bool OVESP(char* requete , char* reponse ,int socket )
{
    // ***** Récupération nom de la requete *****************
    char* ptr = strok(requete,"#");

    // ***** LOGIN ******************************************
    if (strcmp(ptr,"LOGIN") == 0) 
    {
        char user[50], password[50];
        strcpy(user,strtok(NULL,"#"));
        strcpy(password,strtok(NULL,"#"));
        printf("\t[THREAD %p] LOGIN de %s\n",pthread_self(),user);
        if(EstPresent(socket) != - 1)
        {
            sprintf(reponse,"LOGIN#ko#Client non loggé !");
            exit(1);
        }
    }


}

int EstPresent(int socket)
{
    MYSQL* connexion;
    char requete[200];
    MYSQL_RES  *resultat;
    MYSQL_ROW  Tuple;
    int indice = -1;

    connexion = mysql_init(NULL);
    if (mysql_real_connect(connexion,"localhost","Student","PassStudent1_","PourStudent",0,0,0) == NULL)
    {
        fprintf(stderr,"(OVESP) Erreur de connexion à la base de données...\n");
        exit(1);  
    }

    strcpy(requete,"select * from UNIX_FINAL");
    if(mysql_query(connexion,requete) != 0)
    {
        fprintf(stderr, "Erreur de mysql_query: %s\n",mysql_error(connexion));
        exit(1);
    }

    if((resultat = mysql_store_result(connexion))==NULL)
    {
        fprintf(stderr, "Erreur de mysql_store_result: %s\n",mysql_error(connexion));
        exit(1);
    }

    while ((Tuple = mysql_fetch_row(resultat)) != NULL)
    {
        if(Tuple != NULL && atoi(Tuple[0]) == socket)
        {
            indice = socket; break;
        }
    }
    return indice;

    

}