//---------------------------------------------------
/** @file d_image.cpp
  * @brief Classe QDialog affichant une image
  *
  * @author Vincent VICTORIN
  * @since 22/07/2014
  * @version 0.1
  * @date 22/07/2014
  *
  * Cette classe ajoute le composant Lb_Image qui affiche l'image filename
  *
  * Fabrication QtCreator
  *
*/

// En-têtes standards -----------------------------------------------------------
#include <QPixmap>
#include <QDesktopWidget>
#include <QRect>
#include <QDir>
#include <QFileDialog>
#include <QDateTime>
#include <QSqlQuery>
#include <QSettings>
#include <QCloseEvent>

// En-tête propre à  l'application ----------------------------------------------
#include "d_image.h"
#include "f_preferences.h"
#include "lb_image.h"
#include "ui_d_image.h"
#include "fonctions_globale.h"

/**
 *  @brief Constructeur de la classe.
 *
 *  @param parent
 *  @param lb_img : Lb_Image du parent
 */
D_Image::D_Image(QWidget *parent,Lb_Image* lb_img) :
    QDialog(parent),
    ui(new Ui::D_Image)
{
    ui->setupUi(this);

    lb_image=0;
    lb_image2=0;
    lb_image3=0;
    /*// Mets la fenêtre au centre de l'écran
    QDesktopWidget *desktop = QApplication::desktop();
    QRect geo_d_i = this->geometry();
    int x = (desktop->width() - geo_d_i.width()) / 2;
    int y = (desktop->height() - geo_d_i.height()) / 2;
    this->setGeometry(x, y, geo_d_i.width(), geo_d_i.height());*/
    ImagesBGG=(lb_img==0);
    if(this->parent()->objectName()=="F_AjoutSuppModifJeux" && !ImagesBGG)
    {
        this->lb_img=lb_img;
        iDecalage=0;
        iInversionDecalage=0;
        ui->SW_Boutons->setCurrentIndex(0);
        ui->Cb_FiltreLangue->setVisible(false);
        ui->Lb_Langue->setVisible(false);
    }
    else
    {
        this->lb_img=0;
        iDecalage=0;
        iInversionDecalage=0;
        ui->SW_Boutons->setCurrentIndex(1);
        ui->Lb_Defaut_Ludopen->setVisible(false);
        ui->Lb_Defaut_Web->setVisible(false);
        ui->Lb_Defaut_Deux->setVisible(false);
        if(ImagesBGG)
        {
            this->setGeometry(this->x(),this->y(),this->width(),500);
            ui->Cb_FiltreLangue->blockSignals(true);
            ui->Cb_FiltreLangue->addItem("Français",2187);
            ui->Cb_FiltreLangue->addItem("Anglais",2184);
            ui->Cb_FiltreLangue->addItem("Allemand",2188);
            ui->Cb_FiltreLangue->addItem("Toutes les langues",0);
            ui->Cb_FiltreLangue->blockSignals(false);
        }
        else
        {
            // Ajoute le label Lb_Image dans le dialog
            ui->gridLayout->addWidget(lb_img);
            // Connexion de l'évenement clic du Lb_Image
            connect( lb_img, SIGNAL( SignalClic() ), this, SLOT( on_Lb_Image_clicked() ) );
        }
    }

    if(this->parent()->objectName()=="F_AjoutSuppModifJeux")
    {
        // Récupère le chemin de la clé privée de la machine
        uploader = new SecureFileUploader(this);
        // Connexion de l'évenement de fin d'opération sur le serveur
        connect( uploader, SIGNAL( SignalOperationFini(bool) ), this, SLOT( slot_OperationFini(bool) ) ) ;
        // Connexion des évenements de fermeture de la QDialog
        connect(this, SIGNAL(accepted()), this, SLOT(onCloseDialog()));
        connect(this, SIGNAL(rejected()), this, SLOT(onCloseDialog()));
        connect(this, SIGNAL(dialogIsClosing()), this, SLOT(onCloseDialog()));
    }
}

/**
 *  @brief Destructeur de la classe
 */
D_Image::~D_Image()
{
    delete ui;
}

/**
 *  @brief Gestion du click de Lb_Image
 */
void D_Image::on_Lb_Image_clicked()
{
    if(this->parent()->objectName()=="F_AjoutSuppModifJeux")
    {
        // Sélectionne le Lb_Image cliqué
        ChangerSelection((QLabel *)sender());
    }
    else
    {
        close();
    }
}

/**
 *  @brief Affiche une bordure sur le label passé en paramètre
 *
 *  @param QLabel * : QLabel a sélectionné
 */
void D_Image::ChangerSelection(QLabel * pLabel)
{
    QStringList ListeLabel;
    // Trouve le numéro du Lb_Image sélectionné
    ListeLabel<<"lb_image1"<<"lb_image2"<<"lb_image3";
    // Si l'image sélectionnée n'a pas changée, on sort de la fonction
    if(iLbImageSelectionnee==ListeLabel.indexOf(pLabel->objectName()))
    {
        return;
    }
    iLbImageSelectionnee=ListeLabel.indexOf(pLabel->objectName());
    // Si l'image sélectionnée n'existe pas, on quitte la fonction
    if(iDecalage+iLbImageSelectionnee>=sCheminImage.count())
    {
        return;
    }
    // Déselectionne tous les Lb_Image
    if(lb_image->frameShape()!=QFrame::NoFrame)
    {
        lb_image->setFrameShape(QFrame::NoFrame);
    }
    if(lb_image2->frameShape()!=QFrame::NoFrame)
    {
        lb_image2->setFrameShape(QFrame::NoFrame);
    }
    if(lb_image3->frameShape()!=QFrame::NoFrame)
    {
        lb_image3->setFrameShape(QFrame::NoFrame);
    }
    // Selectionne le Lb_Image cliqué
    pLabel->setFrameShape(QFrame::WinPanel);
    // Active les boutons déplacer
    ui->Bt_DeplacerGauche->setEnabled(true);
    ui->Bt_DeplacerDroite->setEnabled(true);
    // Si on a sélectionné la 1° image,
    // on désactive le bouton déplacer gauche
    if(iLbImageSelectionnee+iDecalage<=0)
    {
        ui->Bt_DeplacerGauche->setEnabled(false);
    }
    // Si on a sélectionné la dernière image,
    // on désactive le bouton déplacer droite
    if(iLbImageSelectionnee+iDecalage+1>=sCheminImage.count())
    {
        ui->Bt_DeplacerDroite->setEnabled(false);
    }
}

