//------------------------------------------------------------------------------
/** @file       D_PopUpCode.cpp
 *  @brief      D_PopUpCode permet la gestion de la partie "Administration" en ayant un code.
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

// En-tête propre à l'objet ----------------------------------------------------
#include "d_popupcode.h"
#include "ui_d_popupcode.h"
//------------------------------------------------------------------------------

/**
 *  @brief  Constructeur de la classe.
 *
 *  @param  parent
 */
D_PopUpCode::D_PopUpCode(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::D_PopUpCode)
{
    ui->setupUi(this);

    enableClose=false;
    ui->Lb_Erreur->hide();
    ui->LE_Code->setEchoMode(QLineEdit::Password);

    QSqlQuery RequeteBenevoles;

    // Récupère toutes les activités
    RequeteBenevoles.prepare("SELECT Prenom as NomPrenom,IdMembre FROM membres WHERE TypeMembres_IdTypeMembres=2 ORDER BY NomPrenom") ;

    //Exectution de la requête
    if( !RequeteBenevoles.exec() )
    {
        qDebug() << getLastExecutedQuery(RequeteBenevoles) << RequeteBenevoles.lastError() ;
    }
    else
    {
        ui->CBx_Benevole->addItem("");
        while( RequeteBenevoles.next() )
        {
            // Récupère la valeurs des champs
            ui->CBx_Benevole->addItem(ObtenirValeurParNom(RequeteBenevoles,"NomPrenom").toString(),ObtenirValeurParNom(RequeteBenevoles,"IdMembre").toString());
        }
    }
    ui->Lb_Image->setVisible(false);
    ui->Lb_Code->setVisible(false);
    ui->LE_Code->setVisible(false);
    ui->Bt_Annuler->setVisible(false);
}

/**
 *  @brief  Destructeur de la classe.
 *
 */
D_PopUpCode::~D_PopUpCode()
{
    delete ui;
}

/**
 *  @brief  Permet de valider ou non le code d'accès administrateur.
 *
 *  @pre    Etre connecté à  la BDD.
 */
void D_PopUpCode::on_Bt_OK_clicked()
{
    enableClose=true;
    this->close();
    qDebug()<<"Connexion de "<<ui->CBx_Benevole->currentText();
    emit(SignalMembreIdentifier(ui->CBx_Benevole->currentData().toInt()));
/*    QString sCode;

    sCode = ui->LE_Code->text();

    if(F_Preferences::ObtenirValeur("CodeAdmin").compare(sCode) == 0)
    {
        this->close();
        ui->LE_Code->clear();
    }
    else
    {
        ui->Lb_Erreur->show();
        ui->Lb_Erreur->setText("<font color=red> Code d'accès incorrect. </font>");
    }*/
}

/**
 *  @brief  Ferme la fenêtre D_PopUpCode et ré-affiche le premier onglet.
 *
 */
void D_PopUpCode::on_Bt_Annuler_clicked()
{
    this->close();
    emit(this->SignalOnglet());
}

/**
 *  @brief  Verrouille le bouton Bt_OK si le champs est vide.
 *
 *  @param  arg1
 */
void D_PopUpCode::on_LE_Code_textEdited(const QString &arg1)
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

void D_PopUpCode::on_LE_Code_returnPressed()
{
    this->on_Bt_OK_clicked();
}

void D_PopUpCode::on_CBx_Benevole_currentIndexChanged(int index)
{
    if(ui->CBx_Benevole->currentText()!="")
    {
        ui->Bt_OK->setEnabled(true);
    }
    else
    {
        ui->Bt_OK->setEnabled(false);
    }
}

void D_PopUpCode::closeEvent(QCloseEvent *event)
 {
    if(!enableClose)
    {
         event->ignore();
    }
 }
