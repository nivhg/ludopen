//------------------------------------------------------------------------------
/** @file       f_preferences.cpp
 *  @brief      F_Preferences permet de modifier les options du programme.
 *
 *  @author     NORMAND Julien
 *  @author     STS IRIS, Lycée Nicolas APPERT, ORVAULT (FRANCE)
 *  @since      05/2012
 *  @version    1.0
 *  @date       01/02/2012
 *
 *  Les options disponibles dans cette classe sont:
 *      - Préférences Générales avec :
 *          - Nombre de jeux autorisés par défaut.
 *          - Prix d'une unité de location.
 *          - Nombre de jours de retard toléré.
 *          - Nombre de jours avant l'envoi d'un mail.
 *          - Modification du code administrateur.
 *      - Réseaux avec :
 *          - Information de la BDD.
 *          - Messagerie.
 *      - Membres/Emprunt avec :
 *          - Titre Membre.
 *          - Type Membre.
            - Type Emprunt.
 *      - Jeux avec :
 *          - Type.
 *          - Etat.
 *          - Statut.
 *          - Emplacement.
 *          - Chemin des règles des jeux.
 *          - Chemin des images des jeux.
 *      - Informations de la ludothèque.
 *          - Lieux.
 *
 *  Fabrication   Qt Creator, projet    .pro
 *
 *  @todo       Classe terminée.
 */
//------------------------------------------------------------------------------

// En-têtes standards (ATTENTION : garder toujours le meme ordre) --------------
#include <QtGui>
#include <QtSql>
#include <fenv.h>
#include <iostream>
using namespace std;
//------------------------------------------------------------------------------

// En-tête propre à  l'application ----------------------------------------------
#include "f_preferences.h"
#include "ui_f_preferences.h"
//------------------------------------------------------------------------------

/**
 *  @brief  Constructeur de la classe.
 *
 *  @param  parent
 *  @see    MettreAJourBDD()
 *  @see    AfficherInfoDemarrage()
 */
F_Preferences::F_Preferences(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::F_Preferences)
{
    ui->setupUi(this);
    fesetround(FE_TOWARDZERO);

    this->pEtatJeuAjMod = new F_PopUpCLESTTEM(0);
    this->pEtatJeuAjMod->setWindowModality(Qt::ApplicationModal);

    this->pStatutJeuAjMod = new F_PopUpCLESTTEM(1);
    this->pStatutJeuAjMod->setWindowModality(Qt::ApplicationModal);

    this->pEmplacementJeuAjMod = new F_PopUpCLESTTEM(2);
    this->pEmplacementJeuAjMod->setWindowModality(Qt::ApplicationModal);

    this->pTypeJeuAjMod = new F_PopUpCLESTTEM(3);
    this->pTypeJeuAjMod->setWindowModality(Qt::ApplicationModal);

    this->pTypeMembreAjMod = new F_PopUpCLESTTEM(5);
    this->pTypeMembreAjMod->setWindowModality(Qt::ApplicationModal);

    this->pTitreMembreAjMod = new F_PopUpCLESTTEM(6);
    this->pTitreMembreAjMod->setWindowModality(Qt::ApplicationModal);

    this->pTypeEmpruntAjMod = new F_PopUpCLESTTEM(7);
    this->pTypeEmpruntAjMod->setWindowModality(Qt::ApplicationModal);

    this->pLieuxInfoLudoAjMod = new F_PopUpCLESTTEM(8);
    this->pLieuxInfoLudoAjMod->setWindowModality(Qt::ApplicationModal);

    this->pPaiementMembreAjMod = new F_PopUpCLESTTEM(9);
    this->pPaiementMembreAjMod->setWindowModality(Qt::ApplicationModal);

    this->TbMembresTitre = new QStandardItemModel();
    ui->TbV_MembresTitre->setModel(this->TbMembresTitre);
    ui->TbV_MembresTitre->setEditTriggers(0);
    ui->TbV_MembresTitre->verticalHeader()->setVisible(false);

    this->TbMembresType = new QStandardItemModel();
    ui->TbV_MembresType->setModel(this->TbMembresType);
    ui->TbV_MembresType->setEditTriggers(0);
    ui->TbV_MembresType->verticalHeader()->setVisible(false);

    this->TbMembresPaiement = new QStandardItemModel();
    ui->TbV_MembresPaiement->setModel(this->TbMembresPaiement);
    ui->TbV_MembresPaiement->setEditTriggers(0);
    ui->TbV_MembresPaiement->verticalHeader()->setVisible(false);

    this->TbEmpruntType= new QStandardItemModel();
    ui->TbV_EmpruntType->setModel(this->TbEmpruntType);
    ui->TbV_EmpruntType->setEditTriggers(0);
    ui->TbV_EmpruntType->verticalHeader()->setVisible(false);

    this->TbJeuxType = new QStandardItemModel();
    ui->TbV_JeuxType->setModel(this->TbJeuxType);
    ui->TbV_JeuxType->setEditTriggers(0);
    ui->TbV_JeuxType->verticalHeader()->setVisible(false);

    this->TbJeuxEtat = new QStandardItemModel();
    ui->TbV_JeuxEtat->setModel(this->TbJeuxEtat);
    ui->TbV_JeuxEtat->setEditTriggers(0);
    ui->TbV_JeuxEtat->verticalHeader()->setVisible(false);

    this->TbJeuxStatut = new QStandardItemModel();
    ui->TbV_JeuxStatut->setModel(this->TbJeuxStatut);
    ui->TbV_JeuxStatut->setEditTriggers(0);
    ui->TbV_JeuxStatut->verticalHeader()->setVisible(false);

    this->TbJeuxEmplacement = new QStandardItemModel();
    ui->TbV_JeuxEmplacement->setModel(this->TbJeuxEmplacement);
    ui->TbV_JeuxEmplacement->setEditTriggers(0);
    ui->TbV_JeuxEmplacement->verticalHeader()->setVisible(false);

    this->TbInfoLudoLieux = new QStandardItemModel();
    ui->TbV_InfoLieux->setModel(this->TbInfoLudoLieux);
    ui->TbV_InfoLieux->setEditTriggers(0);
    ui->TbV_InfoLieux->verticalHeader()->setVisible(false);

    ui->Lb_VerifCode->hide();
    ui->Lb_InfoConnexion->hide();
    ui->LE_NouveauCode->setEchoMode(QLineEdit::Password);
    ui->LE_ConfirmerCode->setEchoMode(QLineEdit::Password);
    ui->LE_MotdePasse->setEchoMode(QLineEdit::Password);

    this->MettreAJourBDD();
    this->AfficherInfoDemarrage();

    connect(this->pTitreMembreAjMod, SIGNAL(SignalValider()), this, SLOT(slot_Valider()));
    connect(this->pTypeMembreAjMod, SIGNAL(SignalValider()), this, SLOT(slot_Valider()));
    connect(this->pPaiementMembreAjMod, SIGNAL(SignalValider()), this, SLOT(slot_Valider()));
    connect(this->pTypeEmpruntAjMod, SIGNAL(SignalValider()), this, SLOT(slot_Valider()));
    connect(this->pTypeJeuAjMod, SIGNAL(SignalValider()), this, SLOT(slot_Valider()));
    connect(this->pEtatJeuAjMod, SIGNAL(SignalValider()), this, SLOT(slot_Valider()));
    connect(this->pStatutJeuAjMod, SIGNAL(SignalValider()), this, SLOT(slot_Valider()));
    connect(this->pEmplacementJeuAjMod, SIGNAL(SignalValider()), this, SLOT(slot_Valider()));
    connect(this->pLieuxInfoLudoAjMod, SIGNAL(SignalValider()), this, SLOT(slot_Valider()));

    ui->label->hide();
    ui->label_2->hide();
    ui->lineEdit->hide();
    ui->lineEdit_2->hide();
}