/**
 *  @brief Initialise l'affichage des images
 *
 *  @param sCodeJeu : Numéro du code jeu à afficher
 *  @param sCheminImage : Liste des chemin d'image à afficher
 */
void D_Image::AfficherImage(QString sCodeJeu,QStringList sCheminImage)
{
    EffacerItemsLayout(ui->Gl_Images, 0, -1, true);
    if(!ImagesBGG)
    {
        // On affiche les boutons de fermeture du dockwidget
        ui->dockWidget->setVisible(true);
    }
    ui->dockWidget->setTitleBarWidget(new QWidget());
    this->sCodeJeu=sCodeJeu;
    this->sCheminImage=sCheminImage;
    // Si il n'y a aucune image à afficher, on désactive tous les boutons sauf le bouton Ajouter
    if(sCheminImage.count()==0)
    {
            ui->Bt_DeplacerDroite->setEnabled(false);
            ui->Bt_DeplacerGauche->setEnabled(false);
            ui->Bt_Supprimer->setEnabled(false);
            ui->Bt_Sauvegarder->setEnabled(false);
            ui->Bt_Defaut_Deux->setEnabled(false);
            ui->Bt_Defaut_Ludopen->setEnabled(false);
            ui->Bt_Defaut_Web->setEnabled(false);

    }
    // Il y a 3 images ou moins, on désactive le bouton Droite
    ui->Bt_Droite->setEnabled(sCheminImage.count()>3);
    // Liste des Lb_Image
    QList< Lb_Image **> ListLb_Image;
    ListLb_Image<<&lb_image<<&lb_image2<<&lb_image3;
    QList< QLabel *> ListLabel;
    ListLabel<<ui->Lb_NomFichier<<ui->Lb_NomFichier2<<ui->Lb_NomFichier3;
    // On initialise les 3 Lb_Image, on leur donne un nom et on les ajoute dans le layout
    for(int i=0;i<3;i++)
    {
        //ui->gridLayout_2->removeWidget(qobject_cast <QWidget *>(*ListLb_Image[i]));
        Lb_Image * lbi =new Lb_Image(this,ListLabel[i]);
        lbi->setObjectName("lb_image"+QString::number(i+1));
        lbi->setText("");
        ui->Gl_Images->addWidget(lbi,0,i);
        *ListLb_Image[i]=lbi;
        connect( lbi, SIGNAL( SignalClic() ), this, SLOT( on_Lb_Image_clicked() ) );
    }
    // Affiche les Label des images par défaut
    MontrerLudopenWebDeux(true);
    // Affiche les images du jeux
    RechargerImages();
    // Sélectionne la 1° image et force l'actualisation de la sélection
    iLbImageSelectionnee=1;
    ChangerSelection(lb_image);
}

// Removes all layout items which span the given row and column.
void D_Image::EffacerItemsLayout(QGridLayout *layout, int row, int column, bool deleteWidgets) {
  // We avoid usage of QGridLayout::itemAtPosition() here to improve performance.
  for (int i = layout->count() - 1; i >= 0; i--) {
    int r, c, rs, cs;
    layout->getItemPosition(i, &r, &c, &rs, &cs);
    if (
        (row == -1 || (r <= row && r + rs > row)) &&
        (column == -1 || (c <= column && c + cs > column))) {
      // This layout item is subject to deletion.
      QLayoutItem *item = layout->takeAt(i);
      if (deleteWidgets) {
        EffacerWidgetsEnfants(item);
      }
      delete item;
    }
  }
}

// Deletes all child widgets of the given layout item.
void D_Image::EffacerWidgetsEnfants(QLayoutItem *item) {
  QLayout *layout = item->layout();
  if (layout) {
    // Process all child items recursively.
    int itemCount = layout->count();
    for (int i = 0; i < itemCount; i++) {
      EffacerWidgetsEnfants(layout->itemAt(i));
    }
  }
  delete item->widget();
}

/**
 *  @brief Recharge les images sur le disque et les affiche
 */
void D_Image::RechargerImages()
{
    //EffacerItemsLayout(ui->gridLayout_2, 2, -1, true);
    QPixmap Image;
    // Liste des Lb_Image
    QList< Lb_Image *> ListLb_Image;
    ListLb_Image<<lb_image<<lb_image2<<lb_image3;
    // Liste des Label contenant le nom des fichiers
    QList< QLabel *> ListLabel;
    ListLabel<<ui->Lb_NomFichier<<ui->Lb_NomFichier2<<ui->Lb_NomFichier3;
    // Parcours les 2 listes affichent les images et le nom des fichiers
    for(int i=0+iDecalage;i<3+iDecalage;i++)
    {
        Lb_Image * lbi = ListLb_Image[i-iDecalage];
        lbi->DefinirsCheminImage(sCheminImage);
        lbi->DefiniriImage(i);
        QLabel * lbl = ListLabel[i-iDecalage];
        if(i<sCheminImage.count())
        {
            if ( QFile::exists(sCheminImage[i]) )
            {
                if(!ImagesBGG)
                {
                    lbi->AfficherImage(QSize(800,800));
                }
                else
                {
                    lbi->AfficherImage(QSize(200,200),true,sNomVersion[i]);
                }
            }
            else if(ImagesBGG)
            {
                lbi->AfficherImage(QSize(200,200),true,sNomVersion[i]);
            }
        }
        else
        {
            // Supprime les images des Lb_Image qui n'en ont pas
            lbi->DefiniriImage(i);
            lbi->setPixmap(QPixmap(0,0));
            lbi->setText("");
            lbl->setText("");
        }
    }
}

/**
 *  @brief Gestion du clic du bouton "Gauche"
 */
void D_Image::on_Bt_Gauche_clicked()
{
    qDebug()<<"on_Bt_Gauche_clicked()";
    // On décrémente l'indicateur de décalage de 3
    iDecalage-=3;
    ui->Bt_Droite->setEnabled(true);
    RechargerImages();
    // Si on arrive à 0, on désactive le bouton Gauche et on affiche les Label des images par défaut
    if(iDecalage==0)
    {
        ui->Bt_Gauche->setEnabled(false);
        MontrerLudopenWebDeux(true);
    }
    //On sélectionne la 1° image
    ChangerSelection(lb_image);
}

