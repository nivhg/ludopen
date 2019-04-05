//------------------------------------------------------------------------------
/** @file         Courriel.cpp
 *  @brief        Permet l'envoi d'un email via un serveur smtp
 *
 *  @author       Padiou Nicolas
 *  @author       STS IRIS, Lycée Nicolas APPERT, ORVAULT (FRANCE)
 *  @since        05/2012
 *  @version      0.1
 *  @date         13/06/2012
 *  @date         10/05/2014 <William> <Un seul thread pour envoyer tous les emails stockés dans un vecteur
 *
 *
 *  @todo         Connexion avec authentification
 *
 */
//------------------------------------------------------------------------------


// En-tête propre à l'application ----------------------------------------------
#include "Courriel.h"
#include <QtNetwork/QAbstractSocket>

/** Initialise la connexion vers un serveur smtp puis lui envoie les différentes informations pour l'envoie d'un email.
 *  S'il y a une erreur un signal est emis avec le type d'erreur.
 *  Quand le mail est envoyé ou une demande de fermeture est faite, un signal est émis pour que l'objet puisse être détruit.
 *
 *  @param const QString sAdresseSmtp, const int nPort, const QString sFrom, const QString sTo, const QString sSujet, const QString sCorps
 *  @test   Voir la procédure dans le fichier associé.
 */
Courriel::Courriel( const QString sAdresseServeurSNMP, const uint nPort, QVector <EMail> ListeEMailAEnvoyer) :
    QThread()
{
   //  avoir accès au vecteur qui contient les emails à envoyer
   this->ListeEMailAEnvoyer = ListeEMailAEnvoyer ;

   //Récupération des données passées en paramètre
   this->sAdresseSmtp = sAdresseServeurSNMP ;
   this->nPort = nPort ;

    SocketSMTP=0;
}

/** Détruit les objets dynamiques créés
 *  @test   Voir la procédure dans le fichier associé.
 */
Courriel::~Courriel()
{
   qDebug("Courriel::~Courriel DEB") ;
   if(SocketSMTP)
   {
        SocketSMTP->disconnectFromHost();
   }
   qDebug("Courriel::~Courriel FIN") ;
}

/** Indique s'il y a une erreur de connexion et quelle est cette erreur
 *  @param  QAbstractSocket::SocketError ErreurSocket
 *  @test   Voir la procédure dans le fichier associé.
 *  @see    afficherAttribut
 */
void Courriel::slot_ErreurRecue(QAbstractSocket::SocketError ErreurSocket)
{
    if (ErreurSocket != QAbstractSocket::RemoteHostClosedError)
    {
        qDebug() << "Courriel::slot_ErreurRecue :  " << ErreurSocket ;
        emit( this->Signal_Erreur_EMail( this->sTo + ": Impossible de se connecter au serveur smtp." ) ) ;
        emit( this->Signal_Fermer_Thread_EMail( ) ) ;
    }
    else
    {
        qDebug() << "L'hôte SMTP a fermé la connexion" ;
    }
}

/** Indique que il y a une deconnexion avec le serveur
 *  @test   Voir la procédure dans le fichier associé.
 */
void Courriel::slot_Deconnecte()
{
    qDebug() << "Courriel::slot_Deconnecte : Déconnecté du serveur d'email" ;
}

/** Indique que il y a une connexion avec le serveur
 *  @test   Voir la procédure dans le fichier associé.
 */
void Courriel::slot_Connecte()
{
    qDebug() << "Connecté au serveur d'email" ;
}

/** Permet de lancer le thread qui envoie le message par email
 *  @test   Voir la procédure dans le fichier associé.
 */
