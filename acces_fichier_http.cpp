//acces_photo_http.cpp	0.1	<Vincent VICTORIN>

#include "acces_fichier_http.h"

// En-têtes standards necessaires dans ce fichier en-tete seulement ------------
#include<QtCore>
#include<QtNetwork>
#include<QtWidgets>
#include<QFile>
#include<QDebug>
#include <QObject>

#include"fonctions_globale.h"

/**
 *  @brief Constructeur de la classe.
 *
 *  @param parent
 */
AccesFichierParHTTP::AccesFichierParHTTP( QWidget * parent )
{
    manager = new QNetworkAccessManager(this);
}

AccesFichierParHTTP::~AccesFichierParHTTP()
{
    delete manager;
}

/**
 *  @brief Lance le processus de téléchargement
 *
 *  @param sCheminImagePref : Chemin ou URL des fichiers à télécharger
 *  @param code_jeu : Code du jeu correspondant aux fichiers à télécharger
 *  @param ListeExtensionOuURL : Liste des extentions des fichiers à traiter ou URL à traite (dans ce cas, code_jeu=0)
 */
void AccesFichierParHTTP::LancerTelechargements( QString sCheminImagePref,QString code_jeu,
                                                 QStringList ListeExtensionOuURL)
{
    this->sCheminImagePref=sCheminImagePref;
    this->NomFichier=code_jeu;
    this->CodeJeu=code_jeu;
    this->NumeroFichier=1;
    TelechargementAnnuler=false;
    if(code_jeu!="")
    {
        foreach (QString ext, ListeExtensionOuURL)
        {
            ListeExtensionOuURL.removeAt(ListeExtensionOuURL.indexOf(ext));
            ListeExtensionOuURL.append(ext.toLower());
            ListeExtensionOuURL.append(ext.toUpper());
        }
    }
    this->ListeExtensionOuURL=ListeExtensionOuURL;
    IndexExtension=0;
    SlotTelechargementFini();
}

/**
 *  @brief Annule le processus de téléchargement
 */
void AccesFichierParHTTP::AnnulerTelechargements()
{
    IndexExtension=ListeExtensionOuURL.count();
    TelechargementAnnuler=true;
    emit SignalAnnulerTelechargement();
}

/**
 *  @brief Traitement des téléchargements : slot de fin de téléchargement et
 *         lances le prochain téléchargement
 */
void AccesFichierParHTTP::SlotTelechargementFini()
{
    // Si il ne s'agit pas du premier téléchargement
    if(IndexExtension!=0)
    {
        QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
        QFile fichierTelecharger;
        // Nom du fichier venant d'être téléchargé
        QString NomFichier=reply->request().url().path().split("/").last();
        QFileInfo FichierInfo(NomFichier);
        // Extention du fichier venant d'être téléchargé
        QString extension=FichierInfo.suffix();
        int err=reply->error();
        switch (reply->error())
        {
            case QNetworkReply::NoError:
                // Pas d'erreur
                // Création du fichier temporaire
                fichierTelecharger.setFileName(QDir::fromNativeSeparators(QDir::tempPath()+"/"+NomFichier));
                fichierTelecharger.open(QIODevice::WriteOnly);
                //            fichierTelecharger->setAutoRemove(false);
                fichierTelecharger.write(reply->readAll());
                fichierTelecharger.close();
                delete reply;
                emit SignalFichierTelecharger(NomFichier);
                // On a trouvé l'extension du fichier, on passe au fichier suivant
                if(CodeJeu!="")
                {
                    PasserFichierSuivant(true);
                    return;
                }
                break;
            case QNetworkReply::ContentNotFoundError:
                // 404 Not found
                delete reply;
                //GestionTelechargement(fichierTelecharger.fileName());
                break;
        }
        // Si on a fait toutes les extensions, on passe au fichier suivant si il y en a
        if(CodeJeu!=""&&IndexExtension==ListeExtensionOuURL.count())
        {
            PasserFichierSuivant(false);
            return;
        }
    }
    QString URL;
    // URL du prochain téléchargement
    if(this->CodeJeu=="")
    {
        // Dans le cas du téléchargements de images BGG, si on est à la fin du vecteur des URL, on emets le signal de fin et on quitte la fonction
        if(IndexExtension==ListeExtensionOuURL.count())
        {
            emit SignalTelechargementsFini();
            return;
        }
        URL=ListeExtensionOuURL.at(IndexExtension++);
    }
    else
    {
        URL=sCheminImagePref+NomFichier+"."+ListeExtensionOuURL.at(IndexExtension++);
    }
    QNetworkReply *newreply= manager->get(QNetworkRequest(QUrl(URL)));
    QObject::connect(newreply, SIGNAL(finished()),this, SLOT(SlotTelechargementFini()));
    QObject::connect(this, SIGNAL(SignalAnnulerTelechargement()),newreply, SLOT(abort()));
}

/**
 *  @brief Passe au prochain fichier à télécharger
 *
 *  @param FichierTrouve : Booléen qui indique si le fichier vient d'être trouvé ou pas
 */
void AccesFichierParHTTP::PasserFichierSuivant(bool FichierTrouver)
{
    qDebug()<<"AccesFichierParHTTP::PasserFichierSuivant FichierTrouver : "<<FichierTrouver;
    if(TelechargementAnnuler)
    {
        return;
    }
    // Si il ne s'agit pas du premier fichier et qu'aucun fichier n'a été trouvé,
    // on sort de la fonction
    if((NomFichier.indexOf("-")!=-1) && !FichierTrouver)
    {
        emit SignalTelechargementsFini();
        return;
    }
    NumeroFichier++;
    NomFichier=CodeJeu+"-"+QString::number(NumeroFichier);
    IndexExtension=0;
    SlotTelechargementFini();
}

void AccesFichierParHTTP::TelechargementSynchrone(QString URL,QString CheminDestination)
{
    qDebug()<<"AccesFichierParHTTP::TelechargementSynchrone: URL:"<<URL<<" , CheminDestination: " <<CheminDestination;
    QNetworkAccessManager m_NetworkMngr;
    QNetworkReply *reply= m_NetworkMngr.get(QNetworkRequest(
                          QUrl(URL)));
    QEventLoop loop;
    QObject::connect(reply, SIGNAL(finished()),&loop, SLOT(quit()));
    loop.exec();

    QFile fichierTelecharger;
    switch (reply->error())
    {
        case QNetworkReply::NoError:
            // Pas d'erreur
            fichierTelecharger.setFileName(QDir::fromNativeSeparators(CheminDestination));
            if(fichierTelecharger.exists())
            {
                fichierTelecharger.remove();
            }
            fichierTelecharger.open(QIODevice::WriteOnly);
            fichierTelecharger.write(reply->readAll());
            fichierTelecharger.close();

            delete reply;
            break;
        case QNetworkReply::ContentNotFoundError:
          // 404 Not found
          /*int httpStatus = reply->attribute(
            QNetworkRequest::HttpStatusCodeAttribute).toInt();
          QByteArray httpStatusMessage = reply->attribute(
            QNetworkRequest::HttpReasonPhraseAttribute).toByteArray();*/
          delete reply;
          break;
    }

}