/**
 *  @brief Montre ou cache les labels indiquants les images par défaut
 *          sur Ludopen, le site Web ou les 2
 *
 *  @param parent
 */
void D_Image::MontrerLudopenWebDeux(bool Montrer)
{
    if(ImagesBGG) return;
    if(Montrer && sCheminImage.count()!=0 && iDecalage == 0)
    {
        if(EstCeNomFichierContient(sCheminImage[0],"-"))
        {
            ui->Lb_Defaut_Ludopen->setVisible(false);
            ui->Lb_Defaut_Web->setVisible(false);
            ui->Lb_Defaut_Deux->setVisible(true);
        }
        else
        {
            ui->Lb_Defaut_Ludopen->setVisible(true);
            ui->Lb_Defaut_Web->setVisible(true);
            ui->Lb_Defaut_Deux->setVisible(false);
        }
    }
    else
    {
        ui->Lb_Defaut_Ludopen->setVisible(false);
        ui->Lb_Defaut_Web->setVisible(false);
        ui->Lb_Defaut_Deux->setVisible(false);
    }
}

/**
 *  @brief Gestion du clic du bouton "Droite"
 */
void D_Image::on_Bt_Droite_clicked()
{
    qDebug()<<"on_Bt_Droite_clicked()";
    // On incrémente l'indicateur de décalage de 3
    iDecalage+=3;
    ui->Bt_Gauche->setEnabled(true);
    RechargerImages();
    // Si on arrive à la fin des images, on désactive le bouton droite
    if(iDecalage+3>=sCheminImage.count())
    {
        ui->Bt_Droite->setEnabled(false);
    }
    ChangerSelection(lb_image);
    MontrerLudopenWebDeux(false);
}

/**
 *  @brief Gestion du clic du bouton "Déplacer à gauche"
 */
void D_Image::on_Bt_DeplacerGauche_clicked()
{
    qDebug()<<"on_Bt_DeplacerGauche_clicked()";
    // On indique l'opération qui va être effectuer sur le serveur
    iOperationServeur=OP_DEPLACEMENT_GAUCHE;
    // On inverse l'image de gauche par rapport à l'image sélectionnée
    iInversionDecalage=-1;
    // Si l'opération est à faire localement, on exécute slot_OperationFini pour faire les opérations
    // à faire sur l'interface
    if(!EstCeURL(F_Preferences::ObtenirValeur("CheminPhotosJeux")))
    {
        Inverser(EMPLACEMENT_LOCAL);
        slot_OperationFini(true);
    }
    else
    {
        ui->dockWidget->setCursor(Qt::WaitCursor);
        uploader->EffacerCommandes();
        uploader->init(F_Preferences::ObtenirValeur("AdresseServeur"),
                       F_Preferences::ObtenirValeur("LoginServeur"),
                       F_Preferences::ObtenirValeur("CheminClePrivee"));
        Inverser(EMPLACEMENT_SERVEUR);
        uploader->FaireCommandes();
    }
}

/**
 *  @brief Gestion du clic du bouton "Déplacer à droite"
 */
void D_Image::on_Bt_DeplacerDroite_clicked()
{
    qDebug()<<"on_Bt_DeplacerDroite_clicked()";
    iOperationServeur=OP_DEPLACEMENT_DROITE;
    iInversionDecalage=1;
    // Si l'opération est à faire localement, on exécute slot_OperationFini pour faire les opérations
    // à faire sur l'interface
    if(!EstCeURL(F_Preferences::ObtenirValeur("CheminPhotosJeux")))
    {
        Inverser(EMPLACEMENT_LOCAL);
        slot_OperationFini(true);
    }
    else
    {
        ui->dockWidget->setCursor(Qt::WaitCursor);
        uploader->EffacerCommandes();
        uploader->init(F_Preferences::ObtenirValeur("AdresseServeur"),
                       F_Preferences::ObtenirValeur("LoginServeur"),
                       F_Preferences::ObtenirValeur("sCheminClePrivee"));
        Inverser(EMPLACEMENT_SERVEUR);
        uploader->FaireCommandes();
    }
}

/**
 *  @brief Inverse l'image sélectionné avec l'image indiquée
 *          par iInversionDecalage (index par rapport à l'image sélectionnée)
 *
 *  @param emplacement : Emplacement de l'opération (Serveur, Local ou espace temporaire)
 */
