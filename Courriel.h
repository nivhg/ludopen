// Courriel.h	0.1	05/2012     Padiou
#ifndef Courriel_H
#define Courriel_H

// En-têtes standards necessaires dans ce fichier en-tete seulement ------------
#include <QtNetwork/QTcpSocket>
#include <QString>
#include <QTextStream>
#include <QDebug>
#include <QtGui/QMessageBox>
#include <QThread>



/** @enum   Etape
 *  @brief  Permet de repérer chaque étape de la connexion

 */
typedef enum
{
    Init, ///< initialisation
    From, /// < envoie du mail de l'expediteur
    To,   ///< envoie du mail du destinataire
    Data, ///< Indique que l'on va envoyer les différentes information du mail
    Body, ///< envoie du mail
    Quitter, ///< envoie un signal pour arrêter la connexion
    Erreur ///< S'il y a une erreur on passe par cette étape
}Etape ;

/** @brief Permet l'envoie d'un mail via un serveur smpt ne demandant pas d'authentification
 *
 *Initialise la connexion vers un serveur ftp puis lui envoie les différentes informations pour l'envoie d'un mail.
 *S'il y a une erreur un signal est emis avec le type d'erreur.
 *Quand le mail est envoyer ou une demande de fermeture est faites, un signal est emis pour que l'objet puisse être détruit.
 */

class Courriel : public QThread
{
Q_OBJECT


public:
    //! Permet d'initialiser tous les attributs de la classe et les 'connect' avec le socket.
    Courriel(const QString sAdresseCourriel, const int nPort, const QString sFrom, const QString sTo, const QString sSujet, const QString sCorps) ;
    //! Détruit les objets créé par le pointeur
    ~Courriel() ;

    //! Permet la connexion au serveur. Indique que l'on rentre dans l'étape d'initialisation
    void EnvoyerUnMessage() ;

private slots:
    //! Indique qu'il y a une erreur lors de la connection et quelle est cette erreur.
    void slot_ErreurRecu(QAbstractSocket::SocketError ErreurSocket) ;
    //! Indique la deconnexion du serveur
    void slot_Deconnecte() ;
    //! Indique la connexion au serveur
    void slot_Connecte() ;
    //! Permet l'envoie du mail. Est appelé a chaque fois qu'un étape est validé.
    void slot_PretALire() ;

private:
    // METHODEs -----------------------------------------------------------------
    //! Permet de lancer le thread.
    void run() ;

    // ATTRIBUTs ----------------------------------------------------------------
    QString sMessage ;///< Contient le message à envoyer au serveur
    QTextStream *oFlux ; ///< Permet d'utiliser le socket avec un flux. Plus facile à utiliser
    QTcpSocket *oSocket ; ///< Pointeur sur le socket
    QString sFrom ; ///< Email de l'expediteur
    QString sTo ; ///< Email du destinataire
    QString sAdresseSmtp ; ///< Contient l'adresse du serveur smtp
    int nPort ; ///< Contient le numéro du port
    Etape EtapeConnexion ; ///< Contient l'étape ou en rendu la connexion

signals:
    //! Signal indiquand qu'il faut détruire le thread
    void SignalFermerThread( Courriel * ) ;
    //! Signal indiquand que le mail a bien été envoyé et qu'il faut détruire le thread
    void SignalMailEnvoyer( Courriel * ) ;
    //! Signal indiquand qu'il y a une erreur lors de la connexion
    void SignalErreur( QString ) ;
} ;
#endif
