//------------------------------------------------------------------------------
/** @file         Courriel.cpp
 *  @brief        Permet l'envoie d'un mail via un serveur smtp
 *
 *  @author       Padiou Nicolas
 *  @author       STS IRIS, Lycée Nicolas APPERT, ORVAULT (FRANCE)
 *  @since        05/2012
 *  @version      0.1
 *  @date         13/06/2012
 *
 *
 *
 *  @todo         Connexion avec authentification
 *
 */
//------------------------------------------------------------------------------


// En-tête propre à l'application ----------------------------------------------
#include "Courriel.h"
#include <QtNetwork/QAbstractSocket>

/** Initialise la connexion vers un serveur ftp puis lui envoie les différentes informations pour l'envoie d'un mail.
 *  S'il y a une erreur un signal est emis avec le type d'erreur.
 *  Quand le mail est envoyer ou une demande de fermeture est faites, un signal est emis pour que l'objet puisse être détruit.
 *
 *  @param const QString sAdresseSmtp, const int nPort, const QString sFrom, const QString sTo, const QString sSujet, const QString sCorps
 *  @test   Voir la procédure dans le fichier associé.
 */
Courriel::Courriel( const QString sAdresseSmtp, const int nPort, const QString sFrom, const QString sTo, const QString sSujet, const QString sCorps ) :
    QThread ( NULL )
{
    //Création du socket
    this->oSocket = new QTcpSocket( NULL ) ;

    //connect avec le socket-----------------------------------------------------------------------------------------------
    connect( this->oSocket, SIGNAL( readyRead() ), this, SLOT( slot_PretALire() ) ) ;
    connect( this->oSocket, SIGNAL( connected() ), this, SLOT( slot_Connecte() ) ) ;
    connect( this->oSocket, SIGNAL( error( QAbstractSocket::SocketError ) ), this, SLOT( slot_ErreurRecu( QAbstractSocket::SocketError ) ) ) ;
    connect( this->oSocket, SIGNAL( disconnected() ), this, SLOT( slot_Deconnecte() ) ) ;

    //Récupération des données passées en paramètre
    this->sFrom = sFrom ;
    this->sTo = sTo ;
    this->sAdresseSmtp = sAdresseSmtp ;
    this->nPort = nPort ;

    this->sMessage = "To: " + this->sTo + "\n" ;
    this->sMessage.append("From: " + this->sFrom + "\n") ;
    this->sMessage.append("Subject: " + sSujet + "\n") ;
    this->sMessage.append( sCorps ) ;
    this->sMessage.replace( QString::fromLatin1( "\n" ), QString::fromLatin1( "\r\n" ) ) ;
    this->sMessage.replace( QString::fromLatin1( "\r\n.\r\n" ), QString::fromLatin1( "\r\n..\r\n" ) ) ;

    //Création du flux
    this->oFlux = new QTextStream( this->oSocket ) ;

    this->start();
}

/** Détruit les objets dynamiques créés
 *  @test   Voir la procédure dans le fichier associé.
 */
Courriel::~Courriel()
{
    if(this->oFlux != NULL)
    {
        delete this->oFlux ;
        this->oFlux = NULL ;
    }

    if( this->oSocket )
    {
        delete this->oSocket ;
        this->oSocket = NULL ;
    }

}

/** Permet la connexion au serveur smtp. Etape d'initialisation
 *  @test   Voir la procédure dans le fichier associé.
 */
void Courriel::EnvoyerUnMessage()
{
    //Connection au serveur smtp
    this->oSocket->connectToHost( this->sAdresseSmtp , this->nPort) ;

    this->EtapeConnexion = Init ;
}

/** Permet de lancer le thread
 *  @test   Voir la procédure dans le fichier associé.
 */
void Courriel::run()
{
    qDebug("Courriel::run") ;
    this->EnvoyerUnMessage() ;
}

/** Indique s'il y a une erreur de connexion et quelle est cette erreur
 *  @param  QAbstractSocket::SocketError ErreurSocket
 *  @test   Voir la procédure dans le fichier associé.
 *  @see    afficherAttribut
 */
void Courriel::slot_ErreurRecu(QAbstractSocket::SocketError ErreurSocket)
{
    if (ErreurSocket != QAbstractSocket::RemoteHostClosedError)
    {
        qDebug() << "Courriel::slot_ErreurRecu :  " << ErreurSocket ;
        emit( this->SignalErreur( "Impossible de se connecter au serveur smtp. Veuillez vérifier les paramètres de connexion." ) ) ;
        emit( this->SignalFermerThread( this ) ) ;
    }
    else
    {
        qDebug() << "L'hote a fermé la connexion" ;
    }

}

/** Indique que il y a une deconnexion avec le serveur
 *  @test   Voir la procédure dans le fichier associé.
 */