void D_Image::Inverser(int emplacement)
{
    qDebug()<<"Inverser:ImageSelectionnée:"<<iLbImageSelectionnee<<
              ",iInversionDecalage:"<<iInversionDecalage<<",sCheminImage"<<sCheminImage;
    if( iInversionDecalage == 0 )
    {
        // Il n'y a rien à faire, on quitte la fonction
        return;
    }
    QString NomFichierTemporaire = QString::number(QDateTime::currentMSecsSinceEpoch())+".tmp";
    // Info de l'image sélectionnée
    QFileInfo InfoFichierSource(sCheminImage[iLbImageSelectionnee+iDecalage]);
    // Info de l'image qui se trouve à iInversionDecalage de l'image sélectionnée
    QFileInfo InfoFichierDestination(sCheminImage[iLbImageSelectionnee+iInversionDecalage+iDecalage]);
    QString NomFichierSource(InfoFichierSource.fileName()),
            NomFichierDestination(InfoFichierDestination.fileName()),
            // Il faut aussi inverser les extensions des fichiers qui peuvent ne peut être les mêmes
            NouveauNomFichierSource(InfoFichierSource.baseName()+"."+
                                    InfoFichierDestination.completeSuffix()),
            NouveauNomFichierDestination(InfoFichierDestination.baseName()+"."+
                                         InfoFichierSource.completeSuffix());
    if(emplacement==EMPLACEMENT_SERVEUR)
    {
        uploader->AjouterCommande(COMMANDE_RENOMMER,F_Preferences::ObtenirValeur("CheminPhotosServeur") +
           NomFichierDestination, F_Preferences::ObtenirValeur("CheminPhotosServeur") + NomFichierTemporaire);
        uploader->AjouterCommande(COMMANDE_RENOMMER,F_Preferences::ObtenirValeur("CheminPhotosServeur") +
           NomFichierSource, F_Preferences::ObtenirValeur("CheminPhotosServeur") + NouveauNomFichierDestination);
        uploader->AjouterCommande(COMMANDE_RENOMMER,F_Preferences::ObtenirValeur("CheminPhotosServeur") +
           NomFichierTemporaire, F_Preferences::ObtenirValeur("CheminPhotosServeur") + NouveauNomFichierSource);
    }
    else
    {
        QString CheminFichierSource,CheminFichierDestination,CheminFichierTemporaire,CheminNouveauFichierSource,
                CheminNouveauFichierDestination;
        if(emplacement==EMPLACEMENT_TEMP)
        {
            CheminFichierSource=QDir::tempPath()+"/"+NomFichierSource;
            CheminFichierDestination=QDir::tempPath()+"/"+NomFichierDestination;
            CheminFichierTemporaire=QDir::tempPath()+"/"+NomFichierTemporaire;
            CheminNouveauFichierSource=QDir::tempPath()+"/"+NouveauNomFichierSource;
            CheminNouveauFichierDestination=QDir::tempPath()+"/"+NouveauNomFichierDestination;
        }
        else
        {
            CheminFichierSource=F_Preferences::ObtenirValeur("CheminPhotosJeux")+"/"+NomFichierSource;
            CheminFichierDestination=F_Preferences::ObtenirValeur("CheminPhotosJeux")+"/"+NomFichierDestination;
            CheminFichierTemporaire=F_Preferences::ObtenirValeur("CheminPhotosJeux")+"/"+NomFichierTemporaire;
            CheminNouveauFichierSource=F_Preferences::ObtenirValeur("CheminPhotosJeux")+"/"+NouveauNomFichierSource;
            CheminNouveauFichierDestination=F_Preferences::ObtenirValeur("CheminPhotosJeux")+"/"+NouveauNomFichierDestination;
        }
        QFile FichierSource(CheminFichierSource),
              FichierDestination(CheminFichierDestination);

        FichierDestination.rename(CheminFichierTemporaire);
        QFile FichierTemporaire(CheminFichierTemporaire);
        FichierSource.rename(CheminNouveauFichierDestination);
        FichierTemporaire.rename(CheminNouveauFichierSource);
        // On modifie sCheminImage en conséquence
        sCheminImage[iLbImageSelectionnee+iDecalage]=CheminNouveauFichierSource;
        sCheminImage[iLbImageSelectionnee+iInversionDecalage+iDecalage]=CheminNouveauFichierDestination;
    }
    qDebug()<<"Fin Inverser:ImageSelectionnée:"<<iLbImageSelectionnee<<
              ",iInversionDecalage:"<<iInversionDecalage<<",sCheminImage"<<sCheminImage;
}

/**
 *  @brief Gestion du clic du bouton Sauvegarder
 */
void D_Image::on_Bt_Sauvegarder_clicked()
{
    qDebug()<<"on_Bt_Sauvegarder_clicked()";
    QFileInfo NomFichier(sCheminImage[iLbImageSelectionnee+iDecalage]);
    // Affiche la boite de dialogue de sauvegarde de fichier
    QString sNomImage = QFileDialog::getSaveFileName(this,tr( "Sauvegarder l'image." ),
             NomFichier.fileName(),"Fichier image (*.png *.jpg *.bmp *.jpeg *.gif *.tif)" );
    // Si l'utilisateur a cliqué sur Annuler on quitte la fonction
    if(sNomImage==NULL)
    {
        return;
    }
    // On copie le fichier sélectionné vers l'emplacement sélectionné
    QFile Fichier(sCheminImage[iLbImageSelectionnee+iDecalage]);
    Fichier.copy(sNomImage);
}

/**
 *  @brief Gestion du bouton "Ajouter"
 */
void D_Image::on_Bt_Ajouter_clicked()
{
    qDebug()<<"on_Bt_Ajouter_clicked()";
    iOperationServeur=OP_AJOUT;
    QString sCheminImage ;
    // Affiche la boite de dialogue de sélection de plusieurs fichiers
    sNomImagesAjoutees = QFileDialog::getOpenFileNames( this,tr( "Ouvrir une image." ), "", "Fichier image (*.png *.jpg *.bmp *.jpeg *.gif *.tif *.PNG *.JPG *.BMP *.JPEG *.GIF *.TIF)" ) ;
    // Si aucun fichier n'a été sélectionné on quitte la fonction
    if(sNomImagesAjoutees.count()==0)
    {
        return;
    }
    AjoutImageLocalOuServeur();
}

void D_Image::AjoutImageLocalOuServeur()
{
    if(!EstCeURL(F_Preferences::ObtenirValeur("CheminPhotosJeux")))
    {
        Ajouter(EMPLACEMENT_LOCAL);
        slot_OperationFini(true);
    }
    else
    {
        ui->dockWidget->setCursor(Qt::WaitCursor);
        uploader->EffacerCommandes();
        uploader->init(F_Preferences::ObtenirValeur("AdresseServeur"),
                       F_Preferences::ObtenirValeur("sLoginServeur"),
                       F_Preferences::ObtenirValeur("CheminClePrivee"));
        Ajouter(EMPLACEMENT_SERVEUR);
        uploader->FaireCommandes();
    }
}
/**
 *  @brief Ajouter une image de jeu
 *
 *  @param emplacement : Emplacement de l'opération (Serveur, Local ou espace temporaire)
 */
void D_Image::Ajouter(int emplacement)
{
    QFileInfo fileInfo;
    QString DestNomFichier;
    int i;
    if(ImagesBGG)
    {
        i=1;
    }
    else
    {
        i=this->sCheminImage.count()+1;
        // Si il y a dejà des images et que la 1° image est une image "-2", on incrémente i de 1
        if(i>1 && EstCeNomFichierContient(sCheminImage[0],"-"))
        {
            i++;
        }
    }
    // Parcours les images choisies pour être ajoutées
    foreach(QString sNomImageAjoutee,sNomImagesAjoutees)
    {
        fileInfo.setFile(sNomImageAjoutee);
        // Si c'est la première image, on la nomme sans "-"
        if(i==1)
        {
            DestNomFichier=sCodeJeu+"."+fileInfo.completeSuffix();
        }
        else
        {
            DestNomFichier=sCodeJeu+"-"+QString::number(i)+"."+
                fileInfo.completeSuffix();
        }
        i++;
        if(emplacement==EMPLACEMENT_SERVEUR)
        {
            uploader->AjouterCommande(COMMANDE_TELEVERSEMENT,sNomImageAjoutee,
                                      F_Preferences::ObtenirValeur("CheminPhotosServeur")+"/"+DestNomFichier);
        }
        else
        {
            QString DestCheminFichier;
            if( emplacement==EMPLACEMENT_TEMP)
            {
                // Copie le fichier dans le dossier temporaire pour affichage de l'image
                DestCheminFichier=QDir::tempPath()+"/"+DestNomFichier;
            }
            else
            {
                // Copie le fichier dans le dossier temporaire pour affichage de l'image
                DestCheminFichier=F_Preferences::ObtenirValeur("CheminPhotosJeux")+"/"+DestNomFichier;
            }
            QFile FichierSource;
            FichierSource.setFileName(sNomImageAjoutee);
            QFile FichierDestination;
            FichierDestination.setFileName(DestCheminFichier);
            FichierDestination.remove();
            FichierSource.copy(DestCheminFichier);
            // On ajoute l'image dans sCheminImage
            sCheminImage << DestCheminFichier;
            // Si le nombre d'image ajouté sort du cadre (on affiche que 3 images à la fois),
            // On active le bouton Droite
            if(sCheminImage.count()-iDecalage>3)
            {
                ui->Bt_Droite->setEnabled(true);
            }
        }
    }
}

