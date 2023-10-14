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

pthread_key_t cleClientID;
int varidClient = -1;

typedef struct {
    int qtt;
    int idArticle;
} ARTICLESAVE;



bool ConnextionBd(char nomTable[20]);
int EstPresent(char * login);
bool CreationDuClient(int socket, char user[50], char password[50]);
bool MotDePasseCorrecte(char login[50], char password[50]);
void TestArticle(int idArticle, char * reponse, bool consult);
int AchatArticle(int idArticle, int quantite, char * reponse);
bool ConcatenationCaddie(char * reponse);
bool CancelArticle(int idArticle);
int testTableVide();
void HeureActuelle(char *Heure);
bool Montant(float * montant);
bool CreationFacture(char date[11], float montant);
void ajoute(int socket);
void retire(int socket);
int idClient(char login[50], char password[50]);
int vecteurPos(int socket);
float ConvertionFloat(char * chiffre);
bool ViderCaddie();
bool LesiIDuCaddie(char * reponse);
bool ArticlePresent(char * reponse, int id);


//***** Parsing de la requete et creation de la reponse *************
bool OVESP(char* requete , char* reponse ,int socket)
{
    // ***** Récupération nom de la requete *****************
    char* ptr = strtok(requete,"#");

    if(strcmp(ptr, "LOGIN") != 0) {
        printf("Debut : %d\n", *(int *)pthread_getspecific(cleClientID));
    }

    // ***** LOGIN ******************************************
    if (strcmp(ptr,"LOGIN") == 0) 
    {
        // ***** Format requete : LOGIN#log#MDP#nvclient ***************
        pthread_key_create(&cleClientID, NULL);

        char user[50], password[50], nc[1];
        int nvClient;
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
                        pthread_setspecific(cleClientID, (void*)&varidClient);
                        printf("ID : %d\n", *(int *)pthread_getspecific(cleClientID));
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
                        pthread_setspecific(cleClientID, (void*)&varidClient);
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

        printf("ID : %d\n", *(int *)pthread_getspecific(cleClientID));

        strcpy(reponse, "CONSULT#ko#-1"); //non trouvé par défaut

        char idChar[20];
        strcpy(idChar,strtok(NULL,"#"));
        int id = atoi(idChar);

        TestArticle(id, reponse, true);
    }

    if(strcmp(ptr, "ACHAT") == 0)
    {
        //********* ACHAT#idArticle#quantite

        char idChar1[20];
        strcpy(idChar1,strtok(NULL,"#"));
        int idArt = atoi(idChar1);

        // TestArticle(idArt, reponse, false);


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
        int taille = testTableVide();

        // on retire les éléments de caddie et on met a jour la table article

        printf("Nombre d'article dans le panier : %d\n", taille);


        for(int i=0; i<taille; i++)
        {
            if(!CancelArticle(0))
            {
                fprintf(stderr,"Erreur de cancelall !\n");
                break;
            }
        }

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
        if(!CreationFacture(date, montant))
        {
            fprintf(stderr,"Erreur dans la creation de facture !\n");
        }


        //vider caddie
        if(!ViderCaddie())
        {
            sprintf(reponse,"CONFIRMER#ko#Erreur de suppression de caddie!");
        }
    }

    if(strcmp(ptr, "LOGOUT") == 0)
    {
        // ************* LOGOUT
        int nbTupleCaddie = testTableVide();
        if(nbTupleCaddie > 0)
        {

            for(int i=0; i<nbTupleCaddie; i++)
            {
                if(!CancelArticle(0))
                {
                    fprintf(stderr,"Erreur de cancelall 1 !\n");
                    break;
                }
            }

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

        pthread_key_delete(cleClientID);

        return false;

    }

    if(strcmp(ptr, "IDCADDIE") == 0)
    {
        // ************* IDCADDIE
        if(!LesiIDuCaddie(reponse))
        {
            fprintf(stderr,"Erreur dans la fonction de récupération des id du caddies !\n");
        }
    }

    if(strcmp(ptr, "PRESENCECADDIE") == 0)
    {
        // ************* PRESENCECADDIE#idArticle

        char idArtDansCaddie[20];
        strcpy(idArtDansCaddie,strtok(NULL,"#"));
        int idArticleDansCaddie = atoi(idArtDansCaddie);

        if(!ArticlePresent(reponse, idArticleDansCaddie))
        {
            fprintf(stderr,"Erreur dans la fonction de récupération de teste de presence dans le caddies !\n");
        }
    }
    return true;
}

bool ArticlePresent(char * reponse, int id)
{
    sprintf(reponse, "PRESENCECADDIE#ko");
    connexion = mysql_init(NULL);
    char nomTable[20];
    strcpy(nomTable, "caddies");

    char *tmp = (char *)malloc(256);
    if(!ConnextionBd(nomTable))
    {
        fprintf(stderr, "Erreur de mysql connexion clients: %s\n",mysql_error(connexion));
        return false;
    }

    while((Tuple = mysql_fetch_row(resultat)) != NULL)
    {
        if(atoi(Tuple[4]) == *(int *)pthread_getspecific(cleClientID) && atoi(Tuple[0]) == id)
        {
            sprintf(reponse, "PRESENCECADDIE#ok");
            break;
        }
    }

    return true;
}

bool LesiIDuCaddie(char * reponse)
{
    connexion = mysql_init(NULL);
    char nomTable[20];
    strcpy(nomTable, "caddies");

    char *tmp = (char *)malloc(256);
    if(!ConnextionBd(nomTable))
    {
        fprintf(stderr, "Erreur de mysql connexion clients: %s\n",mysql_error(connexion));
        return false;
    }

    strcpy(tmp, "IDCADDIE#ok#");
    while((Tuple = mysql_fetch_row(resultat)) != NULL)
    {
        if(atoi(Tuple[4]) == *(int *)pthread_getspecific(cleClientID))
        {
            strcat(tmp, Tuple[0]);
            strcat(tmp, "#");
        }
    }

    int longueur = strlen(tmp);
    tmp[longueur] = '\0';

    strcpy(reponse, tmp);
    mysql_close(connexion);

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
                sprintf(reponse,"ACHAT#ok#%d#%s#%f#%d#%s", atoi(Tuple[0]), Tuple[1], atof(Tuple[2]), atoi(Tuple[3]), Tuple[4]);
            }
            break;
        }
    }

    mysql_close(connexion);
}

