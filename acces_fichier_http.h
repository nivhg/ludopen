//acces_fichier_http.h	0.1	<Vincent VICTORIN>
#ifndef ACCESFICHIERPARHTTP_H
#define ACCESFICHIERPARHTTP_H

// En-têtes standards necessaires dans ce fichier en-tete seulement ------------
#include<QtCore>
#include<QtNetwork>
#include<QtWidgets>
#include<QFile>
#include<QDebug>

/**
 *  @brief AccesPhotosParHTTP permet la gestion des images stockées sur un serveur HTTP
 */
class AccesFichierParHTTP:public QNetworkAccessManager
{
  Q_OBJECT
public:
    /**
     *  @brief Vérifie l'existence d'une image sur un système de fichier ou en HTTP
     */
    bool FichierEtExtensionsExiste( QStringList *returnfile, QString URLorPath, QString file, QString* TypeImage, QStringList extlist);

    /**
     *  @brief Vérifie l'existence d'un fichier en fonction de l'extention sur un système de fichier ou en HTTP
     *          La fonction va rechercher si le fichier existe avec l'extention en minuscule et majuscule
     */
    bool FichierExiste( QStringList *returnfile, QString URLorPath, QString file, QString ext, QString* TypeImage);

};

#endif // ACCESFICHIERPARHTTP_H