/**
 *  @brief Gestion du clic du bouton Supprimer
 */
void D_Image::on_Bt_Supprimer_clicked()
{
    qDebug()<<"on_Bt_Supprimer_clicked()";
    iOperationServeur=OP_SUPPRESSION;
    if(!EstCeURL(F_Preferences::ObtenirValeur("CheminPhotosJeux")))
    {
        Suppression(EMPLACEMENT_LOCAL);
        slot_OperationFini(true);
    }
    else
    {
        ui->dockWidget->setCursor(Qt::WaitCursor);
        uploader->EffacerCommandes();
        uploader->init(F_Preferences::ObtenirValeur("AdresseServeur"),
                       F_Preferences::ObtenirValeur("LoginServeur")
                       , F_Preferences::ObtenirValeur("CheminClePrivee"));
        Suppression(EMPLACEMENT_SERVEUR);
        uploader->FaireCommandes();
    }
}

/**
 *  @brief Suppression de l'image sélectionné et décalage des images suivantes
 *
 *  @param emplacement : Emplacement de l'opération (Serveur, Local ou espace temporaire)
 */
void D_Image::Suppression(int emplacement)
{
    QString NomFichierSource(sCheminImage[iLbImageSelectionnee+iDecalage]);
    QFileInfo InfoFichierSource,InfoFichierDestination;
    InfoFichierSource.setFile(NomFichierSource);
    if(emplacement==EMPLACEMENT_SERVEUR)
    {
        uploader->AjouterCommande(COMMANDE_SUPPRIMER,
            F_Preferences::ObtenirValeur("CheminPhotosServeur")+"/"+InfoFichierSource.fileName(),"");
    }
    else
    {
        if(emplacement==EMPLACEMENT_LOCAL)
        {
            NomFichierSource=F_Preferences::ObtenirValeur("CheminPhotosJeux")+"/"+InfoFichierSource.fileName();
        }
        QFile FichierSource(NomFichierSource);
        FichierSource.remove();
    }
    DecalerAGauche(emplacement);
}

/**
 *  @brief Décalager à gauche par rapport à l'image sélectionnée
 *
 *  @param emplacement : Emplacement de l'opération (Serveur, Local ou espace temporaire)
 */
void D_Image::DecalerAGauche(int emplacement)
{
    qDebug()<<"DecalerAGauche:ImageSelectionnée:"<<iLbImageSelectionnee;
    QFileInfo InfoFichierSource,InfoFichierDestination;
    QFile FichierSource;
    // On commence le décalage à droite de l'image sélectionnée
    for(int i=iLbImageSelectionnee+1+iDecalage;i<sCheminImage.count();i++)
    {
        InfoFichierSource.setFile(sCheminImage[i]);
        InfoFichierDestination.setFile(sCheminImage[i-1]);
        QString NomFichierSource(InfoFichierSource.fileName()),
                NouveauNomFichierDestination(InfoFichierDestination.baseName()+"."+
                                             InfoFichierSource.completeSuffix());
        // Renommage des images
        if(emplacement==EMPLACEMENT_SERVEUR)
        {
            uploader->AjouterCommande(COMMANDE_RENOMMER,
                F_Preferences::ObtenirValeur("CheminPhotosServeur")+"/"+NomFichierSource,
                F_Preferences::ObtenirValeur("CheminPhotosServeur")+"/"+NouveauNomFichierDestination);
        }
        else
        {
            QString CheminFichierSource,CheminFichierDestination;
            if(emplacement==EMPLACEMENT_TEMP)
            {
                CheminFichierSource=QDir::tempPath()+"/"+NomFichierSource;
                CheminFichierDestination=QDir::tempPath()+"/"+NouveauNomFichierDestination;
            }
            else
            {
                CheminFichierSource=F_Preferences::ObtenirValeur("CheminPhotosJeux")+"/"+NomFichierSource;
                CheminFichierDestination=F_Preferences::ObtenirValeur("CheminPhotosJeux")+"/"+NouveauNomFichierDestination;
            }
            FichierSource.setFileName(CheminFichierSource);
            FichierSource.rename(CheminFichierDestination);
            qDebug()<<"Insert avant:"<<sCheminImage;
            sCheminImage[i-1]=CheminFichierDestination;
            qDebug()<<"Insert après:"<<sCheminImage;
        }
    }
    if(emplacement!=EMPLACEMENT_SERVEUR)
    {
        // On supprime la dernière case du tableau sCheminImage
        sCheminImage.removeLast();
        // Si il n'y a plus d'image à droite, on désactive le bouton de droite
        if(iDecalage+3>=sCheminImage.count())
        {
            // Si il n'y a plus d'image dans la zone sélectionnée,
            // on se décale à gauche sauf si on est déjà sur
            // les premieres images
            if(iDecalage>=sCheminImage.count() && iDecalage > 0)
            {
                on_Bt_Gauche_clicked();
            }
            ui->Bt_Droite->setEnabled(false);
        }
    }
}

/**
 *  @brief Gestion du clic du bouton d'une image par défaut sur Ludopen
 */