int AchatArticle(int idArticle, int quantite, char * reponse)
{
    int res = 1;
    bool present = false, doublon = false;
    connexion = mysql_init(NULL);

    char nomTable[20];
    strcpy(nomTable, "caddies");

    if(!ConnextionBd(nomTable))
    {
        fprintf(stderr, "Erreur de mysql connexion articles: %s\n",mysql_error(connexion));
        exit(1);     
    }

    while ((Tuple = mysql_fetch_row(resultat)) != NULL)
    {
        if(atoi(Tuple[0]) == idArticle && *(int *)pthread_getspecific(cleClientID) == atoi(Tuple[4]))
        {
            doublon = true; //déjà présent => juste modifié la qtt
        }
    }

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


                float prix = ConvertionFloat(Tuple[2]);
                prix = prix * (float)quantite;

                char requeteTmp[256];
                sprintf(requeteTmp, "UPDATE articles SET stock = %d WHERE id = %d", stock, idArticle);

                //sprintf(requeteTmp, "UPDATE articles SET stock = %d WHERE id = %d", 9, 1);
                if (mysql_query(connexion, requeteTmp) == 0)
                {

                    printf("Quantité mise à jour avec succès.\n");
                    sprintf(reponse,"ACHAT#ok#id = %d, quantite = %d, prix = %f", idArticle, quantite, prix);

                    if(!doublon)
                    {
                        sprintf(requeteTmp, "INSERT INTO caddies VALUES (%d, '%s', %d, %f, %d)", idArticle, Tuple[1], quantite, prix, *(int *)pthread_getspecific(cleClientID));
                        printf("requete : %s\n", requeteTmp);                    }
                    else
                    {
                        int stockcaddies = 0;
                        sprintf(requeteTmp, "SELECT quantite FROM caddies WHERE idArticle = %d AND idClient = %d", idArticle, *(int *)pthread_getspecific(cleClientID));
                        if (mysql_query(connexion, requeteTmp) != 0)
                        {
                            fprintf(stderr, "Erreur de mysql_query: %s\n",mysql_error(connexion));
                            mysql_close(connexion);
                            return -1;
                        }

                        MYSQL_RES *resultat = mysql_store_result(connexion);
                        if (resultat == NULL) {
                            fprintf(stderr, "Erreur de mysql_store_result : %s\n", mysql_error(connexion));
                            mysql_close(connexion);
                            exit(1);
                        }

                        MYSQL_ROW ligne = mysql_fetch_row(resultat);
                        if (ligne != NULL) {
                            stockcaddies = atoi(ligne[0]);
                        }

                        stockcaddies = stockcaddies + quantite;
                        sprintf(requeteTmp, "UPDATE caddies SET quantite = %d WHERE idArticle = %d AND idClient = %d", stockcaddies, idArticle, *(int *)pthread_getspecific(cleClientID));
                        printf("requete : %s\n", requeteTmp);
                    }

                    //sauvegarde dans le caddie
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
        if(atoi(Tuple[4])  == * (int *)pthread_getspecific(cleClientID))
        {
            // Allouez de la mémoire pour chaque chaîne de caractères
            char *occurrence = (char *)malloc(256); // Vous pouvez ajuster la taille selon vos besoins

            // Utilisez snprintf pour copier les données dans la mémoire allouée
            sprintf(occurrence, "%d,%s,%d,%f$", atoi(Tuple[0]), Tuple[1], atoi(Tuple[2]), atof(Tuple[3]));


            // Stockez le pointeur vers la mémoire allouée dans le tableau
            strcat(reponse, occurrence);

            i++;
        }
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
    printf("ID : %d\n", idASupprimer);

    sprintf(requete_delete, "DELETE FROM caddies WHERE idArticle = %d AND idClient = %d ORDER BY idArticle LIMIT 1", idASupprimer, *(int *)pthread_getspecific(cleClientID));



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

    printf("OOOOOOOO %d, %d\n", qttstock, quantite);
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

    mysql_free_result(resultat);
    mysql_close(connexion);
    return true;
}

int testTableVide()
{
    int nombre_enregistrements = 0;

    connexion = mysql_init(NULL);
    if (connexion == NULL) {
        fprintf(stderr, "(OVESP) Erreur lors de l'initialisation de la connexion : %s\n", mysql_error(connexion));
        return -1;
    }

    if (mysql_real_connect(connexion, "localhost", "Student", "PassStudent1_", "PourStudent", 0, 0, 0) == NULL) {
        fprintf(stderr, "(OVESP) Erreur de connexion à la base de données : %s\n", mysql_error(connexion));
        mysql_close(connexion);
        return -1;
    }

    sprintf(requete, "SELECT COUNT(*) FROM caddies WHERE idClient = %d", *(int*)pthread_getspecific(cleClientID));

    if (mysql_query(connexion, requete) != 0) {
        fprintf(stderr, "Erreur de mysql_query: %s\n", mysql_error(connexion));
        mysql_close(connexion);
        return -1;
    }
    printf("2 \n");

    MYSQL_RES *resultat = mysql_store_result(connexion);
    if (resultat == NULL) {
        fprintf(stderr, "Erreur de mysql_store_result : %s\n", mysql_error(connexion));
        mysql_close(connexion);
        return -1;
    }

    MYSQL_ROW ligne = mysql_fetch_row(resultat);
    if (ligne != NULL) {
        nombre_enregistrements = atoi(ligne[0]);
    }

    // Libérer la mémoire
    mysql_free_result(resultat);
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
        if(atoi(Tuple[4]) == *(int *)pthread_getspecific(cleClientID))
        {
            printf("%d) %s\n", i, Tuple[3]);

            tmp = atof(Tuple[3]);

            printf("%d) Tmp : %f\n", i, tmp);

            *montant = *montant + tmp;

            printf("%d) Montant : %f\n", i, *montant);

            i++;
        }
    }

    mysql_close(connexion);
    return true;
}