/**
 *  @brief  Destructeur de la classe.
 *
 */
F_Preferences::~F_Preferences()
{
    delete ui;
}

//---------------------------------------------------------------------------
// METHODEs PUBLIQUEs
//---------------------------------------------------------------------------
/**
 *  @brief  Slot qui récupère un signal pour la classe F_Preferences.
 *
 *  @see    MettreAJourBDD()
 */
void F_Preferences::slot_Valider()
{
    this->MettreAJourBDD();
}

void F_Preferences::closeEvent( QCloseEvent * event )
{
    emit( this->SignalFermerFenetre() ) ;
    event->accept() ;
}

//---------------------------------------------------------------------------
// METHODEs PRIVEEs
//---------------------------------------------------------------------------
/**
 *  @brief  Le line edit qui récupère cette fonction se met en forme sous le type XX XX XX XX XX.
 *
 *  @param  sNumero
 *  @return QString
 */
QString F_Preferences::BlocNumeroTelFax(QString sNumero)
{
    QRegExp TelFax("[^0-9]");

    sNumero.replace(TelFax, "");

    for(register int i=2; i<sNumero.length(); i= i+3)
    {
        if(i != sNumero.length())
        {
            sNumero.insert(i, ' ');
        }
    }

    return sNumero;
}

/**
 * @brief   Consulte la BDD et récupère les informations de celle-ci. Il sont ensuite mis dans les champs correspondant.
 *
 * @pre     Etre connecté à  la BDD.
 */
void F_Preferences::AfficherInfoDemarrage()
{
    QSqlQuery RequeteDemarrage;
    QSettings FichierDeConfig("config.ini", QSettings::IniFormat);

    ui->LE_NomBDD->setText(FichierDeConfig.value("BaseDeDonnees/NomDeLaBDD", "config").toString());
    ui->LE_AdresseIP->setText(FichierDeConfig.value("BaseDeDonnees/AdresseIP", "config").toString());
    ui->LE_NomUtilisateur->setText(FichierDeConfig.value("BaseDeDonnees/NomUtilisateur", "config").toString());
    ui->LE_MotdePasse->setText(FichierDeConfig.value("BaseDeDonnees/MotDePasse", "config").toString());
    ui->LE_Port->setText(FichierDeConfig.value("BaseDeDonnees/Port", "config").toString());
    ui->CBx_LieuOrdi->setCurrentIndex(FichierDeConfig.value("Autres/IdLieux", "config").toInt() - 1);


    RequeteDemarrage.exec("SELECT Nom, Adresse, CodePostal, Ville, NumeroTel, NumeroFax, Email, SiteWeb, JeuxAutorises, UniteLocation, "
                          "JourAvantMail, JourRetard, CheminImage, CheminRegle, AdresseServeurSMTP, PortSMTP FROM preferences WHERE IdPreferences = 1");
    RequeteDemarrage.next();

    ui->LE_Nom->setText(RequeteDemarrage.value(0).toString());
    ui->LE_Adresse->setText(RequeteDemarrage.value(1).toString());
    ui->LE_CodePostal->setText(RequeteDemarrage.value(2).toString());
    ui->LE_Ville->setText(RequeteDemarrage.value(3).toString());
    ui->LE_NumeroTel->setText(RequeteDemarrage.value(4).toString());
    ui->LE_NumeroFax->setText(RequeteDemarrage.value(5).toString());
    ui->LE_Mail->setText(RequeteDemarrage.value(6).toString());
    ui->LE_Web->setText(RequeteDemarrage.value(7).toString());
    ui->SBx_JeuxAutorises->setValue(RequeteDemarrage.value(8).toInt());
    ui->DSBx_UniteLocation->setValue(RequeteDemarrage.value(9).toFloat());
    ui->SBx_JourEmail->setValue(RequeteDemarrage.value(10).toInt());
    ui->SBx_JourRetard->setValue(RequeteDemarrage.value(11).toInt());
    ui->LE_CheminImage->setText(RequeteDemarrage.value(12).toString());
    ui->LE_CheminRegle->setText(RequeteDemarrage.value(13).toString());
    ui->LE_AdresseSMTP->setText(RequeteDemarrage.value(14).toString());
    ui->LE_PortSMTP->setText(RequeteDemarrage.value(15).toString());
}

/**
 *  @brief  Fonction qui permet de mettre à  jour le tableau contenant toutes les informations de la BDD.
 *
 *  @pre    Etre connecté à  la BDD.
 */
