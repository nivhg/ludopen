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
#include "lb_image.h"
#include "d_image.h"
#include "fonctions_globale.h"
#include "f_preferences.h"

/**
 *  @brief Constructeur de la classe.
 *
 *  @param parent
 *  @param fname : nom et chemin du fichier à afficher
 */
Lb_Image::Lb_Image( QWidget * parent, QLabel * Lb_NomImage) :
    QLabel(parent)
{
    ChargementEnCours=false;
    this->Lb_NomImage=Lb_NomImage;
    iImage=0;
    sCheminImagePref = F_Preferences::ObtenirValeur("CheminPhotosJeux");
    if ( sCheminImagePref != "" )
    {
       if(!EstCeURL(sCheminImagePref))
       {
            sCheminImagePref="file://"+sCheminImagePref;
       }
    }
    else   // pas de photo à afficher
    {
       setText("Aucune photo disponible pour ce jeu");
    }
    acces=new AccesFichierParHTTP(this);
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
void Lb_Image::ChargerImage(QSize size,QString code_jeu,QCursor curseur)
{
    qDebug()<<"Lb_Image::ChargerImage ; code_jeu="<<code_jeu;
    EffacerFichiers();
    iImage=0;
    sCheminImage.clear();

    setText("Aucune photo disponible pour ce jeu");
    Lb_NomImage->setText("");

    SauvegardeCurseur = curseur;

    QStringList ListeExtension;
    ListeExtension<<"jpg"<<"jpeg"<<"png"<<"bmp"<<"gif"<<"xcf";
    delete acces;
    acces=new AccesFichierParHTTP(this);
    connect( acces, SIGNAL( SignalFichierTelecharger(QString) ), this, SLOT( SlotFichierTelecharger(QString) ) );
    connect( acces, SIGNAL( SignalTelechargementsFini() ), this, SLOT( SlotTelechargementsFini() ) );
    acces->LancerTelechargements(sCheminImagePref,code_jeu,ListeExtension);
    ChargementEnCours=true;
    this->setCursor(Qt::WaitCursor);
    return;
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
    // Si iImage est plus grand que le tableau, on ne fait rien
    if( sCheminImage.count() == 0 || iImage >= sCheminImage.count())
    {
        return;
    }
    // si le chemin est faux ou l'image n'existe pas, efface l'image d'avant automatiquement
    if ( Image.load(sCheminImage[iImage]) )
    {
         //Met l'image à l'échelle du cadre
        setPixmap( Image.scaled(size,Qt::KeepAspectRatio,Qt::SmoothTransformation) );
        QDir* filepath=new QDir(sCheminImage[iImage]);
        if(Lb_NomImage)
        {
            Lb_NomImage->setText(filepath->dirName());
        }
    }
    else   // pas de photo à afficher
    {
       setText("Aucune photo disponible pour ce jeu");
    }
}

/**
 *  @brief Affiche l'image précédente de la liste
 */
void Lb_Image::AfficherImagePrecedente()
{
    if(sCheminImage.count()==0)
    {
        return;
    }
    // Si on est à la fin, on fait attendre l'utilisateur en affichant un sablier
    if(ChargementEnCours && iImage==0)
    {
        iImage=sCheminImage.count();
        this->setCursor(Qt::WaitCursor);
        return;
    }
    this->setCursor(SauvegardeCurseur.shape());
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
}

/**
 *  @brief Affiche l'image suivante
 */

void Lb_Image::AfficherImageSuivante()
{
    if(sCheminImage.count()==0)
    {
        return;
    }
    // Si on est à la fin, on fait attendre l'utilisateur en affichant un sablier
    if(ChargementEnCours && iImage>sCheminImage.count()-2)
    {
        iImage=sCheminImage.count();
        this->setCursor(Qt::WaitCursor);
        return;
    }
    this->setCursor(SauvegardeCurseur.shape());
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

void Lb_Image::SlotFichierTelecharger(QString Fichier)
{
    this->setCursor(SauvegardeCurseur.shape());
    sCheminImage.append(Fichier);
    AfficherImage(QSize(width(),height()));
}

void Lb_Image::SlotTelechargementsFini()
{
    if(iImage==sCheminImage.count())
    {
        iImage=sCheminImage.count()-1;
    }
    ChargementEnCours=false;
    this->setCursor(SauvegardeCurseur.shape());
    emit SignalChargementFini();
}