void D_Image::on_Bt_Defaut_Ludopen_clicked()
{
    qDebug()<<"on_Bt_Defaut_Ludopen_clicked():sCheminImage:"<<sCheminImage;
    iOperationServeur=OP_DEFINIR_LUDOPEN;
    if(!EstCeURL(F_Preferences::ObtenirValeur("CheminPhotosJeux")))
    {
        DefinirLudopen(EMPLACEMENT_LOCAL);
        slot_OperationFini(true);
    }
    else
    {
        ui->dockWidget->setCursor(Qt::WaitCursor);
        uploader->EffacerCommandes();
        uploader->init(F_Preferences::ObtenirValeur("AdresseServeur"),
                       F_Preferences::ObtenirValeur("LoginServeur"),
                       F_Preferences::ObtenirValeur("CheminClePrivee"));
        DefinirLudopen(EMPLACEMENT_SERVEUR);
        uploader->FaireCommandes();
    }
}

/**
 *  @brief Mets l'image sélectionnée par défaut sur Ludopen
 *
 *  @param emplacement : Emplacement de l'opération (Serveur, Local ou espace temporaire)
 */
void D_Image::DefinirLudopen(int emplacement)
{
    // Si il n'y a pas de "-" dans la 1° case du tableau
    if(!EstCeNomFichierContient(sCheminImage[0],"-"))
    {
        // On inverse la 1° image avec l'image sélectionnée
        iInversionDecalage=-iLbImageSelectionnee-iDecalage;
        Inverser(emplacement);
    }
    else
    {
        QString NomFichierSource(sCheminImage[iLbImageSelectionnee+iDecalage]);
        QFileInfo InfoFichierSource(NomFichierSource);
        QFile FichierSource;
        // On renomme l'image sélectionnée en sCodeJeu suivit de son extension
        if(emplacement==EMPLACEMENT_SERVEUR)
        {
            uploader->AjouterCommande(COMMANDE_RENOMMER,
                F_Preferences::ObtenirValeur("CheminPhotosServeur") + InfoFichierSource.fileName(),
                F_Preferences::ObtenirValeur("CheminPhotosServeur") + sCodeJeu+"."+
                                      InfoFichierSource.completeSuffix());
        }
        else
        {
            QString NomFichierDestination;
            if(emplacement==EMPLACEMENT_TEMP)
            {
                NomFichierDestination=
                        QDir::tempPath()+"/"+sCodeJeu+"."+InfoFichierSource.completeSuffix();
            }
            else
            {
                NomFichierDestination=
                        F_Preferences::ObtenirValeur("CheminPhotosJeux")+"/"+sCodeJeu+"."+
                        InfoFichierSource.completeSuffix();
            }
            FichierSource.setFileName(NomFichierSource);
            FichierSource.rename(NomFichierDestination);
            // On supprime l'image sélectionnée du tableau
//            sCheminImage.removeAt(iLbImageSelectionnee+iDecalage);
            // Et on l'ajoute au début
            sCheminImage.insert(0,NomFichierDestination);
            iLbImageSelectionnee++;
        }
        // On décale toutes les autres images à gauche
        DecalerAGauche(emplacement);
        MontrerLudopenWebDeux(true);
    }
}

/**
 *  @brief Gestion du clic du bouton pour mettre l'image sélectionnée
 *          par défaut sur le site Web
 */
void D_Image::on_Bt_Defaut_Web_clicked()
{
    qDebug()<<"on_Bt_Defaut_Web_clicked():sCheminImage:"<<sCheminImage;
    iOperationServeur=OP_DEFINIR_WEB;
    if(!EstCeURL(F_Preferences::ObtenirValeur("CheminPhotosJeux")))
    {
        DefinirWeb(EMPLACEMENT_LOCAL);
        slot_OperationFini(true);
    }
    else
    {
        ui->dockWidget->setCursor(Qt::WaitCursor);
        uploader->EffacerCommandes();
        uploader->init(F_Preferences::ObtenirValeur("AdresseServeur"),
                       F_Preferences::ObtenirValeur("LoginServeur"),
                       F_Preferences::ObtenirValeur("CheminClePrivee"));
        DefinirWeb(EMPLACEMENT_SERVEUR);
        uploader->FaireCommandes();
    }
}

/**
 *  @brief Mets par défaut l'image sélectionnée pour le site Web
 *
 *  @param emplacement : Emplacement de l'opération (Serveur, Local ou espace temporaire)
 */
void D_Image::DefinirWeb(int emplacement)
{
    // On inverse l'image sélectionnée avec la première image
    iInversionDecalage=-iLbImageSelectionnee-iDecalage;
    // Si la première image est une fichier sans "-",
    // on inverse l'image en 2° position dans le tableau (image avec "-2")
    if(!EstCeNomFichierContient(sCheminImage[0],"-"))
    {
        iInversionDecalage++;
    }
    if(iInversionDecalage!=0)
    {
        Inverser(emplacement);
    }
}

/**
 *  @brief Gestion du clic du bouton pour mettre l'image sélectionnée
 *      par défaut sur ludopen et le site Web
 */
void D_Image::on_Bt_Defaut_Deux_clicked()
{
    qDebug()<<"on_Bt_Defaut_Deux_clicked():sCheminImage"<<sCheminImage<<
         ",m_ListeCommandes"<<uploader->m_ListeCommandes<<",iLbImageSelectionnee:"<<iLbImageSelectionnee;
    iOperationServeur=OP_DEFINIR_DEUX;
    if(!EstCeURL(F_Preferences::ObtenirValeur("CheminPhotosJeux")))
    {
        DefinirDeux(EMPLACEMENT_LOCAL);
        slot_OperationFini(true);
    }
    else
    {
        ui->dockWidget->setCursor(Qt::WaitCursor);
        uploader->EffacerCommandes();
        uploader->init(F_Preferences::ObtenirValeur("AdresseServeur"),
                       F_Preferences::ObtenirValeur("LoginServeur"),
                       F_Preferences::ObtenirValeur("CheminClePrivee"));
        DefinirDeux(EMPLACEMENT_SERVEUR);
        uploader->FaireCommandes();
    }
}

/**
 *  @brief Mets l'image sélectionnée par défaut sur Ludopen et le site Web
 *
 *  @param emplacement : Emplacement de l'opération (Serveur, Local ou espace temporaire)
 */
