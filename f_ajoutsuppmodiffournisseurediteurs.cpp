//-------------------------------------------------------------------------------
/** @file         f_ajoutstatut.cpp
 *  @brief        Permet de gérer les fournisseurs et les éditeurs
 *
 *  @author       Florian MARY
 *  @author       STS IRIS, Lycée Nicolas APPERT, ORVAULT (FRANCE)
 *  @since        01/01/2012
 *  @version      0.1
 *  @date         23/02/2013 William
 *
 *  Il permet à partir de l'onglet administration et le widget fournisseurs-éditeurs,
 *  d'ajouter, de supprimer, ou de modifier une fiche de fournisseur ou éditeur.
 *
 *  Fabrication   QtCreator
 *
 *
 */
//-------------------------------------------------------------------------------
// En-têtes standards  ----------------------------------------------------------
#include <QtSql>
#include <QStandardItemModel>
#include <QMessageBox>

// En-tête propre à l'application -----------------------------------------------
#include "f_ajoutsuppmodiffournisseursediteurs.h"
#include "ui_f_ajoutsuppmodiffournisseursediteurs.h"



/**
 * @brief Constructeur de la classe f_ajoutsuppmodiffournisseursediteurs
 *
 * @param parent
 */
F_AjoutSuppModifFournisseursEditeurs::F_AjoutSuppModifFournisseursEditeurs(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::F_AjoutSuppModifFournisseursEditeurs)
{
    ui->setupUi(this);

    /////////////////////////////////////////////////////////
    //////////////////Création table view //////////////////
    ///////////////////////////////////////////////////////

    //Création d'un model pour le TableView des fournisseurs
    this->ModelFournisseur = new QStandardItemModel() ;
    //Associe le model au TableView
    ui->TbV_Recherche->setModel(this->ModelFournisseur);
    // Mise en lecture seule
    ui->TbV_Recherche->setEditTriggers(0);

    // Faire défiler le tableau des jeux avec les flêches du clavier
    connect(ui->TbV_Recherche->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(on_TbV_Recherche_clicked(QModelIndex)));

    // Initialise la table view avec tous les fournisseurs
    on_LE_Recherche_textChanged("") ;
    //Supprime le numéro des lignes
    ui->TbV_Recherche->verticalHeader()->setVisible(false);

    //Coche automatiquement la Rb "Les deux"
    ui->Rb_Les2->setChecked(true);

    AjoutOuModif = false ;
    if(ui->LE_Nom->text() == "")
        {
            ui->Bt_Valider->setDisabled(true);
            ui->Bt_Annuler->setDisabled(true);
            ui->Bt_Supprimer->setDisabled(true);
        }


        ui->LE_CodePostal->setDisabled(true);
        ui->LE_Contact->setDisabled(true);
        ui->LE_Email->setDisabled(true);
        ui->LE_Fax->setDisabled(true);
        ui->LE_Nom->setDisabled(true);
        ui->LE_Pays->setDisabled(true);
        ui->LE_Rue->setDisabled(true);
        ui->LE_Ville->setDisabled(true);
        ui->LE_Telephone->setDisabled(true);
}

/**
 * @brief Destructeur de la classe f_ajoutsuppmodiffournisseursediteurs
 *
 */
F_AjoutSuppModifFournisseursEditeurs::~F_AjoutSuppModifFournisseursEditeurs()
{
    delete ui;
}
/**
 * @brief Méthode qui modifie le contenu de tableau en fonction de ce que l'on rentre dans la case de recherche
 *
 * @param arg1
 */