void F_Preferences::MettreAJourBDD()
{
    QSqlQuery RechercheTableau;
    int nNombreLigne(0);

    //Affichage de la BDD dans le tableau titre qui est dans l'onglet Membres/Emprunts de F_Preferences.
    this->TbMembresTitre->setColumnCount(2);
    this->TbMembresTitre->setRowCount(nNombreLigne);
    this->TbMembresTitre->setHorizontalHeaderItem(0, new QStandardItem("Titre Membre"));
    this->TbMembresTitre->setHorizontalHeaderItem(1, new QStandardItem("Emprunt"));
    ui->TbV_MembresTitre->setColumnWidth(0, 100);
    ui->TbV_MembresTitre->setColumnWidth(1, 65);

    RechercheTableau.exec("SELECT NomTitre, NbrJeuxEmpruntable FROM titremembre");
    while (RechercheTableau.next())
    {
        this->TbMembresTitre->setItem(nNombreLigne, 0, new QStandardItem(RechercheTableau.value(0).toString()));
        this->TbMembresTitre->setItem(nNombreLigne, 1, new QStandardItem(RechercheTableau.value(1).toString()));
        nNombreLigne = nNombreLigne+ 1;
    }
    nNombreLigne = 0;

    //Affichage de la BDD dans le tableau type qui est dans l'onglet Membres/Emprunts de F_Preferences.
    this->TbMembresType->setColumnCount(1);
    this->TbMembresType->setRowCount(nNombreLigne);
    this->TbMembresType->setHorizontalHeaderItem(0, new QStandardItem("Type Membre"));
    ui->TbV_MembresType->setColumnWidth(0, 150);

    RechercheTableau.exec("SELECT TypeMembre FROM typemembres");
    while (RechercheTableau.next())
    {
        this->TbMembresType->setItem(nNombreLigne, 0, new QStandardItem(RechercheTableau.value(0).toString()));
        nNombreLigne = nNombreLigne+ 1;
    }
    nNombreLigne = 0;

    //Affichage de la BDD dans le tableau paiement qui est dans l'onglet Membres/Emprunts de F_Preferences.
    this->TbMembresPaiement->setColumnCount(1);
    this->TbMembresPaiement->setRowCount(nNombreLigne);
    this->TbMembresPaiement->setHorizontalHeaderItem(0, new QStandardItem("Paiement"));
    ui->TbV_MembresPaiement->setColumnWidth(0, 150);

    RechercheTableau.exec("SELECT NomPaiement FROM modepaiement");
    while (RechercheTableau.next())
    {
        this->TbMembresPaiement->setItem(nNombreLigne, 0, new QStandardItem(RechercheTableau.value(0).toString()));
        nNombreLigne = nNombreLigne+ 1;
    }
    nNombreLigne = 0;

    //Affichage de la BDD dans le tableau type d'emprunt qui est dans l'onglet Membres/Emprunts de F_Preferences.
    this->TbEmpruntType->setColumnCount(2);
    this->TbEmpruntType->setRowCount(nNombreLigne);
    this->TbEmpruntType->setHorizontalHeaderItem(0, new QStandardItem("Type Emprunt"));
    this->TbEmpruntType->setHorizontalHeaderItem(1, new QStandardItem("Durée"));
    ui->TbV_EmpruntType->setColumnWidth(0, 180);
    ui->TbV_EmpruntType->setColumnWidth(1, 90);

    RechercheTableau.exec("SELECT TypeEmprunt,DureeEmprunt FROM typeemprunt");
    while (RechercheTableau.next())
    {
        this->TbEmpruntType->setItem(nNombreLigne, 0, new QStandardItem(RechercheTableau.value(0).toString()));
        this->TbEmpruntType->setItem(nNombreLigne, 1, new QStandardItem(RechercheTableau.value(1).toString()));
        nNombreLigne = nNombreLigne+ 1;
    }

    nNombreLigne = 0;

    //Affichage de la BDD dans le tableau type qui est dans l'onglet Jeux de F_Preferences.
    this->TbJeuxType->setColumnCount(2);
    this->TbJeuxType->setRowCount(nNombreLigne);
    this->TbJeuxType->setHorizontalHeaderItem(0, new QStandardItem("Type Jeu"));
    this->TbJeuxType->setHorizontalHeaderItem(1, new QStandardItem("Classification"));
    ui->TbV_JeuxType->setColumnWidth(0, 180);
    ui->TbV_JeuxType->setColumnWidth(1, 90);

    RechercheTableau.exec("SELECT * FROM typejeux");
    while (RechercheTableau.next())
    {
        this->TbJeuxType->setItem(nNombreLigne, 0, new QStandardItem(RechercheTableau.value(0).toString()));
        this->TbJeuxType->setItem(nNombreLigne, 1, new QStandardItem(RechercheTableau.value(1).toString()));
        nNombreLigne = nNombreLigne+ 1;
    }
    nNombreLigne = 0;

    //Affichage de la BDD dans le tableau etat qui est dans l'onglet Jeux de F_Preferences.
    this->TbJeuxEtat->setColumnCount(1);
    this->TbJeuxEtat->setRowCount(nNombreLigne);
    this->TbJeuxEtat->setHorizontalHeaderItem(0, new QStandardItem("Etat"));
    ui->TbV_JeuxEtat->setColumnWidth(0, 180);

    RechercheTableau.exec("SELECT Etat FROM etatsjeu");
    while (RechercheTableau.next())
    {
        this->TbJeuxEtat->setItem(nNombreLigne, 0, new QStandardItem(RechercheTableau.value(0).toString()));
        nNombreLigne = nNombreLigne+ 1;
    }
    nNombreLigne = 0;

    //Affichage de la BDD dans le tableau statut qui est dans l'onglet Jeux de F_Preferences.
    this->TbJeuxStatut->setColumnCount(1);
    this->TbJeuxStatut->setRowCount(nNombreLigne);
    this->TbJeuxStatut->setHorizontalHeaderItem(0, new QStandardItem("Statut"));
    ui->TbV_JeuxStatut->setColumnWidth(0, 180);

    RechercheTableau.exec("SELECT StatutJeu FROM statutjeux");
    while (RechercheTableau.next())
    {
        this->TbJeuxStatut->setItem(nNombreLigne, 0, new QStandardItem(RechercheTableau.value(0).toString()));
        nNombreLigne = nNombreLigne+ 1;
    }
    nNombreLigne = 0;

    //Affichage de la BDD dans le tableau emplacement qui est dans l'onglet Jeux de F_Preferences.
    this->TbJeuxEmplacement->setColumnCount(1);
    this->TbJeuxEmplacement->setRowCount(nNombreLigne);
    this->TbJeuxEmplacement->setHorizontalHeaderItem(0, new QStandardItem("Emplacement"));
    ui->TbV_JeuxEmplacement->setColumnWidth(0, 180);

    RechercheTableau.exec("SELECT Nom FROM emplacement");
    while (RechercheTableau.next())
    {
        this->TbJeuxEmplacement->setItem(nNombreLigne, 0, new QStandardItem(RechercheTableau.value(0).toString()));
        nNombreLigne = nNombreLigne+ 1;
    }
    nNombreLigne=0;

    //Affichage de la BDD dans le tableau lieu qui est dans l'onglet Info Ludo de F_Preferences.
    this->TbInfoLudoLieux->setColumnCount(1);
    this->TbInfoLudoLieux->setRowCount(nNombreLigne);
    this->TbInfoLudoLieux->setHorizontalHeaderItem(0, new QStandardItem("Lieu"));
    ui->TbV_InfoLieux->setColumnWidth(0, 180);

    RechercheTableau.exec("SELECT NomLieux FROM lieux");
    while (RechercheTableau.next())
    {
        this->TbInfoLudoLieux->setItem(nNombreLigne, 0, new QStandardItem(RechercheTableau.value(0).toString()));
        nNombreLigne = nNombreLigne+ 1;
    }

    //Affichage de la BDD dans le combo box qui est dans l'onglet Info Ludo de F_Preferences.
    ui->CBx_LieuOrdi->clear();
    ui->CBx_LieuOrdi->addItem("- - - - -");
    RechercheTableau.exec("SELECT NomLieux FROM lieux");
    RechercheTableau.next();
    while(RechercheTableau.next())
    {
        ui->CBx_LieuOrdi->addItem(RechercheTableau.value(0).toString());
    }

    // Redimensionner les colonnes des tableaux en fonction du contenu des colonnes
    ui->TbV_MembresTitre->resizeColumnsToContents();
    ui->TbV_MembresType->resizeColumnsToContents();
    //ui->TbV_JeuxType->resizeColumnsToContents();
    ui->TbV_JeuxEtat->resizeColumnsToContents();
    ui->TbV_JeuxStatut->resizeColumnsToContents();
    ui->TbV_MembresPaiement->resizeColumnsToContents();
    ui->TbV_EmpruntType->resizeColumnsToContents();
    ui->TbV_InfoLieux->resizeColumnsToContents();
    ui->TbV_JeuxEmplacement->resizeColumnsToContents();
}

