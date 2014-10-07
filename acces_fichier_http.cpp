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
 *  @param fname : nom et chemin du fichier à afficher
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
 *  @brief Vérifie l'existence d'un fichier à partir d'une liste d'extension sur un système de fichier ou en HTTP
 *
 *  @param url :
 *          en entrée : chemin ou URL de l'image à rechercher
 *          en sortie : chemin du fichier trouvé. Dans le cas d'une image HTTP :
 *              chemin et nom du fichier temporaire où a été téléchargé l'image
 *  @param ext : Extention du fichier à rechercher.
 *  @param TypeImage : Type de fichier image trouvé (JPG, BMP...)
 *  @param extlist : Liste des extensions à vérifier
 */
void AccesFichierParHTTP::LancerTelechargements( QString sCheminImagePref,QString code_jeu,QStringList ListeExtension)
{
    this->sCheminImagePref=sCheminImagePref;
    this->NomFichier=code_jeu;
    this->CodeJeu=code_jeu;
    this->NumeroFichier=1;
    TelechargementAnnuler=false;
    foreach (QString ext, ListeExtension)
    {
        ListeExtension.removeAt(ListeExtension.indexOf(ext));
        ListeExtension.append(ext.toLower());
        ListeExtension.append(ext.toUpper());
    }
    this->ListeExtension=ListeExtension;
    IndexExtension=0;
    SlotTelechargementFini();
}

void AccesFichierParHTTP::AnnulerTelechargements()
{
    IndexExtension=ListeExtension.count();
    TelechargementAnnuler=true;
    emit SignalAnnulerTelechargement();
}

void AccesFichierParHTTP::SlotTelechargementFini()
{
    // Si il ne s'agit pas du premier téléchargement
    if(IndexExtension!=0)
    {
        QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
        QFile tempfile;
        QString NomFichier=reply->request().url().path().split("/").last();
        QFileInfo FichierInfo(NomFichier);
        QString extension=FichierInfo.suffix();

        switch (reply->error())
        {
            case QNetworkReply::NoError:
                // Pas d'erreur
                // Création du fichier temporaire
                tempfile.setFileName(QDir::fromNativeSeparators(QDir::tempPath()+"/"+NomFichier));
                tempfile.open(QIODevice::WriteOnly);
                //            tempfile->setAutoRemove(false);
                tempfile.write(reply->readAll());
                tempfile.close();
                delete reply;
                emit SignalFichierTelecharger(tempfile.fileName());
                PasserFichierSuivant(true);
                return;
                break;
            case QNetworkReply::ContentNotFoundError:
                // 404 Not found
                delete reply;
                //GestionTelechargement(tempfile.fileName());
                break;
        }
        if(IndexExtension==ListeExtension.count())
        {
            PasserFichierSuivant(false);
            return;
        }
    }
    QString URL=sCheminImagePref+NomFichier+"."+ListeExtension.at(IndexExtension++);
    QNetworkReply *newreply= manager->get(QNetworkRequest(QUrl(URL)));
    QObject::connect(newreply, SIGNAL(finished()),this, SLOT(SlotTelechargementFini()));
    QObject::connect(this, SIGNAL(SignalAnnulerTelechargement()),newreply, SLOT(abort()));
    qDebug()<<"URL:"<<URL;
    return;
}

void AccesFichierParHTTP::PasserFichierSuivant(bool FichierTrouver)
{
    qDebug()<<"AccesFichierParHTTP::PasserFichierSuivant FichierTrouver : "<<FichierTrouver;
    if(TelechargementAnnuler)
    {
        return;
    }
    // Si il ne s'agit pas du premier fichier et qu'aucun fichier n'a été trouvé, on sort de la fonction
    if(NomFichier.indexOf("-")!=-1 && !FichierTrouver)
    {
        emit SignalTelechargementsFini();
        return;
    }
    NumeroFichier++;
    NomFichier=CodeJeu+"-"+QString::number(NumeroFichier);
    IndexExtension=0;
    SlotTelechargementFini();
}
