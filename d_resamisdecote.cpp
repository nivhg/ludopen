#include "d_resamisdecote.h"
#include "ui_d_resamisdecote.h"

D_ResaMisDeCote::D_ResaMisDeCote(QWidget *parent,QString CodeJeu,QString NomJeu,int IdMembre,int IdJeu,QString Email) : QDialog(parent),
  ui(new Ui::D_ResaMisDeCote)
{
    ui->setupUi(this);
    enableClose=false;
    this->CodeJeu=CodeJeu;
    this->NomJeu=NomJeu;
    this->IdMembre=IdMembre;
    this->IdJeu=IdJeu;
    this->Email=Email;
    setWindowFlag(Qt::Dialog);
    setWindowFlag(Qt::WindowCloseButtonHint,false);
    setWindowFlag(Qt::WindowMaximizeButtonHint,false);
    this->setFixedWidth(289);
    ui->Lb_Reserve->setText(ui->Lb_Reserve->text().replace("#CODENOMJEU#",NomJeu+" ("+CodeJeu+")"));
    //Recupération du corps de l'email et des paramètre de connection
    ServeurSMTP =  F_Preferences::ObtenirValeur("AdresseServeurSMTP");
    PortSMTP = F_Preferences::ObtenirValeur("PortSMTP").toInt();
    if(ServeurSMTP=="" || PortSMTP==0)
    {
        QMessageBox::critical(this, "Erreur de paramétrage",
             "Serveur d'envoi de courriel (SMTP) mal paramétré.\n"
             "Vérifier le paramétrage dans les Préférences->Réseaux et relancer l'application.");
        return;
    }
    if(F_Preferences::ObtenirValeur("Email").indexOf("@")==-1)
    {
        QMessageBox::critical(this, "Erreur de paramétrage",
             "Adresse d'envoi de courriel mal formé ou vide.\n"
             "Vérifier le paramétrage dans les Préférences->Informations et relancer l'application.");
        return;
    }
    qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");
}

D_ResaMisDeCote::~D_ResaMisDeCote()
{
    delete ui;
}


void D_ResaMisDeCote::on_Bt_NonMisDeCote_clicked()
{
    this->setFixedWidth(647);
}

void D_ResaMisDeCote::closeEvent(QCloseEvent *event)
 {
    if(!enableClose)
    {
         event->ignore();
    }
 }

void D_ResaMisDeCote::on_Bt_MisDeCote_clicked()
{
    // On passe le jeu dans le statut "En réservation"
    QSqlQuery Requete;
    Requete.prepare("UPDATE jeux SET StatutJeux_IdStatutJeux="+QString::number(STATUTJEUX_ENRESERVATION)+" WHERE Jeux_IdJeux=:Jeux_IdJeux");
    Requete.bindValue(":Membres_IdMembre",this->IdMembre);
    Requete.bindValue(":Jeux_IdJeux",this->IdJeu);
    //Exectution de la requête
    if( !Requete.exec() )
    {
        qDebug() << getLastExecutedQuery(Requete) << Requete.lastError();
    }

    EMail EMailProchainAEnvoyer;
    //On remplie le vecteur des EMails avec les données de ce membre actuel
    EMailProchainAEnvoyer.sSujet = F_Preferences::ObtenirValeur("SujetEmailMisDeCote");
    EMailProchainAEnvoyer.sCorps = F_Preferences::ObtenirValeur("CorpsEmailMisDeCote");
    EMailProchainAEnvoyer.sCorps.replace("%JEU",NomJeu+" ("+CodeJeu+")").replace("%NB_SEMAINE",F_Preferences::ObtenirValeur("DelaiJeuMisDeCote"));
    EMailProchainAEnvoyer.sFrom = F_Preferences::ObtenirValeur("Email");
//    EMailProchainAEnvoyer.sTo = Email;
    EMailProchainAEnvoyer.sTo = "vincent.victorin@envelo.fr";
    EMailProchainAEnvoyer.IDMembre =  IdMembre;

    this->ListeEMailAEnvoyer.append( EMailProchainAEnvoyer ) ;

    //On crée l'objet Courriel qui permettra l'envoi des emails
    pCourriel = new Courriel( ServeurSMTP , PortSMTP , this->ListeEMailAEnvoyer ) ;

    //Connecter les bons signaux pour être prévenu ici des étapes et problèmes d'envoi d'email
    connect( pCourriel, SIGNAL( Signal_Fermer_Thread_EMail( ) ), this, SLOT( slot_FermerThreadCourriel( ) ) ) ;
    connect( pCourriel, SIGNAL( SignalMailEnvoyer( uint ) ), this, SLOT( slot_ConfirmerMailEnvoyer( uint ) ) ) ;
    connect( pCourriel, SIGNAL( Signal_Erreur_EMail( QString ) ), this, SLOT( slot_AfficherErreurMail( QString ) ) ) ;

    pCourriel->start();

    enableClose=true;
    close();
}