void Courriel::run()
{
    qDebug()<<"Courriel::run DEB";

    // On traite le premier email :
    this->NumeroEmailATraiter = 0 ;

    this->sFrom = this->ListeEMailAEnvoyer.at(this->NumeroEmailATraiter).sFrom ;
    this->sTo = this->ListeEMailAEnvoyer.at(this->NumeroEmailATraiter).sTo ;

    this->sMessage="From: " + this->sFrom + "\n";
    this->sMessage.append("To: " + this->sTo + "\n");
    this->sMessage.append("Subject: " + this->ListeEMailAEnvoyer.at(this->NumeroEmailATraiter).sSujet + "\n") ;
    this->sMessage.append("Content-Type: text/plain; charset=\"UTF-8\"\n\n");
    this->sMessage.append( this->ListeEMailAEnvoyer.at(this->NumeroEmailATraiter).sCorps ) ;
    this->sMessage.replace( QString::fromLatin1( "\n" ), QString::fromLatin1( "\r\n" ) ) ;
    this->sMessage.replace( QString::fromLatin1( "\r\n.\r\n" ), QString::fromLatin1( "\r\n..\r\n" ) ) ;
    qDebug()<<sMessage;

    this->SocketSMTP=new QTcpSocket(this);

    //connect avec le socket-----------------------------------------------------------------------------------------------
    connect( this->SocketSMTP, SIGNAL( readyRead() ), this, SLOT( slot_ReceptionDonnees() ) ) ;
    connect( this->SocketSMTP, SIGNAL( connected() ), this, SLOT( slot_Connecte() ) ) ;
    connect( this->SocketSMTP, SIGNAL( error( QAbstractSocket::SocketError ) ), this, SLOT( slot_ErreurRecue( QAbstractSocket::SocketError ) ) ) ;
    connect( this->SocketSMTP, SIGNAL( disconnected() ), this, SLOT( slot_Deconnecte() ) ) ;
    //connect( this, SIGNAL( SignalMailEnvoyer( uint ) ), this, SLOT( TraiterEMailSuivant( uint ) ) ) ;
    // start the tcp thread
    //SocketSMTP->moveToThread(this);    // move events to thread
    //this->start();   // start the thread

    //Connection au serveur smtp
    SocketSMTP->connectToHost( this->sAdresseSmtp , this->nPort) ;
    this->EtapeConnexion = Init ;

    this->exec();
    qDebug("Courriel::run FIN") ;
}

/** Permet d'exécuter les différentes étapes pour l'envoie d'un mail
 *  @test   Voir la procédure dans le fichier associé.
 */
void Courriel::slot_ReceptionDonnees()
{
    QString ReponseServeurSMTP ;
    //Attente de la réponse
    do
    {
        ReponseServeurSMTP = this->SocketSMTP->readLine() ;
        //qDebug() << ReponseServeurSMTP ;
    }
    while ( this->SocketSMTP->canReadLine() ) ;

    ReponseServeurSMTP.truncate( 3 ) ;

    //Envoi des différents messages email
    switch(this->EtapeConnexion)
    {
    case Init :
    {
        if( ReponseServeurSMTP == "220" )
        {
            //Bannière, On dit bonjour au serveur
            this->SocketSMTP->write(QByteArray("HELO there\r\n"));
            this->SocketSMTP->flush();
            qDebug() << "Etape : INIT-----------------";
            this->EtapeConnexion = From ;
        }
        else
        {
            //Indique qu'il y a une erreur lors de l'initialisation
            this->EtapeConnexion = Erreur ;
            qDebug() << "Erreur, Etape : Init";
            emit( this->Signal_Erreur_EMail( this->sTo + ": Erreur lors de l'étape de connexion." ) ) ;
            emit( this->Signal_Fermer_Thread_EMail( ) ) ;
        }
        break ;
    }

    case From :
    {
        if( ReponseServeurSMTP == "250" )
        {
            // Expediteur. On indique le mail de l'expediteur
            qDebug() << "Etape : FROM=" << this->sFrom;
            QString requete ="MAIL FROM: "+this->sFrom+"\r\n";
            this->SocketSMTP->write(requete.toUtf8());
            this->SocketSMTP->flush();
            this->EtapeConnexion = To ;
        }
        else
        {
            //Indique qu'il y a une erreur lors de l'envoie de la bannière
            this->EtapeConnexion = Erreur ;
            qDebug() << "Erreur, Etape : From=" << this->sFrom;
            emit( this->Signal_Erreur_EMail( this->sTo + ": Erreur lors de l'étape d'initialisation." ) ) ;
            // On passe à l'email suivant:
            this->TraiterEMailSuivant();
            //emit( this->Signal_Fermer_Thread_EMail( ) ) ;
        }
        break ;
    }

    case To :
    {
        if ( ReponseServeurSMTP == "250" )
        {
            //Destinataire. On indique le mail du destinataire
            qDebug() << "Etape : To=" << this->sTo;
            QString requete="RCPT TO: "+this->sTo+"\r\n";
            this->SocketSMTP->write(requete.toUtf8());
            this->SocketSMTP->flush();
            this->EtapeConnexion = Data ;
        }
        else
        {
            //Indique qu'il y a une erreur lors de l'envoi du mail de l'expediteur
            this->EtapeConnexion = Erreur ;
            qDebug() << "Erreur, Etape : To";
            emit( this->Signal_Erreur_EMail( this->sTo + ": Adresse email de l'expediteur incorrect." ) ) ;
            // On passe à l'email suivant:
            this->TraiterEMailSuivant();
            //emit( this->Signal_Fermer_Thread_EMail( ) ) ;
        }
        break ;
    }

    case Data :
    {
        if ( ReponseServeurSMTP == "250" )
        {
            //Indique au serveur que l'on va lui envoyer un message
            qDebug() << "Etape : DATA------------------";
            this->SocketSMTP->write(QByteArray("DATA\r\n"));
            this->SocketSMTP->flush();
            this->EtapeConnexion = Body ;
        }
        else
        {
            //Indique qu'il y a une erreur lors de l'envoie du mail du destinataire
            this->EtapeConnexion = Erreur ;
            qDebug() << "Erreur, Etape : Data";
            emit( this->Signal_Erreur_EMail( this->sTo + ": Adresse email du destinataire incorrect." ) ) ;
            // On passe à l'email suivant:
            this->TraiterEMailSuivant();
            //emit( this->Signal_Fermer_Thread_EMail( ) ) ;
        }
        break ;
    }

    case Body :
    {
        if ( ReponseServeurSMTP == "354" )
        {
            //Message, Envoi du message au serveur

            qDebug() << "Etape Body=" ; //<<this ->sMessage ;
            QString requete =this->sMessage+"\r\n.\r\n";
            this->SocketSMTP->write(requete.toUtf8());
            this->SocketSMTP->flush();
            emit( this->SignalMailEnvoyer(  this->ListeEMailAEnvoyer.at(this->NumeroEmailATraiter).IDMembre ) ) ;
            // On passe à l'email suivant:
            this->TraiterEMailSuivant();
        }
        else
        {
            //Indique qu'il y a une erreur lors de la demande pour envoyer le message
            this->EtapeConnexion = Erreur ;
            qDebug() << "Erreur, Etape : Body";
            emit( this->Signal_Erreur_EMail( this->sTo + ": Erreur lors de l'étape DATA" ) ) ;
            // On passe à l'email suivant:
            this->TraiterEMailSuivant();
            //emit( this->Signal_Fermer_Thread_EMail( ) ) ;
        }
        break ;
    }

    case Quitter :
    {
        if ( ReponseServeurSMTP == "250" )
        {
            //On termine la connection
            QString requete=this->sMessage+"QUIT\r\n";
            this->SocketSMTP->write(requete.toUtf8());
            this->SocketSMTP->flush();
            qDebug() << "Etape QUIT--------------------" ;
            emit( this->Signal_Fermer_Thread_EMail( ) ) ;
        }
        else
        {
            //Indique qu'il y a une erreur lors de l'envoi du message
            this->EtapeConnexion = Erreur ;
            qDebug() << "Erreur, Etape : Quitter";
            //emit( this->Signal_Erreur_EMail( "EMAIL : Fin de connexion impossible" ) ) ;
            emit( this->Signal_Fermer_Thread_EMail( ) ) ;
        }
        break ;
    }

    //S'il y a une erreur on indique qu'il y a une erreur et on quitte la connexion
    default :
        qDebug() << "Erreur de connection" ;
        QString requete=this->sMessage+"QUIT\r\n";
        this->SocketSMTP->write(requete.toUtf8());
        this->SocketSMTP->flush();
        emit( this->Signal_Erreur_EMail( "EMAIL : Fin de connexion impossible" ) ) ;
        emit( this->Signal_Fermer_Thread_EMail( ) ) ;
    }
}

