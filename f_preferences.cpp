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
#include <QtWidgets>
#include <QtSql>
#include <fenv.h>
#include <QtDebug>
using namespace std;
//------------------------------------------------------------------------------

// En-tête propre à  l'application ----------------------------------------------
#include "f_preferences.h"
#include "ui_f_preferences.h"
#include "fonctions_globale.h"
//------------------------------------------------------------------------------

QMap<QString, QString> F_Preferences::TbPreferences;
/**
 *  @brief  Constructeur de la classe.
 *
 *  @param  parent
 *  @see    AfficherTousLesTableaux()
 *  @see    AfficherAutresInformations()
 */
F_Preferences::F_Preferences(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::F_Preferences)
{
    ui->setupUi(this);
    // pour des problèmes de valeur qui n'arrive pas zéro quand on fait des calculs avec des flottants
    fesetround(FE_TOWARDZERO);

    this->pPopUpCLESTTEM = new F_PopUpCLESTTEM(this);
    this->pPopUpCLESTTEM->setWindowModality(Qt::ApplicationModal);

    this->TbCLESTTEM = new QStandardItemModel();
    this->TbV_CLESTTEM =  new QTableView();
    this->TbV_CLESTTEM->setModel(this->TbCLESTTEM);
    this->TbV_CLESTTEM->setEditTriggers(0);
    this->TbV_CLESTTEM->verticalHeader()->setVisible(false);
    //Affichage de la BDD dans le tableau titre qui est dans l'onglet Membres/Emprunts de F_Preferences.
    this->TbCLESTTEM->setColumnCount(2);
    this->TbCLESTTEM->setRowCount(0);

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

    this->TbMembresActivite = new QStandardItemModel();
    ui->TbV_MembresActivite->setModel(this->TbMembresActivite);
    ui->TbV_MembresActivite->setEditTriggers(0);
    ui->TbV_MembresActivite->verticalHeader()->setVisible(false);

    this->TbJeuxMotCle = new QStandardItemModel();
    ui->TbV_JeuxMotCle->setModel(this->TbJeuxMotCle);
    ui->TbV_JeuxMotCle->setEditTriggers(0);
    ui->TbV_JeuxMotCle->verticalHeader()->setVisible(false);

    ui->Lb_VerifCode->hide();
    ui->Lb_InfoConnexion->hide();
    ui->LE_NouveauCode->setEchoMode(QLineEdit::Password);
    ui->LE_ConfirmerCode->setEchoMode(QLineEdit::Password);
    ui->LE_MotdePasse->setEchoMode(QLineEdit::Password);

    sCheminConfig=TrouverFichierConfig();
    QSettings FichierDeConfig(sCheminConfig, QSettings::IniFormat);
    FichierDeConfig.beginGroup("Ludopen");
    QStringList keys = FichierDeConfig.childKeys();
    foreach (QString key, keys)
    {
        TbPreferences[key]=FichierDeConfig.value(key).toString();
    }
    if(TbPreferences["TaillePolice"]==0)
    {
        TbPreferences["TaillePolice"]="11";
    }

    connect(this->pPopUpCLESTTEM, SIGNAL(SignalValider()), this, SLOT(slot_Valider()));

    ui->label->hide();
    ui->label_2->hide();
    ui->lineEdit->hide();
    ui->lineEdit_2->hide();
}

/**
 *  @brief  Retourne l'emplacement du fichier de config trouvé par TrouverFichierConfig
 *
 */
QString F_Preferences::ObtenirsCheminConfig()
{
    return sCheminConfig;
}

/**
 *  @brief  Retourne l'emplacement du fichier de config en fonction des arguments passés à Ludopen
 *
 */
