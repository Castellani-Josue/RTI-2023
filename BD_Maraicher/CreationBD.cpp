#include <stdio.h>
#include <stdlib.h>
#include <mysql.h>
#include <time.h>
#include <string.h>
#include <time.h>

typedef struct
{
  int   id;
  char  intitule[20];
  float prix;
  int   stock;  
  char  image[20];
} ARTICLE;

typedef struct
{
  int id;
  char login[50];
  char password[50];
} CLIENTS;

typedef struct
{
  int id;
  int idClient;
  char date[11];
  float montant;
  bool paye;
} FACTURES;

typedef struct
{
  int idFacture;
  int idArticle;
  int quantite;
} VENTES;

ARTICLE ElmA[] = 
{
  {-1,"carottes",2.16f,9,"carottes.jpg"},
  {-1,"cerises",9.75f,8,"cerises.jpg"},
  {-1,"artichaut",1.62f,15,"artichaut.jpg"},
  {-1,"bananes",2.6f,8,"bananes.jpg"},
  {-1,"champignons",10.25f,4,"champignons.jpg"},
  {-1,"concombre",1.17f,5,"concombre.jpg"},
  {-1,"courgette",1.17f,14,"courgette.jpg"},
  {-1,"haricots",10.82f,7,"haricots.jpg"},
  {-1,"laitue",1.62f,10,"laitue.jpg"},
  {-1,"oranges",3.78f,23,"oranges.jpg"},
  {-1,"oignons",2.12f,4,"oignons.jpg"},
  {-1,"nectarines",10.38f,6,"nectarines.jpg"},
  {-1,"peches",8.48f,11,"peches.jpg"},
  {-1,"poivron",1.29f,13,"poivron.jpg"},
  {-1,"pommes de terre",2.17f,25,"pommesDeTerre.jpg"},
  {-1,"pommes",4.00f,26,"pommes.jpg"},
  {-1,"citrons",4.44f,11,"citrons.jpg"},
  {-1,"ail",1.08f,14,"ail.jpg"},
  {-1,"aubergine",1.62f,17,"aubergine.jpg"},
  {-1,"echalotes",6.48f,13,"echalotes.jpg"},
  {-1,"tomates",5.49f,22,"tomates.jpg"}
};

CLIENTS ElmC[] =
{
  {-1, "Josue_08", "jojo17"},
  {-1, "Cycrolle", "cycy18"}
};

int main(int argc,char *argv[])
{
  // Connection a MySql
  printf("Connection a la BD...\n");
  MYSQL* connexion = mysql_init(NULL);
  mysql_real_connect(connexion,"localhost","Student","PassStudent1_","PourStudent",0,0,0);

  printf("Creation de la table articles...\n");
  mysql_query(connexion,"drop table articles;"); // au cas ou elle existerait deja
  mysql_query(connexion,"create table articles (id INT(4) auto_increment primary key, intitule varchar(20),prix FLOAT(4),stock INT(4),image varchar(20));");

  printf("Creation de la table clients...\n");
  mysql_query(connexion,"drop table clients;"); // au cas ou elle existerait deja
  mysql_query(connexion,"create table clients (id INT(4) auto_increment primary key, login varchar(50), password varchar(50));");

  printf("Creation de la table factures...\n");
  mysql_query(connexion,"drop table factures;"); // au cas ou elle existerait deja
  mysql_query(connexion,"create table factures (id INT(4) auto_increment primary key, idClient INT(4), date varchar(11), montant FLOAT(4), paye BOOLEAN);");

  printf("Creation de la table ventes... \n");
  mysql_query(connexion,"drop table ventes;"); // au cas ou elle existerait deja
  mysql_query(connexion,"create table ventes (idFacture INT(4), idArticle INT(4), quantite INT(4));");


  printf("Ajout de 2 articles la table clients...\n");
  char requete1[256];
  for (int i=0 ; i<2 ; i++)
  {
    sprintf(requete1,"insert into clients values (NULL,'%s','%s');",ElmC[i].login,ElmC[i].password);
    mysql_query(connexion,requete1);
  }

  printf("Ajout de 21 articles la table articles...\n");
  char requete[256];
  for (int i=0 ; i<21 ; i++)
  {
	  sprintf(requete,"insert into articles values (NULL,'%s',%f,%d,'%s');",ElmA[i].intitule,ElmA[i].prix,ElmA[i].stock,ElmA[i].image);
	  mysql_query(connexion,requete);
  }

  // Deconnection de la BD
  mysql_close(connexion);
  exit(0);
}
