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
#include "fonctions_globale.h"

/**
 *  @brief Constructeur de la classe.
 *
 *  @param parent
 *  @param fname : nom et chemin du fichier à afficher
 */
Lb_Image::Lb_Image( QWidget * parent ) :
    QLabel(parent)
{
    ChargementEnCours=false;
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

Lb_Image::~Lb_Image()
{
    EffacerFichiers();
}

/**
 *  @brief Gestion de l'évenement mousePress
 *
 *  @param event
 */
void Lb_Image::mousePressEvent ( QMouseEvent * event )
{
    emit SignalClic();
}

/**
 *  @brief Gestion de l'évenement resize
 *
 *  @param ev
 */
void Lb_Image::resizeEvent ( QResizeEvent * ev )
{
    QSize size;
    if(this->frameShape()==QFrame::WinPanel)
    {
        size=QSize(ev->size().width()-4,ev->size().height()-4);
    }
    else
    {
        size=ev->size();
    }
    AfficherImage(size);
}

void Lb_Image::EffacerFichiers()
{
    // Suppression des fichiers temporaires du précédent affichage des images HTTP
    if( EstCeURL(sCheminImagePref) )
    {
        QDir CheminFichierImage;
        for(int i=0;i<sCheminImage.count();i++)
        {
            CheminFichierImage.remove(sCheminImage[i]);
        }
    }
}

/** @brief Charge les images d'un jeu
 *
 *  @param size : nouvelle taille à donner à l'image
 *  @param code_jeu: code du jeu correspodant à l'image
 *
 */
QString Lb_Image::ChargerImage(QSize size,QString code_jeu)
{
    if(ChargementEnCours)
    {
        if(sCheminImage.count()==0)
        {
            return false;
        }
        QDir* CheminFichier=new QDir(sCheminImage[0]);
        return CheminFichier->dirName();
    }
    ChargementEnCours=true;
    EffacerFichiers();
    iImage=0;
    sCheminImage.clear();

    setText("Aucune photo disponible pour ce jeu");

    QString TypeImage;

    AccesFichierParHTTP manager;

    QString NomFichier;
    QStringList ListeExtension;
    ListeExtension<<"jpg"<<"jpeg"<<"png"<<"bmp"<<"gif"<<"xcf";
    bool ret1=manager.FichierEtExtensionsExiste( &sCheminImage, sCheminImagePref,
                            code_jeu , &TypeImage,ListeExtension);
    bool ret2=manager.FichierEtExtensionsExiste( &sCheminImage, sCheminImagePref,
                            code_jeu + "-2", &TypeImage, ListeExtension);
    // Si il n'y a aucune image, on envoie le signal et on retourne false
    if(!ret1 and !ret2)
    {
        emit SignalChargementFini();
        ChargementEnCours=false;
        return false;
    }
    int i=3;
    NomFichier = code_jeu + "-" + QString::number(i);
    if(ret2)
    {
        // Tant qu'il existe des images avec le même code jeu
        while( manager.FichierEtExtensionsExiste( &sCheminImage,sCheminImagePref,NomFichier,&TypeImage,ListeExtension))
        {
           i++;
           // Nom du prochain fichier à chercher
           NomFichier = code_jeu + "-" + QString::number(i);
        }
    }
    AfficherImage(QSize(width(),height()));
    QDir* CheminFichier=new QDir(sCheminImage[0]);
    ChargementEnCours=false;
    emit SignalChargementFini();
    return CheminFichier->dirName();
}

/**
 *  @brief Affiche et redimension la taille de l'image en fonction de size
 *
 *  @param size : nouvelle taille à donner à l'image
 */
void Lb_Image::AfficherImage(QSize size)
{
    QPixmap Image;
    // Si il n'y pas d'image, on n'affiche rien
    if( sCheminImage.count() == 0)
    {
        return;
    }
    // Si iImage est plus grand que le tableau, on ne fait rien
    if(iImage >= sCheminImage.count())
    {
        return;
    }
    // si le chemin est faux ou l'image n'existe pas, efface l'image d'avant automatiquement
    if ( Image.load(sCheminImage[iImage]) )
    {
         //Met l'image à l'échelle du cadre
        setPixmap( Image.scaled(size,Qt::KeepAspectRatio,Qt::SmoothTransformation) ) ;
    }
    else   // pas de photo à afficher
    {
       setText("Aucune photo disponible pour ce jeu");
    }
}

/**
 *  @brief Affiche l'image précédente de la liste
 */
QString Lb_Image::AfficherImagePrecedente()
{
    if(sCheminImage.count()==0)
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
        iImage=sCheminImage.count()-1;
    }
    AfficherImage(QSize(width(),height()));
    QDir* filepath=new QDir(sCheminImage[iImage]);
    return filepath->dirName();
}

/**
 *  @brief Affiche l'image suivante
 */

QString Lb_Image::AfficherImageSuivante()
{
    if(sCheminImage.count()==0)
    {
        return "";
    }
    // Si on est à la fin, on repart à 0, sinon on incrémente le compteur
    if(iImage>sCheminImage.count()-2)
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

/**
 *  @brief Renvoie le tableau sCheminImage
 */
QStringList Lb_Image::ObtenirsCheminImage()
{
    return sCheminImage;
}

/**
 *  @brief Définie le tableau sCheminImage
 */
void Lb_Image::DefinirsCheminImage(QStringList sCheminImage)
{
    this->sCheminImage=sCheminImage;
}

/**
 *  @brief Définit iImage
 */
void Lb_Image::DefiniriImage(int iImage)
{
    this->iImage = iImage;
}

