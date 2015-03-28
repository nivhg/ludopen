//-------------------------------------------------------------------------------
/** @file         f_ajoutstatut.cpp
 *  @brief        Permet de gérer les fournisseurs et les éditeurs
 *
 *  @author       Florian MARY
 *  @author       STS IRIS, Lycée Nicolas APPERT, ORVAULT (FRANCE)
 *  @since        01/01/2012
 *  @version      0.2
 *  @date         05/08/2014 Vincent
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
#include "fonctions_globale.h"


/**
 * @brief Constructeur de la classe f_ajoutsuppmodiffournisseursediteurs
 *
 * @param parent
 */
F_AjoutSuppModifFournisseursEditeurs::F_AjoutSuppModifFournisseursEditeurs(QWidget *parent, char Mode) :
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

    this->pPopUpCLESTTEM = new F_PopUpCLESTTEM(this);
    this->pPopUpCLESTTEM->setWindowModality(Qt::ApplicationModal);

    this->TbCLESTTEM = new QStandardItemModel();
    this->TbV_CLESTTEM =  new QTableView();
    this->TbV_CLESTTEM->setModel(this->TbCLESTTEM);
    this->TbV_CLESTTEM->setEditTriggers(0);
    this->TbV_CLESTTEM->verticalHeader()->setVisible(false);
    //Affichage de la BDD dans le tableau titre qui est dans l'onglet Membres/Emprunts de F_Preferences.
    this->TbCLESTTEM->setColumnCount(2);
    this->TbCLESTTEM->setRowCount(0);

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

    // Connecte l'évenement textEdited à la fonction toUpper
    connect(ui->LE_Nom, SIGNAL(textEdited(const QString &)), SLOT(toUpper(const QString &)));
    this->Mode=Mode;
    this->Parent=parent;
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
 * @brief Méthode qui modifie le contenu de tableau en fonction de ce que l'on rentre
 * dans la case de recherche
 *
 * @param arg1
 */
void F_AjoutSuppModifFournisseursEditeurs::on_LE_Recherche_textChanged(const QString &arg1)
{
    QString NomFournisseur = "%"+arg1+"%";

    QSqlQuery RequeteRechercheFournisseur;
    if(NomFournisseur.size() >= 2)
    {
        RequeteRechercheFournisseur.prepare("SELECT NomFournisseur,IdFournisseur FROM fournisseursediteur"
                      " WHERE NomFournisseur LIKE \""+NomFournisseur+"\" ORDER BY NomFournisseur ASC");
        RequeteRechercheFournisseur.bindValue(":NomFournissseur",NomFournisseur);
        if(!RequeteRechercheFournisseur.exec())
        {
            qDebug()<<"F_AjoutSuppModifFournisseursEditeurs::on_LE_Recherche_textChanged"<<RequeteRechercheFournisseur.lastQuery();
        }
    }
    else
    {
        RequeteRechercheFournisseur.exec("SELECT NomFournisseur,IdFournisseur FROM fournisseursediteur ORDER BY NomFournisseur ASC");
    }
    ActualiserModele(RequeteRechercheFournisseur);
}

