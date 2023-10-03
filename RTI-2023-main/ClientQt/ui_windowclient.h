/********************************************************************************
** Form generated from reading UI file 'windowclient.ui'
**
** Created by: Qt User Interface Compiler version 5.12.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WINDOWCLIENT_H
#define UI_WINDOWCLIENT_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_WindowClient
{
public:
    QWidget *centralwidget;
    QPushButton *pushButtonLogout;
    QPushButton *pushButtonLogin;
    QLabel *label_2;
    QLabel *label;
    QLineEdit *lineEditMotDePasse;
    QLineEdit *lineEditNom;
    QCheckBox *checkBoxNouveauClient;
    QLabel *label_3;
    QFrame *frame;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QLabel *label_4;
    QLineEdit *lineEditArticle;
    QLabel *label_5;
    QLineEdit *lineEditPrixUnitaire;
    QLabel *label_6;
    QLineEdit *lineEditStock;
    QLabel *label_7;
    QPushButton *pushButtonAcheter;
    QPushButton *pushButtonPrecedent;
    QPushButton *pushButtonSuivant;
    QSpinBox *spinBoxQuantite;
    QLabel *label_8;
    QFrame *frame_2;
    QTableWidget *tableWidgetPanier;
    QLabel *label_9;
    QLineEdit *lineEditTotal;
    QPushButton *pushButtonSupprimer;
    QPushButton *pushButtonViderPanier;
    QPushButton *pushButtonPayer;
    QLineEdit *lineEditPublicite;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *WindowClient)
    {
        if (WindowClient->objectName().isEmpty())
            WindowClient->setObjectName(QString::fromUtf8("WindowClient"));
        WindowClient->resize(770, 618);
        QIcon icon;
        icon.addFile(QString::fromUtf8("images/poivron.jpg"), QSize(), QIcon::Normal, QIcon::Off);
        WindowClient->setWindowIcon(icon);
        centralwidget = new QWidget(WindowClient);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        pushButtonLogout = new QPushButton(centralwidget);
        pushButtonLogout->setObjectName(QString::fromUtf8("pushButtonLogout"));
        pushButtonLogout->setEnabled(false);
        pushButtonLogout->setGeometry(QRect(520, 10, 91, 25));
        QFont font;
        font.setFamily(QString::fromUtf8("DejaVu Sans"));
        font.setPointSize(12);
        pushButtonLogout->setFont(font);
        pushButtonLogout->setStyleSheet(QString::fromUtf8("background-color:rgb(252, 175, 62)"));
        pushButtonLogout->setCheckable(false);
        pushButtonLogin = new QPushButton(centralwidget);
        pushButtonLogin->setObjectName(QString::fromUtf8("pushButtonLogin"));
        pushButtonLogin->setGeometry(QRect(420, 10, 91, 25));
        pushButtonLogin->setFont(font);
        pushButtonLogin->setStyleSheet(QString::fromUtf8("background-color:rgb(138, 226, 52)"));
        label_2 = new QLabel(centralwidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(180, 10, 131, 21));
        label_2->setFont(font);
        label = new QLabel(centralwidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(8, 10, 64, 21));
        label->setFont(font);
        lineEditMotDePasse = new QLineEdit(centralwidget);
        lineEditMotDePasse->setObjectName(QString::fromUtf8("lineEditMotDePasse"));
        lineEditMotDePasse->setGeometry(QRect(298, 10, 113, 25));
        lineEditMotDePasse->setFont(font);
        lineEditMotDePasse->setAlignment(Qt::AlignCenter);
        lineEditNom = new QLineEdit(centralwidget);
        lineEditNom->setObjectName(QString::fromUtf8("lineEditNom"));
        lineEditNom->setGeometry(QRect(58, 10, 113, 25));
        lineEditNom->setFont(font);
        lineEditNom->setAlignment(Qt::AlignCenter);
        checkBoxNouveauClient = new QCheckBox(centralwidget);
        checkBoxNouveauClient->setObjectName(QString::fromUtf8("checkBoxNouveauClient"));
        checkBoxNouveauClient->setGeometry(QRect(620, 10, 151, 23));
        checkBoxNouveauClient->setFont(font);
        label_3 = new QLabel(centralwidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(10, 50, 81, 17));
        label_3->setFont(font);
        frame = new QFrame(centralwidget);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setGeometry(QRect(10, 70, 751, 221));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        scrollArea = new QScrollArea(frame);
        scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
        scrollArea->setGeometry(QRect(10, 10, 201, 201));
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QString::fromUtf8("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 199, 199));
        scrollArea->setWidget(scrollAreaWidgetContents);
        label_4 = new QLabel(frame);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(230, 20, 61, 21));
        label_4->setFont(font);
        lineEditArticle = new QLineEdit(frame);
        lineEditArticle->setObjectName(QString::fromUtf8("lineEditArticle"));
        lineEditArticle->setEnabled(true);
        lineEditArticle->setGeometry(QRect(310, 20, 211, 25));
        lineEditArticle->setFont(font);
        lineEditArticle->setAlignment(Qt::AlignCenter);
        lineEditArticle->setReadOnly(true);
        label_5 = new QLabel(frame);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(230, 70, 121, 21));
        label_5->setFont(font);
        lineEditPrixUnitaire = new QLineEdit(frame);
        lineEditPrixUnitaire->setObjectName(QString::fromUtf8("lineEditPrixUnitaire"));
        lineEditPrixUnitaire->setEnabled(true);
        lineEditPrixUnitaire->setGeometry(QRect(360, 70, 161, 25));
        lineEditPrixUnitaire->setFont(font);
        lineEditPrixUnitaire->setAlignment(Qt::AlignCenter);
        lineEditPrixUnitaire->setReadOnly(true);
        label_6 = new QLabel(frame);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(230, 120, 64, 21));
        label_6->setFont(font);
        lineEditStock = new QLineEdit(frame);
        lineEditStock->setObjectName(QString::fromUtf8("lineEditStock"));
        lineEditStock->setGeometry(QRect(360, 120, 161, 25));
        lineEditStock->setFont(font);
        lineEditStock->setAlignment(Qt::AlignCenter);
        lineEditStock->setReadOnly(true);
        label_7 = new QLabel(frame);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(230, 170, 181, 21));
        label_7->setFont(font);
        pushButtonAcheter = new QPushButton(frame);
        pushButtonAcheter->setObjectName(QString::fromUtf8("pushButtonAcheter"));
        pushButtonAcheter->setEnabled(false);
        pushButtonAcheter->setGeometry(QRect(540, 170, 201, 25));
        pushButtonAcheter->setFont(font);
        pushButtonAcheter->setStyleSheet(QString::fromUtf8("background-color: lightblue"));
        pushButtonPrecedent = new QPushButton(frame);
        pushButtonPrecedent->setObjectName(QString::fromUtf8("pushButtonPrecedent"));
        pushButtonPrecedent->setEnabled(false);
        pushButtonPrecedent->setGeometry(QRect(540, 20, 91, 131));
        QFont font1;
        font1.setFamily(QString::fromUtf8("DejaVu Sans"));
        font1.setPointSize(14);
        pushButtonPrecedent->setFont(font1);
        pushButtonPrecedent->setStyleSheet(QString::fromUtf8("background-color:rgb(182, 250, 217)"));
        pushButtonSuivant = new QPushButton(frame);
        pushButtonSuivant->setObjectName(QString::fromUtf8("pushButtonSuivant"));
        pushButtonSuivant->setEnabled(false);
        pushButtonSuivant->setGeometry(QRect(650, 20, 91, 131));
        pushButtonSuivant->setFont(font1);
        pushButtonSuivant->setStyleSheet(QString::fromUtf8("background-color:rgb(182, 250, 217)"));
        spinBoxQuantite = new QSpinBox(frame);
        spinBoxQuantite->setObjectName(QString::fromUtf8("spinBoxQuantite"));
        spinBoxQuantite->setEnabled(false);
        spinBoxQuantite->setGeometry(QRect(418, 170, 101, 26));
        spinBoxQuantite->setFont(font);
        spinBoxQuantite->setAlignment(Qt::AlignCenter);
        label_8 = new QLabel(centralwidget);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setGeometry(QRect(10, 350, 64, 17));
        label_8->setFont(font);
        frame_2 = new QFrame(centralwidget);
        frame_2->setObjectName(QString::fromUtf8("frame_2"));
        frame_2->setGeometry(QRect(10, 370, 751, 211));
        frame_2->setFrameShape(QFrame::StyledPanel);
        frame_2->setFrameShadow(QFrame::Raised);
        tableWidgetPanier = new QTableWidget(frame_2);
        tableWidgetPanier->setObjectName(QString::fromUtf8("tableWidgetPanier"));
        tableWidgetPanier->setGeometry(QRect(10, 10, 511, 151));
        label_9 = new QLabel(frame_2);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setGeometry(QRect(10, 170, 121, 21));
        label_9->setFont(font);
        lineEditTotal = new QLineEdit(frame_2);
        lineEditTotal->setObjectName(QString::fromUtf8("lineEditTotal"));
        lineEditTotal->setGeometry(QRect(140, 170, 131, 25));
        lineEditTotal->setFont(font);
        lineEditTotal->setAlignment(Qt::AlignCenter);
        lineEditTotal->setReadOnly(true);
        pushButtonSupprimer = new QPushButton(frame_2);
        pushButtonSupprimer->setObjectName(QString::fromUtf8("pushButtonSupprimer"));
        pushButtonSupprimer->setEnabled(false);
        pushButtonSupprimer->setGeometry(QRect(540, 10, 201, 25));
        pushButtonSupprimer->setFont(font);
        pushButtonSupprimer->setStyleSheet(QString::fromUtf8("background-color:rgb(182, 250, 217)"));
        pushButtonViderPanier = new QPushButton(frame_2);
        pushButtonViderPanier->setObjectName(QString::fromUtf8("pushButtonViderPanier"));
        pushButtonViderPanier->setEnabled(false);
        pushButtonViderPanier->setGeometry(QRect(540, 50, 201, 25));
        pushButtonViderPanier->setFont(font);
        pushButtonViderPanier->setStyleSheet(QString::fromUtf8("background-color:rgb(182, 250, 217)"));
        pushButtonPayer = new QPushButton(frame_2);
        pushButtonPayer->setObjectName(QString::fromUtf8("pushButtonPayer"));
        pushButtonPayer->setEnabled(false);
        pushButtonPayer->setGeometry(QRect(540, 130, 201, 25));
        pushButtonPayer->setFont(font);
        pushButtonPayer->setStyleSheet(QString::fromUtf8("background-color: lightblue"));
        lineEditPublicite = new QLineEdit(centralwidget);
        lineEditPublicite->setObjectName(QString::fromUtf8("lineEditPublicite"));
        lineEditPublicite->setEnabled(false);
        lineEditPublicite->setGeometry(QRect(10, 300, 751, 41));
        QFont font2;
        font2.setFamily(QString::fromUtf8("Courier 10 Pitch"));
        font2.setPointSize(18);
        font2.setBold(true);
        font2.setWeight(75);
        lineEditPublicite->setFont(font2);
        lineEditPublicite->setStyleSheet(QString::fromUtf8("background-color: lightyellow;color:red"));
        lineEditPublicite->setAlignment(Qt::AlignCenter);
        WindowClient->setCentralWidget(centralwidget);
        menubar = new QMenuBar(WindowClient);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 770, 22));
        WindowClient->setMenuBar(menubar);
        statusbar = new QStatusBar(WindowClient);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        WindowClient->setStatusBar(statusbar);

        retranslateUi(WindowClient);

        QMetaObject::connectSlotsByName(WindowClient);
    } // setupUi

    void retranslateUi(QMainWindow *WindowClient)
    {
        WindowClient->setWindowTitle(QApplication::translate("WindowClient", "Le Maraicher en ligne", nullptr));
        pushButtonLogout->setText(QApplication::translate("WindowClient", "Logout", nullptr));
        pushButtonLogin->setText(QApplication::translate("WindowClient", "Login", nullptr));
        label_2->setText(QApplication::translate("WindowClient", "Mot de passe :", nullptr));
        label->setText(QApplication::translate("WindowClient", "Nom :", nullptr));
        checkBoxNouveauClient->setText(QApplication::translate("WindowClient", "Nouveau client", nullptr));
        label_3->setText(QApplication::translate("WindowClient", "Magasin :", nullptr));
        label_4->setText(QApplication::translate("WindowClient", "Article :", nullptr));
        label_5->setText(QApplication::translate("WindowClient", "Prix \303\240 l'unit\303\251 :", nullptr));
        label_6->setText(QApplication::translate("WindowClient", "Stock :", nullptr));
        label_7->setText(QApplication::translate("WindowClient", "Quantit\303\251 souhait\303\251e :", nullptr));
        pushButtonAcheter->setText(QApplication::translate("WindowClient", "Acheter", nullptr));
#ifndef QT_NO_TOOLTIP
        pushButtonPrecedent->setToolTip(QApplication::translate("WindowClient", "Pr\303\251c\303\251dent", nullptr));
#endif // QT_NO_TOOLTIP
        pushButtonPrecedent->setText(QApplication::translate("WindowClient", "<<<", nullptr));
#ifndef QT_NO_TOOLTIP
        pushButtonSuivant->setToolTip(QApplication::translate("WindowClient", "Suivant", nullptr));
#endif // QT_NO_TOOLTIP
        pushButtonSuivant->setText(QApplication::translate("WindowClient", ">>>", nullptr));
        label_8->setText(QApplication::translate("WindowClient", "Panier :", nullptr));
        label_9->setText(QApplication::translate("WindowClient", "Total \303\240 payer :", nullptr));
        pushButtonSupprimer->setText(QApplication::translate("WindowClient", "Supprimer article", nullptr));
        pushButtonViderPanier->setText(QApplication::translate("WindowClient", "Vider le panier", nullptr));
        pushButtonPayer->setText(QApplication::translate("WindowClient", "Payer", nullptr));
#ifndef QT_NO_TOOLTIP
        lineEditPublicite->setToolTip(QApplication::translate("WindowClient", "<html><head/><body><p>!!! Publicit\303\251 !!!</p></body></html>", nullptr));
#endif // QT_NO_TOOLTIP
    } // retranslateUi

};

namespace Ui {
    class WindowClient: public Ui_WindowClient {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WINDOWCLIENT_H