QString F_Preferences::TrouverFichierConfig()
{
    QStringList arg=QApplication::arguments();
    QRegExp CheminRegExp("/config:(.*)");
    int CheminIndex=QApplication::arguments().indexOf(CheminRegExp);
    if(CheminIndex==-1)
    {
        return QDesktopServices::storageLocation(QDesktopServices::DataLocation) +
                "ludopen" +QDir::separator()+"config.ini";
    }
    else
    {
        return CheminRegExp.cap(1);
    }
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
 *  @see    AfficherTousLesTableaux()
 */
void F_Preferences::slot_Valider()
{
    this->AfficherTousLesTableaux();
}

void F_Preferences::closeEvent( QCloseEvent * event )
{
    AfficherAutresInformations();
    //on_Bt_Connection_clicked();
    emit( this->SignalFermerFenetre());
    event->accept() ;
}

/**
 *  @brief  Permet de changer d'onglet
 *
 *  @param  nOnglet Permet de choisir l'onglet
 */
void F_Preferences::SelectionnerOnglet(int nOnglet)
{
    ui->tabWidget->setCurrentIndex( nOnglet ) ;
}

//---------------------------------------------------------------------------
// METHODEs PRIVEEs
//---------------------------------------------------------------------------

/**
 *  @brief  Rechargement des préférences à partir de la BDD
 *
 */
void F_Preferences::ChargerPreferencesBDD()
{
    TbPreferences.clear();
    QSqlQuery RequeteDemarrage;

    RequeteDemarrage.exec("SELECT NomChamps,Valeur FROM preferences");
    if ( ! RequeteDemarrage.exec() )
    {
       qDebug()<< "F_Preferences::F_Preferences : RequeteDemarrage" << RequeteDemarrage.lastQuery() << endl;
    }
    while(RequeteDemarrage.next())
    {
        TbPreferences[RequeteDemarrage.value(0).toString()]=RequeteDemarrage.value(1).toString();
    }

    QSettings FichierDeConfig(sCheminConfig, QSettings::IniFormat);
    FichierDeConfig.beginGroup("Ludopen");
    QStringList keys = FichierDeConfig.childKeys();
    foreach (QString key, keys)
    {
        TbPreferences[key]=FichierDeConfig.value(key).toString();
    }
    if(TbPreferences["TaillePolice"]==0)
    {
        TbPreferences["TaillePolice"]="11";
    }
    this->AfficherTousLesTableaux();
    this->AfficherAutresInformations();
}

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
 * @brief   Consulte la BDD et récupère les informations de celle-ci. Ils sont ensuite mis dans les champs correspondant.
 *
 * @pre     Etre connecté à  la BDD.
 */
void F_Preferences::AfficherAutresInformations()
{
    ui->LE_NomBDD->setText(TbPreferences["NomDeLaBDD"]);
    ui->LE_AdresseIP->setText(TbPreferences["AdresseIP"]);
    ui->LE_NomUtilisateur->setText(TbPreferences["NomUtilisateur"]);
    ui->LE_MotdePasse->setText(TbPreferences["MotDePasse"]);
    ui->LE_Port->setText(TbPreferences["Port"]);
    ui->CBx_LieuOrdi->setCurrentIndex(TbPreferences["IdLieux"].toInt() - 1);
    ui->LE_ClePrivee->setText(TbPreferences["CheminClePrivee"]);
    ui->SBx_TaillePolice->setValue(TbPreferences["TaillePolice"].toInt());
    // ORDRE des champs dans la table préférences
    // 0-Nom,1-Adresse,2-CodePostal,3-Ville,4-NumeroTel,5-NumeroFax,6-Email,7-SiteWeb,
    // 8-JeuxAutorises,9-UniteLocation,10-JourAvantMail,11-JourRetard,12-CheminPhotosJeux,"
    // 13-CheminReglesJeux,14-AdresseServeurSMTP,15-PortSMTP,16-PrixAmende,17-NbReservationsMaxi,
    // 18-DelaiConfirmationReservationParEmail
    ui->LE_Nom->setText(TbPreferences["Nom"]);
    ui->LE_Adresse->setText(TbPreferences["Adresse"]);
    ui->LE_CodePostal->setText(TbPreferences["CodePostal"]);
    ui->LE_Ville->setText(TbPreferences["Ville"]);
    ui->LE_NumeroTel->setText(TbPreferences["NumeroTel"]);
    ui->LE_NumeroFax->setText(TbPreferences["NumeroFax"]);
    ui->LE_Mail->setText(TbPreferences["Email"]);
    ui->LE_Web->setText(TbPreferences["SiteWeb"]);
    ui->SBx_JeuxAutorises->setValue(TbPreferences["JeuxAutorises"].toInt());
    QLocale french(QLocale::French);
    ui->DSBx_UniteLocation->setValue(french.toDouble(TbPreferences["UniteLocation"]));
    ui->SBx_JourEmail->setValue(TbPreferences["JourAvantMail"].toInt());
    ui->SBx_JourRetard->setValue(TbPreferences["JourRetard"].toInt());
    ui->LE_CheminPhotosJeux->setText(TbPreferences["CheminPhotosJeux"]);
    ui->LE_CheminRegle->setText(TbPreferences["CheminReglesJeux"]);
    ui->LE_AdresseSMTP->setText(TbPreferences["AdresseServeurSMTP"]);
    ui->LE_PortSMTP->setText(TbPreferences["PortSMTP"]);
    ui->DSBx_PrixAmende->setValue(french.toDouble(TbPreferences["PrixAmende"]));
    ui->LE_CheminPhotosServeur->setText(TbPreferences["CheminPhotosServeur"]);
    ui->LE_CheminReglesServeur->setText(TbPreferences["CheminReglesServeur"]);
    ui->LE_AdresseServeur->setText(TbPreferences["AdresseServeur"]);
    ui->LE_Login->setText(TbPreferences["LoginServeur"]);
    ui->LE_MAJ->setText(TbPreferences["URLMAJeur"]);
    ui->LE_MAJLocal->setText(TbPreferences["URLMAJeurLocale"]);
}

/**
 *  @brief  Fonction qui permet de mettre à  jour les tableaux contenant toutes les informations de la BDD.
 *
 *  @pre    Etre connecté à  la BDD.
 */
void F_Preferences::AfficherTousLesTableaux()
{
    QSqlQuery RechercheTableau;
    int nNombreLigne(0);

    //################################ onglet Membres/Emprunts #################################################

    //Affichage de la BDD dans le tableau titre qui est dans l'onglet Membres/Emprunts de F_Preferences.
    this->TbMembresTitre->setColumnCount(3);
    this->TbMembresTitre->setRowCount(nNombreLigne);
    this->TbMembresTitre->setHorizontalHeaderItem(0, new QStandardItem("Titre Membre"));
    this->TbMembresTitre->setHorizontalHeaderItem(1, new QStandardItem("Emprunt"));
    ui->TbV_MembresTitre->setColumnWidth(0, 100);
    ui->TbV_MembresTitre->setColumnWidth(1, 65);

    RechercheTableau.exec("SELECT NomTitre, NbrJeuxEmpruntables,IdTitreMembre FROM titremembre");
    while (RechercheTableau.next())
    {
        this->TbMembresTitre->setItem(nNombreLigne, 0, new QStandardItem(RechercheTableau.value(0).toString()));
        this->TbMembresTitre->setItem(nNombreLigne, 1, new QStandardItem(RechercheTableau.value(1).toString()));
        this->TbMembresTitre->setItem(nNombreLigne++, 2, new QStandardItem(RechercheTableau.value(2).toString()));
    }
    nNombreLigne = 0;

    //Affichage de la BDD dans le tableau type qui est dans l'onglet Membres/Emprunts de F_Preferences.
    this->TbMembresType->setColumnCount(2);
    this->TbMembresType->setRowCount(nNombreLigne);
    this->TbMembresType->setHorizontalHeaderItem(0, new QStandardItem("Type Membre"));
    ui->TbV_MembresType->setColumnWidth(0, 150);

    RechercheTableau.exec("SELECT TypeMembre,IdTypeMembres FROM typemembres");
    while (RechercheTableau.next())
    {
        this->TbMembresType->setItem(nNombreLigne, 0, new QStandardItem(RechercheTableau.value(0).toString()));
        this->TbMembresType->setItem(nNombreLigne++, 1, new QStandardItem(RechercheTableau.value(1).toString()));
    }
    nNombreLigne = 0;

    //Affichage de la BDD dans le tableau paiement qui est dans l'onglet Membres/Emprunts de F_Preferences.
    this->TbMembresPaiement->setColumnCount(2);
    this->TbMembresPaiement->setRowCount(nNombreLigne);
    this->TbMembresPaiement->setHorizontalHeaderItem(0, new QStandardItem("Paiement"));
    ui->TbV_MembresPaiement->setColumnWidth(0, 150);

    RechercheTableau.exec("SELECT NomPaiement,IdModePaiement FROM modepaiement");
    while (RechercheTableau.next())
    {
        this->TbMembresPaiement->setItem(nNombreLigne, 0, new QStandardItem(RechercheTableau.value(0).toString()));
        this->TbMembresPaiement->setItem(nNombreLigne++, 1, new QStandardItem(RechercheTableau.value(1).toString()));
    }
    nNombreLigne = 0;

    //Affichage de la BDD dans le tableau type d'emprunt qui est dans l'onglet Membres/Emprunts de F_Preferences.
    this->TbEmpruntType->setColumnCount(3);
    this->TbEmpruntType->setRowCount(nNombreLigne);
    this->TbEmpruntType->setHorizontalHeaderItem(0, new QStandardItem("Type Emprunt"));
    this->TbEmpruntType->setHorizontalHeaderItem(1, new QStandardItem("Durée"));
    ui->TbV_EmpruntType->setColumnWidth(0, 180);
    ui->TbV_EmpruntType->setColumnWidth(1, 90);

    RechercheTableau.exec("SELECT TypeEmprunt,DureeEmprunt,IdTypeEmprunt FROM typeemprunt");
    while (RechercheTableau.next())
    {
        this->TbEmpruntType->setItem(nNombreLigne, 0, new QStandardItem(RechercheTableau.value(0).toString()));
        this->TbEmpruntType->setItem(nNombreLigne, 1, new QStandardItem(RechercheTableau.value(1).toString()));
        this->TbEmpruntType->setItem(nNombreLigne++, 2, new QStandardItem(RechercheTableau.value(2).toString()));
    }

    nNombreLigne = 0;

    //Affichage de la BDD dans le tableau type d'emprunt qui est dans l'onglet Membres/Emprunts de F_Preferences.
    this->TbMembresActivite->setColumnCount(2);
    this->TbMembresActivite->setRowCount(nNombreLigne);
    this->TbMembresActivite->setHorizontalHeaderItem(0, new QStandardItem("Activite"));
    ui->TbV_MembresActivite->setColumnWidth(0, 180);

    RechercheTableau.exec("SELECT Activite,IdActivite FROM activite");
    while (RechercheTableau.next())
    {
        this->TbMembresActivite->setItem(nNombreLigne, 0, new QStandardItem(RechercheTableau.value(0).toString()));
        this->TbMembresActivite->setItem(nNombreLigne++, 1, new QStandardItem(RechercheTableau.value(1).toString()));
    }
    nNombreLigne = 0;

    //Affichage de la BDD dans le tableau type d'emprunt qui est dans l'onglet Membres/Emprunts de F_Preferences.
    this->TbJeuxMotCle->setColumnCount(1);
    this->TbJeuxMotCle->setRowCount(nNombreLigne);
    this->TbJeuxMotCle->setHorizontalHeaderItem(0, new QStandardItem("Mot-clé"));
    ui->TbV_JeuxMotCle->setColumnWidth(0, 180);

    RechercheTableau.exec("SELECT MotCle,Id_MotCle FROM motscles");
    while (RechercheTableau.next())
    {
        this->TbJeuxMotCle->setItem(nNombreLigne, 0, new QStandardItem(RechercheTableau.value(0).toString()));
        this->TbJeuxMotCle->setItem(nNombreLigne++, 1, new QStandardItem(RechercheTableau.value(1).toString()));
    }
    nNombreLigne = 0;

    //################################ onglet Jeux #################################################

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

    RechercheTableau.exec("SELECT Etat,IdEtatsJeu FROM etatsjeu");
    while (RechercheTableau.next())
    {
        this->TbJeuxEtat->setItem(nNombreLigne, 0, new QStandardItem(RechercheTableau.value(0).toString()));
        this->TbJeuxEtat->setItem(nNombreLigne++, 1, new QStandardItem(RechercheTableau.value(1).toString()));
    }
    nNombreLigne = 0;

    //Affichage de la BDD dans le tableau statut qui est dans l'onglet Jeux de F_Preferences.
    this->TbJeuxStatut->setColumnCount(1);
    this->TbJeuxStatut->setRowCount(nNombreLigne);
    this->TbJeuxStatut->setHorizontalHeaderItem(0, new QStandardItem("Statut"));

    RechercheTableau.exec("SELECT StatutJeu,IdStatutJeux FROM statutjeux");
    while (RechercheTableau.next())
    {
        this->TbJeuxStatut->setItem(nNombreLigne, 0, new QStandardItem(RechercheTableau.value(0).toString()));
        this->TbJeuxStatut->setItem(nNombreLigne++, 1, new QStandardItem(RechercheTableau.value(1).toString()));
    }
    nNombreLigne = 0;

    //Affichage de la BDD dans le tableau emplacement qui est dans l'onglet Jeux de F_Preferences.
    this->TbJeuxEmplacement->setColumnCount(1);
    this->TbJeuxEmplacement->setRowCount(nNombreLigne);
    this->TbJeuxEmplacement->setHorizontalHeaderItem(0, new QStandardItem("Emplacement"));
    ui->TbV_JeuxEmplacement->setColumnWidth(0, 180);

    RechercheTableau.exec("SELECT Nom,IdEmplacement FROM emplacement");
    while (RechercheTableau.next())
    {
        this->TbJeuxEmplacement->setItem(nNombreLigne, 0, new QStandardItem(RechercheTableau.value(0).toString()));
        this->TbJeuxEmplacement->setItem(nNombreLigne++, 1, new QStandardItem(RechercheTableau.value(1).toString()));
    }
    nNombreLigne=0;

    //################################ onglet Info Ludo #################################################

    //Affichage de la BDD dans le tableau lieu qui est dans l'onglet Info Ludo de F_Preferences.
    this->TbInfoLudoLieux->setColumnCount(2);
    this->TbInfoLudoLieux->setRowCount(nNombreLigne);
    this->TbInfoLudoLieux->setHorizontalHeaderItem(0, new QStandardItem("Lieu"));
    ui->TbV_InfoLieux->setColumnWidth(0, 180);

    RechercheTableau.exec("SELECT NomLieux,IdLieux FROM lieux");
    while (RechercheTableau.next())
    {
        this->TbInfoLudoLieux->setItem(nNombreLigne, 0, new QStandardItem(RechercheTableau.value(0).toString()));
        this->TbInfoLudoLieux->setItem(nNombreLigne++, 1, new QStandardItem(RechercheTableau.value(1).toString()));
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
    ui->TbV_MembresTitre->setColumnWidth(2, 0);
    ui->TbV_MembresType->resizeColumnsToContents();
    ui->TbV_MembresType->setColumnWidth(1, 0);
    //ui->TbV_JeuxType->resizeColumnsToContents();
    ui->TbV_JeuxEtat->resizeColumnsToContents();
    ui->TbV_JeuxStatut->resizeColumnsToContents();
    ui->TbV_JeuxStatut->setColumnWidth(1, 0);
    ui->TbV_MembresPaiement->resizeColumnsToContents();
    ui->TbV_MembresPaiement->setColumnWidth(1, 0);
    ui->TbV_EmpruntType->resizeColumnsToContents();
    ui->TbV_EmpruntType->setColumnWidth(2, 0);
    ui->TbV_InfoLieux->resizeColumnsToContents();
    ui->TbV_InfoLieux->setColumnWidth(1, 0);
    ui->TbV_JeuxEmplacement->resizeColumnsToContents();
    ui->TbV_JeuxEmplacement->setColumnWidth(1, 0);
    ui->TbV_MembresActivite->resizeColumnsToContents();
    ui->TbV_MembresActivite->setColumnWidth(1, 0);
    ui->TbV_JeuxMotCle->resizeColumnsToContents();
    ui->TbV_JeuxMotCle->setColumnWidth(1, 0);
    ui->TbV_JeuxType->selectRow(0);
    ui->TbV_JeuxEtat->selectRow(0);
    ui->TbV_JeuxEtat->setColumnWidth(1, 0);
    on_TbV_JeuxStatut_clicked(this->TbJeuxStatut->index(0,0));
    //ui->TbV_JeuxStatut->selectRow(0);
    ui->TbV_JeuxEmplacement->selectRow(0);
    ui->TbV_InfoLieux->selectRow(0);
    ui->TbV_MembresTitre->selectRow(0);
    ui->TbV_MembresType->selectRow(0);
    ui->TbV_MembresPaiement->selectRow(0);
    ui->TbV_EmpruntType->selectRow(0);
    ui->TbV_MembresActivite->selectRow(0);
    ui->TbV_JeuxMotCle->selectRow(0);
}

/**
 *  @brief  Permet de sauvegarder une préférence
 *
 *  @pre    Etre connecté à  la BDD.
 */
void F_Preferences::SauverPreference(QString NomChamps, QString Valeur, bool BDD)
{
    if(BDD)
    {
        QSqlQuery RequeteEnregistrer;
        RequeteEnregistrer.prepare("INSERT INTO preferences (NomChamps,Valeur) VALUES (:NomChamps,:Valeur)"
                                   " ON DUPLICATE KEY UPDATE Valeur =:Valeur");
        RequeteEnregistrer.bindValue(":NomChamps", NomChamps);
        RequeteEnregistrer.bindValue(":Valeur", Valeur);
        if ( ! RequeteEnregistrer.exec() )
        {
           qDebug()<< "F_Preferences::SauverPreference : RequeteEnregistrer" << RequeteEnregistrer.lastQuery() << endl;
        }
        qDebug()<< "F_Preferences::SauverPreference : RequeteEnregistrer" << RequeteEnregistrer.lastQuery() << endl;
    }
    else
    {
        QSettings FichierDeConfig(sCheminConfig, QSettings::IniFormat);
        FichierDeConfig.setValue("Ludopen/" + NomChamps, Valeur);
    }
    this->TbPreferences[NomChamps]=Valeur;
}

/**
 *  @brief  Permet d'enregistrer les actions effectuées depuis l'ouverture de la F_Preferences.
 *
 *  @pre    Etre connecté à  la BDD.
 */
void F_Preferences::on_Bt_Enregistrer_clicked()
{
    QSqlQuery RequeteEnregistrer;
    QSqlQuery RequeteCombo;

    if(ui->LE_CheminPhotosJeux->text().at(ui->LE_CheminPhotosJeux->text().count()-1)!=QChar('/'))
    {
        ui->LE_CheminPhotosJeux->setText(ui->LE_CheminPhotosJeux->text().append("/"));
    }
    if(ui->LE_CheminRegle->text().at(ui->LE_CheminRegle->text().count()-1)!=QChar('/'))
    {
        ui->LE_CheminRegle->setText(ui->LE_CheminRegle->text().append("/"));
    }
    if(ui->LE_CheminReglesServeur->text().at(ui->LE_CheminReglesServeur->text().count()-1)!=QChar('/'))
    {
        ui->LE_CheminReglesServeur->setText(ui->LE_CheminReglesServeur->text().append("/"));
    }
    if(ui->LE_CheminPhotosServeur->text().at(ui->LE_CheminPhotosServeur->text().count()-1)!=QChar('/'))
    {
        ui->LE_CheminPhotosServeur->setText(ui->LE_CheminPhotosServeur->text().append("/"));
    }
    SauverPreference("Nom", ui->LE_Nom->text());
    SauverPreference("Adresse", ui->LE_Adresse->text());
    SauverPreference("CodePostal", ui->LE_CodePostal->text());
    SauverPreference("Ville", ui->LE_Ville->text());
    SauverPreference("NumeroTel", ui->LE_NumeroTel->text());
    SauverPreference("NumeroFax", ui->LE_NumeroFax->text());
    SauverPreference("Email", ui->LE_Mail->text());
    SauverPreference("SiteWeb", ui->LE_Web->text());
    SauverPreference("JeuxAutorises", ui->SBx_JeuxAutorises->text());
    SauverPreference("UniteLocation", ui->DSBx_UniteLocation->text());
    SauverPreference("JourRetard", ui->SBx_JourRetard->text());
    SauverPreference("JourAvantMail", ui->SBx_JourEmail->text());
    SauverPreference("CheminPhotosJeux", ui->LE_CheminPhotosJeux->text());
    SauverPreference("CheminReglesJeux", ui->LE_CheminRegle->text());
    SauverPreference("AdresseServeurSMTP", ui->LE_AdresseSMTP->text());
    SauverPreference("PortSMTP", ui->LE_PortSMTP->text());
    SauverPreference("PrixAmende", ui->DSBx_PrixAmende->text());
    SauverPreference("CheminPhotosServeur", ui->LE_CheminPhotosServeur->text());
    SauverPreference("CheminReglesServeur", ui->LE_CheminReglesServeur->text());
    SauverPreference("AdresseServeur", ui->LE_AdresseServeur->text());
    SauverPreference("LoginServeur", ui->LE_Login->text());
    SauverPreference("URLMAJeur", ui->LE_MAJ->text());
    SauverPreference("URLMAJeurLocale", ui->LE_MAJLocal->text());
    // écriture de certaines données dans le fichier de configuration config.ini
    RequeteCombo.prepare("SELECT * FROM lieux WHERE NomLieux=:NomLieux");
    RequeteCombo.bindValue(":NomLieux", ui->CBx_LieuOrdi->currentText());
    RequeteCombo.exec();
    while(RequeteCombo.next())
    {
        SauverPreference("IdLieux", RequeteCombo.value(0).toString(),false);
        SauverPreference("NomLieux", RequeteCombo.value(1).toString(),false);
        SauverPreference("CheminClePrivee", ui->LE_ClePrivee->text(),false);
        SauverPreference("TaillePolice", ui->SBx_TaillePolice->text(),false);
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

        SauverPreference("CodeAdmin", ui->LE_ConfirmerCode->text());
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

        SauverPreference("NomDeLaBDD", ui->LE_NomBDD->text(),false);
        SauverPreference("AdresseIP", ui->LE_AdresseIP->text(),false);
        SauverPreference("NomUtilisateur", ui->LE_NomUtilisateur->text(),false);
        SauverPreference("MotDePasse", ui->LE_MotdePasse->text()),false;
        SauverPreference("Port", ui->LE_Port->text(),false);
    }
    else
    {
        qDebug()<< "Erreur : " << db.lastError() << endl;
        ui->Lb_InfoConnexion->setText("<font color=red> " + QTime::currentTime().toString() +
                                      " : La connexion a échouée. Veuiller réessayer. </font>");
    }
}

/**
 *  @brief  Permet d'ajouter un titre d'un membre.
 *
 *  @pre    Etre connecté à  la BDD.
 */
void F_Preferences::on_Bt_AjouterMembreTitre_clicked()
{
    this->pPopUpCLESTTEM->Ajouter(6);
}

/**
 *  @brief   Permet de modifier un titre d'un membre.
 *
 *  @pre    Etre connecté à  la BDD.
 */
void F_Preferences::on_Bt_ModifierMembreTitre_clicked()
{
    QSqlQuery RequeteModifier;

    RequeteModifier.prepare("SELECT NbrJeuxEmpruntables FROM titremembre WHERE NomTitre=:NomTitre");
    RequeteModifier.bindValue(":NomTitre", ui->TbV_MembresTitre->selectionModel()->selectedRows(0).first().data().toString());
    RequeteModifier.exec();
    RequeteModifier.next();
    int NbrJeuxEmpruntables=RequeteModifier.value(0).toInt();
    int ret=this->pPopUpCLESTTEM->Modifier(ui->TbV_MembresTitre->selectionModel()->selectedRows(0).first().data().toString(),6);
    if(ret==0)
    {
        return;
    }
    RequeteModifier.prepare("SELECT NbrJeuxEmpruntables,IdTitreMembre FROM titremembre WHERE NomTitre=:NomTitre");
    RequeteModifier.bindValue(":NomTitre", ui->TbV_MembresTitre->selectionModel()->selectedRows(0).first().data().toString());
    RequeteModifier.exec();
    RequeteModifier.next();
    if(NbrJeuxEmpruntables != RequeteModifier.value(0).toInt())
    {
        if(QMessageBox::question(this, "Modification du nombre de jeux empruntables",
                                 "Vous avez modifier le nombre de jeux empruntables\n"
                                 "Voulez-vous modifier les membres existants avec cette nouvelle valeur ?", "Oui", "Non") == 0)
        {
            QSqlQuery RequeteSupprimer;
            RequeteSupprimer.prepare("UPDATE membres SET NbreJeuxAutorises=:NbreJeuxAutorises WHERE "
                                   "TitreMembre_IdTitreMembre=:IdTitreMembre");
            RequeteSupprimer.bindValue(":IdTitreMembre", RequeteModifier.value(1).toInt());
            RequeteSupprimer.bindValue(":NbreJeuxAutorises", RequeteModifier.value(0).toInt());
            RequeteSupprimer.exec();
        }
    }
}

/**
 *  @brief   Permet de supprimer un titre d'un membre.
 *
 *  @pre    Etre connecté à  la BDD.
 *  @see    AfficherTousLesTableaux()
 */
void F_Preferences::on_Bt_SupprimerMembreTitre_clicked()
{
    if(QMessageBox::question(this, "Suppression", "Etes-vous sur de vouloir supprimer ce titre ?", "Oui", "Non") == 0)
    {
        QString IdTitreMembre=ui->TbV_MembresTitre->selectionModel()->selectedRows(2).first().data().toString();
        QSqlQuery RequeteSupprimer;
        RequeteSupprimer.prepare("SELECT * FROM membres WHERE TitreMembre_IdTitreMembre=:IdTitreMembre");
        RequeteSupprimer.bindValue(":IdTitreMembre", IdTitreMembre);
        RequeteSupprimer.exec();
        if(RequeteSupprimer.size()!=0)
        {
            RequeteSupprimer.prepare("SELECT NomTitre,IdTitreMembre FROM titremembre WHERE "
                              "IdTitreMembre!=:IdTitreMembre");
            RequeteSupprimer.bindValue(":IdTitreMembre", IdTitreMembre);
            RequeteSupprimer.exec();
            int i=0;
            this->TbCLESTTEM->clear();
            this->TbCLESTTEM->setHorizontalHeaderItem(0, new QStandardItem("Valeurs"));
            while(RequeteSupprimer.next())
            {
                this->TbCLESTTEM->setItem(i, 0, new QStandardItem(RequeteSupprimer.value(0).toString()));
                this->TbCLESTTEM->setItem(i++, 1, new QStandardItem(RequeteSupprimer.value(1).toString()));
            }
            this->TbV_CLESTTEM->setSelectionMode(QAbstractItemView::SingleSelection);
            this->TbV_CLESTTEM->setSelectionBehavior(QAbstractItemView::SelectRows);
            this->TbV_CLESTTEM->resizeColumnsToContents();
            this->TbV_CLESTTEM->setColumnWidth(1,0);
            this->TbV_CLESTTEM->selectRow(0);
            int ret=this->pPopUpCLESTTEM->Modifier(0, 255, this->TbV_CLESTTEM);
            if(ret==0)
            {
                return;
            }
            RequeteSupprimer.prepare("UPDATE membres SET TitreMembre_IdTitreMembre=:IdTitreMembre WHERE "
                                   "TitreMembre_IdTitreMembre=:IdTitreMembrePrecedent");
            QString IdTitre=this->TbV_CLESTTEM->selectionModel()->selectedRows(1).first().data().toString();
            RequeteSupprimer.bindValue(":IdTitreMembre",IdTitre);
            RequeteSupprimer.bindValue(":IdTitreMembrePrecedent", IdTitreMembre);
            RequeteSupprimer.exec();
        }
        RequeteSupprimer.prepare("DELETE FROM titremembre WHERE IdTitreMembre=:IdTitreMembre");
        RequeteSupprimer.bindValue(":IdTitreMembre", IdTitreMembre);
        RequeteSupprimer.exec();
        RequeteSupprimer.next();
    }

    this->AfficherTousLesTableaux();
}

/**
 *  @brief  Permet d'ajouter un type de membre.
 *
 */
void F_Preferences::on_Bt_AjouterMembreType_clicked()
{
    this->pPopUpCLESTTEM->Ajouter(5);
}

/**
 *  @brief Permet de modifier un type de membre.
 *
 */
void F_Preferences::on_Bt_ModifierMembreType_clicked()
{
    QSqlQuery RequeteModifier;

    RequeteModifier.prepare("SELECT TypeMembre FROM typemembres WHERE TypeMembre=:TypeMembre");
    RequeteModifier.bindValue(":TypeMembre", ui->TbV_MembresType->selectionModel()->selectedRows(0).first().data().toString());
    RequeteModifier.exec();
    RequeteModifier.next();

    this->pPopUpCLESTTEM->Modifier(RequeteModifier.value(0).toString(),5);
}

/**
 *  @brief  Permet de supprimer un type de membre.
 *
 *  @see AfficherTousLesTableaux()
 */
void F_Preferences::on_Bt_SupprimerMembreType_clicked()
{
    QSqlQuery RequeteSupprimer;

    if(QMessageBox::question(this, "Suppression", "Etes-vous sur de vouloir supprimer ce type de membre ?", "Oui", "Non") == 0)
    {
        QString IdTypeMembres=ui->TbV_MembresType->selectionModel()->selectedRows(1).first().data().toString();
        QSqlQuery RequeteSupprimer;
        RequeteSupprimer.prepare("SELECT * FROM membres WHERE TypeMembres_IdTypeMembres=:IdTypeMembres");
        RequeteSupprimer.bindValue(":IdTypeMembres", IdTypeMembres);
        RequeteSupprimer.exec();
        if(RequeteSupprimer.size()!=0)
        {
            RequeteSupprimer.prepare("SELECT TypeMembre,IdTypeMembres FROM typemembres WHERE "
                              "IdTypeMembres!=:IdTypeMembres");
            RequeteSupprimer.bindValue(":IdTypeMembres", IdTypeMembres);
            RequeteSupprimer.exec();
            int i=0;
            this->TbCLESTTEM->clear();
            this->TbCLESTTEM->setHorizontalHeaderItem(0, new QStandardItem("Valeurs"));
            while(RequeteSupprimer.next())
            {
                this->TbCLESTTEM->setItem(i, 0, new QStandardItem(RequeteSupprimer.value(0).toString()));
                this->TbCLESTTEM->setItem(i++, 1, new QStandardItem(RequeteSupprimer.value(1).toString()));
            }
            this->TbV_CLESTTEM->setSelectionMode(QAbstractItemView::SingleSelection);
            this->TbV_CLESTTEM->setSelectionBehavior(QAbstractItemView::SelectRows);
            this->TbV_CLESTTEM->resizeColumnsToContents();
            this->TbV_CLESTTEM->setColumnWidth(1,0);
            this->TbV_CLESTTEM->selectRow(0);
            int ret=this->pPopUpCLESTTEM->Modifier(0, 255, this->TbV_CLESTTEM);
            if(ret==0)
            {
                return;
            }
            RequeteSupprimer.prepare("UPDATE membres SET TypeMembres_IdTypeMembres=:IdTypeMembres WHERE "
                                   "TypeMembres_IdTypeMembres=:IdTypeMembresPrecedent");
            QString IdType=this->TbV_CLESTTEM->selectionModel()->selectedRows(1).first().data().toString();
            RequeteSupprimer.bindValue(":IdTypeMembres",IdType);
            RequeteSupprimer.bindValue(":IdTypeMembresPrecedent", IdTypeMembres);
            RequeteSupprimer.exec();
        }
        RequeteSupprimer.prepare("DELETE FROM typemembres WHERE IdTypeMembres=:IdTypeMembres");
        RequeteSupprimer.bindValue(":IdTypeMembres", IdTypeMembres);
        RequeteSupprimer.exec();
        RequeteSupprimer.next();
    }

    this->AfficherTousLesTableaux();
}

void F_Preferences::on_Bt_AjouterMembrePaiement_clicked()
{
    this->pPopUpCLESTTEM->Ajouter(9);
}

void F_Preferences::on_Bt_ModifierMembrePaiement_clicked()
{
    QSqlQuery RequeteModifier;

    RequeteModifier.prepare("SELECT NomPaiement FROM modepaiement WHERE NomPaiement=:NomPaiement");
    RequeteModifier.bindValue(":NomPaiement", ui->TbV_MembresPaiement->selectionModel()->selectedRows(0).first().data().toString());
    RequeteModifier.exec();
    RequeteModifier.next();

    this->pPopUpCLESTTEM->Modifier(RequeteModifier.value(0).toString(),9);
}

void F_Preferences::on_Bt_SupprimerMembrePaiement_clicked()
{
    QSqlQuery RequeteSupprimer;

    if(QMessageBox::question(this, "Suppression", "Etes-vous sur de vouloir supprimer ce paiement ?", "Oui", "Non") == 0)
    {
        QString IdModePaiement=ui->TbV_MembresPaiement->selectionModel()->selectedRows(1).first().data().toString();
        QSqlQuery RequeteSupprimer;
        RequeteSupprimer.prepare("SELECT * FROM abonnements WHERE ModePaiement_IdModePaiement=:IdModePaiement");
        RequeteSupprimer.bindValue(":IdModePaiement", IdModePaiement);
        RequeteSupprimer.exec();
        QSqlQuery RequeteSupprimer2;
        RequeteSupprimer2.prepare("SELECT * FROM emprunts WHERE ModePaiement_IdModePaiement=:IdModePaiement");
        RequeteSupprimer2.bindValue(":IdModePaiement", IdModePaiement);
        RequeteSupprimer2.exec();
        if(RequeteSupprimer.size()!=0 || RequeteSupprimer2.size()!=0)
        {
            RequeteSupprimer.prepare("SELECT NomPaiement,IdModePaiement FROM modepaiement WHERE "
                              "IdModePaiement!=:IdModePaiement");
            RequeteSupprimer.bindValue(":IdModePaiement", IdModePaiement);
            RequeteSupprimer.exec();
            int i=0;
            this->TbCLESTTEM->clear();
            this->TbCLESTTEM->setHorizontalHeaderItem(0, new QStandardItem("Valeurs"));
            while(RequeteSupprimer.next())
            {
                this->TbCLESTTEM->setItem(i, 0, new QStandardItem(RequeteSupprimer.value(0).toString()));
                this->TbCLESTTEM->setItem(i++, 1, new QStandardItem(RequeteSupprimer.value(1).toString()));
            }
            this->TbV_CLESTTEM->setSelectionMode(QAbstractItemView::SingleSelection);
            this->TbV_CLESTTEM->setSelectionBehavior(QAbstractItemView::SelectRows);
            this->TbV_CLESTTEM->resizeColumnsToContents();
            this->TbV_CLESTTEM->setColumnWidth(1,0);
            this->TbV_CLESTTEM->selectRow(0);
            int ret=this->pPopUpCLESTTEM->Modifier(0, 255, this->TbV_CLESTTEM);
            if(ret==0)
            {
                return;
            }
            RequeteSupprimer.prepare("UPDATE abonnements SET ModePaiement_IdModePaiement=:IdModePaiement"
                                   " WHERE ModePaiement_IdModePaiement=:IdModePaiementPrecedent");
            QString IdPaiement=this->TbV_CLESTTEM->selectionModel()->selectedRows(1).first().data().toString();
            RequeteSupprimer.bindValue(":IdModePaiement",IdPaiement);
            RequeteSupprimer.bindValue(":IdModePaiementPrecedent", IdModePaiement);
            RequeteSupprimer.exec();
            RequeteSupprimer.prepare("UPDATE emprunts SET ModePaiement_IdModePaiement=:IdModePaiement"
                                   " WHERE ModePaiement_IdModePaiement=:IdModePaiementPrecedent");
            RequeteSupprimer.bindValue(":IdModePaiement",IdPaiement);
            RequeteSupprimer.bindValue(":IdModePaiementPrecedent", IdModePaiement);
            RequeteSupprimer.exec();
        }
        RequeteSupprimer.prepare("DELETE FROM modepaiement WHERE IdModePaiement=:IdModePaiement");
        RequeteSupprimer.bindValue(":IdModePaiement", IdModePaiement);
        RequeteSupprimer.exec();
        RequeteSupprimer.next();
    }

    this->AfficherTousLesTableaux();
}

void F_Preferences::on_Bt_AjouterEmpruntType_clicked()
{
    this->pPopUpCLESTTEM->Ajouter(7);
}

void F_Preferences::on_Bt_ModifierEmpruntType_clicked()
{
    QSqlQuery RequeteModifier;

    RequeteModifier.prepare("SELECT TypeEmprunt FROM typeemprunt WHERE TypeEmprunt=:TypeEmprunt");
    RequeteModifier.bindValue(":TypeEmprunt", ui->TbV_EmpruntType->selectionModel()->selectedRows(0).first().data().toString());
    RequeteModifier.exec();
    RequeteModifier.next();

    this->pPopUpCLESTTEM->Modifier(RequeteModifier.value(0).toString(),7);
}

void F_Preferences::on_Bt_SupprimerEmpruntType_clicked()
{
    QSqlQuery RequeteSupprimer;

    if(QMessageBox::question(this, "Suppression", "Etes-vous sur de vouloir supprimer ce type d'emprunt ?", "Oui", "Non") == 0)
    {
        QString IdTypeEmprunt=ui->TbV_EmpruntType->selectionModel()->selectedRows(2).first().data().toString();
        QSqlQuery RequeteSupprimer;
        RequeteSupprimer.prepare("SELECT * FROM emprunts WHERE TypeEmprunt_IdTypeEmprunt=:IdTypeEmprunt");
        RequeteSupprimer.bindValue(":IdTypeEmprunt", IdTypeEmprunt);
        RequeteSupprimer.exec();
        if(RequeteSupprimer.size()!=0 || RequeteSupprimer.size()!=0)
        {
            RequeteSupprimer.prepare("SELECT TypeEmprunt,IdTypeEmprunt FROM typeemprunt WHERE "
                              "IdTypeEmprunt!=:IdTypeEmprunt");
            RequeteSupprimer.bindValue(":IdTypeEmprunt", IdTypeEmprunt);
            RequeteSupprimer.exec();
            int i=0;
            this->TbCLESTTEM->clear();
            this->TbCLESTTEM->setHorizontalHeaderItem(0, new QStandardItem("Valeurs"));
            while(RequeteSupprimer.next())
            {
                this->TbCLESTTEM->setItem(i, 0, new QStandardItem(RequeteSupprimer.value(0).toString()));
                this->TbCLESTTEM->setItem(i++, 1, new QStandardItem(RequeteSupprimer.value(1).toString()));
            }
            this->TbV_CLESTTEM->setSelectionMode(QAbstractItemView::SingleSelection);
            this->TbV_CLESTTEM->setSelectionBehavior(QAbstractItemView::SelectRows);
            this->TbV_CLESTTEM->resizeColumnsToContents();
            this->TbV_CLESTTEM->setColumnWidth(1,0);
            this->TbV_CLESTTEM->selectRow(0);
            int ret=this->pPopUpCLESTTEM->Modifier(0, 255, this->TbV_CLESTTEM);
            if(ret==0)
            {
                return;
            }
            RequeteSupprimer.prepare("UPDATE emprunts SET TypeEmprunt_IdTypeEmprunt=:IdTypeEmprunt"
                                   " WHERE TypeEmprunt_IdTypeEmprunt=:IdTypeEmpruntPrecedent");
            QString IdType=this->TbV_CLESTTEM->selectionModel()->selectedRows(1).first().data().toString();
            RequeteSupprimer.bindValue(":IdTypeEmprunt",IdType);
            RequeteSupprimer.bindValue(":IdTypeEmpruntPrecedent", IdTypeEmprunt);
            RequeteSupprimer.exec();
        }
        RequeteSupprimer.prepare("DELETE FROM typeemprunt WHERE IdTypeEmprunt=:IdTypeEmprunt");
        RequeteSupprimer.bindValue(":IdTypeEmprunt", IdTypeEmprunt);
        RequeteSupprimer.exec();
    }

    this->AfficherTousLesTableaux();
}

/**
 *  @brief  Sélectionne la ligne entière du tableau statut de jeu.
 *
 *  @param  index
 */
void F_Preferences::on_TbV_JeuxStatut_clicked(const QModelIndex &index)
{
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
 *  @brief  Permet d'ajouter un type de jeu (classification).
 *
 */
void F_Preferences::on_Bt_AjouterJeuxType_clicked()
{
    this->pPopUpCLESTTEM->Ajouter(3);
}

/**
 *  @brief  Permet de modifier un type de jeu.
 *
 */
void F_Preferences::on_Bt_ModifierJeuxType_clicked()
{
    QSqlQuery RequeteModifier;

    RequeteModifier.prepare("SELECT TypeJeux FROM typejeux WHERE TypeJeux=:TypeJeux");
    RequeteModifier.bindValue(":TypeJeux", ui->TbV_JeuxType->selectionModel()->selectedRows(0).first().data().toString());
    RequeteModifier.exec();
    RequeteModifier.next();

    this->pPopUpCLESTTEM->Modifier(RequeteModifier.value(0).toString(),3);
}

/**
 *  @brief  Permet de supprimer un type de jeu.
 *
 *  @see    AfficherTousLesTableaux()
 */
void F_Preferences::on_Bt_SupprimerJeuxType_clicked()
{

    if(QMessageBox::question(this, "Suppression", "Etes-vous sur de vouloir supprimer ce type de jeu ?", "Oui", "Non") == 0)
    {
        QString IdJeuxType=ui->TbV_JeuxType->selectionModel()->selectedRows(1).first().data().toString();
        QSqlQuery RequeteSupprimer;
        RequeteSupprimer.prepare("SELECT * FROM jeux WHERE TypeJeux_Classification=:Classification");
        RequeteSupprimer.bindValue(":Classification", IdJeuxType);
        RequeteSupprimer.exec();
        if(RequeteSupprimer.size()!=0)
        {
            RequeteSupprimer.prepare("SELECT TypeJeux,Classification FROM typejeux WHERE "
                              "Classification!=:Classification");
            RequeteSupprimer.bindValue(":Classification", IdJeuxType);
            RequeteSupprimer.exec();
            int i=0;
            this->TbCLESTTEM->clear();
            this->TbCLESTTEM->setHorizontalHeaderItem(0, new QStandardItem("Valeurs"));
            while(RequeteSupprimer.next())
            {
                this->TbCLESTTEM->setItem(i, 0, new QStandardItem(RequeteSupprimer.value(0).toString()));
                this->TbCLESTTEM->setItem(i++, 1, new QStandardItem(RequeteSupprimer.value(1).toString()));
            }
            this->TbV_CLESTTEM->setSelectionMode(QAbstractItemView::SingleSelection);
            this->TbV_CLESTTEM->setSelectionBehavior(QAbstractItemView::SelectRows);
            this->TbV_CLESTTEM->resizeColumnsToContents();
            this->TbV_CLESTTEM->setColumnWidth(1,0);
            this->TbV_CLESTTEM->selectRow(0);
            int ret=this->pPopUpCLESTTEM->Modifier(IdJeuxType, 255, this->TbV_CLESTTEM);
            if(ret==0)
            {
                return;
            }
            RequeteSupprimer.prepare("UPDATE jeux SET TypeJeux_Classification=:Classification WHERE "
                                   "TypeJeux_Classification=:TypeJeuxPrecedent");
            RequeteSupprimer.bindValue(":TypeJeux",
                             this->TbV_CLESTTEM->selectionModel()->selectedRows(1).first().data().toString());
            RequeteSupprimer.bindValue(":TypeJeuxPrecedent", IdJeuxType);
            RequeteSupprimer.exec();
        }

        RequeteSupprimer.prepare("DELETE FROM typejeux WHERE Classification=:Classification");
        RequeteSupprimer.bindValue(":Classification", IdJeuxType);
        RequeteSupprimer.exec();
    }

    this->AfficherTousLesTableaux();
}

/**
 *  @brief  Permet d'ajouter un état de jeu.
 *
 */
void F_Preferences::on_Bt_AjouterEtat_clicked()
{
    this->pPopUpCLESTTEM->Ajouter(0);
}

/**
 *  @brief  Permet de modifier un état de jeu.
 *
 */
void F_Preferences::on_Bt_ModifierEtat_clicked()
{
    QSqlQuery RequeteModifier;

    RequeteModifier.prepare("SELECT Etat FROM etatsjeu WHERE Etat=:Etat");
    RequeteModifier.bindValue(":Etat", ui->TbV_JeuxEtat->selectionModel()->selectedRows(0).first().data().toString());
    RequeteModifier.exec();
    RequeteModifier.next();

    this->pPopUpCLESTTEM->Modifier(RequeteModifier.value(0).toString(),0);
}

/**
 *  @brief  Permet de supprimer un état de jeu.
 *
 *  @see    AfficherTousLesTableaux()
 */
void F_Preferences::on_Bt_SupprimerEtat_clicked()
{
    QSqlQuery RequeteSupprimer;

    if(QMessageBox::question(this, "Suppression", "Etes-vous sur de vouloir supprimer cet état de jeu ?", "Oui", "Non") == 0)
    {
        QSqlQuery RequeteSupprimer;
        QString IdEtatJeux=ui->TbV_JeuxEtat->selectionModel()->selectedRows(1).first().data().toString();
        RequeteSupprimer.prepare("SELECT * FROM jeux WHERE EtatsJeu_IdEtatsJeu=:EtatJeux");
        RequeteSupprimer.bindValue(":EtatJeux", IdEtatJeux);
        RequeteSupprimer.exec();
        if(RequeteSupprimer.size()!=0)
        {
            RequeteSupprimer.prepare("SELECT Etat,IdEtatsJeu FROM etatsjeu WHERE "
                              "IdEtatsJeu!=:IdEtatsJeu");
            RequeteSupprimer.bindValue(":IdEtatsJeu", IdEtatJeux);
            RequeteSupprimer.exec();
            int i=0;
            this->TbCLESTTEM->clear();
            this->TbCLESTTEM->setHorizontalHeaderItem(0, new QStandardItem("Valeurs"));
            while(RequeteSupprimer.next())
            {
                this->TbCLESTTEM->setItem(i, 0, new QStandardItem(RequeteSupprimer.value(0).toString()));
                this->TbCLESTTEM->setItem(i++, 1, new QStandardItem(RequeteSupprimer.value(1).toString()));
            }
            this->TbV_CLESTTEM->setSelectionMode(QAbstractItemView::SingleSelection);
            this->TbV_CLESTTEM->setSelectionBehavior(QAbstractItemView::SelectRows);
            this->TbV_CLESTTEM->resizeColumnsToContents();
            this->TbV_CLESTTEM->setColumnWidth(1,0);
            this->TbV_CLESTTEM->selectRow(0);
            int ret=this->pPopUpCLESTTEM->Modifier(0, 255, this->TbV_CLESTTEM);
            if(ret==0)
            {
                return;
            }
            RequeteSupprimer.prepare("UPDATE jeux SET EtatsJeu_IdEtatsJeu=:EtatJeux WHERE "
                                   "EtatsJeu_IdEtatsJeu=:EtatJeuxPrecedent");
            QString IdEtat=this->TbV_CLESTTEM->selectionModel()->selectedRows(1).first().data().toString();
            RequeteSupprimer.bindValue(":EtatJeux", IdEtat);
            RequeteSupprimer.bindValue(":EtatJeuxPrecedent", IdEtatJeux);
            RequeteSupprimer.exec();
        }
        RequeteSupprimer.prepare("DELETE FROM etatsjeu WHERE IdEtatsJeu=:IdEtatsJeu");
        RequeteSupprimer.bindValue(":IdEtatsJeu", IdEtatJeux);
        RequeteSupprimer.exec();
    }

    this->AfficherTousLesTableaux();
}

/**
 *  @brief  Permet d'ajouter un statut de jeu.
 *
 */
void F_Preferences::on_Bt_AjouterStatut_clicked()
{
    this->pPopUpCLESTTEM->Ajouter(1);
}

/**
 *  @brief  Permet de modifier un statut de jeu.
 *
 */
void F_Preferences::on_Bt_ModifierStatut_clicked()
{
    QSqlQuery RequeteModifier;

    RequeteModifier.prepare("SELECT StatutJeu FROM statutjeux WHERE StatutJeu=:StatutJeu");
    RequeteModifier.bindValue(":StatutJeu", ui->TbV_JeuxStatut->selectionModel()->selectedRows(0).first().data().toString());
    RequeteModifier.exec();
    RequeteModifier.next();

    this->pPopUpCLESTTEM->Modifier(RequeteModifier.value(0).toString(),1);
}

/**
 *  @brief  Permet de supprimer un statut de jeu.
 *
 *  @see    AfficherTousLesTableaux()
 */
void F_Preferences::on_Bt_SupprimerStatut_clicked()
{
    QSqlQuery RequeteSupprimer;
    QString IdStatutJeux=ui->TbV_JeuxStatut->selectionModel()->selectedRows(1).first().data().toString();

    if(QMessageBox::question(this, "Suppression", "Etes-vous sur de vouloir supprimer ce statut de jeu ?", "Oui", "Non") == 0)
    {
        QSqlQuery RequeteSupprimer;
        RequeteSupprimer.prepare("SELECT * FROM jeux WHERE StatutJeux_IdStatutJeux=:StatutJeux");
        RequeteSupprimer.bindValue(":StatutJeux", IdStatutJeux);
        RequeteSupprimer.exec();
        if(RequeteSupprimer.size()!=0)
        {
            RequeteSupprimer.prepare("SELECT StatutJeu,IdStatutJeux FROM statutjeux WHERE "
                              "IdStatutJeux!=:StatutJeux");
            RequeteSupprimer.bindValue(":StatutJeux", IdStatutJeux);
            RequeteSupprimer.exec();
            int i=0;
            this->TbCLESTTEM->clear();
            this->TbCLESTTEM->setHorizontalHeaderItem(0, new QStandardItem("Valeurs"));
            while(RequeteSupprimer.next())
            {
                this->TbCLESTTEM->setItem(i, 0, new QStandardItem(RequeteSupprimer.value(0).toString()));
                this->TbCLESTTEM->setItem(i++, 1, new QStandardItem(RequeteSupprimer.value(1).toString()));
            }
            this->TbV_CLESTTEM->setSelectionMode(QAbstractItemView::SingleSelection);
            this->TbV_CLESTTEM->setSelectionBehavior(QAbstractItemView::SelectRows);
            this->TbV_CLESTTEM->resizeColumnsToContents();
            this->TbV_CLESTTEM->setColumnWidth(1,0);
            this->TbV_CLESTTEM->selectRow(0);
            int ret=this->pPopUpCLESTTEM->Modifier(IdStatutJeux, 255, this->TbV_CLESTTEM);
            if(ret==0)
            {
                return;
            }
            RequeteSupprimer.prepare("UPDATE jeux SET StatutJeux_IdStatutJeux=:StatutJeux WHERE "
                                   "StatutJeux_IdStatutJeux=:StatutJeuxPrecedent");
            QString IdStatut=this->TbV_CLESTTEM->selectionModel()->selectedRows(1).first().data().toString();
            RequeteSupprimer.bindValue(":StatutJeux", IdStatut);
            RequeteSupprimer.bindValue(":StatutJeuxPrecedent", IdStatutJeux);
            RequeteSupprimer.exec();
        }
        RequeteSupprimer.prepare("DELETE FROM statutjeux WHERE IdStatutJeux =:IdStatutJeux");
        RequeteSupprimer.bindValue(":IdStatutJeux", IdStatutJeux);
        RequeteSupprimer.exec();
    }

    this->AfficherTousLesTableaux();
}

/**
 *  @brief  Permet d'ajouter un emplacement du jeu.
 *
 */
void F_Preferences::on_Bt_AjouterEmplacement_clicked()
{
    this->pPopUpCLESTTEM->Ajouter(2);
}

/**
 *  @brief  Permet de modifier un emplacement du jeu.
 *
 */
void F_Preferences::on_Bt_ModifierEmplacement_clicked()
{
    QSqlQuery RequeteModifier;

    RequeteModifier.prepare("SELECT Nom FROM emplacement WHERE Nom=:Nom");
    RequeteModifier.bindValue(":Nom", ui->TbV_JeuxEmplacement->selectionModel()->selectedRows(0).first().data().toString());
    RequeteModifier.exec();
    RequeteModifier.next();

    this->pPopUpCLESTTEM->Modifier(RequeteModifier.value(0).toString(),2);
}

/**
 *  @brief  Permet de supprimer un emplacement du jeu.
 *
 *  @see    AfficherTousLesTableaux()
 */
void F_Preferences::on_Bt_SupprimerEmplacement_clicked()
{
    QSqlQuery RequeteSupprimer;
    QString IdEmplacement=ui->TbV_JeuxEmplacement->selectionModel()->selectedRows(1).first().data().toString();

    if(QMessageBox::question(this, "Suppression", "Etes-vous sur de vouloir supprimer cette emplacement de jeu ?", "Oui", "Non") == 0)
    {
        QSqlQuery RequeteSupprimer;
        RequeteSupprimer.prepare("SELECT * FROM jeux WHERE Emplacement_IdEmplacement=:IdEmplacement");
        RequeteSupprimer.bindValue(":IdEmplacement", IdEmplacement);
        RequeteSupprimer.exec();
        if(RequeteSupprimer.size()!=0)
        {
            RequeteSupprimer.prepare("SELECT Nom,IdEmplacement FROM emplacement WHERE "
                              "IdEmplacement!=:IdEmplacement");
            RequeteSupprimer.bindValue(":IdEmplacement", IdEmplacement);
            RequeteSupprimer.exec();
            int i=0;
            this->TbCLESTTEM->clear();
            this->TbCLESTTEM->setHorizontalHeaderItem(0, new QStandardItem("Valeurs"));
            while(RequeteSupprimer.next())
            {
                this->TbCLESTTEM->setItem(i, 0, new QStandardItem(RequeteSupprimer.value(0).toString()));
                this->TbCLESTTEM->setItem(i++, 1, new QStandardItem(RequeteSupprimer.value(1).toString()));
            }
            this->TbV_CLESTTEM->setSelectionMode(QAbstractItemView::SingleSelection);
            this->TbV_CLESTTEM->setSelectionBehavior(QAbstractItemView::SelectRows);
            this->TbV_CLESTTEM->resizeColumnsToContents();
            this->TbV_CLESTTEM->setColumnWidth(1,0);
            this->TbV_CLESTTEM->selectRow(0);
            int ret=this->pPopUpCLESTTEM->Modifier(IdEmplacement, 255, this->TbV_CLESTTEM);
            if(ret==0)
            {
                return;
            }
            RequeteSupprimer.prepare("UPDATE jeux SET Emplacement_IdEmplacement=:IdEmplacement WHERE "
                                   "Emplacement_IdEmplacement=:IdEmplacementPrecedent");
            QString IdEmp=this->TbV_CLESTTEM->selectionModel()->selectedRows(1).first().data().toString();
            RequeteSupprimer.bindValue(":IdEmplacement", IdEmp);
            RequeteSupprimer.bindValue(":IdEmplacementPrecedent", IdEmplacement);
            RequeteSupprimer.exec();
        }
        RequeteSupprimer.prepare("DELETE FROM emplacement WHERE IdEmplacement=:IdEmplacement");
        RequeteSupprimer.bindValue(":IdEmplacement", IdEmplacement);
        RequeteSupprimer.exec();
    }

    this->AfficherTousLesTableaux();
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

    ui->LE_CheminPhotosJeux->setText(NomCheminImage);
}

/**
 *  @brief  Permet d'ajouter un lieu de l'info ludo.
 *
 */
void F_Preferences::on_Bt_AjouterLieux_clicked()
{
    this->pPopUpCLESTTEM->Ajouter(8);
}

/**
 *  @brief  Permet de modifier un lieu de l'info ludo.
 *
 */
void F_Preferences::on_Bt_ModifierLieux_clicked()
{
    QSqlQuery RequeteModifier;

    RequeteModifier.prepare("SELECT NomLieux FROM lieux WHERE NomLieux=:NomLieux");
    RequeteModifier.bindValue(":NomLieux", ui->TbV_InfoLieux->selectionModel()->selectedRows(0).first().data().toString());
    RequeteModifier.exec();
    RequeteModifier.next();

    this->pPopUpCLESTTEM->Modifier(RequeteModifier.value(0).toString(),8);
}

/**
 *  @brief  Permet de supprimer un lieu de l'info ludo.
 *
 *  @see    AfficherTousLesTableaux()
 */
void F_Preferences::on_Bt_SupprimerLieux_clicked()
{
    QSqlQuery RequeteSupprimer;

    if(QMessageBox::question(this, "Suppression", "Etes-vous sur de vouloir supprimer ce lieu de l'info ludo ?", "Oui", "Non") == 0)
    {
        QString IdLieux=ui->TbV_InfoLieux->selectionModel()->selectedRows(1).first().data().toString();
        QSqlQuery RequeteSupprimer;
        RequeteSupprimer.prepare("SELECT * FROM  emprunts WHERE Lieux_IdLieux=:IdLieux");
        RequeteSupprimer.bindValue(":IdLieux", IdLieux);
        RequeteSupprimer.exec();
        QSqlQuery RequeteSupprimer2;
        RequeteSupprimer2.prepare("SELECT * FROM  reservation WHERE Lieux_IdLieuxReservation=:IdLieux");
        RequeteSupprimer2.bindValue(":IdLieux", IdLieux);
        RequeteSupprimer2.exec();
        if(RequeteSupprimer.size()!=0 || RequeteSupprimer2.size()!=0)
        {
            RequeteSupprimer.prepare("SELECT NomLieux,IdLieux FROM lieux WHERE IdLieux!=:IdLieux");
            RequeteSupprimer.bindValue(":IdLieux", IdLieux);
            RequeteSupprimer.exec();
            int i=0;
            this->TbCLESTTEM->clear();
            this->TbCLESTTEM->setHorizontalHeaderItem(0, new QStandardItem("Valeurs"));
            while(RequeteSupprimer.next())
            {
                this->TbCLESTTEM->setItem(i, 0, new QStandardItem(RequeteSupprimer.value(0).toString()));
                this->TbCLESTTEM->setItem(i++, 1, new QStandardItem(RequeteSupprimer.value(1).toString()));
            }
            this->TbV_CLESTTEM->setSelectionMode(QAbstractItemView::SingleSelection);
            this->TbV_CLESTTEM->setSelectionBehavior(QAbstractItemView::SelectRows);
            this->TbV_CLESTTEM->resizeColumnsToContents();
            this->TbV_CLESTTEM->setColumnWidth(1,0);
            this->TbV_CLESTTEM->selectRow(0);
            int ret=this->pPopUpCLESTTEM->Modifier(0, 255, this->TbV_CLESTTEM);
            if(ret==0)
            {
                return;
            }
            RequeteSupprimer.prepare("UPDATE emprunts SET Lieux_IdLieux=:IdLieux"
                                   " WHERE Lieux_IdLieux=:IdLieuxPrecedent");
            QString IdL=this->TbV_CLESTTEM->selectionModel()->selectedRows(1).first().data().toString();
            RequeteSupprimer.bindValue(":IdLieux",IdL);
            RequeteSupprimer.bindValue(":IdLieuxPrecedent", IdLieux);
            RequeteSupprimer.exec();
            RequeteSupprimer.prepare("UPDATE reservation SET Lieux_IdLieuxReservation=:IdLieux"
                                   " WHERE Lieux_IdLieuxReservation=:IdLieuxPrecedent");
            QString Idl=this->TbV_CLESTTEM->selectionModel()->selectedRows(1).first().data().toString();
            RequeteSupprimer.bindValue(":IdLieux",Idl);
            RequeteSupprimer.bindValue(":IdLieuxPrecedent", IdLieux);
            RequeteSupprimer.exec();
        }
        RequeteSupprimer.prepare("DELETE FROM lieux WHERE IdLieux=:IdLieux");
        RequeteSupprimer.bindValue(":IdLieux", IdLieux);
        RequeteSupprimer.exec();
    }

    this->AfficherTousLesTableaux();
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

/**
 *  @brief  Permet d'ajouter une activité.
 *
 *  @pre    Etre connecté à  la BDD.
 */
void F_Preferences::on_Bt_AjouterActivite_clicked()
{
    this->pPopUpCLESTTEM->Ajouter(10);
}

/**
 *  @brief   Permet de modifier une activité.
 *
 *  @pre    Etre connecté à  la BDD.
 */
void F_Preferences::on_Bt_ModifierActivite_clicked()
{
    this->pPopUpCLESTTEM->Modifier(
        ui->TbV_MembresActivite->selectionModel()->selectedRows(0).first().data().toString(),10);
}

/**
 *  @brief   Permet de supprimer une activité
 *
 *  @pre    Etre connecté à  la BDD.
 *  @see    AfficherTousLesTableaux()
 */
void F_Preferences::on_Bt_SupprimerActivite_clicked()
{
    QSqlQuery RequeteSupprimer;

    if(QMessageBox::question(this, "Suppression", "Etes-vous sur de vouloir supprimer cette activité ?", "Oui", "Non") == 0)
    {
        QString IdActivite=ui->TbV_MembresActivite->selectionModel()->selectedRows(1).first().data().toString();
        QSqlQuery RequeteSupprimer;
        RequeteSupprimer.prepare("SELECT * FROM  activitemembre WHERE Activite_IdActivite=:IdActivite");
        RequeteSupprimer.bindValue(":IdActivite", IdActivite);
        RequeteSupprimer.exec();
        if(RequeteSupprimer.size()!=0)
        {
            RequeteSupprimer.prepare("SELECT Activite,IdActivite FROM activite WHERE "
                              "IdActivite!=:IdActivite");
            RequeteSupprimer.bindValue(":IdActivite", IdActivite);
            RequeteSupprimer.exec();
            int i=0;
            this->TbCLESTTEM->clear();
            this->TbCLESTTEM->setHorizontalHeaderItem(0, new QStandardItem("Valeurs"));
            while(RequeteSupprimer.next())
            {
                this->TbCLESTTEM->setItem(i, 0, new QStandardItem(RequeteSupprimer.value(0).toString()));
                this->TbCLESTTEM->setItem(i++, 1, new QStandardItem(RequeteSupprimer.value(1).toString()));
            }
            this->TbV_CLESTTEM->setSelectionMode(QAbstractItemView::SingleSelection);
            this->TbV_CLESTTEM->setSelectionBehavior(QAbstractItemView::SelectRows);
            this->TbV_CLESTTEM->resizeColumnsToContents();
            this->TbV_CLESTTEM->setColumnWidth(1,0);
            this->TbV_CLESTTEM->selectRow(0);
            int ret=this->pPopUpCLESTTEM->Modifier(0, 255, this->TbV_CLESTTEM);
            if(ret==0)
            {
                return;
            }
            RequeteSupprimer.prepare("UPDATE  activitemembre SET Activite_IdActivite=:IdActivite"
                                   " WHERE Activite_IdActivite=:IdActivitePrecedent");
            QString IdAct=this->TbV_CLESTTEM->selectionModel()->selectedRows(1).first().data().toString();
            RequeteSupprimer.bindValue(":IdActivite",IdAct);
            RequeteSupprimer.bindValue(":IdActivitePrecedent", IdActivite);
            RequeteSupprimer.exec();
        }
        RequeteSupprimer.prepare("DELETE FROM activite WHERE IdActivite=:IdActivite");
        RequeteSupprimer.bindValue(":IdActivite", IdActivite);
        RequeteSupprimer.exec();
        RequeteSupprimer.next();
    }

    this->AfficherTousLesTableaux();
}

//---------------------------------------------------------------------------

/**
 *  @brief  Permet d'ajouter un mot-clé.
 *
 *  @pre    Etre connecté à  la BDD.
 */
void F_Preferences::on_Bt_AjouterMotCle_clicked()
{
    this->pPopUpCLESTTEM->Ajouter(11);
}

/**
 *  @brief   Permet de modifier un mot-clé.
 *
 *  @pre    Etre connecté à  la BDD.
 */
void F_Preferences::on_Bt_ModifierMotCle_clicked()
{
    this->pPopUpCLESTTEM->Modifier(
        ui->TbV_JeuxMotCle->selectionModel()->selectedRows(0).first().data().toString(),11);
}

/**
 *  @brief   Permet de supprimer un mot-clé
 *
 *  @pre    Etre connecté à  la BDD.
 *  @see    AfficherTousLesTableaux()
 */
void F_Preferences::on_Bt_SupprimerMotCle_clicked()
{
    QSqlQuery RequeteSupprimer;

    QString IdMotCle=ui->TbV_JeuxMotCle->selectionModel()->selectedRows(1).first().data().toString();
    if(QMessageBox::question(this, "Suppression", "Etes-vous sur de vouloir supprimer ce mot-clé ?", "Oui", "Non") == 0)
    {
        QSqlQuery RequeteSupprimer;
        RequeteSupprimer.prepare("SELECT * FROM jeux WHERE MotCle1=:IdMotCle OR MotCle2=:IdMotCle OR MotCle3=:IdMotCle");
        RequeteSupprimer.bindValue(":IdMotCle", IdMotCle);
        RequeteSupprimer.exec();
        if(RequeteSupprimer.size()!=0)
        {
            RequeteSupprimer.prepare("SELECT MotCle,Id_MotCle FROM motscles WHERE Id_MotCle!=:IdMotCle");
            RequeteSupprimer.bindValue(":IdMotCle", IdMotCle);
            RequeteSupprimer.exec();
            int i=0;
            this->TbCLESTTEM->clear();
            this->TbCLESTTEM->setHorizontalHeaderItem(0, new QStandardItem("Valeurs"));
            while(RequeteSupprimer.next())
            {
                this->TbCLESTTEM->setItem(i, 0, new QStandardItem(RequeteSupprimer.value(0).toString()));
                this->TbCLESTTEM->setItem(i++, 1, new QStandardItem(RequeteSupprimer.value(1).toString()));
            }
            this->TbV_CLESTTEM->setSelectionMode(QAbstractItemView::SingleSelection);
            this->TbV_CLESTTEM->setSelectionBehavior(QAbstractItemView::SelectRows);
            this->TbV_CLESTTEM->resizeColumnsToContents();
            this->TbV_CLESTTEM->setColumnWidth(1,0);
            this->TbV_CLESTTEM->selectRow(0);
            int ret=this->pPopUpCLESTTEM->Modifier(0, 255, this->TbV_CLESTTEM);
            if(ret==0)
            {
                return;
            }
            QString IdMot=this->TbV_CLESTTEM->selectionModel()->selectedRows(1).first().data().toString();
            RequeteSupprimer.prepare("UPDATE jeux SET MotCle1=:IdMotCle WHERE MotCle1=:IdMotClePrecedent");
            RequeteSupprimer.bindValue(":IdMotCle", IdMot);
            RequeteSupprimer.bindValue(":IdMotClePrecedent", IdMotCle);
            RequeteSupprimer.exec();

            RequeteSupprimer.prepare("UPDATE jeux SET MotCle2=:IdMotCle WHERE MotCle2=:IdMotClePrecedent");
            RequeteSupprimer.bindValue(":IdMotCle", IdMot);
            RequeteSupprimer.bindValue(":IdMotClePrecedent", IdMotCle);
            RequeteSupprimer.exec();

            RequeteSupprimer.prepare("UPDATE jeux SET MotCle3=:IdMotCle WHERE MotCle3=:IdMotClePrecedent");
            RequeteSupprimer.bindValue(":IdMotCle", IdMot);
            RequeteSupprimer.bindValue(":IdMotClePrecedent", IdMotCle);
            RequeteSupprimer.exec();
        }

        RequeteSupprimer.prepare("DELETE FROM motscles WHERE Id_MotCle=:IdMotCle");
        RequeteSupprimer.bindValue(":IdMotCle", IdMotCle);
        RequeteSupprimer.exec();
        RequeteSupprimer.next();
    }

    this->AfficherTousLesTableaux();
}

void F_Preferences::on_Bt_ParcourirClePrivee_clicked()
{
    QString NomClePrivee = QFileDialog::getOpenFileName(this, tr("Sélectionner le fichier..."),
                                      QDir::currentPath());
    ui->LE_ClePrivee->setText(NomClePrivee);
}

QString F_Preferences::ObtenirValeur( QString NomChamps )
{
    return TbPreferences[NomChamps];
}
