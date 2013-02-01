//-------------------------------------------------------------------------------
/** @file         f_ajouterfournisseur.cpp
 *  @brief        Permet d'ajouter un fourniseeur
 *
 *  @author       Florian MARY
 *  @author       STS IRIS, Lycée Nicolas APPERT, ORVAULT (FRANCE)
 *  @since        01/01/2012
 *  @version      0.1
 *  @date         01/02/2013
 *
 *  Il permet à partir de l'onglet administration et le widget jeux d'ajouter un fournisseur lors de l'ajout d'un jeu, si
 *  celui-ci n'existe pas, directement à partir du ComboBox.
 *
 *  Fabrication   QtCreator
 *
 *  @todo         Ajout d'un fournisseur
 *
 */
//-------------------------------------------------------------------------------
// En-têtes standards  ----------------------------------------------------------
#include <QtSql>
#include <QMessageBox>

// En-tête propre à l'application -----------------------------------------------
#include "f_ajouterfournisseur.h"
#include "ui_f_ajouterfournisseur.h"


/**
 * @brief Constructeur de la classe f_ajouterfournisseur
 *
 * @param pFenetre
 * @param parent
 */
F_AjouterFournisseur::F_AjouterFournisseur(F_AjoutSuppModifJeux * pFenetre, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::F_AjouterFournisseur)
{
    ui->setupUi(this);
    
    this->pFenetreAjoutJeu = pFenetre ;

    this->setWindowTitle("Ajouter un fournisseur");
    
    this->hide();
}

/**
 * @brief Destructeur de la classe f_ajouterfournisseur
 *
 */
F_AjouterFournisseur::~F_AjouterFournisseur()
{
    delete ui;
}

/**
 * @brief Méthode qui ajoute le fournisseur dans la BDD
 *
 */
void F_AjouterFournisseur::on_Bt_Valider_clicked()
{
    if(this->ui->LE_NomFourn->text() == "")
    {
        QMessageBox::warning(this, "Attention", "Vous devez au moins donner un nom avant de valider", "OK") ;
        return;
    }
    else
    {
        QSqlQuery RequeteAjouterFournisseur ;
        
        RequeteAjouterFournisseur.prepare("INSERT INTO fournisseursediteur(NomFournisseur, AdresseFournisseur, CPFournisseur, VilleFournisseur, PersonneContacte, NumTelephone, Email, NumFax, Pays, Commentaires, Fournisseur) VALUES(:NomFourn, :AdresseFourn, :CPFourn, :VilleFourn, :PersonneAContacte, :NumFourn, :EmailFourn, :FaxFourn, :PaysFourn, :CommentairesFourn, 1)") ;
        RequeteAjouterFournisseur.bindValue(":NomFourn", ui->LE_NomFourn->text());
        RequeteAjouterFournisseur.bindValue(":AdresseFourn", ui->LE_AdresseFourn->text());
        RequeteAjouterFournisseur.bindValue(":CPFourn", ui->LE_CPFourn->text());
        RequeteAjouterFournisseur.bindValue(":VilleFourn", ui->LE_VilleFourn->text());
        RequeteAjouterFournisseur.bindValue(":PersonneAContacte", ui->LE_PersonneContactFourn->text());
        RequeteAjouterFournisseur.bindValue(":NumFourn", ui->LE_NumTel->text());
        RequeteAjouterFournisseur.bindValue(":EmailFourn", ui->LE_EmailFourn->text());
        RequeteAjouterFournisseur.bindValue(":FaxFourn", ui->LE_FaxFourn->text());
        RequeteAjouterFournisseur.bindValue(":PaysFourn", ui->LE_PaysFourn->text());
        RequeteAjouterFournisseur.bindValue(":CommentairesFourn", ui->TxE_CommentairesFourn->toPlainText());
        if(!RequeteAjouterFournisseur.exec())
        {
            qDebug() << "Erreur RequeteAjouterFournisseur :" << RequeteAjouterFournisseur.lastError() ;
        }
         
        RequeteAjouterFournisseur.next() ;
        
        this->pFenetreAjoutJeu->ActualiserCBx_Fournisseur();
        this->hide() ;
        
    }
    this->ui->LE_AdresseFourn->clear();
    this->ui->LE_CPFourn->clear();
    this->ui->LE_EmailFourn->clear();
    this->ui->LE_FaxFourn->clear() ;
    this->ui->LE_NomFourn->clear();
    this->ui->LE_NumTel->clear();
    this->ui->LE_PaysFourn->clear();
    this->ui->LE_PersonneContactFourn->clear();
    ui->TxE_CommentairesFourn->clear() ;
    ui->LE_VilleFourn->clear();
}
/**
 * @brief Méthode qui annule l'ajout du fournisseur
 *
 */
void F_AjouterFournisseur::on_Bt_Annuler_clicked()
{
    //Vide tout les champs lorsque la fenêtre est quitté après un clic sur Annuler
    this->ui->LE_AdresseFourn->clear();
    this->ui->LE_CPFourn->clear();
    this->ui->LE_EmailFourn->clear();
    this->ui->LE_FaxFourn->clear() ;
    this->ui->LE_NomFourn->clear();
    this->ui->LE_NumTel->clear();
    this->ui->LE_PaysFourn->clear();
    this->ui->LE_PersonneContactFourn->clear();
    this->ui->TxE_CommentairesFourn->clear();
    //Quitte la fenêtre
    this->hide();
}