void F_AjoutSuppModifFournisseursEditeurs::on_LE_Recherche_textChanged(const QString &arg1)
{
    unsigned int NumeroLigne (0);
    QString NomFournisseur = arg1;

    if(NomFournisseur.size() >= 2)
    {
        NomFournisseur="%"+NomFournisseur+"%";

        QSqlQuery RequeteRechercheFournisseur;
        NumeroLigne=0;

        RequeteRechercheFournisseur.prepare("SELECT  NomFournisseur FROM fournisseursediteur WHERE NomFournisseur LIKE (:NomFournissseur)");
        RequeteRechercheFournisseur.bindValue(":NomFournissseur",NomFournisseur);
        RequeteRechercheFournisseur.exec();

        //On vide le model
        this->ModelFournisseur->clear();
        //Indique le nombes de colones puis leurs noms
        this->ModelFournisseur->setColumnCount(1);
        this->ModelFournisseur->setHorizontalHeaderItem(0, new QStandardItem("Nom"));
        //impose une taille aux colones
        ui->TbV_Recherche->setColumnWidth(0,300);

        //Tant qu'il y a des fournisseursediteur dans la table fournisseursediteur
        while(RequeteRechercheFournisseur.next())
        {
            //on ajoute une nouvelle ligne du table view
            this->ModelFournisseur->setItem(NumeroLigne, 0, new QStandardItem(RequeteRechercheFournisseur.value(0).toString() ));
            NumeroLigne++;
         }
    }
    else
    {
        QSqlQuery RequeteRechercheFournisseur;
        NumeroLigne =0;
        RequeteRechercheFournisseur.exec("SELECT  NomFournisseur FROM fournisseursediteur ORDER BY NomFournisseur ASC");

        //On vide le model
        this->ModelFournisseur->clear();
        //Indique le nombes de colones puis leurs noms
        this->ModelFournisseur->setColumnCount(1);
        this->ModelFournisseur->setHorizontalHeaderItem(0, new QStandardItem("Nom"));
        //impose une taille aux colones
        ui->TbV_Recherche->setColumnWidth(0,300);

        //Tant qu'il y a des jeux dans la table jeux,
        while(RequeteRechercheFournisseur.next())
        {
            //on ajoute une nouvelle ligne du table view
            this->ModelFournisseur->setItem(NumeroLigne, 0, new QStandardItem(RequeteRechercheFournisseur.value(0).toString() ));
            NumeroLigne++;
         }
    }
}
/**
 * @brief Méthode qui récupère et sélectionne le nom du fournisseur de la case sur laquelle on a cliqué
 *
 * @param index
 */
void F_AjoutSuppModifFournisseursEditeurs::on_TbV_Recherche_clicked(const QModelIndex &index)
{
    // Récupère le nom du fournisseurediteur séléctionné
    this->Selection = (this->ModelFournisseur->index(index.row(), 0).data().toString());

    qDebug()<< "Le nom recuperer est :" << Selection ;

    //Valide le nom du fournisseur récupéré
    on_Bt_Ok_clicked() ;
    //Séléctionne la ligne sur laquelle on a cliqué
    ui->TbV_Recherche->selectRow(index.row());
}
/**
 * @brief Méthode qui affiche les informations sur le fournisseur ou l'éditeur dans les champs correspondants
 *
 */
