#ifndef WINDOWCLIENT_H
#define WINDOWCLIENT_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class WindowClient; }
QT_END_NAMESPACE

class WindowClient : public QMainWindow
{
    Q_OBJECT

public:
    WindowClient(QWidget *parent = nullptr);
    ~WindowClient();

    // Fonctions utiles (ne pas modifier)
    void setNom(const char *Text);
    const char *getNom();
    void setMotDePasse(const char *Text);
    const char *getMotDePasse();
    void setPublicite(const char *Text);
    void setImage(const char *image);
    int isNouveauClientChecked();
    int getQuantite();
    int getTotal();
    void loginOK();
    void logoutOK();
    void setArticle(const char *intitule, float prix, int stock, const char *image);
    void setTotal(float montant);

    void ajouteArticleTablePanier(const char *article, float prix, int quantite);
    void videTablePanier();
    int getIndiceArticleSelectionne();

    // Clic sur la croix de la fenetre
    void closeEvent(QCloseEvent *event);

    // Boites de dialogue
    void dialogueMessage(const char *titre, const char *message);
    void dialogueErreur(const char *titre, const char *message);

    void Actualiser_Panier();
    void AfficherArticle(int idArticle);

private slots:
    void on_pushButtonLogin_clicked();
    void on_pushButtonLogout_clicked();
    void on_pushButtonPrecedent_clicked();
    void on_pushButtonSuivant_clicked();
    void on_pushButtonAcheter_clicked();
    void on_pushButtonSupprimer_clicked();
    void on_pushButtonViderPanier_clicked();
    void on_pushButtonPayer_clicked();

private:
    Ui::WindowClient *ui;

    char motDePasse[20];
    char nom[20];
};
#endif // WINDOWCLIENT_H