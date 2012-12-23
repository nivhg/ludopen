//------------------------------------------------------------------------------
/** @file       f_popupcode.cpp
 *  @brief      F_PopUpCode permet la gestion de la partie "Administration" en ayant un code.
 *
 *  @author     NORMAND Julien
 *  @author     STS IRIS, Lycée Nicolas APPERT, ORVAULT (FRANCE)
 *  @since      01/2012
 *  @version    1.0
 *  @date       29/06/2012
 *
 *  Fabrication Qt Creator, projet  .pro
 *
 *  @todo       Classe terminée.
 */
//------------------------------------------------------------------------------

// En-têtes standards (ATTENTION : garder toujours le meme ordre) --------------
#include <iostream>
#include <QtGui>
using namespace std ;
//------------------------------------------------------------------------------

// En-tête propre à l'objet ----------------------------------------------------
#include "f_popupcode.h"
#include "ui_f_popupcode.h"
//------------------------------------------------------------------------------

/**
 *  @brief  Constructeur de la classe.
 *
 *  @param  parent
 */
F_PopUpCode::F_PopUpCode(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::F_PopUpCode)
{
    ui->setupUi(this);
    ui->Lb_Erreur->hide();
    ui->LE_Code->setEchoMode(QLineEdit::Password);
}

/**
 *  @brief  Destructeur de la classe.
 *
 */
F_PopUpCode::~F_PopUpCode()
{
    delete ui;
}

/**
 *  @brief  Permet de valider ou non le code d'accès administrateur.
 *
 *  @pre    Etre connecté à  la BDD.
 */
void F_PopUpCode::on_Bt_OK_clicked()
{
    QSqlQuery RequeteCode;
    QString sCompare;
    QString sCode;

    RequeteCode.prepare("SELECT CodeAdmin FROM preferences WHERE IdPreferences=1");
    RequeteCode.exec();
    RequeteCode.next();

    sCompare = RequeteCode.value(0).toString();
    sCode = ui->LE_Code->text();

    if(sCompare.compare(sCode) == 0)
    {
        this->close();
        ui->LE_Code->clear();
    }
    else
    {
        ui->Lb_Erreur->show();
        ui->Lb_Erreur->setText("<font color=red> Code d'accès incorrect. </font>");
    }
}

/**
 *  @brief  Ferme la fenêtre F_PopUpCode et ré-affiche le premier onglet.
 *
 */
void F_PopUpCode::on_Bt_Annuler_clicked()
{
    this->close();
    emit(this->SignalOnglet());
}

/**
 *  @brief  Verrouille le bouton Bt_OK si le champs est vide.
 *
 *  @param  arg1
 */
void F_PopUpCode::on_LE_Code_textEdited(const QString &arg1)
{
    if(arg1 == "")
    {
        ui->Bt_OK->setEnabled(false) ;
    }
    else
    {
        ui->Bt_OK->setEnabled(true);
    }
}

/**
 *  @brief
 *
 */

void F_PopUpCode::on_LE_Code_returnPressed()
{
    this->on_Bt_OK_clicked();
}
