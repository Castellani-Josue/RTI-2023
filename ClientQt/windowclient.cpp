#include "windowclient.h"
#include "ui_windowclient.h"
#include "../Socket/socket.h"
#include <unistd.h>
#include <QMessageBox>
#include <string>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

extern WindowClient *w;

int sClient;
int idArticleEnCours;
float TotCaddie = 0.0;
bool logged = false;
int nbArticlePanier = 0;

#define REPERTOIRE_IMAGES "/home/student/Bureau/RTI-2023-main/ClientQt/images/"

bool OVESP_Login(char *, char *, int, int);
int compterOccurrences(char *chaine, char caractere);


WindowClient::WindowClient(QWidget *parent) : QMainWindow(parent), ui(new Ui::WindowClient)
{
    ui->setupUi(this);

    // Configuration de la table du panier (ne pas modifer)
    ui->tableWidgetPanier->setColumnCount(3);
    ui->tableWidgetPanier->setRowCount(0);
    QStringList labelsTablePanier;
    labelsTablePanier << "Article" << "Prix à l'unité" << "Quantité";
    ui->tableWidgetPanier->setHorizontalHeaderLabels(labelsTablePanier);
    ui->tableWidgetPanier->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidgetPanier->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidgetPanier->horizontalHeader()->setVisible(true);
    ui->tableWidgetPanier->horizontalHeader()->setDefaultSectionSize(160);
    ui->tableWidgetPanier->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidgetPanier->verticalHeader()->setVisible(false);
    ui->tableWidgetPanier->horizontalHeader()->setStyleSheet("background-color: lightyellow");

    ui->pushButtonPayer->setText("Confirmer achat");
    setPublicite("!!! Bienvenue sur le Maraicher en ligne !!!");
}