void D_Image::DefinirDeux(int emplacement)
{
    int iLbImageSelectionneeSauvegardee;
    // Si le 1° fichier n'a pas de "-"
    if(!EstCeNomFichierContient(sCheminImage[0],"-"))
    {
        QString CheminFichierSource;
        CheminFichierSource=sCheminImage[0];
        QFileInfo InfoFichierSource(CheminFichierSource);
        CheminFichierSource=F_Preferences::ObtenirValeur("CheminPhotosJeux")+"/"+InfoFichierSource.fileName();
        QFile FichierSource;
        QString NouveauNomFichierDestination(sCodeJeu+"-"+QString::number(sCheminImage.count()+1)+
                                             "."+InfoFichierSource.completeSuffix());
        // On déplace le 1° fichier en fin de liste (nouvel élément)
        if(emplacement==EMPLACEMENT_SERVEUR)
        {
            // Si on a sélectionné le 1° fichier, on déplace le 2° à la fin
            // et on renomme le 1° fichier en 2° fichier
            if(iLbImageSelectionnee-iDecalage==0 && sCheminImage.count() > 1 )
            {
                QString Chemin2emeFichier=sCheminImage[1];
                QFileInfo Info2emeFichier(Chemin2emeFichier);
                QString NouveauNom2emeFichier(sCodeJeu+"-"+QString::number(sCheminImage.count()+1)+
                                                     "."+Info2emeFichier.completeSuffix());                uploader->AjouterCommande(COMMANDE_RENOMMER,
                    F_Preferences::ObtenirValeur("CheminPhotosServeur") + Info2emeFichier.fileName(),
                    F_Preferences::ObtenirValeur("CheminPhotosServeur") + NouveauNom2emeFichier);
                QString NomFichierDestination(sCodeJeu+"-2."+InfoFichierSource.completeSuffix());
                uploader->AjouterCommande(COMMANDE_RENOMMER,
                    F_Preferences::ObtenirValeur("CheminPhotosServeur") + InfoFichierSource.fileName(),
                    F_Preferences::ObtenirValeur("CheminPhotosServeur") + NomFichierDestination);
            }
            else
            {
                uploader->AjouterCommande(COMMANDE_RENOMMER,
                    F_Preferences::ObtenirValeur("CheminPhotosServeur") + InfoFichierSource.fileName(),
                    F_Preferences::ObtenirValeur("CheminPhotosServeur") + NouveauNomFichierDestination);
                iInversionDecalage=-iLbImageSelectionnee-iDecalage+1;
            }
        }
        else
        {
            QString CheminNouvelleDestination;
            if(emplacement==EMPLACEMENT_TEMP)
            {
                CheminNouvelleDestination=QDir::tempPath()+"/"+NouveauNomFichierDestination;
            }
            else
            {
                CheminNouvelleDestination=F_Preferences::ObtenirValeur("CheminPhotosJeux")+"/"+
                        NouveauNomFichierDestination;
            }
            FichierSource.setFileName(CheminFichierSource);
            FichierSource.rename(CheminNouvelleDestination);

            // Le 1° fichier est mis à la fin et on le supprime du tableau
            sCheminImage<<CheminNouvelleDestination;
            sCheminImage.removeAt(0);
            // Si on a sélectionné le 1° fichier, on va inverser ce fichier
            // (qui se trouve désormais en fin de liste) avec le fichier -2
            if(iLbImageSelectionnee-iDecalage==0)
            {
                iLbImageSelectionneeSauvegardee=iLbImageSelectionnee;
                iLbImageSelectionnee=sCheminImage.count()-1;
            }
            else
            // Sinon, on inverse l'image sélectionnée
            // (qui se trouve décalée de 1, vu que la 1° case du tableau a été supprimée) avec le fichier -2
            {
                iLbImageSelectionnee--;
            }
            // On inverse la première image (qui est une "-2") avec celle sélectionnée
            iInversionDecalage=-iLbImageSelectionnee-iDecalage;
        }
        MontrerLudopenWebDeux(true);
    }
    else
    {
        // On inverse l'image sélectionnée avec la première image
        iInversionDecalage=-iLbImageSelectionnee-iDecalage;
    }
    if(emplacement!=EMPLACEMENT_SERVEUR&&(iLbImageSelectionnee-iDecalage)!=0)
    {
        Inverser(emplacement);
    }
    // Si l'image sélectionnée a la valeur de la taille du tableau, on restaure sa valeur
    if(iLbImageSelectionnee==sCheminImage.count())
    {
        iLbImageSelectionnee=iLbImageSelectionneeSauvegardee;
    }
}

/**
 *  @brief Slot qui est exécuté à chaque fois qu'une opération sur
 *      le serveur est terminée
 *
 *  @param DerniereCommande: Indique si il s'agit de la dernière opération
 */
void D_Image::slot_OperationFini(bool DerniereCommande)
{
    if(DerniereCommande)
    {
        QFileInfo fileInfo;
        QFile Fichier;
        QString DestNomFichier;
        // Trouve l'opération qui vient d'être faite
        switch(iOperationServeur)
        {
            case OP_AJOUT:
                // Si c'est la 1° image qu'on ajoute, on réactive les boutons
                if(sCheminImage.count()==0)
                {
                    ui->Bt_DeplacerDroite->setEnabled(true);
                    ui->Bt_DeplacerGauche->setEnabled(true);
                    ui->Bt_Supprimer->setEnabled(true);
                    ui->Bt_Sauvegarder->setEnabled(true);
                    ui->Bt_Defaut_Deux->setEnabled(true);
                    ui->Bt_Defaut_Ludopen->setEnabled(true);
                    ui->Bt_Defaut_Web->setEnabled(true);
                }
                // Si c'était la dernière image qui avait été sélectionnée, on active le bouton Droite
                if(iLbImageSelectionnee+iDecalage>sCheminImage.count())
                {
                    ui->Bt_Droite->setEnabled(true);
                }
                if(EstCeURL(F_Preferences::ObtenirValeur("CheminPhotosJeux"))) Ajouter(EMPLACEMENT_TEMP);
                // Si il y a au moins 2 images, on autorise le déplacement à droite
                if(sCheminImage.count()>1)
                {
                    ui->Bt_DeplacerDroite->setEnabled(true);
                }
                RechargerImages();
                iLbImageSelectionnee=2;
                ChangerSelection(lb_image);
                MontrerLudopenWebDeux(true);
                break;
            case OP_DEPLACEMENT_DROITE:
                if(EstCeURL(F_Preferences::ObtenirValeur("CheminPhotosJeux"))) Inverser(EMPLACEMENT_TEMP);
                RechargerImages();
                break;
            case OP_DEPLACEMENT_GAUCHE:
                if(EstCeURL(F_Preferences::ObtenirValeur("CheminPhotosJeux"))) Inverser(EMPLACEMENT_TEMP);
                RechargerImages();
                break;
            case OP_SUPPRESSION:
                if(EstCeURL(F_Preferences::ObtenirValeur("CheminPhotosJeux"))) Suppression(EMPLACEMENT_TEMP);
                // Si c'était la dernière image, on désactive le bouton Droite
                if(iDecalage==sCheminImage.count())
                {
                    ui->Bt_Droite->setEnabled(false);
                }
                RechargerImages();
                iLbImageSelectionnee=2;
                ChangerSelection(lb_image);
                MontrerLudopenWebDeux(true);
                break;
            case OP_DEFINIR_LUDOPEN:
                if(EstCeURL(F_Preferences::ObtenirValeur("CheminPhotosJeux"))) DefinirLudopen(EMPLACEMENT_TEMP);
                RechargerImages();
                break;
            case OP_DEFINIR_WEB:
                if(EstCeURL(F_Preferences::ObtenirValeur("CheminPhotosJeux"))) DefinirWeb(EMPLACEMENT_TEMP);
                RechargerImages();
                break;
            case OP_DEFINIR_DEUX:
                if(EstCeURL(F_Preferences::ObtenirValeur("CheminPhotosJeux"))) DefinirDeux(EMPLACEMENT_TEMP);
                RechargerImages();
                iLbImageSelectionnee=2;
                ChangerSelection(lb_image);
                break;
        }
        ui->dockWidget->setCursor(Qt::ArrowCursor);
    }
}