/**
 *  @brief  Permet d'enregistrer les actions effectuées depuis l'ouverture de la F_Preferences.
 *
 *  @pre    Etre connecté à  la BDD.
 */
void F_Preferences::on_Bt_Enregistrer_clicked()
{
    QSettings FichierDeConfig("config.ini", QSettings::IniFormat);
    QSqlQuery RequeteEnregistrer;
    QSqlQuery RequeteCombo;

    // Vérifie si les préférences ont été au moins une fois enregistrée.
    // Si la table est vide, il faut un INSERT sinon un UPDATE
    QSqlQuery RequeteVerification ("SELECT IdPreferences FROM preferences");
    unsigned int nIdPreferences;
    RequeteVerification.next();
    nIdPreferences = RequeteVerification.value(0).toInt();

    if(nIdPreferences == 0)
    {
        RequeteEnregistrer.prepare("INSERT INTO preferences (IdPreferences, Nom, Adresse, CodePostal, Ville, NumeroTel, NumeroFax, Email, SiteWeb, JeuxAutorises, UniteLocation, JourRetard, JourAvantMail, CheminImage, CheminRegle, AdresseServeurSMTP, PortSMTP)"
                                   "VALUES (:IdPreferences, :Nom, :Adresse, :CodePostal, :Ville, :NumeroTel, :NumeroFax, :Email, :SiteWeb, :JeuxAutorises, :UniteLocation, :JourRetard, :JourAvantMail, :CheminImage, :CheminRegle, :AdresseServeurSMTP, :PortSMTP)");
        RequeteEnregistrer.bindValue("IdPreferences", 1);
        RequeteEnregistrer.bindValue(":Nom", ui->LE_Nom->text());
        RequeteEnregistrer.bindValue(":Adresse", ui->LE_Adresse->text());
        RequeteEnregistrer.bindValue(":CodePostal", ui->LE_CodePostal->text().toInt());
        RequeteEnregistrer.bindValue(":Ville", ui->LE_Ville->text());
        RequeteEnregistrer.bindValue(":NumeroTel", ui->LE_NumeroTel->text());
        RequeteEnregistrer.bindValue(":NumeroFax", ui->LE_NumeroFax->text());
        RequeteEnregistrer.bindValue(":Email", ui->LE_Mail->text());
        RequeteEnregistrer.bindValue(":SiteWeb", ui->LE_Web->text());
        RequeteEnregistrer.bindValue(":JeuxAutorises", ui->SBx_JeuxAutorises->value());
        RequeteEnregistrer.bindValue(":UniteLocation", ui->DSBx_UniteLocation->value());
        RequeteEnregistrer.bindValue(":JourRetard", ui->SBx_JourRetard->value());
        RequeteEnregistrer.bindValue(":JourAvantMail", ui->SBx_JourEmail->value());
        RequeteEnregistrer.bindValue(":CheminImage", ui->LE_CheminImage->text());
        RequeteEnregistrer.bindValue(":CheminRegle", ui->LE_CheminRegle->text());
        RequeteEnregistrer.bindValue(":AdresseServeurSMTP", ui->LE_AdresseSMTP->text());
        RequeteEnregistrer.bindValue(":PortSMTP", ui->LE_PortSMTP->text().toInt());
        RequeteEnregistrer.exec();        
    }
    else
    {
        RequeteEnregistrer.prepare("UPDATE preferences SET Nom=:Nom, Adresse=:Adresse, CodePostal=:CodePostal, Ville=:Ville, NumeroTel=:NumeroTel, NumeroFax=:NumeroFax, Email=:Email, SiteWeb=:SiteWeb, JeuxAutorises=:JeuxAutorises,"
                                   "UniteLocation=:UniteLocation, JourAvantMail=:JourAvantMail, JourRetard=:JourRetard, CheminImage=:CheminImage, CheminRegle=:CheminRegle, AdresseServeurSMTP=:AdresseServeurSMTP, PortSMTP=:PortSMTP WHERE IdPreferences = 1");
        RequeteEnregistrer.bindValue(":Nom", ui->LE_Nom->text());
        RequeteEnregistrer.bindValue(":Adresse", ui->LE_Adresse->text());
        RequeteEnregistrer.bindValue(":CodePostal", ui->LE_CodePostal->text().toInt());
        RequeteEnregistrer.bindValue(":Ville", ui->LE_Ville->text());
        RequeteEnregistrer.bindValue(":NumeroTel", ui->LE_NumeroTel->text());
        RequeteEnregistrer.bindValue(":NumeroFax", ui->LE_NumeroFax->text());
        RequeteEnregistrer.bindValue(":Email", ui->LE_Mail->text());
        RequeteEnregistrer.bindValue(":SiteWeb", ui->LE_Web->text());
        RequeteEnregistrer.bindValue(":JeuxAutorises", ui->SBx_JeuxAutorises->value());
        RequeteEnregistrer.bindValue(":UniteLocation", ui->DSBx_UniteLocation->value());
        RequeteEnregistrer.bindValue(":JourRetard", ui->SBx_JourRetard->value());
        RequeteEnregistrer.bindValue(":JourAvantMail", ui->SBx_JourEmail->value());
        RequeteEnregistrer.bindValue(":CheminImage", ui->LE_CheminImage->text());
        RequeteEnregistrer.bindValue(":CheminRegle", ui->LE_CheminRegle->text());
        RequeteEnregistrer.bindValue(":AdresseServeurSMTP", ui->LE_AdresseSMTP->text());
        RequeteEnregistrer.bindValue(":PortSMTP", ui->LE_PortSMTP->text().toInt());
        RequeteEnregistrer.exec();
    }
    // écriture de certaines données dans le fichier de configuration config.ini
    RequeteCombo.prepare("SELECT * FROM lieux WHERE NomLieux=:NomLieux");
    RequeteCombo.bindValue(":NomLieux", ui->CBx_LieuOrdi->currentText());
    RequeteCombo.exec();
    while(RequeteCombo.next())
    {
        FichierDeConfig.beginGroup("Autres");
        FichierDeConfig.setValue("IdLieux", RequeteCombo.value(0).toInt());
        FichierDeConfig.setValue("NomLieux", RequeteCombo.value(1).toString());
        FichierDeConfig.endGroup();
        // TO DO Ecrire ici tout ce qui doit aller dans le fichier ini
        // string Qt::md5 ( data )
    }

    emit( this->SignalFermerFenetre() ) ;
    this->close();
    ui->Lb_ConfirmerCode->clear();
    ui->Lb_InfoConnexion->clear();
}

