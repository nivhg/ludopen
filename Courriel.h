// Courriel.h	0.1	05/2012     Padiou
#ifndef Courriel_H
#define Courriel_H

// En-têtes standards necessaires dans ce fichier en-tete seulement ------------
#include <QtNetwork/QTcpSocket>
#include <QString>
#include <QTextStream>
#include <QDebug>
#include <QMessageBox>
#include <QThread>
#include <QVector>


/** @enum   Etape
 *  @brief  Permet de repérer chaque étape de la connexion

 */
typedef enum
{
    Init,      ///< initialisation
    From,      /// < courriel de l'expediteur
    To,        ///< courriel du destinataire
    Cc,        ///< courriel du destinataire en copie
    Bcc,        ///< courriel du destinataire en copie caché
    Data,      ///< Indique que l'on va envoyer les différentes information du mail
    Body,      ///< envoie du mail
    Quitter,   ///< envoie un signal pour arrêter la connexion
    Erreur     ///< S'il y a une erreur on passe par cette étape
}Etape ;

typedef struct
{
   uint    IDMembre ;         ///< ID du membre destinataire de l'email
   QString sSujet ;           ///< Sujet de l'email
   QString sCorps ;           ///< Corps de l'email
   QString sFrom ;            ///< Email de l'expéditeur
   QString sTo ;              ///< Email du destinataire
   QString sCc ;              ///< Email du destinataire en copie
   QString sBcc ;              ///< Email du destinataire en copie caché
} EMail ;

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
    Courriel(const QString sAdresseServeurSNMP, const uint nPort, QList <EMail> ListeEMailAEnvoyer ) ;
    //! Détruit les objets créé par le pointeur
    ~Courriel() ;

private slots:
    //! Indique qu'il y a une erreur lors de la connection et quelle est cette erreur.
    void slot_ErreurRecue(QAbstractSocket::SocketError ErreurSocket) ;
    //! Indique la deconnexion du serveur
    void slot_Deconnecte() ;
    //! Indique la connexion au serveur
    void slot_Connecte() ;
    //! Permet l'envoie du mail. Est appelé a chaque fois qu'un étape est validé.
    void slot_ReceptionDonnees() ;

private:
    // METHODEs -----------------------------------------------------------------
    //! Permet de passer à l'email suivant du vecteur, s'il en reste à traiter
    void TraiterEMailSuivant( ) ;
    //! Permet de lancer le thread d'envoie d'email
    void run() ;

    // ATTRIBUTs ----------------------------------------------------------------

    QString sAdresseSmtp ;       ///< l'adresse du serveur smtp
    int nPort ;                  ///< le numéro du port
    Etape EtapeConnexion ;       ///< l'étape ou en rendu la connexion

    QString sFrom ;            ///< Email de l'expéditeur
    QString sTo ;              ///< Email du destinataire
    QString sCc ;              ///< Email du destinataire en copie
    QString sBcc ;              ///< Email du destinataire en copie caché
    QString sMessage ;         ///< le message à envoyer au serveur

    QList <EMail> ListeEMailAEnvoyer ;  ///< vecteur qui contient la liste des emails à envoyer.
    unsigned short NumeroEmailATraiter ;  ///< Numéro de la case du vecteur qui contient la liste des emails à envoyer

private:
    QTcpSocket *SocketSMTP ;      ///< socket pour l'envoi d'email

signals:
    //! Signal indiquand qu'il faut détruire le thread
    void Signal_Fermer_Thread_EMail( ) ;
    //! Signal indiquand que le mail a bien été envoyé et qu'il faut détruire le thread
    void SignalMailEnvoyer( uint IDMembre ) ;
    //! Signal indiquand qu'il y a une erreur lors de la connexion
    void Signal_Erreur_EMail( QString ) ;
} ;

#endif