WindowClient::~WindowClient()
{
    delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// Fonctions utiles : ne pas modifier /////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::setNom(const char* Text)
{
  if (strlen(Text) == 0 )
  {
    ui->lineEditNom->clear();
    return;
  }
  ui->lineEditNom->setText(Text);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
const char* WindowClient::getNom()
{
  strcpy(nom,ui->lineEditNom->text().toStdString().c_str());
  return nom;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::setMotDePasse(const char* Text)
{
  if (strlen(Text) == 0 )
  {
    ui->lineEditMotDePasse->clear();
    return;
  }
  ui->lineEditMotDePasse->setText(Text);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
const char* WindowClient::getMotDePasse()
{
  strcpy(motDePasse,ui->lineEditMotDePasse->text().toStdString().c_str());
  return motDePasse;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::setPublicite(const char* Text)
{
  if (strlen(Text) == 0 )
  {
    ui->lineEditPublicite->clear();
    return;
  }
  ui->lineEditPublicite->setText(Text);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::setImage(const char* image)
{
  // Met à jour l'image
  char cheminComplet[80];
  sprintf(cheminComplet,"%s%s",REPERTOIRE_IMAGES,image);
  QLabel* label = new QLabel();
  label->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
  label->setScaledContents(true);
  QPixmap *pixmap_img = new QPixmap(cheminComplet);
  label->setPixmap(*pixmap_img);
  label->resize(label->pixmap()->size());
  ui->scrollArea->setWidget(label);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
int WindowClient::isNouveauClientChecked()
{
  if (ui->checkBoxNouveauClient->isChecked()) return 1;
  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::setArticle(const char* intitule,float prix,int stock,const char* image)
{
  ui->lineEditArticle->setText(intitule);
  if (prix >= 0.0)
  {
    char Prix[20];
    sprintf(Prix,"%.2f",prix);
    ui->lineEditPrixUnitaire->setText(Prix);
  }
  else ui->lineEditPrixUnitaire->clear();
  if (stock >= 0)
  {
    char Stock[20];
    sprintf(Stock,"%d",stock);
    ui->lineEditStock->setText(Stock);
  }
  else ui->lineEditStock->clear();
  setImage(image);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
int WindowClient::getQuantite()
{
  return ui->spinBoxQuantite->value();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::setTotal(float total)
{
  if (total >= 0.0)
  {
    char Total[20];
    sprintf(Total,"%.2f",total);
    ui->lineEditTotal->setText(Total);
  }
  else ui->lineEditTotal->clear();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::loginOK()
{
  ui->pushButtonLogin->setEnabled(false);
  ui->pushButtonLogout->setEnabled(true);
  ui->lineEditNom->setReadOnly(true);
  ui->lineEditMotDePasse->setReadOnly(true);
  ui->checkBoxNouveauClient->setEnabled(false);

  ui->spinBoxQuantite->setEnabled(true);
  ui->pushButtonPrecedent->setEnabled(true);
  ui->pushButtonSuivant->setEnabled(true);
  ui->pushButtonAcheter->setEnabled(true);
  ui->pushButtonSupprimer->setEnabled(true);
  ui->pushButtonViderPanier->setEnabled(true);
  ui->pushButtonPayer->setEnabled(true);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::logoutOK()
{
  ui->pushButtonLogin->setEnabled(true);
  ui->pushButtonLogout->setEnabled(false);
  ui->lineEditNom->setReadOnly(false);
  ui->lineEditMotDePasse->setReadOnly(false);
  ui->checkBoxNouveauClient->setEnabled(true);

  ui->spinBoxQuantite->setEnabled(false);
  ui->pushButtonPrecedent->setEnabled(false);
  ui->pushButtonSuivant->setEnabled(false);
  ui->pushButtonAcheter->setEnabled(false);
  ui->pushButtonSupprimer->setEnabled(false);
  ui->pushButtonViderPanier->setEnabled(false);
  ui->pushButtonPayer->setEnabled(false);

  setNom("");
  setMotDePasse("");
  ui->checkBoxNouveauClient->setCheckState(Qt::CheckState::Unchecked);

  setArticle("",-1.0,-1,"");

  w->videTablePanier();
  w->setTotal(-1.0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// Fonctions utiles Table du panier (ne pas modifier) /////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::ajouteArticleTablePanier(const char* article,float prix,int quantite)
{
    char Prix[20],Quantite[20];

    sprintf(Prix,"%.2f",prix);
    sprintf(Quantite,"%d",quantite);

    // Ajout possible
    int nbLignes = ui->tableWidgetPanier->rowCount();
    nbLignes++;
    ui->tableWidgetPanier->setRowCount(nbLignes);
    ui->tableWidgetPanier->setRowHeight(nbLignes-1,10);

    QTableWidgetItem *item = new QTableWidgetItem;
    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    item->setTextAlignment(Qt::AlignCenter);
    item->setText(article);
    ui->tableWidgetPanier->setItem(nbLignes-1,0,item);

    item = new QTableWidgetItem;
    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    item->setTextAlignment(Qt::AlignCenter);
    item->setText(Prix);
    ui->tableWidgetPanier->setItem(nbLignes-1,1,item);

    item = new QTableWidgetItem;
    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    item->setTextAlignment(Qt::AlignCenter);
    item->setText(Quantite);
    ui->tableWidgetPanier->setItem(nbLignes-1,2,item);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::videTablePanier()
{
    ui->tableWidgetPanier->setRowCount(0);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
int WindowClient::getIndiceArticleSelectionne()
{
    QModelIndexList liste = ui->tableWidgetPanier->selectionModel()->selectedRows();
    if (liste.size() == 0) return -1;
    QModelIndex index = liste.at(0);
    int indice = index.row();
    return indice;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// Fonctions permettant d'afficher des boites de dialogue (ne pas modifier ////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::dialogueMessage(const char* titre,const char* message)
{
   QMessageBox::information(this,titre,message);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::dialogueErreur(const char* titre,const char* message)
{
   QMessageBox::critical(this,titre,message);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////// CLIC SUR LA CROIX DE LA FENETRE /////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::closeEvent(QCloseEvent *event)
{
   char requete[200],reponse[200];
   int nbEcrits, nbLus;

   if(logged)
   {
      // ***** Construction de la requete *********************
      sprintf(requete,"LOGOUT");
      // ***** Envoi requete  *********************************

      if ((nbEcrits = Send(sClient,requete,strlen(requete))) == -1)
      {
        perror("Erreur de Send");
        ::close(sClient);
        exit(1);
      }

      // ***** Attente de la reponse **************************
      if ((nbLus = Receive(sClient,reponse)) < 0)
      {
        perror("Erreur de Receive");
        ::close(sClient);
        exit(1);
      }
   }

   nbArticlePanier = 0;

   ::close(sClient);
   exit(0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// Fonctions clics sur les boutons ////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::on_pushButtonLogin_clicked()
{
    char login[50];
    char mdp[50];
    int NouveauClient = isNouveauClientChecked();

    strcpy(login, w->getNom());
    strcpy(mdp, w->getMotDePasse());

    char ipServeur[] = "192.168.146.128"; 
    int portServeur = 50000; 

    sClient = ClientSocket(ipServeur, portServeur);

    if (sClient == -1) 
    {
        perror("Erreur de ClientSocket");
        exit(1);
    } 
    else 
    {
        printf("Connecte sur le serveur.\n");
      
        if(!OVESP_Login(login, mdp, NouveauClient, sClient))
        {
          perror("Erreur dans OVESP_Login");
          exit(1);
        }
    }

    logged = true;
}

bool OVESP_Login(char * user, char * password, int NouveauClient, int sClient)
{
  //LOGIN#log#MDP#nvclient

  int off = 0;
  bool onContinue = true;
  char requete[200],reponse[200];


  // ***** Construction de la requete *********************
  sprintf(requete,"LOGIN#%s#%s#%d", user, password, NouveauClient);

  // ***** Envoi requete + réception réponse **************

  int nbEcrits, nbLus;

  // ***** Envoi de la requete ****************************

  if ((nbEcrits = Send(sClient,requete,strlen(requete))) == -1)
  {
   perror("Erreur de Send");
   close(sClient);
   exit(1);
  }

  // ***** Attente de la reponse **************************
  if ((nbLus = Receive(sClient,reponse)) < 0)
  {
   perror("Erreur de Receive");
   close(sClient);
   exit(1);
  }

  if (nbLus == 0)
  {
   printf("Serveur arrete, pas de reponse reçue...\n");
   close(sClient);
   exit(1);
  }

  // ***** Parsing de la réponse **************************
  char *ptr = strtok(reponse,"#"); //login
  ptr = strtok(NULL,"#"); // statut = ok ou ko


  if (strcmp(ptr,"ok") == 0)
  {
    ptr = strtok(NULL,"#"); 
    printf("Résultat = %s\n",ptr); 
    w->loginOK();
    w->dialogueMessage("Login","Login reussi !");

    sprintf(requete,"CONSULT#1");

    if ((nbEcrits = Send(sClient,requete,strlen(requete))) == -1)
    {
     perror("Erreur de Send");
     close(sClient);
     exit(1);
    }

    if ((nbLus = Receive(sClient,reponse)) < 0)
    {
     perror("Erreur de Receive");
     close(sClient);
     exit(1);
    }

    ptr = strtok(reponse,"#"); //consult
    ptr = strtok(NULL,"#"); // statut = ok ou ko

    if(strcmp(ptr,"ok") == 0)
    {
      idArticleEnCours = 1;
      int id, stock;
      char intitule[20], image[20];
      float prix;

      ptr = strtok(NULL,"#"); 
      //id
      id = atoi(ptr);
      ptr = strtok(NULL,"#"); 
      //intitule
      strcpy(intitule, ptr);
      ptr = strtok(NULL,"#"); 

      //prix
      char tmp[10];
      strcpy(tmp, ptr);
      int longueur = strlen(ptr);
      for (int i = 0; i < longueur; i++) 
      {
          if(tmp[i] == '.') 
          {
              tmp[i] = ','; 
          }
      }
      prix = atof(tmp);

      ptr = strtok(NULL,"#"); 
      //stock
      stock = atoi(ptr);
      ptr = strtok(NULL,"#"); 
      //image
      strcpy(image, ptr);


      printf("id = %d\n", id);
      printf("intitule = %s\n", intitule);
      printf("prix = %f\n", prix);
      printf("stock = %d\n", stock);
      printf("image = %s\n", image);

      w->setArticle(intitule, prix, stock, image);
    }
    else
    {
      ptr = strtok(NULL,"#"); 
      printf("Erreur: %s\n",ptr);
      onContinue = false;
    }
  }
  else
  {
    ptr = strtok(NULL,"#"); 
    printf("Erreur: %s\n",ptr);
    if(strcmp(ptr, "Mot de passe incorrecte !") == 0)
    {
      off++;
      w->dialogueMessage("Login","Mot de passe incorrecte !");
      if(off == 3)
      {
        onContinue = false;
      }

    }
    else
    {
        w->dialogueMessage("Login","Erreur fatal de Login  !");
        onContinue = false;
    }
  }

  return onContinue;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::on_pushButtonLogout_clicked()
{
  char requete[200],reponse[200];
    int nbEcrits, nbLus;

   
    // ***** Construction de la requete *********************
    sprintf(requete,"LOGOUT");
    // ***** Envoi requete  *********************************

    if ((nbEcrits = Send(sClient,requete,strlen(requete))) == -1)
    {
      perror("Erreur de Send");
      ::close(sClient);
      exit(1);
    }

    // ***** Attente de la reponse **************************
    if ((nbLus = Receive(sClient,reponse)) < 0)
    {
      perror("Erreur de Receive");
      ::close(sClient);
      exit(1);
    }

    
    w->dialogueMessage("LOGOUT","Logout reussi !");
    w->logoutOK();
    nbArticlePanier = 0;
    logged = false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::on_pushButtonSuivant_clicked()
{
    char requete[200],reponse[200];
    int nbEcrits, nbLus;

    if(idArticleEnCours==21)
    {
      w->dialogueErreur("CONSULT","Pas d'article suivant !");
    }
    else
    {
      printf("IDTEST : %d\n",idArticleEnCours);
      idArticleEnCours++;
      sprintf(requete,"CONSULT#%d",idArticleEnCours);


      if ((nbEcrits = Send(sClient,requete,strlen(requete))) == -1)
      {
       perror("Erreur de Send");
       ::close(sClient);
       exit(1);
      }

      if ((nbLus = Receive(sClient,reponse)) < 0)
      {
       perror("Erreur de Receive");
       ::close(sClient);
       exit(1);
      }

      char* ptr = strtok(reponse,"#"); //consult
      ptr = strtok(NULL,"#"); // statut = ok ou ko

      if(strcmp(ptr,"ok") == 0)
      {

        int id, stock, ptrLecture = 0, ptrEcriture = 0;
        char intitule[20], image[20], imageTmp[20];
        float prix;

        ptr = strtok(NULL,"#"); 
        //id
        id = atoi(ptr);
        ptr = strtok(NULL,"#"); 
        //intitule
        strcpy(intitule, ptr);
        ptr = strtok(NULL,"#"); 

        //prix
        char tmp[10];
        strcpy(tmp, ptr);
        int longueur = strlen(ptr);
        for (int i = 0; i < longueur; i++) 
        {
            if(tmp[i] == '.') 
            {
                tmp[i] = ','; 
            }
        }
        prix = atof(tmp);

        ptr = strtok(NULL,"#"); 
        //stock
        stock = atoi(ptr);
        ptr = strtok(NULL,"#"); 
        //image
        ptr = strtok(ptr, "."); //ici on fait ça pour évité les caractère spéciaux qu'il y a en fin de chaine
        strcpy(image, ptr);
        strcat(image, ".jpg");

        printf("id = %d\n", id);
        printf("intitule = %s\n", intitule);
        printf("prix = %f\n", prix);
        printf("stock = %d\n", stock);
        printf("image = %s\n", image);

        w->setArticle(intitule, prix, stock, image);
      }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::on_pushButtonPrecedent_clicked()
{
  char requete[200],reponse[200];
    int nbEcrits, nbLus;

    if(idArticleEnCours==1)
    {
      w->dialogueErreur("CONSULT","Pas d'article précédant !");
    }
    else
    {
      printf("IDTEST : %d\n",idArticleEnCours);
      idArticleEnCours--;
      sprintf(requete,"CONSULT#%d",idArticleEnCours);

      printf("\nCONSULT REQUETE : %s", requete);


      if ((nbEcrits = Send(sClient,requete,strlen(requete))) == -1)
      {
       perror("Erreur de Send");
       ::close(sClient);
       exit(1);
      }

      if ((nbLus = Receive(sClient,reponse)) < 0)
      {
       perror("Erreur de Receive");
       ::close(sClient);
       exit(1);
      }

      char* ptr = strtok(reponse,"#"); //consult
      ptr = strtok(NULL,"#"); // statut = ok ou ko

      if(strcmp(ptr,"ok") == 0)
      {

        int id, stock, ptrLecture = 0, ptrEcriture = 0;
        char intitule[20], image[20], imageTmp[20];
        float prix;

        ptr = strtok(NULL,"#"); 
        //id
        id = atoi(ptr);
        ptr = strtok(NULL,"#"); 
        //intitule
        strcpy(intitule, ptr);
        ptr = strtok(NULL,"#"); 

        //prix
        char tmp[10];
        strcpy(tmp, ptr);
        int longueur = strlen(ptr);
        for (int i = 0; i < longueur; i++) 
        {
            if(tmp[i] == '.') 
            {
                tmp[i] = ','; 
            }
        }
        prix = atof(tmp);

        ptr = strtok(NULL,"#"); 
        //stock
        stock = atoi(ptr);
        ptr = strtok(NULL,"#"); 
        //image
        ptr = strtok(ptr, "."); //ici on fait ça pour évité les caractère spéciaux qu'il y a en fin de chaine
        strcpy(image, ptr);
        strcat(image, ".jpg");

        printf("id = %d\n", id);
        printf("intitule = %s\n", intitule);
        printf("prix = %f\n", prix);
        printf("stock = %d\n", stock);
        printf("image = %s\n", image);

        w->setArticle(intitule, prix, stock, image);
      }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::on_pushButtonAcheter_clicked()
{ 
  //********* ACHAT#idArticle#quantite
  char requete[200],reponse[200];
  int nbEcrits, nbLus;

  bool etaitpresent = false;
  sprintf(requete, "PRESENCECADDIE#%d", idArticleEnCours);

  printf("\nSEND\n");
  //envoie - réception
  if ((nbEcrits = Send(sClient,requete,strlen(requete))) == -1)
  {
    perror("Erreur de Send");
    ::close(sClient);
    exit(1);
  }

  printf("\nRECEIVE\n");


  if ((nbLus = Receive(sClient,reponse)) < 0)
  {
    perror("Erreur de Receive");
    ::close(sClient);
    exit(1);
  }

  char * reponsepars;
  reponsepars = strtok(reponse, "#");
  reponsepars = strtok(NULL, "#");

  if (strcmp(reponsepars, "ok") == 0)
  {
    etaitpresent = true;
  }

  if((nbArticlePanier < 5 || etaitpresent) && w->getQuantite() != 0)
  {

    sprintf(requete, "ACHAT#%d#%d", idArticleEnCours, w->getQuantite());


    printf("\nSEND ACHAT\n");
    //envoie - réception
    if ((nbEcrits = Send(sClient,requete,strlen(requete))) == -1)
    {
      perror("Erreur de Send");
      ::close(sClient);
      exit(1);
    }

    printf("\nRECEIVE ACHAT\n");


    if ((nbLus = Receive(sClient,reponse)) < 0)
    {
      perror("Erreur de Receive");
      ::close(sClient);
      exit(1);
    }

    reponsepars = strtok(reponse, "#");
    reponsepars = strtok(NULL, "#");

    
    if (strcmp(reponsepars, "ko") == 0)
    {
      perror("Erreur de la requête achat ");
      reponsepars = strtok(NULL, "#");
      printf("Erreur : %s\n", reponsepars);
       w->dialogueErreur("Erreur","L'achat a echoue !");
      return;
    }

    strcpy(requete, "");
    strcpy(reponse, "");

    


    //**************** CADDIE

    //mise a jour du caddie

    sprintf(requete, "CADDIE");

    printf("\nSEND CADDIE\n");


    //envoie - réception
    if ((nbEcrits = Send(sClient,requete,strlen(requete))) == -1)
    {
      perror("Erreur de Send");
      ::close(sClient);
      exit(1);
    }

    printf("\nReceive CADDIE: \n");


    if ((nbLus = Receive(sClient,reponse)) < 0)
    {
      perror("Erreur de Receive");
      ::close(sClient);
      exit(1);
    }

    printf("\nReceive CADDIE ok: \n");
    puts(reponse);


    int occurrences = compterOccurrences(reponse, '$');

    char * parsing = strtok(reponse, "#");
    char * tmp;

    //ok ou ko
    parsing = strtok(NULL, "#");

    //char * tab[5];
    char **tab = (char **)malloc(occurrences * sizeof(char *));
    // Allouer de la mémoire pour chaque élément du tableau
    for (int i = 0; i < occurrences; i++) 
    {
      tab[i] = (char *)malloc(256); 
      
    }



    if(strcmp(parsing, "ok") == 0)
    {
      printf("%d\n", occurrences);
      for(int i = 0; i<occurrences ;i++)
      {
          parsing = strtok(NULL, "$");
          strcpy(tab[i], parsing);

          printf("%s\n", parsing);
      }

      //%d,%s,%d,%f
      char * parsingTab;
      int idArticle, quantite;
      float prix;
      char intitule[20];
      w->videTablePanier();
      TotCaddie = 0;
      for(int j = 0 ; j < occurrences ; j++)
      {
        //char *tmpTab = strdup(tab[j]);  // Créez une copie de la chaîne pour éviter de la modifier.
        parsingTab = strtok(tab[j], ",");

        if (parsingTab != NULL) 
        {
            printf("COUCOU\n");
            idArticle = atoi(parsingTab);
            printf("%d\n", idArticle);
        }

        // Réinitialisez parsingTab avec la copie non modifiée.
        parsingTab = strtok(NULL, ",");

        //intitule
        strcpy(intitule, parsingTab);
        printf("%s\n", intitule);
        

        parsingTab = strtok(NULL, ",");
        //quantite
        if (parsingTab != NULL) 
        {
            quantite = atoi(parsingTab);
            printf("%d\n", quantite);
        }

        parsingTab = strtok(NULL, "$");
        //prix
       
            char tmp[10];
            strcpy(tmp, parsingTab);
            int longueur = strlen(parsingTab);
            for (int i = 0; i < longueur; i++) 
            {
                if (tmp[i] == '.') 
                {
                    tmp[i] = ',';
                }
            }
            prix = atof(tmp);
            printf("%f\n", prix);
        

        //free(tmpTab);
        TotCaddie = TotCaddie + prix;
        w->setTotal(TotCaddie);
        w->ajouteArticleTablePanier(intitule, prix, quantite);
       
              
      }
      if(!etaitpresent)
      {
              nbArticlePanier++;
      }

      for (int i = 0; i < occurrences; i++) 
      {
        free(tab[i]);
      }

      free(tab);
      
    }
    else
    {
      //ko = erreur
      parsing = strtok(reponse, "#");
      printf("Erreur : %s", parsing);
      perror("Erreur dans le caddie");
      w->dialogueErreur("Erreur","L'achat a echoue !");
      ::close(sClient);
      exit(1);
    }
  }
  else
  {
    if(nbArticlePanier == 5)
    { 
      w->dialogueErreur("Achat", "Nombre maximum d'achat atteint !");
    }
    if(w->getQuantite() == 0)
    {
      w->dialogueErreur("Achat", "Veuillez choisir une quantité valide !");
    }
  }
}
int compterOccurrences(char *chaine, char caractere) 
{
    int compteur = 0;
    while (*chaine) {
        if (*chaine == caractere) 
        {
            compteur++;
        }
        chaine++;
    }
    return compteur;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::on_pushButtonSupprimer_clicked()
{
  char requete[200],reponse[200];
  int nbEcrits, nbLus;

    int indice = getIndiceArticleSelectionne();
    if(indice == -1)
    {
        dialogueErreur("Erreur","veuillez selectionner un article");
        return;
    }

   
    // ***** Construction de la requete *********************
    sprintf(requete,"CANCEL#%d",indice);
    // ***** Envoi requete  *********************************

    if ((nbEcrits = Send(sClient,requete,strlen(requete))) == -1)
    {
      perror("Erreur de Send");
      ::close(sClient);
      exit(1);
    }

    // ***** Attente de la reponse **************************
    if ((nbLus = Receive(sClient,reponse)) < 0)
    {
      perror("Erreur de Receive");
      ::close(sClient);
      exit(1);
    }

    if (nbLus == 0)
    {
      printf("Serveur arrete, pas de reponse reçue...\n");
      ::close(sClient);
      exit(1);
    }

    // ***** Parsing de la réponse **************************
    char *ptr = strtok(reponse,"#"); 
    ptr = strtok(NULL,"#"); 


    if (strcmp(ptr,"ok") == 0)
    {
      ptr = strtok(NULL,"#"); 
      printf("Résultat = %s\n",ptr);
      w->dialogueMessage("CANCEL","Suppression reussie !"); 
      nbArticlePanier--;

      // Mise à jour du caddie
      w->videTablePanier();
      w->setTotal(-1.0);

       //**************** CADDIE

        //mise a jour du caddie

        sprintf(requete, "CADDIE");

        //envoie - réception
        if ((nbEcrits = Send(sClient,requete,strlen(requete))) == -1)
        {
          perror("Erreur de Send");
          ::close(sClient);
          exit(1);
        }

        if ((nbLus = Receive(sClient,reponse)) < 0)
        {
          perror("Erreur de Receive");
          ::close(sClient);
          exit(1);
        }

        int occurrences = compterOccurrences(reponse, '$');
        //occurrences--; //ici on fait -- parce que il y a 1 # en trop dans la réponse pour savoir le nombre d'élément dans caddie

        char * parsing = strtok(reponse, "#");
        char * tmp;

        //ok ou ko
        parsing = strtok(NULL, "#");

        //char * tab[5];
        char **tab = (char **)malloc(occurrences * sizeof(char *));
      // Allouer de la mémoire pour chaque élément du tableau
        for (int i = 0; i < occurrences; i++) 
        {
          tab[i] = (char *)malloc(256); 
          
        }

        if(strcmp(parsing, "ok") == 0)
        {
          printf("%d\n", occurrences);
          for(int i = 0; i<occurrences ;i++)
          {
              parsing = strtok(NULL, "$");
              strcpy(tab[i], parsing);

              printf("%s\n", parsing);
          }

      


          //%d,%s,%d,%f
          char * parsingTab;
          int idArticle, quantite;
          float prix;
          char intitule[20];
          TotCaddie = 0.0;
          for(int j = 0 ; j < occurrences ; j++)
          {
            char *tmpTab = strdup(tab[j]);  // Créez une copie de la chaîne pour éviter de la modifier.
            parsingTab = strtok(tmpTab, ",");

            
            printf("COUCOU\n");
            idArticle = atoi(parsingTab);
            printf("%d\n", idArticle);
            

            // Réinitialisez parsingTab avec la copie non modifiée.
            parsingTab = strtok(NULL, ",");

            //intitule
            strcpy(intitule, parsingTab);
            printf("%s\n", intitule);
            

            parsingTab = strtok(NULL, ",");
            //quantite
        
            quantite = atoi(parsingTab);
            printf("%d\n", quantite);
            

            parsingTab = strtok(NULL, "$");
            //prix
            
            char tmp[10];
            strcpy(tmp, parsingTab);
            int longueur = strlen(parsingTab);
            for (int i = 0; i < longueur; i++) 
            {
                if (tmp[i] == '.') 
                {
                    tmp[i] = ',';
                }
            }
            prix = atof(tmp);
            printf("%f\n", prix);
            

            free(tmpTab);
            TotCaddie = TotCaddie + prix;
            //set le caddie

            w->ajouteArticleTablePanier(intitule, prix, quantite);
            w->setTotal(TotCaddie);
          }
          for (int i = 0; i < occurrences; i++) 
          {
            free(tab[i]);
          }

          free(tab);
        }
        else
        {
          //ko = erreur
          parsing = strtok(reponse, "#");
          printf("Erreur : %s", parsing);
          perror("Erreur dans le caddie");
          w->dialogueErreur("Erreur","L'achat a echoue !");
          ::close(sClient);
          exit(1);
        }
    }
    else
    {
      ptr = strtok(NULL,"#"); 
      printf("Erreur: %s\n",ptr);
      w->dialogueErreur("CANCEL","Suppression echouee !");
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::on_pushButtonViderPanier_clicked()
{
    // ************* CANCELALL
    char requete[200],reponse[200];
    int nbEcrits, nbLus;

   
    // ***** Construction de la requete *********************
    sprintf(requete,"CANCELALL");
    // ***** Envoi requete  *********************************

    if ((nbEcrits = Send(sClient,requete,strlen(requete))) == -1)
    {
      perror("Erreur de Send");
      ::close(sClient);
      exit(1);
    }

    // ***** Attente de la reponse **************************
    if ((nbLus = Receive(sClient,reponse)) < 0)
    {
      perror("Erreur de Receive");
      ::close(sClient);
      exit(1);
    }

    /*if (nbLus == 0)
    {
      printf("Serveur arrete, pas de reponse reçue...\n");
      ::close(sClient);
      exit(1);
    }*/

      // Mise à jour du caddie
      w->videTablePanier();
      TotCaddie = 0.0;
      w->setTotal(-1.0);
      nbArticlePanier = 0;

     
    w->dialogueMessage("CANCELALL","Vidage du Caddie reussie !"); 
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::on_pushButtonPayer_clicked()
{

    // ************* CONFIRMER
    char requete[200],reponse[200];
    int nbEcrits, nbLus;

   
    // ***** Construction de la requete *********************
    sprintf(requete,"CONFIRMER");
    // ***** Envoi requete  *********************************

    if ((nbEcrits = Send(sClient,requete,strlen(requete))) == -1)
    {
      perror("Erreur de Send");
      ::close(sClient);
      exit(1);
    }

    // ***** Attente de la reponse **************************
    if ((nbLus = Receive(sClient,reponse)) < 0)
    {
      perror("Erreur de Receive");
      ::close(sClient);
      exit(1);
    }

    if (nbLus == 0)
    {
      printf("Serveur arrete, pas de reponse reçue...\n");
      ::close(sClient);
      exit(1);
    }

    char* ptr = strtok(reponse,"#"); 
    ptr = strtok(NULL,"#"); 

    if(strcmp(ptr,"ok") == 0)
    {
      int numero;
      ptr = strtok(NULL,"#"); 
      //NumeroFacture
      numero = atoi(ptr);

      
      // Mise à jour du caddie
        w->videTablePanier();
        TotCaddie = 0.0;
        w->setTotal(-1.0);
        nbArticlePanier = 0;

      
      w->dialogueMessage("CONFIMER","ACHAT CONFIRME !"); 

  } 
}