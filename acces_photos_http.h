//acces_photo_http.h	0.1	<Vincent VICTORIN>
#ifndef ACCESPHOTOSPARHTTP_H
#define ACCESPHOTOSPARHTTP_H

// En-têtes standards necessaires dans ce fichier en-tete seulement ------------
#include<QtCore>
#include<QtNetwork>
#include<QtWidgets>
#include<QFile>
#include<QDebug>

/**
 *  @brief AccesPhotosParHTTP permet la gestion des images stockées sur un serveur HTTP
 */
class AccesPhotosParHTTP:public QNetworkAccessManager
{
  Q_OBJECT
public:
    /**
     *  @brief Vérifie l'existence d'une image sur un système de fichier ou en HTTP
     *
     *  @param url :
     *          en entrée : chemin ou URL de l'image à rechercher
     *          en sortie : chemin du fichier trouvé. Dans le cas d'une image HTTP :
     *              chemin et nom du fichier temporaire où a été téléchargé l'image
     *  @param TypeImage : Type de fichier image trouvé (JPG, BMP...)
     */
    bool ImageOrURLExists( QString *returnfile, QString URLorPath, QString file, QString* TypeImage);

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
    bool FileOrURLExists( QString *returnfile, QString URLorPath, QString file, QString ext, QString* TypeImage);

};

#endif // ACCESPHOTOSPARHTTP_H