/**
 *  @brief  Permet d'annuler tous les actions effectuées depuis l'ouverture de la F_Preferences.
 *
 */
void F_Preferences::on_Bt_Annuler_clicked()
{
    emit( this->SignalFermerFenetre() ) ;
    this->close();
}

// Onglet Préférences Générales ---------------------------------------------
/**
 *  @brief  Verifie si le nouveau code administrateur est correct.
 *
 *  @pre    Etre connecté à  la BDD.
 */
void F_Preferences::on_Bt_ValiderCode_clicked()
{
    QSqlQuery RequeteMDP;
    QString sNouveauCode;
    QString sConfirmerCode;
    ui->Lb_VerifCode->show();

    sNouveauCode = ui->LE_NouveauCode->text();
    sConfirmerCode = ui->LE_ConfirmerCode->text();

    if(sNouveauCode.compare(sConfirmerCode) == 0)
    {
        ui->Lb_VerifCode->setText("<font color=green> Le nouveau mot de passe est confirmé. </font>");

        RequeteMDP.prepare("UPDATE preferences SET CodeAdmin=:CodeAdmin WHERE IdPreferences = 1");
        RequeteMDP.bindValue(":CodeAdmin", ui->LE_ConfirmerCode->text());
        RequeteMDP.exec();
        RequeteMDP.next();
    }
    else
    {
        ui->Lb_VerifCode->setText("<font color=red> Erreur, les mots de passe sont différents. </font>");
    }
}

/**
 *  @brief  Affiche le manuel d'utilisation du programme.
 *
 */
void F_Preferences::on_Bt_Aide_clicked()
{
    QString sAide;

    sAide = QDir::currentPath() + "/Manuel.pdf";
    QDesktopServices::openUrl(QUrl::fromLocalFile(sAide));
}

// Onglet Réseaux -----------------------------------------------------------
/**
 *  @brief  Test la nouvelle connection à  la nouvelle BDD.
 *
 */
void F_Preferences::on_Bt_Connection_clicked()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    ui->Lb_InfoConnexion->show();

    db.setDatabaseName(ui->LE_NomBDD->text());
    db.setHostName(ui->LE_AdresseIP->text());
    db.setUserName(ui->LE_NomUtilisateur->text());
    db.setPassword(ui->LE_MotdePasse->text());
    db.setPort(ui->LE_Port->text().toInt());

    if(db.open())
    {
        ui->Lb_InfoConnexion->setText("<font color=green> La connexion est réussi. </font>");

        QSettings FichierDeConfig("config.ini", QSettings::IniFormat);

        FichierDeConfig.beginGroup("BaseDeDonnees");
        FichierDeConfig.setValue("NomDeLaBDD", ui->LE_NomBDD->text());
        FichierDeConfig.setValue("AdresseIP", ui->LE_AdresseIP->text());
        FichierDeConfig.setValue("NomUtilisateur", ui->LE_NomUtilisateur->text());
        FichierDeConfig.setValue("MotDePasse", ui->LE_MotdePasse->text());
        FichierDeConfig.setValue("Port", ui->LE_Port->text());
        FichierDeConfig.endGroup();
    }
    else
    {
        cerr << "Erreur : " << db.lastError().text().toStdString() << endl;
        ui->Lb_InfoConnexion->setText("<font color=red> La connexion a échouée. Veuiller réassayer. </font>");
    }
}

