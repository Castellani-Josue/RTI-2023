#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <mysql.h>
#include "OVESP.h"

int nbClients = 0;
MYSQL* connexion;
char requete[200];
MYSQL_RES  *resultat;
MYSQL_ROW  Tuple;

int EstPresent(int socket);
bool CreationDuClient(char user[50], char password[50]);
bool MotDePasseCorrecte(char login[50], char password[50]);
void TestArticle(int idArticle, char * reponse, bool consult);
int AchatArticle(int idArticle, int quantite, char * reponse);
bool ConcatenationCaddie(char * reponse);


//***** Parsing de la requete et creation de la reponse *************
bool OVESP(char* requete , char* reponse ,int socket )
{
    // ***** Récupération nom de la requete *****************
    char* ptr = strtok(requete,"#");

    // ***** LOGIN ******************************************
    if (strcmp(ptr,"LOGIN") == 0) 
    {
        // ***** Format requete : LOGIN#log#MDP ***************


        char user[50], password[50];
        strcpy(user,strtok(NULL,"#"));
        strcpy(password,strtok(NULL,"#"));
        printf("\t[THREAD %p] LOGIN de %s\n",pthread_self(),user);
        if(EstPresent(socket) == - 1)
        {
            printf("Client non créer !\n");

            //creation d'un nouveau client
            if(CreationDuClient(user, password))
            {
                sprintf(reponse,"LOGIN#ok#Client créer ! id : %d", socket); //ici on demande l'id client mais si il est égual a la socket je pense que c bon
            }
            else
            {
                sprintf(reponse,"LOGIN#ko#Erreur de creation du client !");
                exit(1);
            }
        }
        else
        {
            printf("Test mot de passe.\n");
            if(MotDePasseCorrecte(user, password))
            {
                sprintf(reponse,"LOGIN#ok#Mot de passe correcte, connexion ! id : %d", socket);
            }
            else
            {
                sprintf(reponse,"LOGIN#ko#Mot de passe incorrecte !");
                exit(1);
            }
        }
    }

    if(strcmp(ptr,"CONSULT") == 0)
    {
        // ********** CONSULT#idArticle

        strcpy(reponse, "CONSULT#ko#-1"); //non trouvé par défaut

        char idChar[20];
        strcpy(idChar,strtok(NULL,"#"));
        int id = atoi(idChar);

        TestArticle(id, reponse, true);
    }

    if(strcmp(ptr, "ACHAT"))
    {
        //********* ACHAT#idArticle#quantite

        //test si article présent 

        strcpy(reponse, "ACHAT#ko#-1"); //non trouvé par défaut

        char idChar1[20];
        strcpy(idChar1,strtok(NULL,"#"));
        int idArt = atoi(idChar1);

        TestArticle(idArt, reponse, false);

        if(strcmp(reponse, "ACHAT#ko#-1") == 0) //n'est pas présent
            exit(1);


        char qttChar[20];
        strcpy(qttChar, strtok(NULL,"#"));
        int qtt = atoi(qttChar);

        //gestion de l'achat dans la bd
        if(AchatArticle(idArt, qtt, reponse) == -1)
        {
            sprintf(reponse,"CONSULT#ko#Erreur dans l'achat !");
            exit(1);
        }

    }

    if(strcmp(ptr, "CADDIE"))
    {
        //**************** CADDIE
        if(!ConcatenationCaddie(reponse))
        {
            sprintf(reponse,"CADDIE#ko#Erreur dans le caddie !");
            exit(1);
        }
    }

        

    return true;

}

int EstPresent(int socket)
{
    int indice = -1;

    connexion = mysql_init(NULL);
    if (mysql_real_connect(connexion,"localhost","Student","PassStudent1_","PourStudent",0,0,0) == NULL)
    {
        fprintf(stderr,"(OVESP) Erreur de connexion à la base de données...\n");
        exit(1);  
    }

    strcpy(requete,"select * from clients");
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

    mysql_close(connexion);
    return indice;
}

bool MotDePasseCorrecte(char login[50], char password[50])
{
    bool resultat = false;
    connexion = mysql_init(NULL);

    if (mysql_real_connect(connexion,"localhost","Student","PassStudent1_","PourStudent",0,0,0) == NULL)
    {
        fprintf(stderr,"(OVESP) Erreur de connexion à la base de données...\n");
        exit(1);  
    }

    strcpy(requete,"select * from clients");
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
        if(Tuple != NULL && strcmp(Tuple[1], login) == 0)
        {
            if(strcmp(Tuple[2], password) == 0)
            {
                resultat = true; break;
            }
        }
    }

    mysql_close(connexion);
    return resultat;
}

bool CreationDuClient(int socket, char user[50], char password[50])
{
    bool resultat = true;
    connexion = mysql_init(NULL);

    if (mysql_real_connect(connexion,"localhost","Student","PassStudent1_","PourStudent",0,0,0) == NULL)
    {
        fprintf(stderr,"(OVESP) Erreur de connexion à la base de données...\n");
        resultat = false;
    }
    else
    {
        sprintf(requete,"insert into clients values (NULL,'%s','%s');", user, password);
        if(mysql_query(connexion,requete) != 0)
        {
            fprintf(stderr, "Erreur de mysql_query: %s\n",mysql_error(connexion));
            resultat = false;
        }
    }

    mysql_close(connexion);
    return resultat;
}

