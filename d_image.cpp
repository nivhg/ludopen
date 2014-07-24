//---------------------------------------------------
/** @file d_image.cpp
  * @brief Classe QDialog affichant une image
  *
  * @author Vincent VICTORIN
  * @since 22/07/2014
  * @version 0.1
  * @date 22/07/2014
  *
  * Cette classe ajoute le composant lb_image qui affiche l'image filename
  *
  * Fabrication QtCreator
  *
*/

// En-têtes standards -----------------------------------------------------------
#include <QPixmap>
#include <QDesktopWidget>
#include <QRect>

// En-tête propre à  l'application ----------------------------------------------
#include "d_image.h"
#include "lb_image.h"
#include "ui_d_image.h"

/**
 *  @brief Constructeur de la classe.
 *
 *  @param parent
 *  @param filename : nom et chemin du fichier à afficher
 */
d_image::d_image(QWidget *parent,lb_image* Lb_Image) :
    QDialog(parent),
    ui(new Ui::d_image)
{
    ui->setupUi(this);

    /*// Mets la fenêtre au centre de l'écran
    QDesktopWidget *desktop = QApplication::desktop();
    QRect geo_d_i = this->geometry();
    int x = (desktop->width() - geo_d_i.width()) / 2;
    int y = (desktop->height() - geo_d_i.height()) / 2;
    this->setGeometry(x, y, geo_d_i.width(), geo_d_i.height());*/


    // Ajoute le label lb_image dans le dialog
    ui->gridLayout->addWidget(Lb_Image);
    connect( Lb_Image, SIGNAL( clicked() ), this, SLOT( on_Lb_Image_clicked() ) );
    // Mets la fenêtre au max
    this->showMaximized();
}

/**
 *  @brief Destructeur de la classe
 */
d_image::~d_image()
{
    delete ui;
}

/**
 *  @brief Gestion du click
 */
void d_image::on_Lb_Image_clicked()
{
    close();
}