/**
 *  @brief Slot exécuté lorsque la fenêtre se ferme
 */
void D_Image::onCloseDialog()
{
    if(ImagesBGG) return;
    // Mets à jour le Lb_Image de la fenêtre d'admin Jeux et affiche la 1° image
    lb_img->DefiniriImage(0);
    lb_img->DefinirsCheminImage(sCheminImage);
    lb_img->AfficherImage(QSize(200,160));
}

/**
 *  @brief Gestion de l'évenement de fermeture
 *
 *  @param event : objet QCloseEvent
 */
void D_Image::closeEvent(QCloseEvent *event)
{
    emit dialogIsClosing();
    event->accept();
}

void D_Image::AfficherImageBGG(QString sCodeJeu,QVector<QVector<QVector<QString>>> Matrice)
{
    this->sCodeJeu=sCodeJeu;
    MatriceBGG=Matrice;
    RecupererImagesBGG();

}

void D_Image::SlotTelechargementsFini()
{
    this->setCursor(Qt::ArrowCursor);
    iDecalage=0;
    ui->Bt_Gauche->setEnabled(false);
    AfficherImage(sCodeJeu,sCheminImage);
}

void D_Image::on_Cb_FiltreLangue_currentIndexChanged(int index)
{
    RecupererImagesBGG();
}

void D_Image::RecupererImagesBGG()
{
    sCheminImage.clear();
    sNomVersion.clear();
    sURLImage.clear();
    QStringList URLs_Apercus;
    for (int i = 0; i < MatriceBGG.count(); ++i) {
        if(ui->Cb_FiltreLangue->currentData().toInt()==0||MatriceBGG[i][BGG_LANGUE][0]==ui->Cb_FiltreLangue->currentData().toString())
        {
            if(MatriceBGG[i][BGG_APERCU][0]=="")
            {
                URLs_Apercus.append("");
                sCheminImage.append("");
            }
            else
            {
                URLs_Apercus.append(MatriceBGG[i][BGG_APERCU][0]);
                sCheminImage.append(QDir::tempPath()+"/"+MatriceBGG[i][BGG_APERCU][0].split("/").last());
            }
            sNomVersion.append(MatriceBGG[i][BGG_NOM][0]);
            sIdVersion.append(MatriceBGG[i][BGG_VERSION][0]);
            sURLImage.append(MatriceBGG[i][BGG_IMAGE][0]);
        }
    }

    // Si il n'y a pas d'images, on mets toutes les langues, sinon on sort de la fonction
    if(URLs_Apercus.count()==0)
    {
        if(ui->Cb_FiltreLangue->currentData().toInt()==0) return;
        ui->Cb_FiltreLangue->setCurrentIndex(ui->Cb_FiltreLangue->count()-1);
    }

    acces=new AccesFichierParHTTP(this);
    //connect( acces, SIGNAL( SlotTelechargementFini(QString) ), this, SLOT( SlotFichierTelecharger(QString) ) );
    connect( acces, SIGNAL( SignalTelechargementsFini() ), this, SLOT( SlotTelechargementsFini() ) );
    this->setCursor(Qt::WaitCursor);
    acces->LancerTelechargements(QDir::tempPath(),"",URLs_Apercus);
}

void D_Image::on_Bt_Annuler_clicked()
{
    this->close();
}

void D_Image::on_Bt_Valider_clicked()
{
    bool SauvegarderImage=false;
    if(sURLImage[iLbImageSelectionnee+iDecalage]!="")
    {
        SauvegarderImage=QMessageBox::question(this,"Sauvegarder l'image du jeu","Êtes-vous sûr de vouloir enregistrer l'image du jeu sélectionné,\n"
                             "l'ancien image sera écrasée ?","Oui","Non")!=1;
    }
    acces=new AccesFichierParHTTP(this);
    //connect( acces, SIGNAL( SlotTelechargementFini(QString) ), this, SLOT( SlotFichierTelecharger(QString) ) );
    connect( acces, SIGNAL( SignalTelechargementsFini() ), this, SLOT( SlotTelechargementImageFinale() ) );
    this->setCursor(Qt::WaitCursor);
    sNomImagesAjoutees.clear();
    if(SauvegarderImage) sNomImagesAjoutees.append(sURLImage[iLbImageSelectionnee+iDecalage]);
    acces->LancerTelechargements(QDir::tempPath(),"",sNomImagesAjoutees);
    sNomImagesAjoutees.clear();
    sNomImagesAjoutees.append(QDir::tempPath()+"/"+sURLImage[iLbImageSelectionnee+iDecalage].split("/").last());
}

void D_Image::SlotTelechargementImageFinale()
{
    AjoutImageLocalOuServeur();
    this->setCursor(Qt::ArrowCursor);
    emit(SignalVersionBGGChoisi(sIdVersion[iLbImageSelectionnee+iDecalage]));
    close();    
}