void TestArticle(int idArticle, char * reponse, bool consult)
{

    connexion = mysql_init(NULL);

    if (mysql_real_connect(connexion,"localhost","Student","PassStudent1_","PourStudent",0,0,0) == NULL)
    {
        fprintf(stderr,"(OVESP) Erreur de connexion à la base de données...\n");
        exit(1);
    }
    
    strcpy(requete,"select * from articles");
    if(mysql_query(connexion,requete) != 0)
    {
        fprintf(stderr, "Erreur de mysql_query: %s\n",mysql_error(connexion));
        exit(1);
    }

    //trouvé et correct
    while ((Tuple = mysql_fetch_row(resultat)) != NULL)
    {
        if(Tuple != NULL && atoi(Tuple[0]) == idArticle)
        {
            if(consult)
            {
                sprintf(reponse,"CONSULT#ok#id = %d, intitule = '%s', prix = %f, stock = %d, image = '%s'", Tuple[0], Tuple[1], Tuple[2], Tuple[3], Tuple[4]);
            }
            else
            {
                sprintf(reponse,"ACHAT#ok#id = %d, intitule = '%s', prix = %f, stock = %d, image = '%s'", Tuple[0], Tuple[1], Tuple[2], Tuple[3], Tuple[4]);
            }
            break;
        }
    }

    mysql_close(connexion);
}

int AchatArticle(int idArticle, int quantite, char * reponse)
{
    int res = 1;
    connexion = mysql_init(NULL);

    if (mysql_real_connect(connexion,"localhost","Student","PassStudent1_","PourStudent",0,0,0) == NULL)
    {
        fprintf(stderr,"(OVESP) Erreur de connexion à la base de données...\n");
        exit(1);
    }
    
    strcpy(requete,"select * from articles");
    if(mysql_query(connexion,requete) != 0)
    {
        fprintf(stderr, "Erreur de mysql_query: %s\n",mysql_error(connexion));
        exit(1);
    }

    while ((Tuple = mysql_fetch_row(resultat)) != NULL)
    {
        if(Tuple != NULL && atoi(Tuple[0]) == idArticle)
        {
            if(Tuple[3] <= quantite)
            {
                //pas assez de qtt
                sprintf(reponse,"ACHAT#ok#id = %d, quantite = 0, prix = %f", Tuple[0],  Tuple[2]); 
            }
            else
            {
                printf("Supression de %d %s\n", quantite, Tuple[1]);
                Tuple[3] = Tuple[3] - quantite;

                char requeteTmp[256];
                strcpy(requeteTmp, "UPDATE articles SET qtt = %d WHERE id = %d", Tuple[3], idArticle);

                if (mysql_query(connexion, requeteTmp) == 0)
                {
                    printf("Quantité mise à jour avec succès.\n");
                    sprintf(reponse,"ACHAT#ok#id = %d, quantite = %f, prix = %f", Tuple[0], Tuple[3],Tuple[2]);


                    //sauvegarde dans le caddie

                    strcpy(requeteTmp, "INSERT INTO caddies VALUES (%d, '%s', %d, %f)", idArticle, Tuple[1], quantite, Tuple[2]);

                    if (mysql_query(connexion, requeteTmp) == 0)
                    {
                        printf("Succès de la sauvegarde dans le caddie.\n");
                    }

                } 
                else 
                {
                    printf("Erreur lors de la mise à jour de la quantité : %s\n", mysql_error(connexion));
                    res = -1;
                }
            }
            break;
        }
    }

    mysql_close(connexion);
    return res;
}

bool ConcatenationCaddie(char * reponse)
{
    bool resultat = false;

    connexion = mysql_init(NULL);

    if (mysql_real_connect(connexion,"localhost","Student","PassStudent1_","PourStudent",0,0,0) == NULL)
    {
        fprintf(stderr,"(OVESP) Erreur de connexion à la base de données...\n");
        exit(1);
    }
    
    strcpy(requete,"select * from caddie");
    if(mysql_query(connexion,requete) != 0)
    {
        fprintf(stderr, "Erreur de mysql_query: %s\n",mysql_error(connexion));
        exit(1);
    }   

    int i = 0;

    sprintf(reponse, "CADDIE#ok#");
    while ((Tuple = mysql_fetch_row(resultat)) != NULL && i < 5)
    {
        // Allouez de la mémoire pour chaque chaîne de caractères
        char *occurrence = malloc(256); // Vous pouvez ajuster la taille selon vos besoins

        // Utilisez snprintf pour copier les données dans la mémoire allouée
        sprintf(occurrence, "%d, %s, %d, %f\n", atoi(Tuple[0]), Tuple[1], atoi(Tuple[2]), atof(Tuple[3]));


        // Stockez le pointeur vers la mémoire allouée dans le tableau
        strcat(reponse, occurrence);

        i++;
    }

    resultat = true;
    return resultat;
}