/** Permet de traiter l'email suivant du vecteur des emails à envoyer ou de finir la connexion avec le serveur SMTP si plus d'email à traiter
 *  @param Courriel virera surement plus tard car servait seulement si pls thread pour envoyer chacun un seul email
 */
void Courriel::TraiterEMailSuivant()
{
   // On passe à l'email suivant:
   this->NumeroEmailATraiter++ ;

   if ( this->NumeroEmailATraiter < this->ListeEMailAEnvoyer.count() )
   {
      this->sFrom = this->ListeEMailAEnvoyer.at(this->NumeroEmailATraiter).sFrom ;
      this->sTo = this->ListeEMailAEnvoyer.at(this->NumeroEmailATraiter).sTo ;
      this->sMessage = "To: " + this->sTo + "\n" ;
      this->sMessage.append("From: " + this->sFrom + "\n") ;
      this->sMessage.append("Subject: " + this->ListeEMailAEnvoyer.at(this->NumeroEmailATraiter).sSujet + "\n") ;
      this->sMessage.append("Content-Type: text/plain; charset=\"UTF-8\"\n\n");
      this->sMessage.append( this->ListeEMailAEnvoyer.at(this->NumeroEmailATraiter).sCorps ) ;
      this->sMessage.replace( QString::fromLatin1( "\n" ), QString::fromLatin1( "\r\n" ) ) ;
      this->sMessage.replace( QString::fromLatin1( "\r\n.\r\n" ), QString::fromLatin1( "\r\n..\r\n" ) ) ;
      this->EtapeConnexion = From ;
   }
   else
   {
      this->EtapeConnexion = Quitter ;
   }
}