void F_AjoutSuppModifFournisseursEditeurs::on_Bt_Ok_clicked()
{
    ui->LE_CodePostal->setEnabled(true);
    ui->LE_Contact->setEnabled(true);
    ui->LE_Email->setEnabled(true);
    ui->LE_Fax->setEnabled(true);
    ui->LE_Nom->setEnabled(true);
    ui->LE_Pays->setEnabled(true);
    ui->LE_Rue->setEnabled(true);
    ui->LE_Ville->setEnabled(true);
    ui->LE_Telephone->setEnabled(true);


        QSqlQuery RequeteInfoFournisseur ;
        RequeteInfoFournisseur.prepare("SELECT NomFournisseur, AdresseFournisseur, CPFournisseur, VilleFournisseur, PersonneContacte, NumTelephone, Email, NumFax, Remarque FROM fournisseursediteur WHERE NomFournisseur =:NomDuFournisseur") ;
        RequeteInfoFournisseur.bindValue(":NomDuFournisseur", this->Selection);
        if(!RequeteInfoFournisseur.exec())
        {
            qDebug() << "F_AjoutSuppModifFournisseursEditeurs::on_Bt_Ok_clicked()" << RequeteInfoFournisseur.lastQuery();
        }
        RequeteInfoFournisseur.next() ;

        QString NomFournisseur = RequeteInfoFournisseur.value(0).toString() ;
        QString Adresse = RequeteInfoFournisseur.value(1).toString() ;
        QString CP = RequeteInfoFournisseur.value(2).toString() ;
        QString VilleFournisseur = RequeteInfoFournisseur.value(3).toString() ;
        QString Contact = RequeteInfoFournisseur.value(4).toString() ;
        QString Tel = RequeteInfoFournisseur.value(5).toString() ;
        QString Email = RequeteInfoFournisseur.value(6).toString() ;
        QString Fax = RequeteInfoFournisseur.value(7).toString() ;
        QString Remarque = RequeteInfoFournisseur.value(8).toString() ;

        ui->LE_Contact->setText(Contact);
        ui->LE_Email->setText(Email);
        ui->LE_Fax->setText(Fax);
        ui->LE_Nom->setText(NomFournisseur);
        ui->LE_Telephone->setText(Tel);
        ui->LE_Ville->setText(VilleFournisseur);
        ui->LE_CodePostal->setText(CP);
        ui->LE_Rue->setText(Adresse);
        ui->TxE_Remarques->setText(Remarque);
}
/**
 * @brief Méthode qui permet de passe le booléen AjoutOuModif à true pour dire que l'on se trouve désormais en mode ajout
 *
 */
void F_AjoutSuppModifFournisseursEditeurs::on_Bt_Ajouter_clicked()
{
    ui->LE_CodePostal->setEnabled(true);
    ui->LE_Contact->setEnabled(true);
    ui->LE_Email->setEnabled(true);
    ui->LE_Fax->setEnabled(true);
    ui->LE_Nom->setEnabled(true);
    ui->LE_Pays->setEnabled(true);
    ui->LE_Rue->setEnabled(true);
    ui->LE_Ville->setEnabled(true);
    ui->LE_Telephone->setEnabled(true);

    ViderChamps();

    AjoutOuModif = true ;


}

/**
 * @brief Méthode qui permet de supprimer la fiche de fournisseur ou d'éditeur de la BDD
 *
 */
void F_AjoutSuppModifFournisseursEditeurs::on_Bt_Supprimer_clicked()
{
    if(ui->LE_Nom->text() == "")
    {
        ui->Bt_Supprimer->setDisabled(true);
    }
    else
    {
        if(QMessageBox::warning(this, "Confirmation suppression", "Êtes-vous sur de vouloir le supprimer ?", "Oui", "Non") == 0)
        {
            QSqlQuery RequeteSuppFournOuEdit ;
            RequeteSuppFournOuEdit.prepare("DELETE FROM fournisseursediteur WHERE NomFournisseur =:LeNom") ;
            RequeteSuppFournOuEdit.bindValue(":LeNom", ui->LE_Nom->text());
            RequeteSuppFournOuEdit.exec() ;
            RequeteSuppFournOuEdit.next() ;
            ViderChamps();
        }
        else
        {

        }
    }
    on_Rb_Les2_clicked();


}

/**
 * @brief Méthode qui ajoute le fournisseur ou l'éditeur ou qui le modifier selon le mode dans lequel on se trouve
 *
 */