// Onglet Membre ------------------------------------------------------------
/**
 *  @brief  Sélectionne la ligne entière du tableau titre de membre.
 *
 *  @param  index
 */
void F_Preferences::on_TbV_MembresTitre_clicked(const QModelIndex &index)
{
    this->sTitreMembre = this->TbMembresTitre->index(index.row(), 0).data().toString();
}

/**
 *  @brief  Sélectionne la ligne entière du tableau type de membre.
 *
 *  @param  index
 */
void F_Preferences::on_TbV_MembresType_clicked(const QModelIndex &index)
{
    this->sTypeMembre = this->TbMembresType->index(index.row(), 0).data().toString();
}

void F_Preferences::on_TbV_MembresPaiement_clicked(const QModelIndex &index)
{
    this->sPaiementMembre = this->TbMembresPaiement->index(index.row(), 0).data().toString();
}

void F_Preferences::on_TbV_EmpruntType_clicked(const QModelIndex &index)
{
    this->sTypeEmprunt = this->TbEmpruntType->index(index.row(), 0).data().toString();
}

/**
 *  @brief  Permet d'ajouter un titre d'un membre.
 *
 *  @pre    Etre connecté à  la BDD.
 */
void F_Preferences::on_Bt_AjouterMembreTitre_clicked()
{
    this->pTitreMembreAjMod->Ajouter();
}

/**
 *  @brief   Permet de modifier un titre d'un membre.
 *
 *  @pre    Etre connecté à  la BDD.
 */
void F_Preferences::on_Bt_ModifierMembreTitre_clicked()
{
    QSqlQuery RequeteModifier;

    RequeteModifier.prepare("SELECT NomTitre FROM titremembre WHERE NomTitre=:NomTitre");
    RequeteModifier.bindValue(":NomTitre", this->sTitreMembre);
    RequeteModifier.exec();
    RequeteModifier.next();

    this->pTitreMembreAjMod->Modifier(RequeteModifier.value(0).toString());
}

/**
 *  @brief   Permet de supprimer un titre d'un membre.
 *
 *  @pre    Etre connecté à  la BDD.
 *  @see    MettreAJourBDD()
 */
void F_Preferences::on_Bt_SupprimerMembreTitre_clicked()
{
    QSqlQuery RequeteSupprimer;

    if(QMessageBox::question(this, "Suppression", "Etes-vous sur de vouloir supprimer ce titre ?", "Oui", "Non") == 0)
    {
        RequeteSupprimer.prepare("DELETE FROM titremembre WHERE NomTitre=:NomTitre");
        RequeteSupprimer.bindValue(":NomTitre", this->sTitreMembre);
        RequeteSupprimer.exec();
        RequeteSupprimer.next();
    }

    this->MettreAJourBDD();
}

/**
 *  @brief  Permet d'ajouter un type de membre.
 *
 */
void F_Preferences::on_Bt_AjouterMembreType_clicked()
{
    this->pTypeMembreAjMod->Ajouter();
}

/**
 *  @brief Permet de modifier un type de membre.
 *
 */
void F_Preferences::on_Bt_ModifierMembreType_clicked()
{
    QSqlQuery RequeteModifier;

    RequeteModifier.prepare("SELECT TypeMembre FROM typemembres WHERE TypeMembre=:TypeMembre");
    RequeteModifier.bindValue(":TypeMembre", this->sTypeMembre);
    RequeteModifier.exec();
    RequeteModifier.next();

    this->pTypeMembreAjMod->Modifier(RequeteModifier.value(0).toString());
}

/**
 *  @brief  Permet de supprimer un type de membre.
 *
 *  @see MettreAJourBDD()
 */
void F_Preferences::on_Bt_SupprimerMembreType_clicked()
{
    QSqlQuery RequeteSupprimer;

    if(QMessageBox::question(this, "Suppression", "Etes-vous sur de vouloir supprimer ce type de membre ?", "Oui", "Non") == 0)
    {
        RequeteSupprimer.prepare("DELETE FROM typemembres WHERE TypeMembre=:TypeMembre");
        RequeteSupprimer.bindValue(":TypeMembre", this->sTypeMembre);
        RequeteSupprimer.exec();
        RequeteSupprimer.next();
    }

    this->MettreAJourBDD();
}

void F_Preferences::on_Bt_AjouterMembrePaiement_clicked()
{
    this->pPaiementMembreAjMod->Ajouter();
}

void F_Preferences::on_Bt_ModifierMembrePaiement_clicked()
{
    QSqlQuery RequeteModifier;

    RequeteModifier.prepare("SELECT NomPaiement FROM modepaiement WHERE NomPaiement=:NomPaiement");
    RequeteModifier.bindValue(":NomPaiement", this->sPaiementMembre);
    RequeteModifier.exec();
    RequeteModifier.next();

    this->pPaiementMembreAjMod->Modifier(RequeteModifier.value(0).toString());
}

void F_Preferences::on_Bt_SupprimerMembrePaiement_clicked()
{
    QSqlQuery RequeteSupprimer;

    if(QMessageBox::question(this, "Suppression", "Etes-vous sur de vouloir supprimer ce paiement ?", "Oui", "Non") == 0)
    {
        RequeteSupprimer.prepare("DELETE FROM modepaiement WHERE NomPaiement=:NomPaiement");
        RequeteSupprimer.bindValue(":NomPaiement", this->sPaiementMembre);
        RequeteSupprimer.exec();
        RequeteSupprimer.next();
    }

    this->MettreAJourBDD();
}