/**
 * @brief Permet de supprimer un objet dynamique de type Courriel une fois qu'il fait son travail
 *
 * @param pCourriel
 */
void D_ResaMisDeCote::slot_FermerThreadCourriel( )
{
   //On vérifie que le thread est en route puis on arrête le thread d'envoi d'email.
   if( this->pCourriel->isRunning() )
   {
      this->pCourriel->terminate();
      this->pCourriel->wait();
   }
   else
   {
      qDebug() << "thread Courrier pas running" ;
   }
}

/**
 * @brief Permet de supprimer un objet dynamique de type Courriel une fois que le mail est envoyé et
 * met à jour la base données pour dire que le mail a bien été envoyé
 *
 * @param pCourriel
 */
void D_ResaMisDeCote::slot_ConfirmerMailEnvoyer( uint IDMembre  )
{
    //On vérifie que le pointeur n'est pas null puis on supprime l'objet et on met à  jour la base de données
    //pour dire que le mail a bien été envoyé
    if( pCourriel == NULL)
    {
       qDebug() << "Erreur : F_Retards::slot_ConfirmerMailEnvoyer : Pointeur retourné est NULL" ;
    }
}

/**
 * @brief Afficher les erreurs qu'il peut y avoir lors de l'envoie d'un mail
 *
 * @param sErreur
 */
void D_ResaMisDeCote::slot_AfficherErreurMail( QString sErreur )
{
    //Affiche l'erreur en paramètre dans le label prévu à  cette effet
    QMessageBox::critical(this, "Erreur lors de l'envoi du courriel",sErreur ) ;
}

void D_ResaMisDeCote::on_Bt_Valider_clicked()
{
    EMail EMailProchainAEnvoyer;
    //On remplie le vecteur des EMails avec les données de ce membre actuel
    EMailProchainAEnvoyer.sSujet = F_Preferences::ObtenirValeur("SujetEmailNonMisDeCote");
    EMailProchainAEnvoyer.sCorps = F_Preferences::ObtenirValeur("CorpsEmailNonMisDeCote");
    EMailProchainAEnvoyer.sCorps.replace("%JEU",NomJeu+" ("+CodeJeu+")").replace("%RAISON",ui->Te_NonMisDeCote->toPlainText());
    EMailProchainAEnvoyer.sFrom = F_Preferences::ObtenirValeur("Email");
//    EMailProchainAEnvoyer.sTo = Email;
    EMailProchainAEnvoyer.sTo = "vincent.victorin@envelo.fr";
    EMailProchainAEnvoyer.IDMembre =  IdMembre;

    this->ListeEMailAEnvoyer.append( EMailProchainAEnvoyer ) ;

    //On crée l'objet Courriel qui permettra l'envoi des emails
    pCourriel = new Courriel( ServeurSMTP , PortSMTP , this->ListeEMailAEnvoyer ) ;

    //Connecter les bons signaux pour être prévenu ici des étapes et problèmes d'envoi d'email
    connect( pCourriel, SIGNAL( Signal_Fermer_Thread_EMail( ) ), this, SLOT( slot_FermerThreadCourriel( ) ) ) ;
    connect( pCourriel, SIGNAL( SignalMailEnvoyer( uint ) ), this, SLOT( slot_ConfirmerMailEnvoyer( uint ) ) ) ;
    connect( pCourriel, SIGNAL( Signal_Erreur_EMail( QString ) ), this, SLOT( slot_AfficherErreurMail( QString ) ) ) ;

    pCourriel->start();

    enableClose=true;
    close();
}