void F_AjoutSuppModifFournisseursEditeurs::on_Bt_Valider_clicked()
{
    if(AjoutOuModif == true)
    {
        if(ui->LE_Nom->text() == "")
        {
            QMessageBox::warning(this, "Pré-requis pour l'ajout", "Vous devez au minimum saisir un nom !", "OK");
        }
        else
        {
            QSqlQuery RequeteAjoutFournEdit ;
            RequeteAjoutFournEdit.prepare("INSERT INTO fournisseursediteur(NomFournisseur, AdresseFournisseur, CPFournisseur, VilleFournisseur, PersonneContacte, NumTelephone, Email, NumFax, Pays, Fournisseur, Editeur, Remarque) VALUES (:LeNom, :LaAdresse, :LeCP, :LaVille, :LeContact, :LeTel, :LeEmail, :LeFax, :LePays, :LeFournisseur, :LeEditeur, :LaRemarque)") ;
            RequeteAjoutFournEdit.bindValue(":LeNom", ui->LE_Nom->text());
            RequeteAjoutFournEdit.bindValue(":LaAdresse", ui->LE_Rue->text());
            RequeteAjoutFournEdit.bindValue(":LeCP", ui->LE_CodePostal->text());
            RequeteAjoutFournEdit.bindValue(":LaVille", ui->LE_Ville->text());
            RequeteAjoutFournEdit.bindValue(":LeContact", ui->LE_Contact->text());
            RequeteAjoutFournEdit.bindValue(":LeTel", ui->LE_Telephone->text());
            RequeteAjoutFournEdit.bindValue(":LeEmail", ui->LE_Email->text());
            RequeteAjoutFournEdit.bindValue(":LeFax", ui->LE_Fax->text());
            RequeteAjoutFournEdit.bindValue(":LePays", ui->LE_Pays->text());
            switch(QMessageBox::information(this, "Qu'est ce ?", "Cet ajout est-il un fournisseur ou un éditeur ?", "Fournisseur", "Editeur","Les deux") )
            {
                case 0 : RequeteAjoutFournEdit.bindValue(":LeFournisseur", 1);
                         RequeteAjoutFournEdit.bindValue(":LeEditeur", 0);
                break ;
                case 1 : RequeteAjoutFournEdit.bindValue(":LeFournisseur", 0);
                         RequeteAjoutFournEdit.bindValue(":LeEditeur", 1);
                break ;
                case 2 : RequeteAjoutFournEdit.bindValue(":LeFournisseur", 1);
                         RequeteAjoutFournEdit.bindValue(":LeEditeur", 1);
                break ;

            }
            RequeteAjoutFournEdit.bindValue(":LaRemarque", ui->TxE_Remarques->toPlainText());

            if(!RequeteAjoutFournEdit.exec())
            {
                qDebug() << "F_AjoutSuppModifFournisseursEditeurs::on_Bt_Valider_clicked()" << RequeteAjoutFournEdit.lastQuery();
            }            
            ViderChamps();
            this->AjoutOuModif = false ;
            // réactualiser la liste des fournisseurs
            this->on_LE_Recherche_textChanged("") ;
        }
    }
    else   // Création d'un nouveau fournisseur/éditeur
    {

        QSqlQuery RequeteModifFournOuEdit ;

        RequeteModifFournOuEdit.prepare("UPDATE fournisseursediteur SET AdresseFournisseur =:LaAdresse, CPFournisseur =:LeCP, VilleFournisseur =:LaVille, PersonneContacte =:LeContact, NumTelephone =:LeNum, Email =:LeEmail, NumFax =:LeFax, Pays =:LePays, Remarque =:LaRemarque WHERE NomFournisseur =:LeNom") ;
        RequeteModifFournOuEdit.bindValue(":LeNom", ui->LE_Nom->text());
        RequeteModifFournOuEdit.bindValue(":LaAdresse", ui->LE_Rue->text());
        RequeteModifFournOuEdit.bindValue(":LeCP", ui->LE_CodePostal->text());
        RequeteModifFournOuEdit.bindValue(":LaVille", ui->LE_Ville->text());
        RequeteModifFournOuEdit.bindValue(":LeContact", ui->LE_Contact->text());
        RequeteModifFournOuEdit.bindValue(":LeNum", ui->LE_Telephone->text());
        RequeteModifFournOuEdit.bindValue(":LeEmail", ui->LE_Email->text());
        RequeteModifFournOuEdit.bindValue(":LeFax", ui->LE_Fax->text());
        RequeteModifFournOuEdit.bindValue(":LePays", ui->LE_Pays->text());
        RequeteModifFournOuEdit.bindValue(":LaRemarque", ui->TxE_Remarques->toPlainText());

        if(!RequeteModifFournOuEdit.exec())
        {
            qDebug() << "F_AjoutSuppModifFournisseursEditeurs::on_Bt_Valider_clicked()" << RequeteModifFournOuEdit.lastQuery();
        }
        RequeteModifFournOuEdit.next() ;
        QMessageBox::information(this, "Confirmation", "Nouveau fournisseur/éditeur ajouter avec succès", "OK") ;
        ViderChamps();
        // réactualiser la liste des fournisseurs
        this->on_LE_Recherche_textChanged("") ;
        this->AjoutOuModif = false ;
    }
}

