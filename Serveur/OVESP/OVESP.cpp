#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <mysql.h>
#include "OVESP.h"


thread_local MYSQL* connexion;
thread_local char requete[200];
thread_local MYSQL_RES  *resultat;
thread_local MYSQL_ROW  Tuple;

pthread_mutex_t mutexClients = PTHREAD_MUTEX_INITIALIZER;

int nbClients = 0;
int clients[NB_MAX_CLIENTS];

int clientID = -1; 


bool ConnextionBd(char nomTable[20]);
int EstPresent(char * login);
bool CreationDuClient(int socket, char user[50], char password[50]);
bool MotDePasseCorrecte(char login[50], char password[50]);
void TestArticle(int idArticle, char * reponse, bool consult);
int AchatArticle(int idArticle, int quantite, char * reponse);
bool ConcatenationCaddie(char * reponse);
bool CancelArticle(int idArticle);
bool CancelAll(int * tab, int *taille);
int testTableVide();
void HeureActuelle(char *Heure);
bool Montant(float * montant);
bool CreationFacture(int client_ID, char date[11], float montant);
void ajoute(int socket);
void retire(int socket);
int idClient(char login[50], char password[50]);
int vecteurPos(int socket);
float ConvertionFloat(char * chiffre);


//***** Parsing de la requete et creation de la reponse *************
bool OVESP(char* requete , char* reponse ,int socket)
{
    // ***** Récupération nom de la requete *****************
    char* ptr = strtok(requete,"#");

    // ***** LOGIN ******************************************
    if (strcmp(ptr,"LOGIN") == 0) 
    {
        // ***** Format requete : LOGIN#log#MDP#nvclient ***************


        char user[50], password[50], nc[1];
        int nvClient, varidClient;
        strcpy(user,strtok(NULL,"#"));
        strcpy(password,strtok(NULL,"#"));
        strcpy(nc, strtok(NULL, "#"));
        nvClient = atoi(nc);

        printf("\t[THREAD %p] LOGIN de %s\n",pthread_self(),user);

        if(nvClient == 0) //pas nouveau client
        {
            if(EstPresent(user) >= 0)
            {
                printf("Test mot de passe.\n");
                if(MotDePasseCorrecte(user, password))
                {
                    varidClient = idClient(user, password);
                    if(varidClient != -1)
                    {
                        sprintf(reponse,"LOGIN#ok#%d", varidClient);
                        ajoute(socket);
                        clientID = varidClient;
                    }
                    else
                    {
                        sprintf(reponse,"LOGIN#ko#Erreur de recherche client !");
                    }
                }
                else
                {
                    sprintf(reponse,"LOGIN#ko#Mot de passe incorrecte !");
                }
            }
            else
            {
                sprintf(reponse,"LOGIN#ko#Client pas dans la bd !");
                return false;
            }
        }
        else //nouveau client
        {
            if(EstPresent(user) == - 1)
            {
                //ok client pas dans la bd
                
                printf("Client non créer !\n");                
                //creation d'un nouveau client
                if(CreationDuClient(socket, user, password))
                {
                    varidClient = idClient(user, password);
                    if(varidClient != -1)
                    {
                        sprintf(reponse,"LOGIN#ok#%d", varidClient);
                        ajoute(socket);
                    }
                    else
                    {
                        sprintf(reponse,"LOGIN#ko#Erreur de recherche client !");
                    }
                }
                else
                {
                    sprintf(reponse,"LOGIN#ko#Erreur de creation du client !");
                    return false;
                }
            }
            else
            {
                //client dans la bd
                sprintf(reponse,"LOGIN#ko#Client deja dans la bd ! ");
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

    if(strcmp(ptr, "ACHAT") == 0)
    {
        //********* ACHAT#idArticle#quantite

        //test si article présent 

        char idChar1[20];
        strcpy(idChar1,strtok(NULL,"#"));
        int idArt = atoi(idChar1);

        TestArticle(idArt, reponse, false);


        char qttChar[20];
        strcpy(qttChar, strtok(NULL,"#"));
        int qtt = atoi(qttChar);

        //gestion de l'achat dans la bd
        if(AchatArticle(idArt, qtt, reponse) == -1)
        {
            sprintf(reponse,"CONSULT#ko#Erreur dans l'achat !");
        }

    }

    if(strcmp(ptr, "CADDIE") == 0)
    {
        //**************** CADDIE
        if(!ConcatenationCaddie(reponse))
        {
            sprintf(reponse,"CADDIE#ko#Erreur dans le caddie !");
        }
    }

    if(strcmp(ptr, "CANCEL") == 0)
    {
        // ************* CANCEL#idArticle
            
        char idChar2[20];
        strcpy(idChar2,strtok(NULL,"#"));
        int idArt1 = atoi(idChar2);

        if(!CancelArticle(idArt1))
        {
            sprintf(reponse,"CANCEL#ko");
        }
        else
        {
            sprintf(reponse,"CANCEL#ok");
        }
    }

    if(strcmp(ptr, "CANCELALL") == 0)
    {
        // ************* CANCELALL

        //récupération des id des article
        int tab[5];
        int taille;

        if(!CancelAll(tab, &taille))
        {
            fprintf(stderr, "Erreur dans la recuperation de idArticle !\n");
        }

        // on retire les éléments de caddie et on met a jour la table article


        for(int i=0; i<taille; i++)
        {
            if(!CancelArticle(0))
            {
                fprintf(stderr,"Erreur de cancelall !\n");
            }
        }

        //cancelall effectuer correctement

    }

    if(strcmp(ptr, "CONFIRMER") == 0)
    {
        // ************* CONFIRMER

        // récupération de la date actuelle
        char date[11];

        HeureActuelle(date);

        //récupérer le montant totale
        float montant = 0;

        if(!Montant(&montant))
        {
            fprintf(stderr,"Erreur dans le calcule du montant !\n");
        }

        //création de la facture
        if(!CreationFacture(clientID, date, montant))
        {
            fprintf(stderr,"Erreur dans la creation de facture !\n");
        }


    }

    if(strcmp(ptr, "LOGOUT") == 0)
    {
        // ************* LOGOUT
        int nbTupleCaddie = testTableVide();
        if(nbTupleCaddie > 0)
        {
            //table contient des données

            //récupération des id des article
            int tab1[5];
            int taille;

            if(!CancelAll(tab1, &taille))
            {
                fprintf(stderr, "Erreur dans la recuperation de idArticle 1 !\n");
            }

            // on retire les éléments de caddie et on met a jour la table article

            for(int i=0; i<taille; i++)
            {
                if(!CancelArticle(0))
                {
                    fprintf(stderr,"Erreur de cancelall 1 !\n");
                }
            }

            //cancelall effectuer correctement
        }
        if(nbTupleCaddie == 0)
        {
            //table vide, rien à faire
        }
        if(nbTupleCaddie == -1)
        {
            //erreur
            fprintf(stderr,"Erreur de count !\n");
        }
        retire(socket);

        return false;

    }

    return true;
}

int idClient(char login[50], char password[50])
{
    int idClient = -1;
    connexion = mysql_init(NULL);
    
    char nomTable[20];
    strcpy(nomTable, "clients");
    if(!ConnextionBd(nomTable))
    {
        fprintf(stderr, "Erreur de mysql connexion clients: %s\n",mysql_error(connexion));
        return -1;
    }

    while ((Tuple = mysql_fetch_row(resultat)) != NULL)
    {
        if(Tuple != NULL && strcmp(Tuple[1], login) == 0)
        {
            idClient = atoi(Tuple[0]);
            break;
        }
    }

    mysql_close(connexion);
    printf("Id client : %d\n", idClient);
    return idClient;
}

void ajoute(int socket)
{
    pthread_mutex_lock(&mutexClients);
    clients[nbClients] = socket;
    nbClients++;
    pthread_mutex_unlock(&mutexClients);
    printf("socket %d ajoute\n", socket);
}

void retire(int socket)
{
    int pos = vecteurPos(socket);
    if (pos == -1) return;
    pthread_mutex_lock(&mutexClients);
    for (int i=pos ; i<=nbClients-2 ; i++)
    clients[i] = clients[i+1];
    nbClients--;
    pthread_mutex_unlock(&mutexClients);
}

int vecteurPos(int socket)
{
 int indice = -1;
 pthread_mutex_lock(&mutexClients);
 for(int i=0 ; i<nbClients ; i++)
 if (clients[i] == socket) { indice = i; break; }
 pthread_mutex_unlock(&mutexClients);
 return indice;
}

bool ConnextionBd(char nomTable[20])
{
    connexion = mysql_init(NULL);
    if (mysql_real_connect(connexion,"localhost","Student","PassStudent1_","PourStudent",0,0,0) == NULL)
    {
        fprintf(stderr,"(OVESP) Erreur de connexion à la base de données...\n");
        mysql_close(connexion);
        return false;
    }

    strcpy(requete,"select * from ");
    strcat(requete, nomTable);

    if(mysql_query(connexion,requete) != 0)
    {
        fprintf(stderr, "Erreur de mysql_query: %s\n",mysql_error(connexion));
        mysql_close(connexion);
        return false;
    }

    if((resultat = mysql_store_result(connexion))==NULL)
    {
        fprintf(stderr, "Erreur de mysql_store_result: %s\n",mysql_error(connexion));
        mysql_close(connexion);
        return false;
    }
    return true;
}


int EstPresent(char * login)
{
    int indice = -1;

    char nomTable[20];
    strcpy(nomTable, "clients");
    if(!ConnextionBd(nomTable))
    {
        fprintf(stderr, "Erreur de mysql connexion clients: %s\n",mysql_error(connexion));
        exit(1);
    }


    while ((Tuple = mysql_fetch_row(resultat)) != NULL)
    {
        if(Tuple != NULL && (strcmp(Tuple[1], login) == 0))
        {
            indice = atoi(Tuple[0]); break;
        }
    }

    mysql_close(connexion);
    return indice;
}

bool MotDePasseCorrecte(char login[50], char password[50])
{
    bool result = false;
    connexion = mysql_init(NULL);
    
    char nomTable[20];
    strcpy(nomTable, "clients");
    if(!ConnextionBd(nomTable))
    {
        fprintf(stderr, "Erreur de mysql connexion clients: %s\n",mysql_error(connexion));
        return false;
    }

    while ((Tuple = mysql_fetch_row(resultat)) != NULL)
    {
        if(Tuple != NULL && strcmp(Tuple[1], login) == 0)
        {
            if(strcmp(Tuple[2], password) == 0)
            {
                result = true; break;
            }
        }
    }

    mysql_close(connexion);
    return result;
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
    char nomTable[20];
    strcpy(nomTable, "articles");

    if(!ConnextionBd(nomTable))
    {
        fprintf(stderr, "Erreur de mysql connexion articles: %s\n",mysql_error(connexion));
        exit(1);     
    }

    //trouvé et correct
    while ((Tuple = mysql_fetch_row(resultat)) != NULL)
    {
        if(Tuple != NULL && atoi(Tuple[0]) == idArticle)
        {
            if(consult)
            {
                sprintf(reponse,"CONSULT#ok#%d#%s#%f#%d#%s", atoi(Tuple[0]), Tuple[1], atof(Tuple[2]), atoi(Tuple[3]), Tuple[4]);
            }
            else
            {
                sprintf(reponse,"CONSULT#ok#%d#%s#%f#%d#%s", atoi(Tuple[0]), Tuple[1], atof(Tuple[2]), atoi(Tuple[3]), Tuple[4]);
            }
            break;
        }
    }

    mysql_close(connexion);
}

int AchatArticle(int idArticle, int quantite, char * reponse)
{
    int res = 1;
    bool present = false;
    connexion = mysql_init(NULL);

    char nomTable[20];
    strcpy(nomTable, "articles");

    if(!ConnextionBd(nomTable))
    {
        fprintf(stderr, "Erreur de mysql connexion articles: %s\n",mysql_error(connexion));
        exit(1);     
    }

    while ((Tuple = mysql_fetch_row(resultat)) != NULL)
    {
        if(Tuple != NULL && atoi(Tuple[0]) == idArticle)
        {
            present = true;
            if(atoi(Tuple[3]) < quantite)
            {
                //pas assez de qtt
                sprintf(reponse,"ACHAT#ko#id = %d, quantite = 0, prix = %f", atoi(Tuple[0]),  ConvertionFloat(Tuple[2])); 
            }
            else
            {
                printf("Supression de %d %s\n", quantite, Tuple[1]);
                int stock = atoi(Tuple[3]) - quantite;

                char requeteTmp[256];
                sprintf(requeteTmp, "UPDATE articles SET stock = %d WHERE id = %d", stock, idArticle);

                //sprintf(requeteTmp, "UPDATE articles SET stock = %d WHERE id = %d", 9, 1);
                if (mysql_query(connexion, requeteTmp) == 0)
                {
                    
                    strcpy(nomTable, "articles");

                    if(!ConnextionBd(nomTable))
                    {
                        fprintf(stderr, "Erreur de mysql connexion articles: %s\n",mysql_error(connexion));
                        exit(1);     
                    }

                    while ((Tuple = mysql_fetch_row(resultat)) != NULL)
                    {
                        if(Tuple != NULL && atoi(Tuple[0]) == idArticle)
                        {
                            float prix = ConvertionFloat(Tuple[2]);
                            prix = prix * (float)quantite;


                            printf("Quantité mise à jour avec succès.\n");
                            sprintf(reponse,"ACHAT#ok#id = %d, quantite = %d, prix = %f", idArticle, quantite, prix);


                            //sauvegarde dans le caddie

                            sprintf(requeteTmp, "INSERT INTO caddies VALUES (%d, '%s', %d, %f)", idArticle, Tuple[1], quantite, prix);

                            if (mysql_query(connexion, requeteTmp) == 0)
                            {
                                printf("Succès de la sauvegarde dans le caddie.\n");
                            }
                        }
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
    if(!present)
    {
        sprintf(reponse, "ACHAT#ko#-1"); //non trouvé
    }

    mysql_close(connexion);
    return res;
}

bool ConcatenationCaddie(char * reponse)
{
    char nomTable[20];
    strcpy(nomTable, "caddies");

    if(!ConnextionBd(nomTable))
    {
        fprintf(stderr, "Erreur de mysql connexion caddies: %s\n",mysql_error(connexion));
        return false;
    } 

    int i = 0;

    sprintf(reponse, "CADDIE#ok#0");//0 ne change pas atoi
    while ((Tuple = mysql_fetch_row(resultat)) != NULL && i < 5)
    {
        // Allouez de la mémoire pour chaque chaîne de caractères
        char *occurrence = (char *)malloc(256); // Vous pouvez ajuster la taille selon vos besoins

        // Utilisez snprintf pour copier les données dans la mémoire allouée
        sprintf(occurrence, "%d,%s,%d,%f$", atoi(Tuple[0]), Tuple[1], atoi(Tuple[2]), atof(Tuple[3]));


        // Stockez le pointeur vers la mémoire allouée dans le tableau
        strcat(reponse, occurrence);

        i++;
    }

    /*int longueur = strlen (reponse);
    reponse[longueur-1] = '\0';*/

    mysql_close(connexion);
    return true;
}

bool CancelArticle(int indiceArticle)
{
    int qtt = 0, qttstock;

    char nomTable[20];
    strcpy(nomTable, "caddies");

    if(!ConnextionBd(nomTable))
    {
        fprintf(stderr, "Erreur de mysql connexion caddies: %s\n",mysql_error(connexion));
        return false;
    } 
    int i = 0;
    Tuple = mysql_fetch_row(resultat);
    while (i != indiceArticle)
    {
        Tuple = mysql_fetch_row(resultat);
        i++;
    }

    //trouver

    int idASupprimer = atoi(Tuple[0]);
    int quantite = atoi(Tuple[2]);

    //je supprime le tuple
    char requete_delete[256];
    printf("ID : %d", idASupprimer);
    sprintf(requete_delete, "DELETE FROM caddies WHERE idArticle = %d", idASupprimer);


    if(mysql_query(connexion,requete_delete) != 0)
    {
        fprintf(stderr, "Erreur de mysql_query: %s\n",mysql_error(connexion));
        mysql_close(connexion);
        return false;
    }

    sprintf(requete, "SELECT * FROM articles where id = %d", idASupprimer);


    if(mysql_query(connexion,requete) != 0)
    {
        fprintf(stderr, "Erreur de mysql_query: %s\n",mysql_error(connexion));
        mysql_close(connexion);
        return false;
    }

    if((resultat = mysql_store_result(connexion))==NULL)
    {
        fprintf(stderr, "Erreur de mysql_store_result: %s\n",mysql_error(connexion));
        mysql_close(connexion);
        return false;
    }

    Tuple = mysql_fetch_row(resultat);

    qttstock = atoi(Tuple[3]);

    //qtt à mettre dans la table article

    printf("OOOOOOOO %d, %d", qttstock, quantite);
    qttstock = qttstock + quantite;
    
    //maj
    char requete_update[256];
    sprintf(requete_update, "UPDATE articles SET stock = %d WHERE id = %d", qttstock, idASupprimer);

    if(mysql_query(connexion,requete_update) != 0)
    {
        fprintf(stderr, "Erreur de mysql_query: %s\n",mysql_error(connexion));
        mysql_close(connexion);
        return false;
    }


    mysql_close(connexion);
    return true;
}

bool CancelAll(int * tab, int* taille)
{
    char nomTable[20];
    strcpy(nomTable, "caddies");

    if(!ConnextionBd(nomTable))
    {
        fprintf(stderr, "Erreur de mysql connexion caddies: %s\n",mysql_error(connexion));
        return false;
    } 

    int i = 0;
    while ((Tuple = mysql_fetch_row(resultat)) != NULL)
    {
        tab[i] = atoi(Tuple[0]);
        i++;
    }

    *taille = i;

    mysql_close(connexion);
    return true;
}

int testTableVide()
{
    int nombre_enregistrements;

    connexion = mysql_init(NULL);
    if (mysql_real_connect(connexion,"localhost","Student","PassStudent1_","PourStudent",0,0,0) == NULL)
    {
        fprintf(stderr,"(OVESP) Erreur de connexion à la base de données...\n");
        mysql_close(connexion);
        return -1;
    }

    strcpy(requete,"SELECT COUNT(*) FROM caddies");

    if(mysql_query(connexion,requete) != 0)
    {
        fprintf(stderr, "Erreur de mysql_query: %s\n",mysql_error(connexion));
        mysql_close(connexion);
        return -1;
    }

    if((resultat = mysql_store_result(connexion))==NULL)
    {
        if (resultat != NULL) 
        {
            MYSQL_ROW ligne = mysql_fetch_row(resultat);
            nombre_enregistrements = atoi(ligne[0]);
        }
        else
        {
            fprintf(stderr, "Erreur de mysql_store_result !\n");
            mysql_close(connexion);
            return -1;
        }
    }

    mysql_close(connexion);

    return nombre_enregistrements;
}

void HeureActuelle(char *Heure)
{
    // Obtenir la date et l'heure actuelles
    time_t maintenant;
    struct tm *infoTemps;

    time(&maintenant); // Obtenez l'heure actuelle
    infoTemps = localtime(&maintenant); // Convertissez-le en une structure tm

    // Formatez la date en tant que chaîne au format DD/MM/YYYY
    snprintf(Heure, sizeof(Heure), "%02d/%02d/%04d", infoTemps->tm_mday, infoTemps->tm_mon + 1, infoTemps->tm_year + 1900);

    // Affichez la chaîne de date formatée
    printf("Date actuelle : %s\n", Heure);
}

bool Montant(float * montant)
{
    float tmp = 0;
    printf("MONTANT TEST\n\n");

    char nomTable[20];
    strcpy(nomTable, "caddies");

    if(!ConnextionBd(nomTable))
    {
        fprintf(stderr, "Erreur de mysql connexion caddies: %s\n",mysql_error(connexion));
        return false;
    } 

    int i = 0;
    while ((Tuple = mysql_fetch_row(resultat)) != NULL && i < 5)
    {
        tmp = atof(Tuple[2]) * atof(Tuple[3]);
        *montant = *montant + tmp;
        i++;
        printf("MONTANT = %f\n\n",*montant);
    }

    mysql_close(connexion);
    return true;
}

bool CreationFacture(int client_ID, char date[11], float montant)
{
    connexion = mysql_init(NULL);
    if (mysql_real_connect(connexion,"localhost","Student","PassStudent1_","PourStudent",0,0,0) == NULL)
    {
        fprintf(stderr,"(OVESP) Erreur de connexion à la base de données...\n");
        mysql_close(connexion);
        return false;
    }


    sprintf(requete,"insert into factures values (NULL, %d,'%s', %f, %d);",client_ID, date, montant, 0);
    //sprintf(requete, "INSERT INTO factures (id,idClient, date, montant, paye) VALUES (NULL,%d, '%s', %f, %d);", client_ID, date, montant, 0);
    if(!mysql_query(connexion,requete))
    {
        fprintf(stderr,"(OVESP) Erreur de mysql_query...\n");
        mysql_close(connexion);
        return false;
    }

    mysql_close(connexion);
    return true;
}
float ConvertionFloat(char * chiffre)
{
    // int longueur = strlen(chiffre);

    // for (int i = 0; i < longueur; i++) 
    // {
    //     if(chiffre[i] == '.') 
    //     {
    //         chiffre[i] = ','; 
    //     }
    // }
    float prix = atof(chiffre);
    printf("chiffre : %s\n", chiffre);
    printf("prix encore : %f\n", prix);
    return prix;
}
