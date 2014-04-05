//-------------------------------------------------------------------------------
/** @file         f_ajoutediteur.cpp
 *  @brief        Permet d'ajouter un éditeur
 *
 *  @author       Florian MARY
 *  @author       STS IRIS, Lycée Nicolas APPERT, ORVAULT (FRANCE)
 *  @since        01/01/2012
 *  @version      0.1
 *  @date         11/05/2012
 *
 *  Il permet à partir de l'onglet administration et le widget jeux d'ajouter un éditeur lors de l'ajout d'un jeu, si
 *  celui-ci n'existe pas, directement à partir du ComboBox.
 *
 *  Fabrication   QtCreator
 *
 *  @todo         Ajout d'un éditeur
 *
 */
//-------------------------------------------------------------------------------
// En-têtes standards  ----------------------------------------------------------
#include <QMessageBox>
#include <QtSql>

// En-tête propre à l'application -----------------------------------------------
#include "f_ajouterediteur.h"
#include "ui_f_ajouterediteur.h"



/**
 * @brief Construcuteur de la classe f_ajouterediteur
 *
 * @param pFenetreAdmin
 * @param parent
 */
F_AjouterEditeur::F_AjouterEditeur(F_AjoutSuppModifJeux * pFenetreAdmin, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::F_AjouterEditeur)
{
    ui->setupUi(this);

    this->pFenetreAdminJeu = pFenetreAdmin ;

    this->setWindowTitle("Ajouter un éditeur");

    this->hide() ;
}

/**
 * @brief Desctructeur de la classe f_ajouterediteur
 *
 */
F_AjouterEditeur::~F_AjouterEditeur()
{
    delete ui;
}

/**
 * @brief Méthode qui permet d'ajouter un éditeur
 *
 */
void F_AjouterEditeur::on_Bt_Valider_clicked()
{
    if(this->ui->LE_NomEdit->text() == "")
    {
        QMessageBox::warning(this, "Attention", "Vous devez au moins donnez un nom avant de valider", "OK") ;
        return ;
    }
    else
    {
        QSqlQuery RequeteAjouterEditeur ;

        RequeteAjouterEditeur.prepare("INSERT INTO fournisseursediteur(NomFournisseur, AdresseFournisseur, CPFournisseur, VilleFournisseur, PersonneContacte, NumTelephone, Email, NumFax, Pays, Commentaires, Editeur) VALUES(:NomEdit, :AdresseEdit, :CPEdit, :VilleEdit, :PersonneAContacte, :NumEdit, :EmailEdit, :FaxEdit, :PaysEdit, :CommentairesEdit, 1)") ;
        RequeteAjouterEditeur.bindValue(":NomEdit", ui->LE_NomEdit->text());
        RequeteAjouterEditeur.bindValue(":AdresseEdit", ui->LE_AdresseEdit->text());
        RequeteAjouterEditeur.bindValue(":CPEdit", ui->LE_CPEdit->text());
        RequeteAjouterEditeur.bindValue(":VilleEdit", ui->LE_VilleEdit->text());
        RequeteAjouterEditeur.bindValue(":PersonneAContacte", ui->LE_PersonneContactEdit->text());
        RequeteAjouterEditeur.bindValue(":NumEdit", ui->LE_NumTel->text());
        RequeteAjouterEditeur.bindValue(":EmailEdit", ui->LE_EmailEdit->text());
        RequeteAjouterEditeur.bindValue(":FaxEdit", ui->LE_FaxEdit->text());
        RequeteAjouterEditeur.bindValue(":PaysEdit", ui->LE_PaysEdit->text());
        RequeteAjouterEditeur.bindValue(":CommentairesEdit", ui->TxE_CommentairesEdit->toPlainText());
        if(!RequeteAjouterEditeur.exec())
        {
            qDebug() << "Erreur RequeteAjouterEditeur :" << RequeteAjouterEditeur.lastQuery() ;
        }

        RequeteAjouterEditeur.next() ;

        this->pFenetreAdminJeu->ActualiserCBx_Editeur();
        this->hide() ;

    }
    this->ui->LE_AdresseEdit->clear();
    this->ui->LE_CPEdit->clear();
    this->ui->LE_EmailEdit->clear();
    this->ui->LE_FaxEdit->clear() ;
    this->ui->LE_NomEdit->clear();
    this->ui->LE_NumTel->clear();
    this->ui->LE_PaysEdit->clear();
    this->ui->LE_PersonneContactEdit->clear();
    ui->TxE_CommentairesEdit->clear() ;
    ui->LE_VilleEdit->clear();
}

/**
 * @brief Méthode pour annuler l'ajout de l'éditeur
 *
 */
void F_AjouterEditeur::on_Bt_Annuler_clicked()
{
    //Vide tout les champs lorsque la fenêtre est quitté après un clic sur Annuler
    this->ui->LE_AdresseEdit->clear();
    this->ui->LE_CPEdit->clear();
    this->ui->LE_EmailEdit->clear();
    this->ui->LE_FaxEdit->clear() ;
    this->ui->LE_NomEdit->clear();
    this->ui->LE_NumTel->clear();
    this->ui->LE_PaysEdit->clear();
    this->ui->LE_PersonneContactEdit->clear();
    this->ui->TxE_CommentairesEdit->clear();
    //Quitte la fenêtre
    this->hide();
}