void F_Preferences::on_Bt_AjouterEmpruntType_clicked()
{
    this->pTypeEmpruntAjMod->Ajouter();
}

void F_Preferences::on_Bt_ModifierEmpruntType_clicked()
{
    QSqlQuery RequeteModifier;

    RequeteModifier.prepare("SELECT TypeEmprunt FROM typeemprunt WHERE TypeEmprunt=:TypeEmprunt");
    RequeteModifier.bindValue(":TypeEmprunt", this->sTypeEmprunt);
    RequeteModifier.exec();
    RequeteModifier.next();

    this->pTypeEmpruntAjMod->Modifier(RequeteModifier.value(0).toString());
}

void F_Preferences::on_Bt_SupprimerEmpruntType_clicked()
{
    QSqlQuery RequeteSupprimer;

    if(QMessageBox::question(this, "Suppression", "Etes-vous sur de vouloir supprimer ce type d'emprunt ?", "Oui", "Non") == 0)
    {
        RequeteSupprimer.prepare("DELETE FROM typeemprunt WHERE TypeEmprunt=:TypeEmprunt");
        RequeteSupprimer.bindValue(":TypeEmprunt", this->sTypeEmprunt);
        RequeteSupprimer.exec();
    }

    this->MettreAJourBDD();
}

// Onglet Jeux - Partie Type. -----------------------------------------------
/**
 *  @brief  Sélectionne la ligne entière du tableau type de jeu.
 *
 *  @param  index
 */
void F_Preferences::on_TbV_JeuxType_clicked(const QModelIndex &index)
{
    this->sTypeJeux = this->TbJeuxType->index(index.row(), 0).data().toString();
}

/**
 *  @brief  Sélectionne la ligne entière du tableau état de jeu.
 *
 *  @param  index
 */
void F_Preferences::on_TbV_JeuxEtat_clicked(const QModelIndex &index)
{
    this->sEtatJeux = this->TbJeuxEtat->index(index.row(), 0).data().toString();
}

/**
 *  @brief  Sélectionne la ligne entière du tableau statut de jeu.
 *
 *  @param  index
 */
void F_Preferences::on_TbV_JeuxStatut_clicked(const QModelIndex &index)
{
    this->sStatutJeux = this->TbJeuxStatut->index(index.row(), 0).data().toString();
    if(index.row() < 4)
    {
        ui->Bt_ModifierStatut->setEnabled(false);
        ui->Bt_SupprimerStatut->setEnabled(false);
    }
    else
    {
        ui->Bt_ModifierStatut->setEnabled(true);
        ui->Bt_SupprimerStatut->setEnabled(true);
    }
}

/**
 *  @brief  Sélectionne la ligne entière du tableau emplacement du jeu.
 *
 *  @param  index
 */
void F_Preferences::on_TbV_JeuxEmplacement_clicked(const QModelIndex &index)
{
    this->sEmplacementJeux = this->TbJeuxEmplacement->index(index.row(), 0).data().toString();
}

/**
 *  @brief  Permet d'ajouter un type de jeu.
 *
 */
void F_Preferences::on_Bt_AjouterJeuxType_clicked()
{
    this->pTypeJeuAjMod->Ajouter();
}

/**
 *  @brief  Permet de modifier un type de jeu.
 *
 */
void F_Preferences::on_Bt_ModifierJeuxType_clicked()
{
    QSqlQuery RequeteModifier;

    RequeteModifier.prepare("SELECT TypeJeux FROM typejeux WHERE TypeJeux=:TypeJeux");
    RequeteModifier.bindValue(":TypeJeux", this->sTypeJeux);
    RequeteModifier.exec();
    RequeteModifier.next();

    this->pTypeJeuAjMod->Modifier(RequeteModifier.value(0).toString());
}

/**
 *  @brief  Permet de supprimer un type de jeu.
 *
 *  @see    MettreAJourBDD()
 */
void F_Preferences::on_Bt_SupprimerJeuxType_clicked()
{
    QSqlQuery RequeteSupprimer;

    if(QMessageBox::question(this, "Suppression", "Etes-vous sur de vouloir supprimer ce type de jeu ?", "Oui", "Non") == 0)
    {
        RequeteSupprimer.prepare("DELETE FROM typejeux WHERE TypeJeux=:TypeJeux");
        RequeteSupprimer.bindValue(":NomTitre", this->sTypeJeux);
        RequeteSupprimer.exec();
    }

    this->MettreAJourBDD();
}

/**
 *  @brief  Permet d'ajouter un état de jeu.
 *
 */
void F_Preferences::on_Bt_AjouterEtat_clicked()
{
    this->pEtatJeuAjMod->Ajouter();
}

/**
 *  @brief  Permet de modifier un état de jeu.
 *
 */
void F_Preferences::on_Bt_ModifierEtat_clicked()
{
    QSqlQuery RequeteModifier;

    RequeteModifier.prepare("SELECT Etat FROM etatsjeu WHERE Etat=:Etat");
    RequeteModifier.bindValue(":Etat", this->sEtatJeux);
    RequeteModifier.exec();
    RequeteModifier.next();

    this->pEtatJeuAjMod->Modifier(RequeteModifier.value(0).toString());
}

/**
 *  @brief  Permet de supprimer un état de jeu.
 *
 *  @see    MettreAJourBDD()
 */
void F_Preferences::on_Bt_SupprimerEtat_clicked()
{
    QSqlQuery RequeteSupprimer;

    if(QMessageBox::question(this, "Suppression", "Etes-vous sur de vouloir supprimer cet état de jeu ?", "Oui", "Non") == 0)
    {
        RequeteSupprimer.prepare("DELETE FROM etatsjeu WHERE Etat=:Etat");
        RequeteSupprimer.bindValue(":Etat", this->sEtatJeux);
        RequeteSupprimer.exec();
    }

    this->MettreAJourBDD();
}

/**
 *  @brief  Permet d'ajouter un statut de jeu.
 *
 */
void F_Preferences::on_Bt_AjouterStatut_clicked()
{
    this->pStatutJeuAjMod->Ajouter();
}

/**
 *  @brief  Permet de modifier un statut de jeu.
 *
 */