/**
 * @brief Méthode qui annule l'ajout ou la modification d'une fiche de fournisseur ou d'éditeur
 *
 */
void F_AjoutSuppModifFournisseursEditeurs::on_Bt_Annuler_clicked()
{
    QSqlQuery RequeteAnnulModifFournOuEdit ;

    RequeteAnnulModifFournOuEdit.prepare("SELECT NomFournisseur, AdresseFournisseur, CPFournisseur, VilleFournisseur, PersonneContacte, NumTelephone, Email, NumFax, Pays, Remarque FROM fournisseursediteur WHERE NomFournisseur =:LeNom") ;
    RequeteAnnulModifFournOuEdit.bindValue(":LeNom", ui->LE_Nom->text());
    if(!RequeteAnnulModifFournOuEdit.exec())
    {
        qDebug() << "F_AjoutSuppModifFournisseursEditeurs::on_Bt_Annuler_clicked()" << RequeteAnnulModifFournOuEdit.lastQuery() ;
    }
    RequeteAnnulModifFournOuEdit.next();

    ui->LE_Nom->setText(RequeteAnnulModifFournOuEdit.value(0).toString());
    ui->LE_Rue->setText(RequeteAnnulModifFournOuEdit.value(1).toString());
    ui->LE_CodePostal->setText(RequeteAnnulModifFournOuEdit.value(2).toString());
    ui->LE_Ville->setText(RequeteAnnulModifFournOuEdit.value(3).toString());
    ui->LE_Contact->setText(RequeteAnnulModifFournOuEdit.value(4).toString());
    ui->LE_Telephone->setText(RequeteAnnulModifFournOuEdit.value(5).toString());
    ui->LE_Email->setText(RequeteAnnulModifFournOuEdit.value(6).toString());
    ui->LE_Fax->setText(RequeteAnnulModifFournOuEdit.value(7).toString());
    ui->LE_Pays->setText(RequeteAnnulModifFournOuEdit.value(8).toString());
    ui->TxE_Remarques->setText(RequeteAnnulModifFournOuEdit.value(9).toString());
}
/**
 * @brief Méthode qui permet de faire une recherche seulement parmi les fournisseurs ou d'affiche la liste de tout les fournisseurs
 *
 */
void F_AjoutSuppModifFournisseursEditeurs::on_Rb_Fournisseur_clicked()
{
    ViderChamps();
    ui->LE_Recherche->setText("");

    QSqlQuery RequeteRechercheFournisseur;
    unsigned int NumeroLigne(0) ;

    RequeteRechercheFournisseur.prepare("SELECT  NomFournisseur, Fournisseur FROM fournisseursediteur WHERE Fournisseur = 1 ORDER BY NomFournisseur ASC");
    RequeteRechercheFournisseur.exec();

    //On vide le model
    this->ModelFournisseur->clear();
    //Indique le nombes de colones puis leurs noms
    this->ModelFournisseur->setColumnCount(1);
    this->ModelFournisseur->setHorizontalHeaderItem(0, new QStandardItem("Nom fournisseur"));
    //impose une taille aux colones
    ui->TbV_Recherche->setColumnWidth(0,300);

    //Tant qu'il y a des fournisseursediteur dans la table fournisseursediteur
    while(RequeteRechercheFournisseur.next())
    {
        //on ajoute une nouvelle ligne du table view
        this->ModelFournisseur->setItem(NumeroLigne, 0, new QStandardItem(RequeteRechercheFournisseur.value(0).toString() ));
        NumeroLigne ++ ;
    }
}

/**
 * @brief Méthode qui permet de faire une recherche seulement parmi les éditeurs ou d'affiche la liste de tout les éditeurs
 *
 */
