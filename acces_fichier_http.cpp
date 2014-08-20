//acces_photo_http.cpp	0.1	<Vincent VICTORIN>

#include "acces_fichier_http.h"

// En-têtes standards necessaires dans ce fichier en-tete seulement ------------
#include<QtCore>
#include<QtNetwork>
#include<QtWidgets>
#include<QFile>
#include<QDebug>

#include"fonctions_globale.h"

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
 *
 */
    bool AccesFichierParHTTP::FichierEtExtensionsExiste( QStringList *returnfile, QString URLorPath, QString file, QString* TypeImage, QStringList extlist)
    {
        bool ret;
        foreach (QString ext, extlist)
        {
            if(FichierExiste( returnfile, URLorPath,file, ext, TypeImage ))
            {
                return true;
            }
        }
        return false;
    }

    /**
     *  @brief Vérifie l'existence d'un fichier en fonction de l'extention sur un système de fichier ou en HTTP
     *          La fonction va rechercher si le fichier existe avec l'extention en minuscule et majuscule
     *
     *  @param url :
     *          en entrée : chemin ou URL de l'image à rechercher
     *          en sortie : chemin du fichier trouvé. Dans le cas d'une image HTTP :
     *              chemin et nom du fichier temporaire où a été téléchargé l'image
     *  @param ext : Extention du fichier à rechercher.
     *  @param TypeImage : Type de fichier image trouvé (JPG, BMP...)
     */
    bool AccesFichierParHTTP::FichierExiste( QStringList *returnfile, QString URLorPath, QString file, QString ext, QString* TypeImage)
    {
        // On suppose que l'argument ext est toujours mis en minuscule,
        // et on rappele la fonction avec ext en majuscule
        if(ext!=ext.toUpper()) {
            if(FichierExiste(returnfile,URLorPath,file,ext.toUpper(),TypeImage))
            {
                return true;
            }
            ext=ext.toLower();
        }
        *TypeImage=ext.toUpper();
        // Si le chemin des images est une URL
        if( EstCeURL(URLorPath) )
        {
            QNetworkAccessManager m_NetworkMngr;
            QNetworkReply *reply= m_NetworkMngr.get(QNetworkRequest(QUrl(URLorPath + "/" + file +"." + ext)));
            QEventLoop loop;
            QObject::connect(reply, SIGNAL(finished()),&loop, SLOT(quit()));
            loop.exec();

            // Création du fichier temporaire
            QFile tempfile;
            tempfile.setFileName(QDir::fromNativeSeparators(QDir::tempPath()+"/"+file +"." + ext));
            tempfile.open(QIODevice::WriteOnly);
//            tempfile->setAutoRemove(false);
            tempfile.write(reply->readAll());
            tempfile.close();


            switch (reply->error())
              {
                case QNetworkReply::NoError:
                  // No error
                  delete reply;
                  returnfile->append(tempfile.fileName());
                  return true;
                case QNetworkReply::ContentNotFoundError:
                  // 404 Not found
                  /*int httpStatus = reply->attribute(
                    QNetworkRequest::HttpStatusCodeAttribute).toInt();
                  QByteArray httpStatusMessage = reply->attribute(
                    QNetworkRequest::HttpReasonPhraseAttribute).toByteArray();*/
                  delete reply;
                  tempfile.remove();
                  return false;
                  break;
                }
        }
        else
        {
            // Si le fichier existe, on rajoute l'extention à URL
            if(QFile::exists(URLorPath+"/"+file+"."+ext))
            {
                returnfile->append(URLorPath+"/"+file+"."+ext);
                return true;
            }
            else
            {
                return false;
            }
        }

    }