void F_AjoutSuppModifFournisseursEditeurs::ActualiserModele(QSqlQuery requete)
{
    //On vide le model
    this->ModelFournisseur->clear();
    //Indique le nombes de colones puis leurs noms
    this->ModelFournisseur->setColumnCount(2);
    this->ModelFournisseur->setHorizontalHeaderItem(0, new QStandardItem("Nom"));
    this->ModelFournisseur->setHorizontalHeaderItem(1, new QStandardItem("Id"));
    //impose une taille aux colones
    ui->TbV_Recherche->setColumnWidth(0,300);
    ui->TbV_Recherche->setColumnWidth(1,0);

    unsigned int NumeroLigne (0);
    //Tant qu'il y a des jeux dans la table jeux,
    while(requete.next())
    {
        //on ajoute une nouvelle ligne du table view
        this->ModelFournisseur->setItem(NumeroLigne, 0, new QStandardItem(requete.value(0).toString() ));
        this->ModelFournisseur->setItem(NumeroLigne, 1, new QStandardItem(requete.value(1).toString() ));
        NumeroLigne++;
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
    qDebug()<< "Le nom recuperé est :" << Selection ;

    // Récupère l'Id du fournisseurediteur séléctionné
    this->IdSelection = (this->ModelFournisseur->index(index.row(), 1).data().toInt());
    qDebug()<< "L'Id recuperé est :" << this->IdSelection ;


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
    RequeteInfoFournisseur.prepare("SELECT IdFournisseur, NomFournisseur, AdresseFournisseur, CPFournisseur,"
             "VilleFournisseur, PersonneContacte, NumTelephone, Email, NumFax, Remarque, SiteWeb,"
             "Pays FROM fournisseursediteur WHERE IdFournisseur =:IdFournisseur") ;
    RequeteInfoFournisseur.bindValue(":IdFournisseur", this->IdSelection);
    if(!RequeteInfoFournisseur.exec())
    {
        qDebug() << "F_AjoutSuppModifFournisseursEditeurs::on_Bt_Ok_clicked()" << RequeteInfoFournisseur.lastQuery();
    }
    RequeteInfoFournisseur.next() ;

    //this->IdSelection = ObtenirValeurParNom(RequeteInfoFournisseur,"IdFournisseur").toInt() ;
    QString NomFournisseur = ObtenirValeurParNom(RequeteInfoFournisseur,"NomFournisseur").toString() ;
    QString Adresse = ObtenirValeurParNom(RequeteInfoFournisseur,"AdresseFournisseur").toString() ;
    QString CP = ObtenirValeurParNom(RequeteInfoFournisseur,"CPFournisseur").toString() ;
    QString VilleFournisseur = ObtenirValeurParNom(RequeteInfoFournisseur,"VilleFournisseur").toString() ;
    QString Contact = ObtenirValeurParNom(RequeteInfoFournisseur,"PersonneContacte").toString() ;
    QString Tel = ObtenirValeurParNom(RequeteInfoFournisseur,"NumTelephone").toString() ;
    QString Email = ObtenirValeurParNom(RequeteInfoFournisseur,"Email").toString() ;
    QString Fax = ObtenirValeurParNom(RequeteInfoFournisseur,"NumFax").toString() ;
    QString Remarque = ObtenirValeurParNom(RequeteInfoFournisseur,"Remarque").toString() ;
    QString SiteWeb = ObtenirValeurParNom(RequeteInfoFournisseur,"SiteWeb").toString() ;

    ui->LE_Contact->setText(Contact);
    ui->LE_Email->setText(Email);
    ui->LE_Fax->setText(Fax);
    ui->LE_Nom->setText(NomFournisseur);
    // SI le nom est vide, on force l'appel de textchanged
    if(NomFournisseur=="")
    {
        this->on_LE_Nom_textChanged("");
    }
    ui->LE_Telephone->setText(Tel);
    ui->LE_Ville->setText(VilleFournisseur);
    ui->LE_CodePostal->setText(CP);
    ui->LE_Rue->setText(Adresse);
    ui->TxE_Remarques->setText(Remarque);
    ui->LE_SiteWeb->setText(SiteWeb);
    ui->LE_Pays->setText(ObtenirValeurParNom(RequeteInfoFournisseur,"Pays").toString());
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
    if(QMessageBox::warning(this, "Confirmation suppression", "Êtes-vous sur de vouloir le supprimer ?", "Oui", "Non") == 0)
    {
        QSqlQuery RequeteSupprimer;
        RequeteSupprimer.prepare("SELECT * FROM  jeux WHERE Fournisseurs_IdFournisseur=:IdFournisseur");
        RequeteSupprimer.bindValue(":IdFournisseur", IdSelection);
        RequeteSupprimer.exec();
        if(RequeteSupprimer.size()!=0)
        {
            RequeteSupprimer.prepare("SELECT NomFournisseur,IdFournisseur FROM fournisseursediteur WHERE "
                              "IdFournisseur!=:IdFournisseur ORDER BY NomFournisseur");
            RequeteSupprimer.bindValue(":IdFournisseur", IdSelection);
            RequeteSupprimer.exec();
            int i=0;
            this->TbCLESTTEM->clear();
            this->TbCLESTTEM->setHorizontalHeaderItem(0, new QStandardItem("Valeurs"));
            while(RequeteSupprimer.next())
            {
                this->TbCLESTTEM->setItem(i, 0, new QStandardItem(RequeteSupprimer.value(0).toString()));
                this->TbCLESTTEM->setItem(i++, 1, new QStandardItem(RequeteSupprimer.value(1).toString()));
            }
            this->TbV_CLESTTEM->setSelectionMode(QAbstractItemView::SingleSelection);
            this->TbV_CLESTTEM->setSelectionBehavior(QAbstractItemView::SelectRows);
            this->TbV_CLESTTEM->resizeColumnsToContents();
            this->TbV_CLESTTEM->setColumnWidth(1,0);
            this->TbV_CLESTTEM->selectRow(0);
            int ret=this->pPopUpCLESTTEM->Modifier(0, 12, this->TbV_CLESTTEM);
            if(ret==0)
            {
                return;
            }
            RequeteSupprimer.prepare("UPDATE jeux SET Fournisseurs_IdFournisseur=:IdFournisseur"
                                   " WHERE Fournisseurs_IdFournisseur=:IdFournisseurPrecedent");
            QString IdSel=this->TbV_CLESTTEM->selectionModel()->selectedRows(1).first().data().toString();
            RequeteSupprimer.bindValue(":IdFournisseur",IdSel);
            RequeteSupprimer.bindValue(":IdFournisseurPrecedent", IdSelection);
            RequeteSupprimer.exec();
        }
        QSqlQuery RequeteSuppFournOuEdit ;
        RequeteSuppFournOuEdit.prepare("DELETE FROM fournisseursediteur WHERE IdFournisseur =:IdFournisseur") ;
        RequeteSuppFournOuEdit.bindValue(":IdFournisseur", IdSelection);
        RequeteSuppFournOuEdit.exec() ;
        RequeteSuppFournOuEdit.next() ;
        ViderChamps();
    }
    else
    {

    }
    on_Rb_Les2_clicked();
}

/**
 * @brief Méthode qui ajoute le fournisseur ou l'éditeur ou qui le modifier selon le mode dans lequel on se trouve
 *
 */
void F_AjoutSuppModifFournisseursEditeurs::on_Bt_Valider_clicked()
{
    // Mode ajout
    if(AjoutOuModif == true)
    {
        if(ui->LE_Nom->text() == "")
        {
            QMessageBox::warning(this, "Pré-requis pour l'ajout", "Vous devez au minimum saisir un nom !", "OK");
        }
        else
        {
            QSqlQuery RequeteRechercheEditeurFournisseur ;
            RequeteRechercheEditeurFournisseur.prepare("SELECT COUNT(*) FROM fournisseursediteur WHERE NomFournisseur LIKE (:NomFournisseur)");
            RequeteRechercheEditeurFournisseur.bindValue(":NomFournisseur",ui->LE_Nom->text());
            RequeteRechercheEditeurFournisseur.exec();
            RequeteRechercheEditeurFournisseur.next();
            // Si le nom du fournisseur/éditeur existe déjà
            if (RequeteRechercheEditeurFournisseur.value(0).toInt() > 0)
            {
               QString type;
               // Personnalise le message suivant le mode
               switch(this->Mode)
               {
                   case MODE_INDEFINI: {
                           type = "de fournisseur ou d'éditeur";
                           break;
                   }
                   case MODE_FOURNISSEUR: {
                           type = "de fournisseur";
                           break;
                   }
                   case MODE_EDITEUR: {
                           type = "d'éditeur";
                           break;
                   }
                   case MODE_EDITEUR_FOURNISSEUR: {
                           type = "de fournisseur ou d'éditeur";
                           break;
                   }
               }
                QMessageBox::critical(this, "Nom " + type + " existant déjà", "Un nom " + type + " existe déjà dans la base de donnée.\nVeuillez modifier l'enregistrement existant.");
                return;
            }

            QSqlQuery RequeteAjoutFournEdit ;
            RequeteAjoutFournEdit.prepare("INSERT INTO fournisseursediteur(NomFournisseur,"
                     "AdresseFournisseur, CPFournisseur, VilleFournisseur, PersonneContacte,"
                     "NumTelephone, Email, NumFax, Pays, Fournisseur, Editeur, Remarque, SiteWeb)"
                     "VALUES (:LeNom, :LaAdresse, :LeCP, :LaVille, :LeContact, :LeTel, :LeEmail,"
                     ":LeFax, :LePays, :LeFournisseur, :LeEditeur, :LaRemarque, :LeSiteWeb)") ;
            RequeteAjoutFournEdit.bindValue(":LeNom", ui->LE_Nom->text());
            RequeteAjoutFournEdit.bindValue(":LaAdresse", ui->LE_Rue->text());
            RequeteAjoutFournEdit.bindValue(":LeCP", ui->LE_CodePostal->text());
            RequeteAjoutFournEdit.bindValue(":LaVille", ui->LE_Ville->text());
            RequeteAjoutFournEdit.bindValue(":LeContact", ui->LE_Contact->text());
            RequeteAjoutFournEdit.bindValue(":LeTel", ui->LE_Telephone->text());
            RequeteAjoutFournEdit.bindValue(":LeEmail", ui->LE_Email->text());
            RequeteAjoutFournEdit.bindValue(":LeFax", ui->LE_Fax->text());
            RequeteAjoutFournEdit.bindValue(":LePays", ui->LE_Pays->text());
            RequeteAjoutFournEdit.bindValue(":LeSiteWeb", ui->LE_SiteWeb->text());
            char choix;
            if( this->Mode == MODE_INDEFINI)
            {
                choix = QMessageBox::information(this, "Qu'est ce ?", "Cet ajout est-il un fournisseur ou un éditeur ?", "Fournisseur", "Editeur","Les deux");
            }
            else
            {
                choix = Mode-1;
            }
            switch( choix )
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
            // Si on est appelé par D_AjoutSuppModifFournisseursEditeurs donc on doit fermer la fenêtre
            if( this->parent()->objectName() == "D_AjoutSuppModifFournisseursEditeurs")
            {
                this->AjoutOuModif = true ;
                // Cache la fenêtre
                this->Parent->hide();
                D_AjoutSuppModifFournisseursEditeurs * pAjoutSuppModifFournisseursEditeurs = (D_AjoutSuppModifFournisseursEditeurs *) this->Parent;
                if( this->Mode == MODE_FOURNISSEUR)
                {
                    // En cas d'ajout, remettre à jour le ComboBox Fournisseur
                    pAjoutSuppModifFournisseursEditeurs->Parent->ActualiserCBx_Fournisseur();
                }
                else
                {
                    // En cas d'ajout, remettre à jour le ComboBox Editeur
                    pAjoutSuppModifFournisseursEditeurs->Parent->ActualiserCBx_Editeur();
                }
            }
        }
    }
    else   // Modification d'un nouveau fournisseur/éditeur
    {

        QSqlQuery RequeteModifFournOuEdit ;

        RequeteModifFournOuEdit.prepare("UPDATE fournisseursediteur SET NomFournisseur=:NomFournisseur,"
                  "AdresseFournisseur =:LaAdresse,"
                  "CPFournisseur =:LeCP, VilleFournisseur =:LaVille, PersonneContacte =:LeContact,"
                  "NumTelephone =:LeNum, Email =:LeEmail, NumFax =:LeFax, Pays =:LePays,"
                  "Remarque =:LaRemarque, SiteWeb =:LeSiteWeb WHERE IdFournisseur =:IdFournisseur") ;
        RequeteModifFournOuEdit.bindValue(":IdFournisseur", this->IdSelection);
        RequeteModifFournOuEdit.bindValue(":NomFournisseur", ui->LE_Nom->text());
        RequeteModifFournOuEdit.bindValue(":LaAdresse", ui->LE_Rue->text());
        RequeteModifFournOuEdit.bindValue(":LeCP", ui->LE_CodePostal->text());
        RequeteModifFournOuEdit.bindValue(":LaVille", ui->LE_Ville->text());
        RequeteModifFournOuEdit.bindValue(":LeContact", ui->LE_Contact->text());
        RequeteModifFournOuEdit.bindValue(":LeNum", ui->LE_Telephone->text());
        RequeteModifFournOuEdit.bindValue(":LeEmail", ui->LE_Email->text());
        RequeteModifFournOuEdit.bindValue(":LeFax", ui->LE_Fax->text());
        RequeteModifFournOuEdit.bindValue(":LePays", ui->LE_Pays->text());
        RequeteModifFournOuEdit.bindValue(":LaRemarque", ui->TxE_Remarques->toPlainText());
        RequeteModifFournOuEdit.bindValue(":LeSiteWeb", ui->LE_SiteWeb->text());

        if(!RequeteModifFournOuEdit.exec())
        {
            qDebug() << "F_AjoutSuppModifFournisseursEditeurs::on_Bt_Valider_clicked()" << RequeteModifFournOuEdit.lastQuery();
        }
        RequeteModifFournOuEdit.next() ;
        QMessageBox::information(this, "Succès", "Fournisseur/éditeur modifié avec succès", "OK") ;
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
    on_Bt_Ok_clicked();
    // Si on est appelé par D_AjoutSuppModifFournisseursEditeurs donc on doit fermer la fenêtre
    if( this->parent()->objectName() == "D_AjoutSuppModifFournisseursEditeurs")
    {
        this->Parent->close();
    }
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

    RequeteRechercheFournisseur.prepare("SELECT  NomFournisseur, IdFournisseur FROM fournisseursediteur WHERE Fournisseur = 1 ORDER BY NomFournisseur ASC");
    RequeteRechercheFournisseur.exec();

    ActualiserModele(RequeteRechercheFournisseur);
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

    RequeteRechercheFournisseur.prepare("SELECT  NomFournisseur, IdFournisseur FROM fournisseursediteur WHERE Editeur = 1 ORDER BY NomFournisseur ASC");
    RequeteRechercheFournisseur.exec();

    ActualiserModele(RequeteRechercheFournisseur);
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

    RequeteRechercheFournisseur.prepare("SELECT NomFournisseur,IdFournisseur FROM fournisseursediteur ORDER BY NomFournisseur ASC");
    RequeteRechercheFournisseur.exec();

    ActualiserModele(RequeteRechercheFournisseur);
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
    ui->LE_Pays->clear();
    ui->TxE_Remarques->clear();
    ui->LE_Recherche->clear();
    ui->LE_SiteWeb->clear();
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

/**
 *  @brief Mets en majuscule le texte saisi dans un QLineEdit
 *
 *  @param text
 */
void F_AjoutSuppModifFournisseursEditeurs::toUpper(const QString &text)
{
    QLineEdit *le = qobject_cast<QLineEdit *>(sender());
    if (!le)
    return;
    le->setText(text.toUpper());
}

/**
 *  @brief Cache la partie recherche de la fenêtre et active les champs fournisseur/editeur
 */
void F_AjoutSuppModifFournisseursEditeurs::AjoutFournisseursEditeursSeulement()
{
    ui->frame->hide();
    ui->Bt_Supprimer->hide();
    on_Bt_Ajouter_clicked();
}