bool CreationFacture( char date[11], float montant)
{
    connexion = mysql_init(NULL);
    if (mysql_real_connect(connexion,"localhost","Student","PassStudent1_","PourStudent",0,0,0) == NULL)
    {
        fprintf(stderr,"(OVESP) Erreur de connexion à la base de données...\n");
        mysql_close(connexion);
        return false;
    }


    sprintf(requete,"insert into factures values (NULL, %d,'%s', %f, %d);",*(int *)pthread_getspecific(cleClientID), date, montant, 0);
    if(mysql_query(connexion,requete) != 0)
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

bool ViderCaddie()
{

    // Exécutez la commande SQL DELETE pour vider la table
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
        if(atoi(Tuple[4]) == *(int *)pthread_getspecific(cleClientID))
        {
            sprintf(requete,"DELETE FROM caddies WHERE idClient = %d",*(int *)pthread_getspecific(cleClientID));
            if (mysql_query(connexion, requete) != 0)
            {
                fprintf(stderr, "Erreur lors de l'exécution de la commande SQL : %s\n", mysql_error(connexion));
                mysql_close(connexion);
                return false;
            }

            i++;
        }
    }

    printf("suppression du caddie effectué avec succès !\n");
    mysql_close(connexion);
    return true;
}

void OVESP_Close()
{
    // ************* CANCELALL

    char nomTable[20];
    strcpy(nomTable, "caddies");

    if(!ConnextionBd(nomTable))
    {
        fprintf(stderr, "Erreur de mysql connexion caddies: %s\n",mysql_error(connexion));
        exit(1);
    } 

    ARTICLESAVE articlesave[250];
    int i = 0;
    while ((Tuple = mysql_fetch_row(resultat)) != NULL)
    {
        articlesave[i].idArticle = atoi(Tuple[0]);
        articlesave[i].qtt = atoi(Tuple[2]);
        //suppression dans caddie

        sprintf(requete, "DELETE FROM caddies WHERE idArticle = %d ORDER BY idArticle LIMIT 1", atoi(Tuple[0]));

        if(mysql_query(connexion,requete) != 0)
        {
            fprintf(stderr,"(OVESP) Erreur de mysql_query... 1\n");
            mysql_close(connexion);
            exit(1);
        }

        i++;
    }
    mysql_close(connexion);

    char requete[256];
    int j = 0;

    connexion = mysql_init(NULL);
    if (mysql_real_connect(connexion,"localhost","Student","PassStudent1_","PourStudent",0,0,0) == NULL)
    {
        fprintf(stderr,"(OVESP) Erreur de connexion à la base de données...\n");
        mysql_close(connexion);
        exit(1);
    }

    int stock = 0;
    while(j < i)
    {
        sprintf(requete, "SELECT stock FROM articles WHERE id = %d", articlesave[j].idArticle);
        printf("Requete : %s\n", requete);

        if(mysql_query(connexion,requete) != 0)
        {
            fprintf(stderr,"(OVESP) Erreur de mysql_query... 2\n");
            mysql_close(connexion);
            exit(1);
        }

        MYSQL_RES *resultat = mysql_store_result(connexion);
        if (resultat == NULL) {
            fprintf(stderr, "Erreur de mysql_store_result : %s\n", mysql_error(connexion));
            mysql_close(connexion);
            exit(1);
        }

        MYSQL_ROW ligne = mysql_fetch_row(resultat);
        if (ligne != NULL) {
            stock = atoi(ligne[0]);
        }

        stock = articlesave[j].qtt + stock;

        sprintf(requete, "UPDATE articles SET stock = %d WHERE id = %d", stock, articlesave[j].idArticle);

        if(mysql_query(connexion,requete) != 0)
        {
            fprintf(stderr,"(OVESP) Erreur de mysql_query... 3\n");
            mysql_close(connexion);
            exit(1);
        }

        j++;
    }


    mysql_close(connexion);

    pthread_mutex_lock(&mutexClients);
    for (int i=0 ; i<nbClients ; i++)
    close(clients[i]);
    pthread_mutex_unlock(&mutexClients);
    
}