void F_Preferences::on_Bt_ModifierStatut_clicked()
{
    QSqlQuery RequeteModifier;

    RequeteModifier.prepare("SELECT StatutJeu FROM statutjeux WHERE StatutJeu=:StatutJeu");
    RequeteModifier.bindValue(":StatutJeu", this->sStatutJeux);
    RequeteModifier.exec();
    RequeteModifier.next();

    this->pStatutJeuAjMod->Modifier(RequeteModifier.value(0).toString());
}

/**
 *  @brief  Permet de supprimer un statut de jeu.
 *
 *  @see    MettreAJourBDD()
 */
void F_Preferences::on_Bt_SupprimerStatut_clicked()
{
    QSqlQuery RequeteSupprimer;

    if(QMessageBox::question(this, "Suppression", "Etes-vous sur de vouloir supprimer ce statut de jeu ?", "Oui", "Non") == 0)
    {
        RequeteSupprimer.prepare("DELETE FROM statutjeux WHERE StatutJeu=:StatutJeu");
        RequeteSupprimer.bindValue(":StatutJeu", this->sStatutJeux);
        RequeteSupprimer.exec();
    }

    this->MettreAJourBDD();
}

/**
 *  @brief  Permet d'ajouter un emplacement du jeu.
 *
 */
void F_Preferences::on_Bt_AjouterEmplacement_clicked()
{
    this->pEmplacementJeuAjMod->Ajouter();
}

/**
 *  @brief  Permet de modifier un emplacement du jeu.
 *
 */
void F_Preferences::on_Bt_ModifierEmplacement_clicked()
{
    QSqlQuery RequeteModifier;

    RequeteModifier.prepare("SELECT Nom FROM emplacement WHERE Nom=:Nom");
    RequeteModifier.bindValue(":Nom", this->sEmplacementJeux);
    RequeteModifier.exec();
    RequeteModifier.next();

    this->pEmplacementJeuAjMod->Modifier(RequeteModifier.value(0).toString());
}

/**
 *  @brief  Permet de supprimer un emplacement du jeu.
 *
 *  @see    MettreAJourBDD()
 */
void F_Preferences::on_Bt_SupprimerEmplacement_clicked()
{
    QSqlQuery RequeteSupprimer;

    if(QMessageBox::question(this, "Suppression", "Etes-vous sur de vouloir supprimer cette emplacement de jeu ?", "Oui", "Non") == 0)
    {
        RequeteSupprimer.prepare("DELETE FROM emplacement WHERE Nom=:Nom");
        RequeteSupprimer.bindValue(":StatutJeu", this->sEmplacementJeux);
        RequeteSupprimer.exec();
    }

    this->MettreAJourBDD();
}

// Onglet Jeux - Chemin document. -------------------------------------------
/**
 *  @brief  Permet de définir le chemin des règles des jeux.
 *
 */
void F_Preferences::on_Bt_ParcourirRegle_clicked()
{
    QString NomCheminRegle;

    NomCheminRegle = QFileDialog::getExistingDirectory(this, tr("Ouvrir le répertoire ..."), QDir::currentPath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    ui->LE_CheminRegle->setText(NomCheminRegle);
}

/**
 *  @brief  Permet de définir le chemin des images des jeux.
 *
 */
void F_Preferences::on_Bt_ParcourirImage_clicked()
{
    QString NomCheminImage;

    NomCheminImage = QFileDialog::getExistingDirectory(this, tr("Ouvrir le répertoire ..."), QDir::currentPath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    ui->LE_CheminImage->setText(NomCheminImage);
}

// Onglet Information Ludo. -------------------------------------------------
/**
 *  @brief  Sélectionne la ligne entière du tableau lieu de l'info ludo.
 *
 *  @param  index
 */
void F_Preferences::on_TbV_InfoLieux_clicked(const QModelIndex &index)
{
    this->sLieuxInfoLudo = this->TbInfoLudoLieux->index(index.row(), 0).data().toString();
}

/**
 *  @brief  Permet d'ajouter un lieu de l'info ludo.
 *
 */
void F_Preferences::on_Bt_AjouterLieux_clicked()
{
    this->pLieuxInfoLudoAjMod->Ajouter();
}

/**
 *  @brief  Permet de modifier un lieu de l'info ludo.
 *
 */
void F_Preferences::on_Bt_ModifierLieux_clicked()
{
    QSqlQuery RequeteModifier;

    RequeteModifier.prepare("SELECT NomLieux FROM lieux WHERE NomLieux=:NomLieux");
    RequeteModifier.bindValue(":NomLieux", this->sLieuxInfoLudo);
    RequeteModifier.exec();
    RequeteModifier.next();

    this->pLieuxInfoLudoAjMod->Modifier(RequeteModifier.value(0).toString());
}

/**
 *  @brief  Permet de supprimer un lieu de l'info ludo.
 *
 *  @see    MettreAJourBDD()
 */
void F_Preferences::on_Bt_SupprimerLieux_clicked()
{
    QSqlQuery RequeteSupprimer;

    if(QMessageBox::question(this, "Suppression", "Etes-vous sur de vouloir supprimer ce lieu de l'info ludo ?", "Oui", "Non") == 0)
    {
        RequeteSupprimer.prepare("DELETE FROM lieux WHERE NomLieux=:NomLieux");
        RequeteSupprimer.bindValue(":NomLieux", this->sLieuxInfoLudo);
        RequeteSupprimer.exec();
    }

    this->MettreAJourBDD();
}

/**
 *  @brief  Récupèration de la fonction pour l'affichage du style XX XX XX XX XX.
 *
 *  @param  arg1
 */
void F_Preferences::on_LE_NumeroTel_textEdited(const QString &arg1)
{
    ui->LE_NumeroTel->setText(this->BlocNumeroTelFax(arg1));
}

/**
 *  @brief  Récupèration de la fonction pour l'affichage du style XX XX XX XX XX.
 *
 *  @param  arg1
 */
void F_Preferences::on_LE_NumeroFax_textEdited(const QString &arg1)
{
    ui->LE_NumeroFax->setText(this->BlocNumeroTelFax(arg1));
}

//---------------------------------------------------------------------------