void F_AjoutSuppModifFournisseursEditeurs::on_Rb_Editeur_clicked()
{
    ViderChamps();
    ui->LE_Recherche->setText("");

    QSqlQuery RequeteRechercheFournisseur;
    unsigned int NumeroLigne(0) ;

    RequeteRechercheFournisseur.prepare("SELECT  NomFournisseur, Editeur FROM fournisseursediteur WHERE Editeur = 1 ORDER BY NomFournisseur ASC");
    //RequeteRechercheFournisseur.bindValue(":NomFournissseur",NomFournisseur);
    RequeteRechercheFournisseur.exec();

    //On vide le model
    this->ModelFournisseur->clear();
    //Indique le nombres de colones puis leurs noms
    this->ModelFournisseur->setColumnCount(1);
    this->ModelFournisseur->setHorizontalHeaderItem(0, new QStandardItem("Nom editeur"));
    //impose une taille aux colones
    ui->TbV_Recherche->setColumnWidth(0,300);

    //Tant qu'il y a des fournisseursediteur dans la table fournisseursediteur
    while(RequeteRechercheFournisseur.next())
    {
        //on ajoute une nouvelle ligne du table view
        this->ModelFournisseur->setItem(NumeroLigne, 0, new QStandardItem(RequeteRechercheFournisseur.value(0).toString() ));
        NumeroLigne ++ ;
    }
}

/**
 * @brief Méthode qui permet de faire une recherche parmi les fournisseurs et les éditeurs ou d'affiche la liste de tout les fournisseurs et éditeurs
 *
 */
void F_AjoutSuppModifFournisseursEditeurs::on_Rb_Les2_clicked()
{
    ViderChamps();
    ui->LE_Recherche->setText("");

    QSqlQuery RequeteRechercheFournisseur;
    unsigned int NumeroLigne(0) ;

    RequeteRechercheFournisseur.prepare("SELECT  NomFournisseur FROM fournisseursediteur ORDER BY NomFournisseur ASC");
    RequeteRechercheFournisseur.exec();

    //On vide le model
    this->ModelFournisseur->clear();
    //Indique le nombes de colones puis leurs noms
    this->ModelFournisseur->setColumnCount(1);
    this->ModelFournisseur->setHorizontalHeaderItem(0, new QStandardItem("Nom"));
    //impose une taille aux colones
    ui->TbV_Recherche->setColumnWidth(0,300);

    //Tant qu'il y a des fournisseursediteur dans la table fournisseursediteur
    while(RequeteRechercheFournisseur.next())
    {
        //on ajoute une nouvelle ligne du table view
        this->ModelFournisseur->setItem(NumeroLigne, 0, new QStandardItem(RequeteRechercheFournisseur.value(0).toString() ));
        NumeroLigne ++ ;
    }
}
/**
 * @brief Méthode qui lorsque l'on appui sur la touche entrée du clavier elle appele la méthode qui appui sur le bouton OK
 *
 */
void F_AjoutSuppModifFournisseursEditeurs::on_LE_Recherche_returnPressed()
{
    on_Bt_Ok_clicked();
}
/**
 * @brief Méthode qui permet de vider tout les champs d'informations
 *
 */
void F_AjoutSuppModifFournisseursEditeurs::ViderChamps()
{
    ui->LE_CodePostal->clear();
    ui->LE_Contact->clear();
    ui->LE_Email->clear();
    ui->LE_Fax->clear();
    ui->LE_Nom->clear();
    ui->LE_Rue->clear();
    ui->LE_Telephone->clear();
    ui->LE_Ville->clear();
    ui->TxE_Remarques->clear();
    ui->LE_Recherche->clear();
}
/**
 * @brief Méthode qui lorsque la case Nom change d'état, active certains boutons
 *
 * @param arg1
 */
void F_AjoutSuppModifFournisseursEditeurs::on_LE_Nom_textChanged(const QString &arg1)
{
    ui->Bt_Annuler->setEnabled(true);
    ui->Bt_Valider->setEnabled(true);
    ui->Bt_Supprimer->setEnabled(true);
}