void Courriel::slot_Deconnecte()
{
    qDebug() << "disconneted" ;
}

/** Indique que il y a une connexion avec le serveur
 *  @test   Voir la procédure dans le fichier associé.
 */
void Courriel::slot_Connecte()
{
    qDebug() << "Connecté " ;
}

/** Permet d'exécuter les différentes étapes pour l'envoie d'un mail
 *  @test   Voir la procédure dans le fichier associé.
 */
void Courriel::slot_PretALire()
{
    QString sReponseDeLigne ;
    //Attente de la réponse
    do
    {
        sReponseDeLigne = this->oSocket->readLine() ;
        qDebug() << sReponseDeLigne ;
    }
    while ( this->oSocket->canReadLine() ) ;

    sReponseDeLigne.truncate( 3 ) ;

    //Envoie des différents messages
    switch(this->EtapeConnexion)
    {
    case Init :
    {
        if( sReponseDeLigne == "220" )
        {
            //Bannière, On dit bonjour au serveur
            *this->oFlux << "HELO there\r\n" ;
            this->oFlux->flush() ;

            this->EtapeConnexion = From ;
        }
        else
        {
            //Indique qu'il y a une erreur lors de l'initialisation
            this->EtapeConnexion = Erreur ;
            qDebug() << "Erreur, Etape : Init";
            emit( this->SignalErreur( "Erreur lors de l'étape de connexion." ) ) ;
        }

        break ;
    }

    case From :
    {
        if( sReponseDeLigne == "250" )
        {
            // Expediteur. On indique le mail de l'expediteur

            *this->oFlux << "MAIL FROM: " << this->sFrom << "\r\n" ;
            this->oFlux->flush() ;
            this->EtapeConnexion = To ;
        }
        else
        {
            //Indique qu'il y a une erreur lors de l'envoie de la bannière
            this->EtapeConnexion = Erreur ;
            qDebug() << "Erreur, Etape : From";
            emit( this->SignalErreur( "Erreur lors de l'étape d'initialisation." ) ) ;
        }


        break ;
    }

    case To :
    {
        if ( sReponseDeLigne == "250" )
        {
            //Destinataire. On indique le mail du destinataire

            *this->oFlux << "RCPT TO: " << this->sTo << "\r\n" ;
            this->oFlux->flush() ;
            this->EtapeConnexion = Data ;
        }
        else
        {
            //Indique qu'il y a une erreur lors de l'envoie du mail de l'expediteur
            this->EtapeConnexion = Erreur ;
            qDebug() << "Erreur, Etape : To";
            emit( this->SignalErreur( "Adresse email de l'expediteur incorrect." ) ) ;
        }


        break ;
    }

    case Data :
    {
        if ( sReponseDeLigne == "250" )
        {
            //Indique au serveur que l'on va lui envoyer un message

            *this->oFlux << "DATA\r\n" ;
            this->oFlux->flush() ;
            this->EtapeConnexion = Body ;
        }
        else
        {
            //Indique qu'il y a une erreur lors de l'envoie du mail du destinataire
            this->EtapeConnexion = Erreur ;
            qDebug() << "Erreur, Etape : Data";
            emit( this->SignalErreur( "Adresse email du destinataire incorrect." ) ) ;
        }


        break ;
    }

    case Body :
    {

        if ( sReponseDeLigne == "354" )
        {
            //Message, Envoie du message au serveur

            qDebug() << this ->sMessage ;
            *this->oFlux << this->sMessage << "\r\n.\r\n" ;
            this->oFlux->flush() ;
            this->EtapeConnexion = Quitter ;
        }
        else
        {
            //Indique qu'il y a une erreur lors de la demande pour envoyer le message
            this->EtapeConnexion = Erreur ;
            qDebug() << "Erreur, Etape : Body";
            emit( this->SignalErreur( "Erreur lors de l'étape DATA" ) ) ;
        }


        break ;
    }

    case Quitter :
    {
        if ( sReponseDeLigne == "250" )
        {
            //On termine la connection

            *this->oFlux << "QUIT\r\n" ;
            this->oFlux->flush() ;

            qDebug() << "Fin de la connection" ;
            emit( this->SignalMailEnvoyer( this ) ) ;
        }
        else
        {
            //Indique qu'il y a une erreur lors de l'envoie du message
            this->EtapeConnexion = Erreur ;
            qDebug() << "Erreur, Etape : Quitter";
            emit( this->SignalErreur( "Message incorrect." ) ) ;
        }


        break ;
    }



    //S'il y a une erreur on indique qu'il y a une erreur et on quitte la connexion
    default :
        qDebug() << "Erreur de connection" ;
        *this->oFlux << "QUIT\r\n" ;
        this->oFlux->flush() ;
        emit( this->SignalFermerThread( this ) ) ;
    }
}


