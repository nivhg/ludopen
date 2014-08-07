//---------------------------------------------------
/** @file lb_image.cpp
  * @brief Classe gérant un composant QLabel
  *
  * @author Vincent VICTORIN
  * @since 22/07/2014
  * @version 0.1
  * @date 22/07/2014
  *
  * Cette classe permet de gérer les évenements mousePress et resize pour un QLabel
  * avec affichage d'une image.
  *
  * Fabrication QtCreator
  *
*/

// En-têtes standards -----------------------------------------------------------
#include <QtSql>
#include <QDebug>
#include <QDir>
#include <QDesktopWidget>
#include <QResizeEvent>
#include <QSize>
// En-tête propre à  l'application ----------------------------------------------
#include "acces_fichier_http.h"
#include "lb_image.h"
#include "d_image.h"

/**
 *  @brief Constructeur de la classe.
 *
 *  @param parent
 *  @param fname : nom et chemin du fichier à afficher
 */
lb_image::lb_image( QWidget * parent ) :
    QLabel(parent)
{
    iNbImage=0;
    iImage=0;
    QSqlQuery RequeteCheminPhotosJeux;
    RequeteCheminPhotosJeux.exec("SELECT CheminPhotosJeux FROM preferences WHERE IdPreferences = 1");
    // si il y a des préférences
    if ( RequeteCheminPhotosJeux.next() )
    {
       sCheminImagePref = RequeteCheminPhotosJeux.value(0).toString();
    }
    else   // pas de photo à afficher
    {
       sCheminImagePref = "";
       setText("Aucune photo disponible pour ce jeu");
    }
}

/**
 *  @brief Gestion de l'évenement mousePress
 *
 *  @param event
 */
void lb_image::mousePressEvent ( QMouseEvent * event )
{
    emit clicked();
}

/**
 *  @brief Gestion de l'évenement resize
 *
 *  @param ev
 */
void lb_image::resizeEvent ( QResizeEvent * ev )
{
    AfficherImage(ev->size());
}

/** @brief Charge les images d'un jeu
 *
 *  @param size : nouvelle taille à donner à l'image
 *  @param code_jeu: code du jeu correspodant à l'image
 *
 */
QString lb_image::ChargerImage(QSize size,QString code_jeu)
{
    // Suppression des fichiers temporaires du précédent affichage des images HTTP
    if( sCheminImagePref.indexOf("http://",0,Qt::CaseInsensitive) != -1)
    {
        QDir CheminFichierImage;
        for(int i=0;i<iNbImage;i++)
        {
            CheminFichierImage.remove(sCheminImage[i]);
        }
    }
    iNbImage=0;
    setText("Aucune photo disponible pour ce jeu");

    QString TypeImage;

    AccesFichierParHTTP manager;

    int i=0;
    QString NomFichier;
    QStringList ListeExtension;
    ListeExtension<<"jpg"<<"jpeg"<<"png"<<"bmp"<<"gif"<<"xcf";
    if(manager.FichierEtExtensionsExiste( &sCheminImage[0], sCheminImagePref, code_jeu , &TypeImage,ListeExtension))
    {
        i++;
    }
    if(manager.FichierEtExtensionsExiste( &sCheminImage[1], sCheminImagePref, code_jeu + "-" + QString::number(i-1)
                                 , &TypeImage, ListeExtension ))
    {
        i++;
    }
    // Si XXXX.XXX ou XXXX-2.XXX existe, on cherche les autres images
    if(i>0)
    {
        NomFichier = code_jeu + "-" + QString::number(i+1);
        // Tant qu'il existe des images avec le même code jeu
        while( manager.FichierEtExtensionsExiste( &sCheminImage[i],sCheminImagePref,NomFichier,&TypeImage,ListeExtension))
        {
           i++;
           // Nom du prochain fichier à chercher
           NomFichier = code_jeu + "-" + QString::number(i+1);
        }
        iNbImage=i;
        AfficherImage(QSize(width(),height()));
        QDir* CheminFichier=new QDir(sCheminImage[iImage]);
        return CheminFichier->dirName();
    }
    else
    {
        return "";
    }
}

/**
 *  @brief Affiche et redimension la taille de l'image en fonction de size
 *
 *  @param size : nouvelle taille à donner à l'image
 */
void lb_image::AfficherImage(QSize size)
{
    QPixmap Image;
    // si le chemin est faux ou l'image n'existe pas, efface l'image d'avant automatiquement
    if ( Image.load(sCheminImage[iImage]) )
    {
         //Met l'image à l'échelle du cadre
        setPixmap( Image.scaled(size,Qt::KeepAspectRatio,Qt::SmoothTransformation) ) ;
        qDebug()<< "lb_image::DisplayImage() =>  sCheminImage=" <<  sCheminImage[iImage];
    }
    else   // pas de photo à afficher
    {
       setText("Aucune photo disponible pour ce jeu");
    }
}

QString lb_image::AfficherImagePrecedente()
{
    if(iNbImage==0)
    {
        return "";
    }
    // Si on n'est pas encore à 0, on décrémente le compteur sinon on repart à la fin des images
    if(iImage>0)
    {
        iImage--;
    }
    else
    {
        iImage=iNbImage-1;
    }
    AfficherImage(QSize(width(),height()));
    QDir* filepath=new QDir(sCheminImage[iImage]);
    return filepath->dirName();
}

QString lb_image::AfficherImageSuivante()
{
    if(iNbImage==0)
    {
        return "";
    }
    // Si on est à la fin, on repart à 0, sinon on incrémente le compteur
    if(iImage>iNbImage-2)
    {
        iImage=0;
    }
    else
    {
        iImage++;
    }
    AfficherImage(QSize(width(),height()));
    QDir* filepath=new QDir(sCheminImage[iImage]);
    return filepath->dirName();
}
