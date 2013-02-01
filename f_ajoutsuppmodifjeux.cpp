//-------------------------------------------------------------------------------
/** @file         f_ajoutsuppmodifjeux.cpp
 *  @brief        Permet d'ajouter, supprimer ou modifier un jeux
 *
 *  @author       Florian MARY
 *  @author       STS IRIS, Lycée Nicolas APPERT, ORVAULT (FRANCE)
 *  @since        01/01/2012
 *  @version      0.1
 *  @date         01/09/2012 William
 *
 *  Il permet à partir de l'onglet administration de supprimer un jeu, d'en ajouter un ou encore de la modifier.
 *
 *  Fabrication   QtCreator
 *
 *  @todo         Modification d'un jeu
 *
 */
//-------------------------------------------------------------------------------

// En-têtes standards  --------------
#include <qfiledialog.h>
#include <QtSql>
#include <QtGui>
#include <QMessageBox>

// En-tête propre à l'application ----------------------------------------------
#include "f_ajoutsuppmodifjeux.h"
#include "ui_f_ajoutsuppmodifjeux.h"

//###################################################################
/**
 * @brief Constructeur de la classe f_ajoutsuppmodifjeux
 *
 * @param parent
 */
F_AjoutSuppModifJeux::F_AjoutSuppModifJeux(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::F_AjoutSuppModifJeux)
{
    ///////////////////////////////////////
    //////// Constructeur ////////////////
    /////////////////////////////////////
    ui->setupUi(this);

    // Création des fenêtres de création de nouveaux éléments
    this->pAjoutFournisseur= new F_AjouterFournisseur(this) ;
    this->pAjoutEditeur= new F_AjouterEditeur(this) ;
    this->pEtatAjMod = new F_PopUpCLESTTEM(0);
    this->pStatutAjMod = new F_PopUpCLESTTEM(1);
    this->pEmplacementAjMod = new F_PopUpCLESTTEM(2);
    this->pClassificationAjMod = new F_PopUpCLESTTEM(3);
    this->pMotCleAjMod = new F_PopUpCLESTTEM(4);
    
    // sert à quoi ??
    //JeuEnConsultation = "";
    
    //Création d'un model pour le TableView des jeux
    this->ModelJeu = new QStandardItemModel() ;
    //Associe le model au TableView
    ui->TbV_Recherche->setModel(this->ModelJeu);
    // Mise en lecture seule
    ui->TbV_Recherche->setEditTriggers(0);
    // Initialise la table view avec tous les jeux
    this->on_LE_RechercheNom_textChanged("") ;
    //Supprime le numéro des lignes
    ui->TbV_Recherche->verticalHeader()->setVisible(false);

    ui->LE_CodeClassification->setReadOnly(true);
    //Affiche la date du jour
    QDate DateDefaut ;
    DateDefaut = DateDefaut.currentDate() ;
    ui->DtE_Achat->setDate(DateDefaut);

    // Cache le bouton pour renseigner l'état initial
    ui->Bt_Renseigner->setVisible(false);        
    
    //////////////////////////////////////////////
    //////////// Remplir les COMBO BOX //////////
    ////////////////////////////////////////////
    
    ////////////////////////////////////
    //////// Remplir EtatJeu ///////////
    ////////////////////////////////////
    connect(this->pEtatAjMod, SIGNAL(SignalValider()), this, SLOT(slot_ValiderEtat()));
    this->ActualiserCBX_Etat();
    //Cache la fenêtre lors du démarrage : après que le CBx ait été actualisé
    this->pEtatAjMod->hide();

   /////////////////////////////////////////
   /////////Remplir StatutJeu//////////////
   ///////////////////////////////////////
    connect(this->pStatutAjMod, SIGNAL(SignalValider()), this, SLOT(slot_ValiderStatut()));
    this->ActualiserCBx_Statut();
    //Cache la fenêtre lors du démarrage : après que le CBx ait été actualisé
    this->pStatutAjMod->hide();

  ////////////////////////////////////////
  ////////// Fournisseurs ///////////////
  //////////////////////////////////////
    this->ActualiserCBx_Fournisseur() ;
    //Cache la fenêtre lors du démarrage : après que le CBx ait été actualisé
    this->pAjoutFournisseur->hide();

  /////////////////////////////////////////
  /////////// Editeurs ///////////////////
  ///////////////////////////////////////
    this->ActualiserCBx_Editeur() ;
    //Cache la fenêtre lors du démarrage : après que le CBx ait été actualisé
    this->pAjoutEditeur->hide();

  ////////////////////////////////////////////
  ///////// Classification //////////////////
  //////////////////////////////////////////
    connect(this->pClassificationAjMod, SIGNAL(SignalValider()), this, SLOT(slot_ValiderClassification()));
    this->ActualiserCBx_Classification();
    //Cache la fenêtre lors du démarrage : après que le CBx ait été actualisé
    this->pClassificationAjMod->hide();

  ////////////////////////////////////////////
  ///////// Emplacement /////////////////////
  //////////////////////////////////////////
    connect(this->pEmplacementAjMod, SIGNAL(SignalValider()), this, SLOT(slot_ValiderEmplacement()));
    this->ActualiserCBx_Emplacement();
    //Cache la fenêtre lors du démarrage : après que le CBx ait été actualisé
    this->pEmplacementAjMod->hide();
    
  ////////////////////////////////////////////
  /////////// Mot Clés //////////////////////
    ////////////////////////////////////////
    connect(this->pMotCleAjMod, SIGNAL(SignalValider()), this, SLOT(slot_ValiderMotCle()));
    this->ActualiserCBx_MotCle();
    //Cache la fenêtre lors du démarrage : après que le CBx ait été actualisé
    this->pMotCleAjMod->hide();

    //Grise les boutons valider/annuler/supprimer
    ui->Bt_Valider->setEnabled(false);
    ui->Bt_Annuler->setEnabled(false);
    ui->Bt_Supprimer->setEnabled(false);

    ////////////////////////////////////////////////
    ///// Bloque tout avant l'ajout ///////////////
    //////////////////////////////////////////////
    ui->CBx_Classification->setDisabled(true);
    ui->CBx_Editeur->setDisabled(true);
    ui->CBx_Emplacement->setDisabled(true);
    ui->CBx_Etat->setDisabled(true);
    ui->CBx_Fournisseur->setDisabled(true);
    ui->CBx_MotCle1->setDisabled(true);
    ui->CBx_MotCle2->setDisabled(true);
    ui->CBx_MotCle3->setDisabled(true);
    ui->CBx_Statut->setDisabled(true);
    ui->LE_Code->setDisabled(true);
    ui->LE_CodeClassification->setDisabled(true);
    ui->LE_Createur->setDisabled(true);
    ui->LE_Nom->setDisabled(true);
    ui->DtE_Achat->setDisabled(true);
    ui->SBx_AgeMax->setDisabled(true);
    ui->SBx_AgeMin->setDisabled(true);
    ui->SBx_Caution->setDisabled(true);
    ui->SBx_JoueursMax->setDisabled(true);
    ui->SBx_JoueursMin->setDisabled(true);
    ui->SBx_PrixAchat->setDisabled(true);
    ui->SBx_PrixLocation->setDisabled(true);
    ui->TxE_Contenu->setDisabled(true);
    ui->TxE_Description->setDisabled(true);
    ui->TxE_Remarques->setDisabled(true);
    ui->RBt_Neuf->setDisabled(true);
    ui->RBt_Occasion->setDisabled(true);
    ui->Bt_ChargerImage->setDisabled(true);
}
//###################################################################
/**
 * @brief Destructeur de la classe f_ajoutsuppmodifjeux
 *
 */
F_AjoutSuppModifJeux::~F_AjoutSuppModifJeux()
{
    delete ui;
}
//###################################################################
void F_AjoutSuppModifJeux::slot_ValiderClassification()
{
    this->ActualiserCBx_Classification();
}
//###################################################################
void F_AjoutSuppModifJeux::slot_ValiderEmplacement()
{
    this->ActualiserCBx_Emplacement();
    ui->CBx_Statut->setCurrentIndex(ui->CBx_Statut->count() - 2);
}
//###################################################################
void F_AjoutSuppModifJeux::slot_ValiderEtat()
{
    this->ActualiserCBX_Etat();
    ui->CBx_Etat->setCurrentIndex(ui->CBx_Etat->count() - 2);
}
//###################################################################
void F_AjoutSuppModifJeux::slot_ValiderMotCle()
{
    this->ActualiserCBx_MotCle();
    ui->CBx_Statut->setCurrentIndex(ui->CBx_Statut->count() - 2);
}
//###################################################################
void F_AjoutSuppModifJeux::slot_ValiderStatut()
{
    this->ActualiserCBx_Statut();
    ui->CBx_Statut->setCurrentIndex(ui->CBx_Statut->count() - 2);
}

//###################################################################
/**
 * @brief Méthode qui met a à jour le tableau en fonction de ce que l'on rentre dans la case de recherche par nom
 *
 * @param arg1
 */
void F_AjoutSuppModifJeux::on_LE_RechercheNom_textChanged(const QString &arg1)
{
    ui->LE_RechercheCode->setText("");
    ///////////////////////////////////////////////////
    ///////////////////// Recherche par nom //////////
    /////////////////////////////////////////////////
    unsigned int NumeroLigne (0);
    QString NomJeu=arg1;

    if(NomJeu.size()>=2)
    {
        NomJeu="%"+NomJeu+"%";

        QSqlQuery RequeteRechercheJeu;
        NumeroLigne=0;

        RequeteRechercheJeu.prepare("SELECT  `CodeJeu`,`NomJeu`, TypeJeux_Classification FROM`jeux` WHERE `NomJeu` LIKE (:NomJeu)");
        RequeteRechercheJeu.bindValue(":NomJeu",NomJeu);
        RequeteRechercheJeu.exec();

        //On vide le model
        this->ModelJeu->clear();
        //Indique le nombes de colones puis leurs noms
        this->ModelJeu->setColumnCount(3);
        this->ModelJeu->setHorizontalHeaderItem(0, new QStandardItem("Code"));
        this->ModelJeu->setHorizontalHeaderItem(1, new QStandardItem("Nom"));
        this->ModelJeu->setHorizontalHeaderItem(2, new QStandardItem("Classification"));
        //impose une taille aux colones
        ui->TbV_Recherche->setColumnWidth(0,45);
        ui->TbV_Recherche->setColumnWidth(1,160);
        ui->TbV_Recherche->setColumnWidth(2, 50);

        //Tant qu'il y a des jeux dans la table jeux,
        while(RequeteRechercheJeu.next())
        {
            //on ajoute une nouvelle ligne du table view
            this->ModelJeu->setItem(NumeroLigne, 0, new QStandardItem(RequeteRechercheJeu.value(0).toString() ));
            this->ModelJeu->setItem(NumeroLigne, 1, new QStandardItem(RequeteRechercheJeu.value(1).toString() ));
            this->ModelJeu->setItem(NumeroLigne, 2, new QStandardItem(RequeteRechercheJeu.value(2).toString() ));
            NumeroLigne++;
         }
    }
    else
    {
        QSqlQuery RequeteRechercheJeu;
        NumeroLigne =0;
        RequeteRechercheJeu.exec("SELECT  `CodeJeu`,`NomJeu`, TypeJeux_Classification FROM`jeux`");

        //On vide le model
        this->ModelJeu->clear();
        //Indique le nombes de colones puis leurs noms
        this->ModelJeu->setColumnCount(3);
        this->ModelJeu->setHorizontalHeaderItem(0, new QStandardItem("Code"));
        this->ModelJeu->setHorizontalHeaderItem(1, new QStandardItem("Nom"));
        this->ModelJeu->setHorizontalHeaderItem(2, new QStandardItem("Classification"));
        //impose une taille aux colones
        ui->TbV_Recherche->setColumnWidth(0,45);
        ui->TbV_Recherche->setColumnWidth(1,130);
        ui->TbV_Recherche->setColumnWidth(2,50);        

        //Tant qu'il y a des jeux dans la table jeux,
        while(RequeteRechercheJeu.next())
        {
            //on ajoute une nouvelle ligne du table view
            this->ModelJeu->setItem(NumeroLigne, 0, new QStandardItem(RequeteRechercheJeu.value(0).toString() ));
            this->ModelJeu->setItem(NumeroLigne, 1, new QStandardItem(RequeteRechercheJeu.value(1).toString() ));
            this->ModelJeu->setItem(NumeroLigne, 2, new QStandardItem(RequeteRechercheJeu.value(2).toString() ));
            NumeroLigne++;
         }
    }
}
//###################################################################
/**
 * @brief Méthode qui récupère le code du jeu sur lequel on a cliqué dans le tableau
 *
 * @param index
 */
void F_AjoutSuppModifJeux::on_TbV_Recherche_clicked(const QModelIndex &index)
{
    // Remplir le code du jeu recherché avec le jeu cliqué dans le tableau
    ui->LE_RechercheCode->setText(this->ModelJeu->index(index.row(), 0).data().toString());
    on_Bt_OK_clicked() ;
    // Griser Valider et Annuler/ Autoriser Ajout et suppression
    ui->Bt_Valider->setEnabled(false);
    ui->Bt_Annuler->setEnabled(false);
    ui->Bt_Supprimer->setEnabled(true);
    ui->Bt_Ajouter->setEnabled(true);
}
//###################################################################
/**
 * @brief Méthode qui appel la fontion Bt_OK_Clicked lorsque l'on appui sur la touche entrée du clavier
 *
 */
void F_AjoutSuppModifJeux::on_LE_RechercheCode_returnPressed()
{
    on_Bt_OK_clicked() ;
}
//###################################################################
/**
 * @brief Méthode qui permet de vider la case de recherche par nom lorsqu'on entre un code
 *
 * @param arg1
 */
void F_AjoutSuppModifJeux::on_LE_RechercheCode_textChanged(const QString &arg1)
{
    //ui->LE_RechercheNom->setText("");
}

//###################################################################
/**
 * @brief Méthode qui permet d'activer les boutons valider et annuler lorsque le contenu de la case Nom change
 *
 * @param arg1
 */
void F_AjoutSuppModifJeux::on_LE_Nom_textChanged(const QString &arg1)
{
    if(ui->LE_RechercheCode->text() != "" || ui->LE_RechercheNom->text() != "")
    {
        this->CacherBoutons();
    }
}

//###################################################################
/**
 * @brief Méthode qui active les boutons valider et annuler lorsque le contenu du SBx_JoueursMin change
 *
 * @param arg1
 */
void F_AjoutSuppModifJeux::on_SBx_JoueursMin_valueChanged(int arg1)
{
    if(ui->LE_RechercheCode->text() != "" || ui->LE_RechercheNom->text() != "")
    {
        this->CacherBoutons();
    }
}
//###################################################################
/**
 * @brief Méthode qui active les boutons valider et annuler lorsque le contenu du SBx_JoueursMax change
 *
 * @param arg1
 */
void F_AjoutSuppModifJeux::on_SBx_JoueursMax_valueChanged(int arg1)
{
    if(ui->LE_RechercheCode->text() != "" || ui->LE_RechercheNom->text() != "")
    {
       this->CacherBoutons();
    }
}
//###################################################################
/**
 * @brief Méthode qui active les boutons valider et annuler lorsque le contenu du SBx_AgeMin change
 *
 * @param arg1
 */
void F_AjoutSuppModifJeux::on_SBx_AgeMin_valueChanged(int arg1)
{
    if(ui->LE_RechercheCode->text() != "" || ui->LE_RechercheNom->text() != "")
    {
        this->CacherBoutons();
    }
}
//###################################################################
/**
 * @brief Méthode qui active les boutons valider et annuler lorsque le contenu du SBx_AgeMax change
 *
 * @param arg1
 */
void F_AjoutSuppModifJeux::on_SBx_AgeMax_valueChanged(int arg1)
{
    if(ui->LE_RechercheCode->text() != "" || ui->LE_RechercheNom->text() != "")
    {
        this->CacherBoutons();
    }
}

//###################################################################
/**
 * @brief Méthode qui active les boutons valider et annuler lorsque le contenu de la case créateur est modifié
 *
 * @param arg1
 */
void F_AjoutSuppModifJeux::on_LE_Createur_textChanged(const QString &arg1)
{
    if(ui->LE_RechercheCode->text() != "" || ui->LE_RechercheNom->text() != "")
    {
        this->CacherBoutons();
    }
}
//###################################################################
/**
 * @brief Méthode qui active les boutons valider et annuler lorsque le contenu de la case date achat est modifié
 *
 * @param date
 */
void F_AjoutSuppModifJeux::on_DtE_Achat_dateChanged(const QDate &date)
{
    if(ui->LE_RechercheCode->text() != "" || ui->LE_RechercheNom->text() != "")
    {
        this->CacherBoutons();
    }
}
//###################################################################
/**
 * @brief Méthode qui active les boutons valider et annuler lorsque le contenu de la case prix achat est modifié
 *
 * @param arg1
 */
void F_AjoutSuppModifJeux::on_SBx_PrixAchat_valueChanged(double arg1)
{
    if(ui->LE_RechercheCode->text() != "" || ui->LE_RechercheNom->text() != "")
    {
        this->CacherBoutons();
    }
}
//###################################################################
/**
 * @brief Méthode qui active les boutons valider et annuler lorsque le contenu de la case prix location est modifié
 *
 * @param arg1
 */
void F_AjoutSuppModifJeux::on_SBx_PrixLocation_valueChanged(int arg1)
{
    if(ui->LE_RechercheCode->text() != "" || ui->LE_RechercheNom->text() != "")
    {
        this->CacherBoutons();
    }
}
//###################################################################
/**
 * @brief Méthode qui active les boutons valider et annuler lorsque le contenu de la case caution est modifié
 *
 * @param arg1
 */
void F_AjoutSuppModifJeux::on_SBx_Caution_valueChanged(int arg1)
{
    if(ui->LE_RechercheCode->text() != "" || ui->LE_RechercheNom->text() != "")
    {
        this->CacherBoutons();
    }
}
//###################################################################
/**
 * @brief Méthode qui active les boutons valider et annuler lorsque le contenu de la case remarque est modifié
 *
 */
void F_AjoutSuppModifJeux::on_TxE_Remarques_textChanged()
{
    if(ui->LE_RechercheCode->text() != "" || ui->LE_RechercheNom->text() != "")
    {
       this->CacherBoutons();
    }
}
//###################################################################
/**
 * @brief Méthode qui active les boutons valider et annuler lorsque le contenu de la case description est modifié
 *
 */
void F_AjoutSuppModifJeux::on_TxE_Description_textChanged()
{
    if(ui->LE_RechercheCode->text() != "" || ui->LE_RechercheNom->text() != "")
    {
       this->CacherBoutons();
    }
}
//###################################################################
/**
 * @brief Méthode qui active les boutons valider et annuler lorsque le contenu de la case contenu est modifié
 *
 */
void F_AjoutSuppModifJeux::on_TxE_Contenu_textChanged()
{
    if(ui->LE_RechercheCode->text() != "" || ui->LE_RechercheNom->text() != "")
    {
        this->CacherBoutons();
    }
}
//###################################################################
/**
 * @brief Méthode qui active les boutons valider et annuler lorsque l'état du bouton neuf est modifié
 *
 * @param checked
 */
void F_AjoutSuppModifJeux::on_RBt_Neuf_toggled(bool checked)
{
    if(checked == false)
    {
        if(ui->LE_RechercheCode->text() != "" || ui->LE_RechercheNom->text() != "")
        {
            this->CacherBoutons();
        }
    }
}
//###################################################################
/**
 * @brief Méthode qui active les boutons valider et annuler lorsque l'état du bouton occasion est modifié
 *
 * @param checked
 */
void F_AjoutSuppModifJeux::on_RBt_Occasion_toggled(bool checked)
{
    if(checked == false)
    {
        if(ui->LE_RechercheCode->text() != "" || ui->LE_RechercheNom->text() != "")
        {
            this->CacherBoutons();
        }
    }
}
//###################################################################
/**
 * @brief Méthode qui lance la fenêtre d'ajout d'un emplacement
 *
 * @param index
 */
void F_AjoutSuppModifJeux::on_CBx_Emplacement_activated(int index)
{
    //Si l'élément sélectionné est le dernier : ajouter un emplacement
    if(index == ui->CBx_Emplacement->count() -1)
    {
        this->pEmplacementAjMod->Ajouter();
        //this->ActualiserCBx_Emplacement();
    }
    else
    {
        if(ui->LE_RechercheCode->text() != "" || ui->LE_RechercheNom->text() != "")
        {
            this->CacherBoutons();
        }
    }
}
//###################################################################
/**
 * @brief Méthode qui lance la fenêtre d'ajout d'un fournisseur
 *
 * @param index
 */
void F_AjoutSuppModifJeux::on_CBx_Fournisseur_activated(int index)
{
    //Si l'élément séléctionné est le dernier : afficher la fenêtre pour ajouter un fournisseur
    if(index == ui->CBx_Fournisseur->count() -1)
    {
        this->pAjoutFournisseur->show();
        // En cas d'ajout, remettre à jour le ComboBox
        //this->ActualiserCBx_Fournisseur();
    }
    else
    {
        if(ui->LE_RechercheCode->text() != "" || ui->LE_RechercheNom->text() != "")
        {
           this->CacherBoutons();
        }
    }
}
//###################################################################
/**
 * @brief Méthode qui lance la fenêtre d'ajout d'un éditeur
 *
 * @param index
 */
void F_AjoutSuppModifJeux::on_CBx_Editeur_activated(int index)
{
    //Si l'élément séléctionné est le dernier : afficher la fenêter pour ajouter un éditeur
    if(index == ui->CBx_Editeur->count() -1)
    {
        this->pAjoutEditeur->show();
        // En cas d'ajout, remettre à jour le ComboBox
        //this->ActualiserCBx_Editeur();
    }
    else
    {
        if(ui->LE_RechercheCode->text() != "" || ui->LE_RechercheNom->text() != "")
        {
            this->CacherBoutons();
        }
    }
}
//###################################################################
/**
 * @brief Méthode qui lance la fenêtre d'ajout d'un statut
 *
 * @param index
 */
void F_AjoutSuppModifJeux::on_CBx_Statut_activated(int index)
{
    //Si l'élément séléctionné est le dernier : ajouter un statut
    if(index == ui->CBx_Statut->count() -1)
    {
        this->pStatutAjMod->Ajouter();
    }
    else
    {
        if(ui->LE_RechercheCode->text() != "" || ui->LE_RechercheNom->text() != "")
        {
           this->CacherBoutons();
        }
    }
}
//###################################################################
/**
 * @brief Méthode qui lance la fenêtre d'ajout d'une classification
 *
 * @param index
 */
void F_AjoutSuppModifJeux::on_CBx_Classification_activated(int index)
{
    if(index != 0)
    {
        //Récupère le code placé devant les : dans le CBx
        QString CodeClass = ui->CBx_Classification->currentText().left(ui->CBx_Classification->currentText().indexOf(":")) ;
        ui->LE_CodeClassification->setText(CodeClass);
    }
    else
    {

        ui->LE_CodeClassification->clear();
    }

    //Si l'élément séléctionné est le dernier : ajouter un emplacement
    if(index == ui->CBx_Classification->count() -1)
    {
        this->pClassificationAjMod->Ajouter();
    }
    else
    {
        if(ui->LE_RechercheCode->text() != "" || ui->LE_RechercheNom->text() != "")
        {
            this->CacherBoutons();
        }
    }
}
//###################################################################
/**
 * @brief Méthode qui lance la fenêtre d'ajout d'un état
 *
 * @param index
 */
void F_AjoutSuppModifJeux::on_CBx_Etat_activated(int index)
{
    //Si l'élément séléctionné est le dernier : ajouter un emplacement
    if(index == ui->CBx_Etat->count() -1)
    {
        this->pEtatAjMod->Ajouter();
    }
    else
    {
        if(ui->LE_RechercheCode->text() != "" || ui->LE_RechercheNom->text() != "")
        {
            this->CacherBoutons();
        }
    }
}
//###################################################################
/**
 * @brief Méthode qui lance la fenêtre d'ajout d'un mot clé (1)
 *
 * @param index
 */
void F_AjoutSuppModifJeux::on_CBx_MotCle1_activated(int index)
{
    //Si l'élément séléctionné est le dernier : ajouter un mot clé
    if(index == ui->CBx_MotCle1->count() -1)
    {
        this->pMotCleAjMod->Ajouter();
    }
    else
    {
        if(ui->LE_RechercheCode->text() != "" || ui->LE_RechercheNom->text() != "")
        {
            this->CacherBoutons();
        }
    }
}
//###################################################################
/**
 * @brief Méthode qui lance la fenêtre d'ajout d'un mot clé (2)
 *
 * @param index
 */
void F_AjoutSuppModifJeux::on_CBx_MotCle2_activated(int index)
{
    //Si l'élément séléctionné est le dernier : ajouter un mot clé
    if(index == ui->CBx_MotCle2->count() -1)
    {
        this->pMotCleAjMod->Ajouter();
    }
    else
    {
        if(ui->LE_RechercheCode->text() != "" || ui->LE_RechercheNom->text() != "")
        {
            this->CacherBoutons();
        }
    }
}
//###################################################################
/**
 * @brief Méthode qui lance la fenêtre d'ajout d'un mot clé (3)
 *
 * @param index
 */
void F_AjoutSuppModifJeux::on_CBx_MotCle3_activated(int index)
{
    //Si l'élément séléctionné est le dernier : ajouter un mot clé
    if(index == ui->CBx_MotCle3->count() -1)
    {
        this->pMotCleAjMod->Ajouter();
    }
    else
    {
        if(ui->LE_RechercheCode->text() != "" || ui->LE_RechercheNom->text() != "")
        {
           this->CacherBoutons();
        }
    }
}

//###################################################################
/**
 * @brief Méthode qui ajoute ou modifie un jeux selon le mode dans lequel on se trouve
 *
 */
void F_AjoutSuppModifJeux::on_Bt_Valider_clicked()
{
    // CREATION D'UN NOUVEAU JEU
    if(AjoutOuModif == true)
    {
        // Si pas de nom de jeu ou pas de code de jeu
        if(ui->LE_Nom->text() == "" || ui->LE_Code->text() == "")
        {
            QMessageBox::warning(this, "Pré-requis pour l'ajout", "Vous devez saisir au minimum un nom de jeu et un code avant de valider !", "OK") ;
        }
        else
        {   // Création du nouveau jeu
            QSqlQuery RequeteAjouterJeu ;
        
            if((QMessageBox::information(this, "Confirmation de l'ajout","Voulez-vous vraiment créer ce nouveau jeu "+ui->LE_Nom->text()+" ?","Oui", "Non")) == 0 )
            {
                //////////////////////////////////////////////////////////////////////////////////////////
                // Requete pour faire le lien avec typejeux et récupérer le'id de la classication
                QSqlQuery RequeteClassification;
                RequeteClassification.prepare("SELECT Classification FROM typejeux WHERE Classification =:CodeClassification ");
                RequeteClassification.bindValue(":CodeClassification",ui->LE_CodeClassification->text());
                if(!RequeteClassification.exec())
                {
                    qDebug()<<"F_AjoutSuppModifJeux::on_Bt_Ajouter_clicked()RequeteClassification "<<RequeteClassification.lastError() ;
                }
                RequeteClassification.next();                
                /////////////////////////////////////////////////////////////////////////////////////////
                // Requete pour faire le lien avec statutjeux et récupérer le'id du statut
                QSqlQuery RequeteStatut ;
                RequeteStatut.prepare("SELECT IdStatutJeux FROM statutjeux WHERE StatutJeu =:StatutDuJeu") ;
                RequeteStatut.bindValue(":StatutDuJeu", ui->CBx_Statut->currentText());
                if(!RequeteStatut.exec())
                {
                    qDebug() << "F_AjoutSuppModifJeux::on_Bt_Ajouter_clicked() : RequeteStatut" << RequeteStatut.lastError() ;
                }
                RequeteStatut.next() ;                
                /////////////////////////////////////////////////////////////////////////////////////////
                // Requete pour faire le lien avec mots cles 1 et récupérer l'id du mot clé
                QSqlQuery RequeteMotCle1 ;
                RequeteMotCle1.prepare("SELECT Id_MotCle FROM motscles WHERE MotCle =:LeMotCle") ;
                RequeteMotCle1.bindValue(":LeMotCle", ui->CBx_MotCle1->currentText());
                if(!RequeteMotCle1.exec())
                {
                    qDebug() << "F_AjoutSuppModifJeux::on_Bt_Ajouter_clicked() : RequeteMotCle1" << RequeteMotCle1.lastError() ;                    
                }
                RequeteMotCle1.next() ;
                /////////////////////////////////////////////////////////////////////////////////////////
                // Requete pour faire le lien avec mots cles 2 et récupérer l'id du mot clé
                QSqlQuery RequeteMotCle2 ;
                RequeteMotCle2.prepare("SELECT Id_MotCle FROM motscles WHERE MotCle =:LeMotCle") ;
                RequeteMotCle2.bindValue(":LeMotCle", ui->CBx_MotCle2->currentText());
                if(!RequeteMotCle2.exec())
                {
                    qDebug() << "F_AjoutSuppModifJeux::on_Bt_Ajouter_clicked() : RequeteMotCle1" << RequeteMotCle2.lastError() ;                    
                }
                RequeteMotCle2.next() ;
                /////////////////////////////////////////////////////////////////////////////////////////
                // Requete pour faire le lien avec mots cles 3 et récupérer l'id du mot clé
                QSqlQuery RequeteMotCle3 ;
                RequeteMotCle3.prepare("SELECT Id_MotCle FROM motscles WHERE MotCle =:LeMotCle") ;
                RequeteMotCle3.bindValue(":LeMotCle", ui->CBx_MotCle3->currentText());
                if(!RequeteMotCle3.exec())
                {
                    qDebug() << "F_AjoutSuppModifJeux::on_Bt_Ajouter_clicked() : RequeteMotCle1" << RequeteMotCle3.lastError() ;                    
                }
                RequeteMotCle3.next() ;                
                //////////////////////////////////////////////////////////////////////////////////////////
                // Requete pour faire le lien avec etatjeux et récupérer l'id de l'état
                QSqlQuery RequeteEtat ;
                RequeteEtat.prepare("SELECT IdEtatsJeu FROM etatsjeu WHERE Etat = :EtatsDuJeu") ;
                RequeteEtat.bindValue(":EtatsDuJeu", ui->CBx_Etat->currentText());
                if(!RequeteEtat.exec())
                {
                    qDebug()<<"F_AjoutSuppModifJeux::on_Bt_Ajouter_clicked()RequeteEtat " << RequeteEtat.lastError() ;                    
                }
                RequeteEtat.next() ;                
                ///////////////////////////////////////////////////////////////////////////////////////////
                // Requete pour faire le lien avec emplacement et récupérer l'id de l'emplacement
                QSqlQuery RequeteEmplacement ;
                RequeteEmplacement.prepare("SELECT IdEmplacement FROM emplacement WHERE Nom =:DeEmplacement") ;
                RequeteEmplacement.bindValue(":DeEmplacement", ui->CBx_Emplacement->currentText());
                if(!RequeteEmplacement.exec())
                {
                    qDebug()<<"F_AjoutSuppModifJeux::on_Bt_Ajouter_clicked()RequeteEmplacement " << RequeteEmplacement.lastError() ;                    
                }
                RequeteEmplacement.next() ;                
                ///////////////////////////////////////////////////////////////////////////////////////////
                // Requete pour faire le lien avec fournisseurs/editeur et récupérer l'id du fournisseur
                QSqlQuery RequeteFournisseur ;
                RequeteFournisseur.prepare("SELECT IdFournisseur FROM fournisseursediteur WHERE NomFournisseur = :DuFournisseur AND Fournisseur = 1") ;
                RequeteFournisseur.bindValue(":DuFournisseur", ui->CBx_Fournisseur->currentText());
                if(!RequeteFournisseur.exec())
                {
                    qDebug()<<"F_AjoutSuppModifJeux::on_Bt_Ajouter_clicked()RequeteFournisseur " << RequeteFournisseur.lastError() ;
                    
                }
                RequeteFournisseur.next() ;                
                //////////////////////////////////////////////////////////////////////////////////////////
                // Requete pour faire le lien avec fournisseursediteur et récupérer le'id de l'éditeur
                QSqlQuery RequeteEditeur ;
                RequeteEditeur.prepare("SELECT IdFournisseur FROM fournisseursediteur WHERE NomFournisseur = :LeEditeur AND Editeur = 1") ;
                RequeteEditeur.bindValue(":LeEditeur", ui->CBx_Editeur->currentText());
                qDebug()<<"F_AjoutSuppModifJeux::on_Bt_Ajouter_clicked() :LeEditeur =" << ui->CBx_Editeur->currentText();
                if(!RequeteEditeur.exec())
                {
                    qDebug()<<"F_AjoutSuppModifJeux::on_Bt_Ajouter_clicked()RequeteEditeur " << RequeteEditeur.lastError() ;
                    
                }
                RequeteEditeur.next() ;

                RequeteAjouterJeu.prepare("INSERT INTO jeux(NomJeu, NomCreateurJeu, DateAchat, PrixAchat, PrixLoc, NbrJoueurMin, NbrJoueurMax, AgeMin, AgeMax, Remarque, ContenuJeu, Caution, CheminReglesJeu, CheminPhotoJeu, DescriptionJeu, CodeJeu, EtatInitial, MotCle1, MotCle2, MotCle3, TypeJeux_Classification, StatutJeux_IdStatutJeux, EtatsJeu_IdEtatsJeu, Emplacement_IdEmplacement, Fournisseurs_IdFournisseur, Editeur_IdEditeur) VALUES (:NomDuJeu, :NomDuCreateurJeu, :DateDeAchat, :PrixDeAchat, :PrixDeLoc, :NbrDeJoueurMin, :NbrDeJoueurMax, :LeAgeMin, :LeAgeMax, :LaRemarque, :ContenuDuJeu, :LaCaution, :CheminReglesDuJeu, :CheminPhotoDuJeu, :DescriptionDuJeu, :CodeDuJeu, :LeEtatInitial, :LeMotCle1, :LeMotCle2, :LeMotCle3, :LeTypeJeux_Classification, :StatutDuJeux_IdStatutJeux, :EtatsDuJeu_IdEtatsJeu, :Emplacement_IdDeEmplacement, :LeFournisseurs_IdFournisseur, :LeEditeur_IdEditeur)" ) ;
                RequeteAjouterJeu.bindValue(":NomDuJeu", this->ui->LE_Nom->text());
                RequeteAjouterJeu.bindValue(":NomDuCreateurJeu", this->ui->LE_Createur->text());
                RequeteAjouterJeu.bindValue(":DateDeAchat", this->ui->DtE_Achat->date());
                RequeteAjouterJeu.bindValue(":PrixDeAchat", this->ui->SBx_PrixAchat->value() );
                RequeteAjouterJeu.bindValue(":PrixDeLoc", this->ui->SBx_PrixLocation->value());
                RequeteAjouterJeu.bindValue(":NbrDeJoueurMin", this->ui->SBx_JoueursMin->value());
                RequeteAjouterJeu.bindValue(":NbrDeJoueurMax", this->ui->SBx_JoueursMax->value());
                RequeteAjouterJeu.bindValue(":LeAgeMin", this->ui->SBx_AgeMin->value());
                RequeteAjouterJeu.bindValue(":LeAgeMax", this->ui->SBx_AgeMax->value());
                RequeteAjouterJeu.bindValue(":LaRemarque", this->ui->TxE_Remarques->toPlainText());
                RequeteAjouterJeu.bindValue(":ContenuDuJeu", this->ui->TxE_Contenu->toPlainText());
                RequeteAjouterJeu.bindValue(":LaCaution", this->ui->SBx_Caution->value());
                RequeteAjouterJeu.bindValue(":DescriptionDuJeu", this->ui->TxE_Description->toPlainText());
                RequeteAjouterJeu.bindValue(":CodeDuJeu", this->ui->LE_Code->text());
                RequeteAjouterJeu.bindValue(":CheminPhotoDuJeu", this->CheminPhoto);

                if(ui->RBt_Neuf->isChecked())
                {
                    RequeteAjouterJeu.bindValue(":LeEtatInitial", "Neuf");
                }
                else
                {
                    RequeteAjouterJeu.bindValue(":LeEtatInitial", "Occasion");
                }
                
                RequeteAjouterJeu.bindValue(":LeTypeJeux_Classification", RequeteClassification.value(0).toString());
                RequeteAjouterJeu.bindValue(":StatutDuJeux_IdStatutJeux", RequeteStatut.value(0).toString() );
                RequeteAjouterJeu.bindValue(":EtatsDuJeu_IdEtatsJeu", RequeteEtat.value(0).toString() );
                RequeteAjouterJeu.bindValue(":Emplacement_IdDeEmplacement", RequeteEmplacement.value(0));
                RequeteAjouterJeu.bindValue(":LeFournisseurs_IdFournisseur", RequeteFournisseur.value(0).toString());
                RequeteAjouterJeu.bindValue(":LeEditeur_IdEditeur", RequeteEditeur.value(0).toString() );
                RequeteAjouterJeu.bindValue(":LeMotCle1", RequeteMotCle1.value(0).toString() );
                RequeteAjouterJeu.bindValue(":LeMotCle2", RequeteMotCle2.value(0).toString() );
                RequeteAjouterJeu.bindValue(":LeMotCle3", RequeteMotCle3.value(0).toString() );
                
                if(!RequeteAjouterJeu.exec())
                {
                     qDebug() << "F_AjoutSuppModifJeux::on_Bt_Valider_clicked()-"<<RequeteAjouterJeu.lastError();
                }
                RequeteAjouterJeu.next() ;
            }
            // Vide tous les champs après l'ajout
            ui->LE_Code->clear();
            ui->LE_CodeClassification->clear();
            ui->LE_Createur->clear();
            ui->LE_Nom->clear();
            ui->TxE_Contenu->clear();
            ui->TxE_Description->clear();
            ui->TxE_Remarques->clear();
            ui->SBx_AgeMax->setValue(0);
            ui->SBx_AgeMin->setValue(0);
            ui->SBx_Caution->setValue(0);
            ui->SBx_JoueursMax->setValue(0);
            ui->SBx_JoueursMin->setValue(0);
            ui->SBx_PrixAchat->setValue(0);
            ui->SBx_PrixLocation->setValue(0);
            ui->CBx_Classification->setCurrentIndex(0);
            ui->CBx_Editeur->setCurrentIndex(0);
            ui->CBx_Emplacement->setCurrentIndex(0);
            ui->CBx_Etat->setCurrentIndex(0);
            ui->CBx_Fournisseur->setCurrentIndex(0);
            ui->CBx_Statut->setCurrentIndex(0);
            ui->CBx_MotCle1->setCurrentIndex(0);
            ui->CBx_MotCle2->setCurrentIndex(0);
            ui->CBx_MotCle3->setCurrentIndex(0);

            // Vider la saisie de la recherche du nom
            ui->LE_RechercheNom->setText("");

            QDate DateDefaut ;
            DateDefaut = DateDefaut.currentDate() ;
            ui->DtE_Achat->setDate(DateDefaut);
            
            // Plus de modif en cours donc :
            // griser les boutons Annuler et Valider
            ui->Bt_Annuler->setEnabled(false);
            ui->Bt_Valider->setEnabled(false);
            // Autoriser la création d'un nouveau jeux
            ui->Bt_Ajouter->setEnabled(true);
        }        
    }
    else  // MISE A JOUR D'UN JEU EXISTANT
    {
        if(AjoutOuModif == false)
        {
        /////// Maj Photo ///////////////////--------------------------------------------------------------
            QSqlQuery RequeteModifCheminPhoto ;
            //prépare le requête de mise à jour
            RequeteModifCheminPhoto.prepare("UPDATE jeux SET CheminPhotoJeu =:CheminPhotoDuJeu WHERE CodeJeu=:CodeDuJeu");
            //Entre les valeurs de la requête
            RequeteModifCheminPhoto.bindValue(":CodeDuJeu",ui->LE_Code->text());
            RequeteModifCheminPhoto.bindValue(":CheminPhotoDuJeu", this->CheminPhoto);
             //Exécute la requête
            if (!RequeteModifCheminPhoto.exec())
            {
                qDebug() << "F_AjoutSuppModifJeux::on_Bt_Valider_clicked() : MajPhoto" << RequeteModifCheminPhoto.lastError() ;
            }            
        ////////////// MAJ du Nbr de joueurs /////////------------------------------------------------------------
            QSqlQuery RequeteModifNbrJoueurs ;        
            //prépare le requête de mise à jour
            RequeteModifNbrJoueurs.prepare("UPDATE jeux SET NbrJoueurMin =:NbrDeJoueurMin, NbrJoueurMax =:NbrDeJoueurMax WHERE CodeJeu=:CodeDuJeu");
            //Entre les valeurs de la requête
            RequeteModifNbrJoueurs.bindValue(":CodeDuJeu",ui->LE_Code->text());
            RequeteModifNbrJoueurs.bindValue(":NbrDeJoueurMin", ui->SBx_JoueursMin->value());
            RequeteModifNbrJoueurs.bindValue(":NbrDeJoueurMax", ui->SBx_JoueursMax->value());
            //Exécutee la requête
            if (!RequeteModifNbrJoueurs.exec())
            {
                qDebug() << "F_AjoutSuppModifJeux::on_Bt_Valider_clicked() : MajNbrJoueur" << RequeteModifNbrJoueurs.lastError() ;
            }
        //////////////// MAJ de l'âge //////////////////-------------------------------------------------------------------
            QSqlQuery RequeteModifAge ;        
            //prépare le requête de mise à jour
            RequeteModifAge.prepare("UPDATE jeux SET AgeMin =:LAgeMin, AgeMax =:LAgeMax WHERE CodeJeu=:CodeDuJeu");        
            //Entre les valeurs de la requête
            RequeteModifAge.bindValue(":CodeDuJeu",ui->LE_Code->text());
            RequeteModifAge.bindValue(":LAgeMin", ui->SBx_AgeMin->value());
            RequeteModifAge.bindValue(":LAgeMax", ui->SBx_AgeMax->value());        
            //Exécute la requête
            if (!RequeteModifAge.exec())
            {
                qDebug() << "F_AjoutSuppModifJeux::on_Bt_Valider_clicked() : MajAge" << RequeteModifAge.lastError() ;
            }
        //////////////// MAJ des prix ////////////////// ---------------------------------------------------------------
            QSqlQuery RequeteModifPrix ;
            //prépare le requête de mise à jour
            RequeteModifPrix.prepare("UPDATE jeux SET PrixAchat =:LePrixAchat, PrixLoc =:LePrixLoc, Caution =:LaCaution WHERE CodeJeu=:CodeDuJeu");        
            //Entre les valeurs de la requête
            RequeteModifPrix.bindValue(":CodeDuJeu",ui->LE_Code->text());
            RequeteModifPrix.bindValue(":LePrixAchat", ui->SBx_PrixAchat->value());
            RequeteModifPrix.bindValue(":LePrixLoc", ui->SBx_PrixLocation->value());
            RequeteModifPrix.bindValue(":LaCaution", ui->SBx_Caution->value());        
            //Exécute la requête
            if (!RequeteModifPrix.exec())
            {
                qDebug() << "F_AjoutSuppModifJeux::on_Bt_Valider_clicked() : MajPrix" << RequeteModifPrix.lastError() ;
            }        
        ////////////// MAJ de la date achat //////////----------------------------------------------------------
            QSqlQuery RequeteModifDateAchat ;
            //prépare le requête de mise à jour
            RequeteModifDateAchat.prepare("UPDATE jeux SET DateAchat =:DateDeAchat WHERE CodeJeu=:CodeDuJeu");        
            //Entre les valeurs de la requête
            RequeteModifDateAchat.bindValue(":CodeDuJeu",ui->LE_Code->text());
            RequeteModifDateAchat.bindValue(":DateDeAchat", ui->DtE_Achat->date());        
            //Exécute la requête
            if (!RequeteModifDateAchat.exec())
            {
                qDebug() << "F_AjoutSuppModifJeux::on_Bt_Valider_clicked() : MajDateAchat" << RequeteModifDateAchat.lastError() ;
            }
        ///////// MAJ du créateur ///////////////--------------------------------------------------------
            QSqlQuery RequeteModifCreateur ;        
            //prépare le requête de mise à jour
            RequeteModifCreateur.prepare("UPDATE jeux SET NomCreateurJeu =:NomCreateurDuJeu WHERE CodeJeu=:CodeDuJeu");        
            //Entre les valeurs de la requête
            RequeteModifCreateur.bindValue(":CodeDuJeu",ui->LE_Code->text());
            RequeteModifCreateur.bindValue(":NomCreateurDuJeu", ui->LE_Createur->text());        
            //Exécute la requête
            if (!RequeteModifCreateur.exec())
            {
                qDebug() << "F_AjoutSuppModifJeux::on_Bt_Valider_clicked() : MajCréateur" << RequeteModifCreateur.lastError() ;
            }        
        ///////// MAJ des Textes Edit (contenu,description,remarque du jeu ///////----------------------------
            QSqlQuery RequeteModifTxE ;
            //prépare le requête de mise à jour
            RequeteModifTxE.prepare("UPDATE jeux SET DescriptionJeu =:DescriptionDuJeu, Remarque =:RemarqueDuJeu, ContenuJeu=:ContenuDuJeu  WHERE CodeJeu=:CodeDuJeu");        
            //Entre les valeurs de la requête
            RequeteModifTxE.bindValue(":CodeDuJeu",ui->LE_Code->text());
            RequeteModifTxE.bindValue(":DescriptionDuJeu", ui->TxE_Description->toPlainText());
            RequeteModifTxE.bindValue(":RemarqueDuJeu", ui->TxE_Remarques->toPlainText());
            RequeteModifTxE.bindValue(":ContenuDuJeu", ui->TxE_Contenu->toPlainText());        
         //Exécute la requête
            if (!RequeteModifTxE.exec())
            {
                qDebug() << "F_AjoutSuppModifJeux::on_Bt_Valider_clicked() : MajTxE" << RequeteModifTxE.lastError() ;
            }
        ///////////////// MAJ EtatInitial  //////////////------------------------------------------------------------
            QSqlQuery RequeteModifEtatInitial ;        
            //prépare le requête de mise à jour
            RequeteModifEtatInitial.prepare("UPDATE jeux SET EtatInitial =:LeEtatInitial WHERE CodeJeu=:CodeDuJeu");
            RequeteModifEtatInitial.bindValue(":CodeDuJeu",ui->LE_Code->text());            
            //Si le RadioBouton Neuf est coché
            if(ui->RBt_Neuf->isChecked())
            {
                RequeteModifEtatInitial.bindValue(":LeEtatInitial", "Neuf");                
            }
            else
            {
                RequeteModifEtatInitial.bindValue(":LeEtatInitial", "Occasion");
            }
            //Exécutee la requête
            if (!RequeteModifEtatInitial.exec())
            {
                qDebug() << "F_AjoutSuppModifJeux::on_Bt_Valider_clicked() : MajEtatInitial" << RequeteModifEtatInitial.lastError() ;
            }            
        ////////// MAJ du nom //////////---------------------------------------------------------
            QSqlQuery RequeteModifNom ;
            //prépare le requête de mise à jour
            RequeteModifNom.prepare("UPDATE jeux SET NomJeu =:NomDuJeu WHERE CodeJeu=:CodeDuJeu");        
            //Entre les valeurs de la requête
            RequeteModifNom.bindValue(":CodeDuJeu",ui->LE_Code->text());
            RequeteModifNom.bindValue(":NomDuJeu", ui->LE_Nom->text());        
            //Exécute la requête
            if (!RequeteModifNom.exec())
            {
                qDebug() << "F_AjoutSuppModifJeux::on_Bt_Valider_clicked() : MajNom" << RequeteModifNom.lastError() ;
            }            
        ////////// MAJ classification ////////////-------------------------------------------------------------------------
            QSqlQuery RequeteModifClassification ;
            //prépare le requête de mise à jour
            RequeteModifClassification.prepare("UPDATE jeux SET TypeJeux_Classification =:CodeClassification WHERE CodeJeu=:CodeDuJeu");        
            //Entre les valeurs de la requête
            RequeteModifClassification.bindValue(":CodeDuJeu",ui->LE_Code->text());
            RequeteModifClassification.bindValue(":CodeClassification", ui->LE_CodeClassification->text());        
            //Exécute la requête
            if (!RequeteModifClassification.exec())
            {
                qDebug() << "F_AjoutSuppModifJeux::on_Bt_Valider_clicked() : MajClassification" << RequeteModifClassification.lastError() ;
            }            
        ////////// MAJ Etat //////////////----------------------------------------------------------------
            QSqlQuery RequeteModifEtat ;
            //prépare le requête de mise à jour
            RequeteModifEtat.prepare("UPDATE jeux, etatsjeu SET EtatsJeu_IdEtatsJeu = IdEtatsJeu WHERE CodeJeu=:CodeDuJeu AND Etat =:EtatDuJeu");
            //Entre les valeurs de la requête
            RequeteModifEtat.bindValue(":CodeDuJeu",ui->LE_Code->text());
            RequeteModifEtat.bindValue(":EtatDuJeu", ui->CBx_Etat->currentText());
            //Exécute la requête
            if (!RequeteModifEtat.exec())
            {
                qDebug() << "F_AjoutSuppModifJeux::on_Bt_Valider_clicked() : MajEtat" << RequeteModifEtat.lastError() ;
            }
        
        ////////// MAJ Statut ////////-----------------------------------------------------------------------
            QSqlQuery RequeteModifStatut ;
            //prépare le requête de mise à jour
            RequeteModifStatut.prepare("UPDATE jeux, statutjeux SET StatutJeux_IdStatutJeux = IdStatutJeux WHERE CodeJeu=:CodeDuJeu AND StatutJeu =:StatutDuJeu");        
            //Entre les valeurs de la requête
            RequeteModifStatut.bindValue(":CodeDuJeu",ui->LE_Code->text());
            RequeteModifStatut.bindValue(":StatutDuJeu", ui->CBx_Statut->currentText());        
            //Exécute la requête
            if (!RequeteModifStatut.exec())
            {
                qDebug() << "F_AjoutSuppModifJeux::on_Bt_Valider_clicked() : MajStatut" << RequeteModifStatut.lastError() ;
            }            
        ////////// MAJ Editeur //////---------------------------------------------------------------------
            QSqlQuery RequeteModifEditeur ;
            //prépare le requête de mise à jour
            RequeteModifEditeur.prepare("UPDATE jeux, fournisseursediteur SET Editeur_IdEditeur = IdFournisseur WHERE CodeJeu=:CodeDuJeu AND NomFournisseur =:NomDuEditeur");        
            //Entre les valeurs de la requête
            RequeteModifEditeur.bindValue(":CodeDuJeu",ui->LE_Code->text());
            RequeteModifEditeur.bindValue(":NomDuEditeur", ui->CBx_Editeur->currentText());        
            //Exécute la requête
            if (!RequeteModifEditeur.exec())
            {
                qDebug() << "F_AjoutSuppModifJeux::on_Bt_Valider_clicked() : MajEditeur" << RequeteModifEditeur.lastError() ;
            }            
        ////////// MAJ Emplacement ///////////----------------------------------------------------------------
            QSqlQuery RequeteModifEmplacement ;
            //prépare le requête de mise à jour
            RequeteModifEmplacement.prepare("UPDATE jeux, emplacement SET Emplacement_IdEmplacement = IdEmplacement WHERE CodeJeu=:CodeDuJeu AND Nom =:NomEmplacement");
            //Entre les valeurs de la requête
            RequeteModifEmplacement.bindValue(":CodeDuJeu",ui->LE_Code->text());
            RequeteModifEmplacement.bindValue(":NomEmplacement", ui->CBx_Emplacement->currentText());
            //Exécute la requête
            if (!RequeteModifEmplacement.exec())
            {
                qDebug() << "F_AjoutSuppModifJeux::on_Bt_Valider_clicked() : MajEmplacement" << RequeteModifEmplacement.lastError() ;
            }
         ////////// MAJ Fournisseur ///////////----------------------------------------------------------------
            QSqlQuery RequeteModifFournisseur ;
            //prépare le requête de mise à jour
            RequeteModifFournisseur.prepare("UPDATE jeux, fournisseursediteur SET Fournisseurs_IdFournisseur = IdFournisseur WHERE CodeJeu=:CodeDuJeu AND NomFournisseur =:NomDuFournisseur");
            //Entre les valeurs de la requête
            RequeteModifFournisseur.bindValue(":CodeDuJeu",ui->LE_Code->text());
            RequeteModifFournisseur.bindValue(":NomDuFournisseur", ui->CBx_Fournisseur->currentText());
            //Exécute la requête
            if (!RequeteModifFournisseur.exec())
            {
                qDebug() << "F_AjoutSuppModifJeux::on_Bt_Valider_clicked() : MajFournisseur" << RequeteModifFournisseur.lastError() ;
            }
        ///////////////  MAJ MotCle 1 /////////////----------------------------------------------------------------------------------
            QSqlQuery RequeteModifMotCle1 ;        
            //prépare le requête de mise à jour
            RequeteModifMotCle1.prepare("UPDATE jeux, motscles SET MotCle1 = Id_MotCle WHERE CodeJeu=:CodeDuJeu AND MotCle =:LeMotCle1");        
            //Entre les valeurs de la requête
            RequeteModifMotCle1.bindValue(":CodeDuJeu",ui->LE_Code->text());
            RequeteModifMotCle1.bindValue(":LeMotCle1", ui->CBx_MotCle1->currentText());        
            //Exécute la requête
            if (!RequeteModifMotCle1.exec())
            {
                qDebug() << "F_AjoutSuppModifJeux::on_Bt_Valider_clicked() : MajMotCle1" << RequeteModifMotCle1.lastError() ;
            }
        ///////////////  MAJ MotCle 2 /////////////----------------------------------------------------------------------------------
            QSqlQuery RequeteModifMotCle2 ;
            //prépare le requête de mise à jour
            RequeteModifMotCle2.prepare("UPDATE jeux, motscles SET MotCle2 = Id_MotCle WHERE CodeJeu=:CodeDuJeu AND MotCle =:LeMotCle2");        
            //Entre les valeurs de la requête
            RequeteModifMotCle2.bindValue(":CodeDuJeu",ui->LE_Code->text());
            RequeteModifMotCle2.bindValue(":LeMotCle2", ui->CBx_MotCle2->currentText());        
            //Exécute la requête
            if (!RequeteModifMotCle2.exec())
            {
                qDebug() << "F_AjoutSuppModifJeux::on_Bt_Valider_clicked() : MajMotCle2" << RequeteModifMotCle2.lastError() ;
            }            
        ///////////////  MAJ MotCle 2 /////////////----------------------------------------------------------------------------------
            QSqlQuery RequeteModifMotCle3 ;
            //prépare le requête de mise à jour
            RequeteModifMotCle3.prepare("UPDATE jeux, motscles SET MotCle3 = Id_MotCle WHERE CodeJeu=:CodeDuJeu AND MotCle =:LeMotCle3");        
            //Entre les valeurs de la requête
            RequeteModifMotCle3.bindValue(":CodeDuJeu",ui->LE_Code->text());
            RequeteModifMotCle3.bindValue(":LeMotCle3", ui->CBx_MotCle3->currentText());        
            //Exécute la requête
            if (!RequeteModifMotCle3.exec())
            {
                qDebug() << "F_AjoutSuppModifJeux::on_Bt_Valider_clicked() : MajMotCle3" << RequeteModifMotCle3.lastError() ;            }

        //--------------------------------------------------------------------------------------------------------
        }
        //Griser les boutons de modification du contenu
        ui->Bt_Valider->setEnabled(false);
        ui->Bt_Annuler->setEnabled(false);
        ui->Bt_Supprimer->setEnabled(true);
        ui->Bt_Ajouter->setEnabled(true);

        // Vider la saisie de la recherche du nom
        //ui->LE_RechercheNom->setText("");
        // Raffraichir la liste des jeux
        this->on_LE_RechercheNom_textChanged(ui->LE_RechercheNom->text()) ;
        ui->LE_RechercheCode->setText(ui->LE_Code->text());
    }
}
//###################################################################
/**
 * @brief Méthode qui annuler ce que l'on est en train de faire
 *
 */
void F_AjoutSuppModifJeux::on_Bt_Annuler_clicked()
{
    if(ui->LE_Nom->text() != "")
    {
        //////////////////////////////////////////////////
        /////// Annulation modif Age ////////////////////
        ////////////////////////////////////////////////
        QSqlQuery RequeteAnnulerAge;


        RequeteAnnulerAge.prepare("SELECT AgeMin, AgeMax FROM jeux WHERE CodeJeu=:CodeDuJeu");
        RequeteAnnulerAge.bindValue(":CodeDuJeu",ui->LE_RechercheCode->text());


    //Exécute la requête
        if (!RequeteAnnulerAge.exec())
        {
            qDebug() << "F_AjoutSuppModifJeux::on_Bt_Annuler_clicked() : AnnulerAge" << RequeteAnnulerAge.lastError() ;
        }

        RequeteAnnulerAge.next();

        //Récupère les informations dans la base de données et les affiches
        unsigned int  AgeMin = (RequeteAnnulerAge.value(0).toInt());
        unsigned int  AgeMax = (RequeteAnnulerAge.value(1).toInt()) ;
        ui->SBx_AgeMin->setValue(AgeMin);
        ui->SBx_AgeMax->setValue(AgeMax);

    //--------------------------------------------------------------------------------------------------------------------------
        //////////////////////////////////////////////////
        /////// Annulation nbr de joueurs ///////////////
        ////////////////////////////////////////////////
        QSqlQuery RequeteAnnulerNbrJoueurs;

        RequeteAnnulerNbrJoueurs.prepare("SELECT NbrJoueurMin, NbrJoueurMax FROM jeux WHERE CodeJeu=:CodeDuJeu");
        RequeteAnnulerNbrJoueurs.bindValue(":CodeDuJeu",ui->LE_RechercheCode->text());

    //Exécute la requête
        if (!RequeteAnnulerNbrJoueurs.exec())
        {
            qDebug() << "F_AjoutSuppModifJeux::on_Bt_Annuler_clicked() : AnnulerNbrJoueurs" << RequeteAnnulerNbrJoueurs.lastError() ;
        }

        RequeteAnnulerNbrJoueurs.next();

    //Récupère les informations dans la base de données et les affiches
        unsigned int  NbrJoueurMin = (RequeteAnnulerNbrJoueurs.value(0).toInt());
        unsigned int  NbrJoueurMax = (RequeteAnnulerNbrJoueurs.value(1).toInt()) ;
        ui->SBx_JoueursMin->setValue(NbrJoueurMin);
        ui->SBx_JoueursMax->setValue(NbrJoueurMax);

    //--------------------------------------------------------------------------------------------------------------------------
        //////////////////////////////////////////////////
        /////// Annulation modif prix ///////////////////
        ////////////////////////////////////////////////
        QSqlQuery RequeteAnnulerPrix;


        RequeteAnnulerPrix.prepare("SELECT PrixAchat, PrixLoc, Caution FROM jeux WHERE CodeJeu=:CodeDuJeu");
        RequeteAnnulerPrix.bindValue(":CodeDuJeu",ui->LE_RechercheCode->text());


    //Exécute la requête
        if (!RequeteAnnulerPrix.exec())
        {
            qDebug() << "F_AjoutSuppModifJeux::on_Bt_Annuler_clicked() : AnnulerPrix" << RequeteAnnulerPrix.lastError() ;
        }

        RequeteAnnulerPrix.next();

    //Récupère les informations dans la base de données et les affiches
        float  PrixAchat = (RequeteAnnulerPrix.value(0).toFloat());
        unsigned int  PrixLoc = (RequeteAnnulerPrix.value(1).toInt()) ;
        unsigned int Caution = (RequeteAnnulerPrix.value(2).toInt()) ;
        ui->SBx_PrixAchat->setValue(PrixAchat);
        ui->SBx_PrixLocation->setValue(PrixLoc);
        ui->SBx_Caution->setValue(Caution);

    //--------------------------------------------------------------------------------------------------------------------------
        //////////////////////////////////////////////////
        /////// Annulation date achat ///////////////////
        ////////////////////////////////////////////////
        QSqlQuery RequeteAnnulerDateAchat;

        RequeteAnnulerDateAchat.prepare("SELECT DateAchat FROM jeux WHERE CodeJeu=:CodeDuJeu");
        RequeteAnnulerDateAchat.bindValue(":CodeDuJeu",ui->LE_RechercheCode->text());

    //Exécute la requête
        if (!RequeteAnnulerDateAchat.exec())
        {
            qDebug() << "F_AjoutSuppModifJeux::on_Bt_Annuler_clicked() : AnnulerDateAchat" << RequeteAnnulerDateAchat.lastError() ;
        }

        RequeteAnnulerDateAchat.next();

    //Récupère les informations dans la base de données et les affiches
        QDate DateAchat = (RequeteAnnulerDateAchat.value(0).toDate());
        ui->DtE_Achat->setDate(DateAchat) ;

    //--------------------------------------------------------------------------------------------------------------------------
        //////////////////////////////////////////////////
        /////// Annulation du créateur //////////////////
        ////////////////////////////////////////////////
        QSqlQuery RequeteAnnulerCreateur;

        RequeteAnnulerCreateur.prepare("SELECT NomCreateurJeu FROM jeux WHERE CodeJeu=:CodeDuJeu");
        RequeteAnnulerCreateur.bindValue(":CodeDuJeu",ui->LE_RechercheCode->text());

    //Exécute la requête
        if (!RequeteAnnulerCreateur.exec())
        {
            qDebug() << "F_AjoutSuppModifJeux::on_Bt_Annuler_clicked() : AnnulerCréateur" << RequeteAnnulerCreateur.lastError() ;
        }

        RequeteAnnulerCreateur.next();

    //Récupère les informations dans la base de données et les affiches
        QString NomCreateur = (RequeteAnnulerCreateur.value(0).toString());
        ui->LE_Createur->setText(NomCreateur);

    //--------------------------------------------------------------------------------------------------------------------------
        //////////////////////////////////////////////////
        /////// Annulation des TxE //////////////////////
        ////////////////////////////////////////////////
        QSqlQuery RequeteAnnulerTxE;

        RequeteAnnulerTxE.prepare("SELECT Remarque, ContenuJeu, DescriptionJeu FROM jeux WHERE CodeJeu=:CodeDuJeu");
        RequeteAnnulerTxE.bindValue(":CodeDuJeu",ui->LE_RechercheCode->text());

    //Exécute la requête
        if (!RequeteAnnulerTxE.exec())
        {
            qDebug() << "F_AjoutSuppModifJeux::on_Bt_Annuler_clicked() : AnnulerTxE" << RequeteAnnulerTxE.lastError() ;
        }

        RequeteAnnulerTxE.next();

    //Récupère les informations dans la base de données et les affiches
        QString Remarque = (RequeteAnnulerTxE.value(0).toString());
        QString ContenuJeu = (RequeteAnnulerTxE.value(1).toString()) ;
        QString DescriptionJeu = (RequeteAnnulerTxE.value(2).toString()) ;
        ui->TxE_Remarques->setText(Remarque);
        ui->TxE_Contenu->setText(ContenuJeu);
        ui->TxE_Description->setText(DescriptionJeu);

    //--------------------------------------------------------------------------------------------------------------------------
        //////////////////////////////////////////////////
        /////// Annulation des Chemin D'accès ///////////
        ////////////////////////////////////////////////
        QSqlQuery RequeteAnnulerCheminAcces;

        RequeteAnnulerCheminAcces.prepare("SELECT CheminReglesJeu, CheminPhotoJeu FROM jeux WHERE CodeJeu=:CodeDuJeu");
        RequeteAnnulerCheminAcces.bindValue(":CodeDuJeu",ui->LE_RechercheCode->text());

    //Exécute la requête
        if (!RequeteAnnulerCheminAcces.exec())
        {
            qDebug() << "F_AjoutSuppModifJeux::on_Bt_Annuler_clicked() : AnnulerCheminAcces" << RequeteAnnulerCheminAcces.lastError() ;
        }

        RequeteAnnulerCheminAcces.next();

    //Récupère les informations dans la base de données et les affiches
        QString CheminReglesJeu = (RequeteAnnulerCheminAcces.value(0).toString());
        QString CheminPhotoJeu = (RequeteAnnulerCheminAcces.value(1).toString()) ;

    //--------------------------------------------------------------------------------------------------------------------------
        //////////////////////////////////////////////////
        /////// Annulation du nom  //////////////////////
        ////////////////////////////////////////////////
        QSqlQuery RequeteAnnulerNomJeu;

        RequeteAnnulerNomJeu.prepare("SELECT Nomjeu FROM jeux WHERE CodeJeu=:CodeDuJeu");
        RequeteAnnulerNomJeu.bindValue(":CodeDuJeu",ui->LE_Code->text());

    //Exécutee la requête
        if (!RequeteAnnulerNomJeu.exec())
        {
            qDebug() << "F_AjoutSuppModifJeux::on_Bt_Annuler_clicked() : AnnulerNomJeu" << RequeteAnnulerNomJeu.lastError() ;
        }

        RequeteAnnulerNomJeu.next();

    //Récupère les informations dans la base de données et les affiches
        QString NomJeu = (RequeteAnnulerNomJeu.value(0).toString());
        ui->LE_Nom->setText(NomJeu);

    //Grise le bouton annuler
        ui->Bt_Valider->setEnabled(false);
        ui->Bt_Annuler->setEnabled(false);

    //--------------------------------------------------------------------------------------------------------------------------
        //////////////////////////////////////////////////
        /////// Annulation de l'etat initial ////////////
        ////////////////////////////////////////////////
        QSqlQuery RequeteAnnulerEtatInitial;

        RequeteAnnulerEtatInitial.prepare("SELECT EtatInitial FROM jeux WHERE CodeJeu=:CodeDuJeu");
        RequeteAnnulerEtatInitial.bindValue(":CodeDuJeu",ui->LE_Code->text());

    //Exécute la requête
        if (!RequeteAnnulerEtatInitial.exec())
        {
            qDebug() << "F_AjoutSuppModifJeux::on_Bt_Annuler_clicked() : AnnulerEtatInitial" << RequeteAnnulerEtatInitial.lastError() ;
        }

        RequeteAnnulerEtatInitial.next();

    //Récupère les informations dans la base de données et les affiches
        QString EtatInitial = (RequeteAnnulerEtatInitial.value(0).toString());
        if(EtatInitial == "Neuf")
        {
            ui->RBt_Neuf->setChecked(true);
        }
        else
        {
            ui->RBt_Occasion->setChecked(true);
        }

    //--------------------------------------------------------------------------------------------------------------------------
        //////////////////////////////////////////////////
        /////// Annulation de la classification /////////
        ////////////////////////////////////////////////
        QSqlQuery RequeteAnnulerClassification;

        RequeteAnnulerClassification.prepare("SELECT TypeJeux_Classification, TypeJeux, Classification FROM jeux, typejeux WHERE CodeJeu=:CodeDuJeu AND Classification = TypeJeux_Classification");
        RequeteAnnulerClassification.bindValue(":CodeDuJeu",ui->LE_Code->text());

    //Exécute la requête
        if (!RequeteAnnulerClassification.exec())
        {
            qDebug() << "F_AjoutSuppModifJeux::on_Bt_Annuler_clicked() : AnnulerClassification" << RequeteAnnulerClassification.lastError() ;
        }

        RequeteAnnulerClassification.next();

    //Récupère les informations dans la base de données et les affiches
        QString CodeClassification = (RequeteAnnulerClassification.value(0).toString());
        QString NomClassification = (RequeteAnnulerClassification.value(1).toString()) ;
        QString RemplissageCBx = CodeClassification + ": " + NomClassification ;
        ui->LE_CodeClassification->setText(CodeClassification);
        ui->CBx_Classification->setItemText(ui->CBx_Classification->currentIndex(), RemplissageCBx);


    //--------------------------------------------------------------------------------------------------------------------------
        //////////////////////////////////////////////////
        /////// Annulation de l'état ////////////////////
        ////////////////////////////////////////////////
        QSqlQuery RequeteAnnulerEtat;

        RequeteAnnulerEtat.prepare("SELECT EtatsJeu_IdEtatsJeu, IdEtatsJeu, Etat FROM jeux, etatsjeu WHERE CodeJeu=:CodeDuJeu AND IdEtatsJeu = EtatsJeu_IdEtatsJeu");
        RequeteAnnulerEtat.bindValue(":CodeDuJeu",ui->LE_Code->text());

    //Exécute la requête
        if (!RequeteAnnulerEtat.exec())
        {
            qDebug() << "F_AjoutSuppModifJeux::on_Bt_Annuler_clicked() : AnnulerEtat" << RequeteAnnulerEtat.lastError() ;
        }

        RequeteAnnulerEtat.next();

        //Se place sur l'index précédent
        ui->CBx_Etat->setCurrentIndex(TableauPositionIndex[0]);

    //--------------------------------------------------------------------------------------------------------------------------
        //////////////////////////////////////////////////
        /////// Annulation du statut ////////////////////
        ////////////////////////////////////////////////
        QSqlQuery RequeteAnnulerStatut;

        RequeteAnnulerStatut.prepare("SELECT StatutJeux_IdStatutJeux, IdStatutJeu, StatutJeu FROM jeux, statutjeu WHERE CodeJeu=:CodeDuJeu AND IdStatutJeu = StatutJeux_IdStatutJeux");
        RequeteAnnulerStatut.bindValue(":CodeDuJeu",ui->LE_Code->text());

    //Exécute la requête
        if (!RequeteAnnulerStatut.exec())
        {
            qDebug() << "F_AjoutSuppModifJeux::on_Bt_Annuler_clicked() : AnnulerStatut" << RequeteAnnulerStatut.lastError() ;
        }

        RequeteAnnulerStatut.next();

        //Se place sur l'index précédent
        ui->CBx_Statut->setCurrentIndex(TableauPositionIndex[1]);

    //--------------------------------------------------------------------------------------------------------------------------
        //////////////////////////////////////////////////
        /////// Annulation de l'éditeur /////////////////
        ////////////////////////////////////////////////
        QSqlQuery RequeteAnnulerEditeur;

        RequeteAnnulerEditeur.prepare("SELECT Editeur_IdEditeur, IdFournisseur, NomFournisseur, Editeur FROM jeux, fournisseursediteur WHERE CodeJeu=:CodeDuJeu AND IdFournisseur = Editeur_IdEditeur AND Editeur = 1");
        RequeteAnnulerEditeur.bindValue(":CodeDuJeu",ui->LE_Code->text());

    //Exécute la requête
        if (!RequeteAnnulerEditeur.exec())
        {
            qDebug() << "F_AjoutSuppModifJeux::on_Bt_Annuler_clicked() : AnnulerEditeur" << RequeteAnnulerEditeur.lastError() ;
        }

        RequeteAnnulerEditeur.next();

        //Se place sur l'index précédent
        ui->CBx_Editeur->setCurrentIndex(TableauPositionIndex[2]);

    //------------------------------------------------------------------------------------------------------
        //////////////////////////////////////////////////
        /////// Annulation du fournisseur ///////////////
        ////////////////////////////////////////////////
        QSqlQuery RequeteAnnulerFournisseur;

        RequeteAnnulerFournisseur.prepare("SELECT Fournisseurs_IdFournisseur, IdFournisseur, NomFournisseur, Fournisseur FROM jeux, fournisseursediteur WHERE CodeJeu=:CodeDuJeu AND IdFournisseur = Fournisseurs_IdFournisseur AND Fournisseur = 1");
        RequeteAnnulerFournisseur.bindValue(":CodeDuJeu",ui->LE_Code->text());

    //Exécute la requête
        if (!RequeteAnnulerFournisseur.exec())
        {
            qDebug() << "F_AjoutSuppModifJeux::on_Bt_Annuler_clicked() : AnnulerFournisseur" << RequeteAnnulerFournisseur.lastError() ;
        }

        RequeteAnnulerFournisseur.next();

        //Se place sur l'index précédent
        ui->CBx_Fournisseur->setCurrentIndex(TableauPositionIndex[3]);

    //--------------------------------------------------------------------------------------------------------------------------
        //////////////////////////////////////////////////
        /////// Annulation de l'emplacement /////////////
        ////////////////////////////////////////////////
        QSqlQuery RequeteAnnulerEmplacement;

        RequeteAnnulerEmplacement.prepare("SELECT Emplacement_IdEmplacement, IdEmplacement, Nom FROM jeux, emplacement WHERE CodeJeu=:CodeDuJeu AND IdEmplacement = Emplacement_IdEmplacement");
        RequeteAnnulerEmplacement.bindValue(":CodeDuJeu",ui->LE_Code->text());

        //Exécute la requête
        if (!RequeteAnnulerEmplacement.exec())
        {
            qDebug() << "F_AjoutSuppModifJeux::on_Bt_Annuler_clicked() : AnnulerEmplacement" << RequeteAnnulerEmplacement.lastError() ;
        }

        RequeteAnnulerEmplacement.next();

        //Se place sur l'index précédent
        ui->CBx_Emplacement->setCurrentIndex(TableauPositionIndex[4]);

    //--------------------------------------------------------------------------------------------------------------------------
        //////////////////////////////////////////////////
        /////// Annulation MotCle1 //////////////////////
        ////////////////////////////////////////////////
        QSqlQuery RequeteAnnulerMotCle1;

        RequeteAnnulerMotCle1.prepare("SELECT MotCle1, Id_MotCle, MotCle FROM jeux, motscles WHERE CodeJeu=:CodeDuJeu AND Id_MotCle = MotCle1");
        RequeteAnnulerMotCle1.bindValue(":CodeDuJeu",ui->LE_Code->text());

        //Exécute la requête
        if (!RequeteAnnulerMotCle1.exec())
        {
            qDebug() << "F_AjoutSuppModifJeux::on_Bt_Annuler_clicked() : AnnulerMotCle1" << RequeteAnnulerMotCle1.lastError() ;
        }

        RequeteAnnulerMotCle1.next();

        //Se place sur l'index précédent
        ui->CBx_MotCle1->setCurrentIndex(TableauPositionIndex[5]);

        //--------------------------------------------------------------------------------------------------------------------------
            //////////////////////////////////////////////////
            /////// Annulation MotCle2 //////////////////////
            ////////////////////////////////////////////////
            QSqlQuery RequeteAnnulerMotCle2;

            RequeteAnnulerMotCle2.prepare("SELECT MotCle2, Id_MotCle, MotCle FROM jeux, motscles WHERE CodeJeu=:CodeDuJeu AND Id_MotCle = MotCle2");
            RequeteAnnulerMotCle2.bindValue(":CodeDuJeu",ui->LE_Code->text());

            //Exécute la requête
            if (!RequeteAnnulerMotCle2.exec())
            {
                qDebug() << "F_AjoutSuppModifJeux::on_Bt_Annuler_clicked() : AnnulerMotCle2" << RequeteAnnulerMotCle2.lastError() ;
            }

            RequeteAnnulerMotCle2.next();

            //Se place sur l'index précédent
            ui->CBx_MotCle2->setCurrentIndex(TableauPositionIndex[6]);

            //--------------------------------------------------------------------------------------------------------------------------
            //////////////////////////////////////////////////
            /////// Annulation MotCle3 //////////////////////
            ////////////////////////////////////////////////
            QSqlQuery RequeteAnnulerMotCle3;

            RequeteAnnulerMotCle3.prepare("SELECT MotCle3, Id_MotCle, MotCle FROM jeux, motscles WHERE CodeJeu=:CodeDuJeu AND Id_MotCle = MotCle1");
            RequeteAnnulerMotCle3.bindValue(":CodeDuJeu",ui->LE_Code->text());

            //Exécute la requête
            if (!RequeteAnnulerMotCle3.exec())
            {
                qDebug() << "F_AjoutSuppModifJeux::on_Bt_Annuler_clicked() : AnnulerMotCle3" << RequeteAnnulerMotCle3.lastError() ;
            }

            RequeteAnnulerMotCle3.next();

            //Se place sur l'index précédent
            ui->CBx_MotCle3->setCurrentIndex(TableauPositionIndex[7]);

            //Grise le bouton annuler
            ui->Bt_Valider->setEnabled(false);
            ui->Bt_Annuler->setEnabled(false);
    }
    else
    {
        ui->CBx_Classification->setDisabled(true);
        ui->CBx_Editeur->setDisabled(true);
        ui->CBx_Emplacement->setDisabled(true);
        ui->CBx_Etat->setDisabled(true);
        ui->CBx_Fournisseur->setDisabled(true);
        ui->CBx_MotCle1->setDisabled(true);
        ui->CBx_MotCle2->setDisabled(true);
        ui->CBx_MotCle3->setDisabled(true);
        ui->CBx_Statut->setDisabled(true);
        ui->LE_Code->setDisabled(true);
        ui->LE_CodeClassification->setDisabled(true);
        ui->LE_Createur->setDisabled(true);
        ui->LE_Nom->setDisabled(true);
        ui->DtE_Achat->setDisabled(true);
        ui->SBx_AgeMax->setDisabled(true);
        ui->SBx_AgeMin->setDisabled(true);
        ui->SBx_Caution->setDisabled(true);
        ui->SBx_JoueursMax->setDisabled(true);
        ui->SBx_JoueursMin->setDisabled(true);
        ui->SBx_PrixAchat->setDisabled(true);
        ui->SBx_PrixLocation->setDisabled(true);
        ui->TxE_Contenu->setDisabled(true);
        ui->TxE_Description->setDisabled(true);
        ui->TxE_Remarques->setDisabled(true);
        ui->RBt_Neuf->setDisabled(true);
        ui->RBt_Occasion->setDisabled(true);
        ui->LE_Code->clear();
        ui->Bt_Valider->setDisabled(true);
        ui->Bt_Annuler->setDisabled(true);
        ui->Bt_ChargerImage->setDisabled(true);
    }
    // Rendre utilisable la création et la suppression d'un jeu en cas
    // d'annulation du jeu courant
    ui->Bt_Supprimer->setEnabled(true);
    ui->Bt_Ajouter->setEnabled(true);
}
//###################################################################
void F_AjoutSuppModifJeux::on_Bt_Renseigner_clicked()
{
    ui->RBt_Neuf->setVisible(true);
    ui->RBt_Occasion->setVisible(true);
    ui->Lb_EtatInitial->setVisible(false);
    ui->Bt_Renseigner->setVisible(false);
    ui->Bt_Valider->setEnabled(true);
    ui->Bt_Annuler->setEnabled(true);
}
//###################################################################
void F_AjoutSuppModifJeux::on_Bt_ChargerImage_clicked()
{
    /*QSqlQuery RecupCheminDossierImage ;

    RecupCheminDossierImage.prepare("SELECT CheminImage, IdPreferences FROM preferences WHERE IdPreferences = 1") ;
    RecupCheminDossierImage.exec() ;
    RecupCheminDossierImage.next() ;
    QString DebutChemin = RecupCheminDossierImage.value(0).toString() ;

    QString CodeDuJeu ;
    CodeDuJeu = ui->LE_Code->text() ;

    QString CheminImageComplet ;
    CheminImageComplet = DebutChemin + "/" + CodeDuJeu + ".png" ;

    qDebug() << "F_AjoutSuppModifJeux::on_Bt_ChargerImage_clicked()-Le chemin complet est :" << CheminImageComplet ;

    this->CheminPhoto = CheminImageComplet ;

    QImage Image(CheminImageComplet) ;
    ui->Lb_Photo->setPixmap(QPixmap::fromImage(Image));

    if(ui->Lb_Photo->isHidden())
    {
        QMessageBox::information(this, "Image", "Aucune image ne correspond à ce jeu, veuillez en ajoutez-une si vous le souhaitez", "OK") ;
    }

    //Met l'image à l'échelle du cadre
    ui->Lb_Photo->setScaledContents(true);

    this->CacherBoutons();*/

    QString sCheminImage ;
    QDir DirDossierImages ;
    sCheminImage = QFileDialog::getOpenFileName(this,tr("Ouvrir une image."), "/home", tr("Image Files (*.png *.jpg *.bmp)"));

    if( sCheminImage != "" )
    {
        DirDossierImages.setPath( QApplication::applicationDirPath() + "/Images" ) ;
        if( !DirDossierImages.exists() )
        {
            DirDossierImages.cd("..") ;
            if( DirDossierImages.mkdir( "Images" ) )
            {
                if( QFile::exists( QApplication::applicationDirPath() + "/Images/" + ui->LE_Code->text() ) )
                {
                    QFile::remove( QApplication::applicationDirPath() + "/Images/" + ui->LE_Code->text() ) ;
                }

                if( !QFile::copy( sCheminImage, QApplication::applicationDirPath() + "/Images/" + ui->LE_Code->text() ) )
                {
                    qDebug() << "Impossible de copier l'image."  ;
                }
                else
                {
                    QString sCheminImage ( QApplication::applicationDirPath() + "/Images/" + ui->LE_Code->text() ) ;

                    QPixmap Image( sCheminImage ) ;
                    ui->Lb_Photo->setPixmap( Image ) ;
                }
            }
            else
            {
                qDebug() << "Impossible de créer le dossier Image."  ;
            }
        }
        else
        {
            if( QFile::exists( QApplication::applicationDirPath() + "/Images/" + ui->LE_Code->text() ) )
            {
                QFile::remove( QApplication::applicationDirPath() + "/Images/" + ui->LE_Code->text() ) ;
            }

            if( !QFile::copy( sCheminImage, QApplication::applicationDirPath() + "/Images/" + ui->LE_Code->text() ) )
            {
                qDebug() << "Impossible de copier l'image."  ;
            }
            else
            {
                QString sCheminImage ( QApplication::applicationDirPath() + "/Images/" + ui->LE_Code->text() ) ;

                QPixmap Image( sCheminImage ) ;
                ui->Lb_Photo->setPixmap( Image ) ;
            }
        }
    }


}
//###################################################################
/**
 * @brief Méthode qui permet de vider les champs ainsi que de passe en mode ajout (modif booléen)
 *
 */
void F_AjoutSuppModifJeux::on_Bt_Ajouter_clicked()
{
    // Rendre utilisable (cliquable) les widgets de l'IHM
    ui->CBx_Classification->setEnabled(true);
    ui->CBx_Editeur->setEnabled(true);
    ui->CBx_Emplacement->setEnabled(true);
    ui->CBx_Etat->setEnabled(true);
    ui->CBx_Fournisseur->setEnabled(true);
    ui->CBx_MotCle1->setEnabled(true);
    ui->CBx_MotCle2->setEnabled(true);
    ui->CBx_MotCle3->setEnabled(true);
    ui->CBx_Statut->setEnabled(true);
    ui->LE_Code->setEnabled(true);
    ui->LE_CodeClassification->setEnabled(true);
    ui->LE_Createur->setEnabled(true);
    ui->LE_Nom->setEnabled(true);
    ui->DtE_Achat->setEnabled(true);
    ui->SBx_AgeMax->setEnabled(true);
    ui->SBx_AgeMin->setEnabled(true);
    ui->SBx_Caution->setEnabled(true);
    ui->SBx_JoueursMax->setEnabled(true);
    ui->SBx_JoueursMin->setEnabled(true);
    ui->SBx_PrixAchat->setEnabled(true);
    ui->SBx_PrixLocation->setEnabled(true);
    ui->TxE_Contenu->setEnabled(true);
    ui->TxE_Description->setEnabled(true);
    ui->TxE_Remarques->setEnabled(true);
    ui->RBt_Neuf->setEnabled(true);
    ui->RBt_Occasion->setEnabled(true);
    ui->Bt_ChargerImage->setEnabled(true);

    // Vide tous les champs avant l'ajout
    ui->LE_Code->clear();
    ui->LE_CodeClassification->clear();
    ui->LE_Createur->clear();
    ui->LE_Nom->clear();
    ui->TxE_Contenu->clear();
    ui->TxE_Description->clear();
    ui->TxE_Remarques->clear();
    ui->SBx_AgeMax->setValue(0);
    ui->SBx_AgeMin->setValue(0);
    ui->SBx_Caution->setValue(0);
    ui->SBx_JoueursMax->setValue(0);
    ui->SBx_JoueursMin->setValue(0);
    ui->SBx_PrixAchat->setValue(0);
    ui->SBx_PrixLocation->setValue(0);

    // Provisoire :
    ui->CBx_Classification->setItemText(0, "Sélectionner une classification");
    ui->CBx_Classification->setCurrentIndex(0);

    //Provisoire :
    ui->CBx_Editeur->setItemText(0, "Sélectionner un éditeur");
    ui->CBx_Editeur->setCurrentIndex(0);

    ui->CBx_Emplacement->setCurrentIndex(0);
    ui->CBx_Etat->setCurrentIndex(0);

    //Provisoire :
    ui->CBx_Fournisseur->setItemText(0, "Sélectionner un fournisseur");
    ui->CBx_Fournisseur->setCurrentIndex(0);

    ui->CBx_Statut->setCurrentIndex(0);
    ui->CBx_MotCle1->setCurrentIndex(0);
    ui->CBx_MotCle2->setCurrentIndex(0);
    ui->CBx_MotCle3->setCurrentIndex(0);
    ui->LE_RechercheCode->clear();
    ui->LE_RechercheNom->clear();

    // Affichage de la date d'achat à la date du jour
    QDate DateDefaut ;
    DateDefaut = DateDefaut.currentDate() ;
    ui->DtE_Achat->setDate(DateDefaut);

    QSqlQuery RequetePremierCodeLibre ;
    QString CodeJeu ;
    //Sélectionne le premier CodeJeu qui est libre (supérieur au plus grand)
    RequetePremierCodeLibre.exec("SELECT MIN(CodeJeu+1) FROM jeux WHERE (CodeJeu+1) NOT IN (SELECT CodeJeu FROM jeux)") ;
    RequetePremierCodeLibre.next() ;
    CodeJeu = RequetePremierCodeLibre.value(0).toString() ;
    ui->LE_Code->setText(CodeJeu);
    // On passe en mode Création d'un nouveau jeu, pas en modif d'un jeu
    AjoutOuModif = true ;
    // Autoriser les boutons pour valider ou annuler
    ui->Bt_Valider->setEnabled(true);
    ui->Bt_Annuler->setEnabled(true);
    // Curseur dans le chanmps de saisie du nom du jeu
    ui->LE_Nom->setFocus();
}
//###################################################################
/**
 * @brief Méthode qui permet de supprimer un jeu de la BDD
 *
 */
void F_AjoutSuppModifJeux::on_Bt_Supprimer_clicked()
{
    QSqlQuery RequeteJeuEmprunte ;
    //Si aucun jeu n'a été sélectionné alors un message d'erreur s'affiche
    if(ui->LE_Nom->text() == "" && ui->LE_Code->text() == "")
    {
        QMessageBox::information(this, "Erreur suppression", "Aucun jeu n'a été sélectionné, la suppression n'est donc pas possible", "Sortir") ;
    }
    else
    {
        if( RequeteJeuEmprunte.exec( "SELECT IdJeux FROM jeux WHERE StatutJeux_IdStatutJeux=3 AND CodeJeu=" + ui->LE_Code->text() ) )
        {
            if( RequeteJeuEmprunte.next() )
            {
                QMessageBox::information(this, "Erreur suppression", "Impossible de supprimer un jeu emprunté.", "Sortir") ;
            }
            else
            {
                //Test permettant de savoir quel bouton a été appuyé, entre dans cette méthode si le bouton oui a été cliqué
                if((QMessageBox::information(this, "Confirmation suppression","Voulez-vous vraiment supprimer le jeu "+ui->LE_Nom->text()+"?","Oui", "Non")) == 0)
                {
                    // Séléction de l'Id correspondant au code //
                    QSqlQuery RequeteSelectionJeux ;
                    RequeteSelectionJeux.prepare("SELECT IdJeux, CodeJeu FROM jeux WHERE CodeJeu = :CodeDuJeu") ;
                    RequeteSelectionJeux.bindValue(":CodeDuJeu", ui->LE_Code->text());
                    RequeteSelectionJeux.exec() ;
                    RequeteSelectionJeux.next() ;

                    ///// Suppr Hist Interventions //////
                    QSqlQuery RequeteSuppHistInter ;
                    RequeteSuppHistInter.prepare("DELETE FROM intervientionsjeu WHERE Jeux_IdJeux = :LeJeux_IdJeux") ;
                    RequeteSuppHistInter.bindValue(":LeJeux_IdJeux", RequeteSelectionJeux.value(0).toString() );
                    if(!RequeteSuppHistInter.exec())
                    {
                        qDebug() << "F_AjoutSuppModifJeux::on_Bt_Supprimer_clicked() : RequeteSuppHistInter" << RequeteSuppHistInter.lastError() ;
                    }
                    RequeteSuppHistInter.next() ;

                    ///// Suppr Hist Emprunt ////////////
                    QSqlQuery RequeteSuppHistEmprunt ;
                    RequeteSuppHistEmprunt.prepare("DELETE FROM emprunts WHERE Jeux_IdJeux = :Jeux_IdDuJeux") ;
                    RequeteSuppHistEmprunt.bindValue(":Jeux_IdDuJeux", RequeteSelectionJeux.value(0).toString());
                    if(!RequeteSuppHistEmprunt.exec())
                    {
                        qDebug() << "F_AjoutSuppModifJeux::on_Bt_Supprimer_clicked()-RequeteSuppHistEmprunt" << RequeteSuppHistEmprunt.lastError() ;

                    }
                    RequeteSuppHistEmprunt.next() ;

                    ///// Suppr Hist Résa ///////////////
                    QSqlQuery RequeteSuppHistReservation ;
                    RequeteSuppHistReservation.prepare("DELETE FROM reservation WHERE Jeux_IdJeux = :Jeux_IdLeJeux");
                    RequeteSuppHistReservation.bindValue(":Jeux_IdLeJeux", RequeteSelectionJeux.value(0).toString());
                    if(!RequeteSuppHistReservation.exec())
                    {
                        qDebug() << "F_AjoutSuppModifJeux::on_Bt_Supprimer_clicked() : RequeteSuppHistReservation" << RequeteSuppHistReservation.lastError() ;

                    }
                    RequeteSuppHistReservation.next() ;

                    //Création de la requête SQL
                    QSqlQuery RequeteSuppressionJeux ;
                    //Préparation de la requête SQL
                    RequeteSuppressionJeux.prepare("DELETE FROM jeux WHERE CodeJeu = :CodeDuJeu") ;
                    //Copie la valeur récuper sur l'interface dans la variable SQL
                    RequeteSuppressionJeux.bindValue(":CodeDuJeu", ui->LE_Code->text() );
                    //Éxécutre la requête, mais affiche une erreur si il y en a une
                    if (!RequeteSuppressionJeux.exec())
                    {
                        //Affiche la dernière erreur
                        qDebug()<<"F_AjoutSuppModifJeux::on_Bt_Supprimer_clicked()-Suppr jeu"<< RequeteSuppressionJeux.lastError();
                    }
                    RequeteSuppressionJeux.next() ;

                    //Vider tous les champs après la suppression du jeu
                    ui->LE_Code->clear();
                    ui->LE_Createur->clear();
                    ui->LE_Nom->clear();
                    ui->LE_CodeClassification->clear();
                    ui->TxE_Contenu->clear();
                    ui->TxE_Description->clear();
                    ui->TxE_Remarques->clear();
                    ui->SBx_AgeMax->setValue(0);
                    ui->SBx_AgeMin->setValue(0);
                    ui->SBx_Caution->setValue(0);
                    ui->SBx_JoueursMax->setValue(0);
                    ui->SBx_JoueursMin->setValue(0);
                    ui->SBx_PrixAchat->setValue(0);
                    ui->SBx_PrixLocation->setValue(0);
                    ui->CBx_Classification->setCurrentIndex(0);
                    ui->CBx_Editeur->setCurrentIndex(0);
                    ui->CBx_Emplacement->setCurrentIndex(0);
                    ui->CBx_Etat->setCurrentIndex(0);
                    ui->CBx_Fournisseur->setCurrentIndex(0);
                    ui->CBx_Statut->setCurrentIndex(0);
                    ui->CBx_MotCle1->setCurrentIndex(0);
                    ui->CBx_MotCle2->setCurrentIndex(0) ;
                    ui->CBx_MotCle3->setCurrentIndex(0);

                    //QDate DateDefaut ;
                    //DateDefaut = DateDefaut.currentDate() ;
                    ui->DtE_Achat->setDate(QDate::currentDate());
                    // Bloquer les boutons Valider/Annuler et Supprimer
                    ui->Bt_Valider->setEnabled(false);
                    ui->Bt_Annuler->setEnabled(false);
                    ui->Bt_Supprimer->setEnabled(false);
                    // Autoriser la création d'un nouveau jeu
                    ui->Bt_Ajouter->setEnabled(true);
                    // Remettre à jour la liste des jeux puisqu'un jeu a été enlevé de la liste
                    ui->LE_RechercheNom->clear();
                    ui->LE_RechercheCode->clear();
                    this->on_Bt_OK_clicked();
                }
            }
        }
        else
        {
            qDebug() << "F_AjoutSuppModifJeux::on_Bt_Supprimer_clicked() : RequetejeuxEmprunte : " << RequeteJeuEmprunte.lastError().text() ;
        }
    }
}
//###################################################################
/**
 * @brief Méthode qui recherche le jeu correspondant au code entrée, et affiche les informations
 *
 */
void F_AjoutSuppModifJeux::on_Bt_OK_clicked()
{
    // Débloque tous les champs de saisie en vue d'une modification possible
    ui->CBx_Classification->setEnabled(true);
    ui->CBx_Editeur->setEnabled(true);
    ui->CBx_Emplacement->setEnabled(true);
    ui->CBx_Etat->setEnabled(true);
    ui->CBx_Fournisseur->setEnabled(true);
    ui->CBx_MotCle1->setEnabled(true);
    ui->CBx_MotCle2->setEnabled(true);
    ui->CBx_MotCle3->setEnabled(true);
    ui->CBx_Statut->setEnabled(true);
    ui->LE_Code->setEnabled(true);
    ui->LE_CodeClassification->setEnabled(true);
    ui->LE_Createur->setEnabled(true);
    ui->LE_Nom->setEnabled(true);
    ui->DtE_Achat->setEnabled(true);
    ui->SBx_AgeMax->setEnabled(true);
    ui->SBx_AgeMin->setEnabled(true);
    ui->SBx_Caution->setEnabled(true);
    ui->SBx_JoueursMax->setEnabled(true);
    ui->SBx_JoueursMin->setEnabled(true);
    ui->SBx_PrixAchat->setEnabled(true);
    ui->SBx_PrixLocation->setEnabled(true);
    ui->TxE_Contenu->setEnabled(true);
    ui->TxE_Description->setEnabled(true);
    ui->TxE_Remarques->setEnabled(true);
    ui->RBt_Neuf->setEnabled(true);
    ui->RBt_Occasion->setEnabled(true);
    ui->Bt_ChargerImage->setEnabled(true);

    // Vérifie si un jeu n'est pas en cours de modification
    // Evite de perdre des modif avant de passer à un autre jeu
    if(ui->Bt_Valider->isEnabled() && ui->Bt_Annuler->isEnabled())
    {
        QMessageBox::information(this, "Pré-requis pour passer à un autre jeu !", "Attention, avant d'effectuer une nouvelle recherche vous devez valider ou annuler vos modifications sur le jeu en cours", "OK") ;
    }
    else
    {
        //////////// Recherche par code //////////////////
        unsigned int NumeroLigne (0);
        QSqlQuery RequeteRechercheJeu;
        NumeroLigne=0;

        RequeteRechercheJeu.prepare("SELECT CodeJeu, NomJeu, TypeJeux_Classification, NomCreateurJeu, DateAchat, PrixAchat, PrixLoc, NbrJoueurMin, NbrJoueurMax, AgeMin, AgeMax, Remarque, ContenuJeu, Caution, CheminReglesJeu, CheminPhotoJeu, DescriptionJeu, StatutJeux_IdStatutJeux, EtatsJeu_IdEtatsJeu, Editeur_IdEditeur, Fournisseurs_IdFournisseur, Emplacement_IdEmplacement, MotCle1, MotCle2, MotCle3, EtatInitial FROM`jeux` WHERE CodeJeu = :CodeDuJeu");
        RequeteRechercheJeu.bindValue(":CodeDuJeu",ui->LE_RechercheCode->text());
        if(!RequeteRechercheJeu.exec())
        {
            RequeteRechercheJeu.lastError() ;
        }
        RequeteRechercheJeu.next();
        //-------------------------------------------------------------------------------------------------
        QString CodeJeu = RequeteRechercheJeu.value(0).toString() ;
        ui->LE_Code->setText(CodeJeu);
        //-------------------------------------------------------------------------------------------------
        QString NomJeu = RequeteRechercheJeu.value(1).toString() ;
        ui->LE_Nom->setText(NomJeu);
        ui->LE_Nom->setCursorPosition(0) ;
        //-------------------------------------------------------------------------------------------------
        QString NomCreateurJeu = RequeteRechercheJeu.value(3).toString() ;
        ui->LE_Createur->setText(NomCreateurJeu);
        //-------------------------------------------------------------------------------------------------
        QDate DateAchat = RequeteRechercheJeu.value(4).toDate() ;
        ui->DtE_Achat->setDate(DateAchat);
        //-------------------------------------------------------------------------------------------------
        unsigned int PrixAchat = RequeteRechercheJeu.value(5).toInt() ;
        ui->SBx_PrixAchat->setValue(PrixAchat);
        //-------------------------------------------------------------------------------------------------
        unsigned int PrixLoc = RequeteRechercheJeu.value(6).toInt() ;
        ui->SBx_PrixLocation->setValue(PrixLoc);
        //-------------------------------------------------------------------------------------------------
        unsigned int NbrJoueurMin = RequeteRechercheJeu.value(7).toInt() ;
        ui->SBx_JoueursMin->setValue(NbrJoueurMin);
        //-------------------------------------------------------------------------------------------------
        unsigned int NbrJoueurMax = RequeteRechercheJeu.value(8).toInt() ;
        ui->SBx_JoueursMax->setValue(NbrJoueurMax);
        //-------------------------------------------------------------------------------------------------
        unsigned int AgeMin = RequeteRechercheJeu.value(9).toInt() ;
        ui->SBx_AgeMin->setValue(AgeMin);
        //-------------------------------------------------------------------------------------------------
        unsigned int AgeMax = RequeteRechercheJeu.value(10).toInt() ;
        ui->SBx_AgeMax->setValue(AgeMax) ;
        //-------------------------------------------------------------------------------------------------
        QString Remarques = RequeteRechercheJeu.value(11).toString() ;
        ui->TxE_Remarques->setText(Remarques);
        //-------------------------------------------------------------------------------------------------
        QString ContenuJeu = RequeteRechercheJeu.value(12).toString() ;
        ui->TxE_Contenu->setText(ContenuJeu);
        //-------------------------------------------------------------------------------------------------
        unsigned int Caution = RequeteRechercheJeu.value(13).toInt() ;
        ui->SBx_Caution->setValue(Caution);
        //-------------------------------------------------------------------------------------------------
        QString Description = RequeteRechercheJeu.value(16).toString() ;
        ui->TxE_Description->setText(Description);
        //--------Remplissage CBx Classification-----------------------------------------------------------------------------------
        QString CodeClassification = RequeteRechercheJeu.value(2).toString() ;
        ui->LE_CodeClassification->setText(CodeClassification);

        QSqlQuery RequeteNomClassification ;
        RequeteNomClassification.prepare("SELECT TypeJeux, Classification FROM typejeux WHERE Classification =:CodeDeClassification") ;
        RequeteNomClassification.bindValue(":CodeDeClassification", ui->LE_CodeClassification->text());
        RequeteNomClassification.exec();
        RequeteNomClassification.next();

        QString NomClassification = RequeteNomClassification.value(0).toString() ;
        QString NumClassification = RequeteNomClassification.value(1).toString() ;
        QString AffichageCBxClassification = NumClassification + ": " + NomClassification ;
        ui->CBx_Classification->setItemText(0, AffichageCBxClassification);

        //--------------Remplissage CBx Statut----------------------------------------------------------------------------
        QSqlQuery RequeteStatut ;
        QString IdStatut = RequeteRechercheJeu.value(17).toString() ;

        RequeteStatut.prepare("SELECT StatutJeu, IdStatutJeux FROM statutjeux WHERE IdStatutJeux =:IdStatutDuJeu") ;
        RequeteStatut.bindValue(":IdStatutDuJeu", IdStatut);
        RequeteStatut.exec() ;
        RequeteStatut.next() ;

        ui->CBx_Statut->setCurrentIndex(RequeteStatut.value(1).toInt());

        TableauPositionIndex[1] = ui->CBx_Statut->currentIndex() ;

    //-------------------Remplissage CBx Etat-------------------------------------------------------------
        QSqlQuery RequeteEtat ;
        QString IdEtat = RequeteRechercheJeu.value(18).toString() ;

        RequeteEtat.prepare("SELECT Etat, IdEtatsJeu FROM etatsjeu WHERE IdEtatsJeu =:IdEtatDuJeu") ;
        RequeteEtat.bindValue(":IdEtatDuJeu", IdEtat);
        RequeteEtat.exec() ;
        RequeteEtat.next() ;
        ui->CBx_Etat->setCurrentIndex(RequeteEtat.value(1).toInt());
        TableauPositionIndex[0] = ui->CBx_Etat->currentIndex() ;
    //-----------------Remplissage CBx Editeur---------------------------------------------------------------
        QSqlQuery RequeteEditeur ;
        QString IdEditeur = RequeteRechercheJeu.value(19).toString() ;
        if(IdEditeur == 0)
        {
        }
        else
        {
            RequeteEditeur.prepare("SELECT NomFournisseur, IdFournisseur, Editeur FROM fournisseursediteur WHERE IdFournisseur =:IdDuEditeur ORDER BY NomFournisseur") ;
            RequeteEditeur.bindValue(":IdDuEditeur", IdEditeur);
            RequeteEditeur.exec() ;
            RequeteEditeur.next() ;
            // Affiche dans le combobox l'éditeur pour ce jeu
            QString NomEditeur = RequeteEditeur.value(0).toString() ;
            ui->CBx_Editeur->setItemText(0, NomEditeur);
            //ui->CBx_Editeur->setCurrentIndex(RequeteEditeur.value(1).toInt());
            TableauPositionIndex[2] = ui->CBx_Editeur->currentIndex() ;
        }
    //-----------------Remplissage CBx Fournisseur---------------------------------------------------------
        QSqlQuery RequeteFournisseur ;
        QString idFournisseur = RequeteRechercheJeu.value(20).toString() ;
        if(idFournisseur == 0)
        {
        }
        else
        {
            RequeteFournisseur.prepare("SELECT NomFournisseur, IdFournisseur, Fournisseur FROM fournisseursediteur WHERE IdFournisseur =:IdDuFournisseur ORDER BY NomFournisseur") ;
            RequeteFournisseur.bindValue(":IdDuFournisseur", idFournisseur);
            RequeteFournisseur.exec() ;
            RequeteFournisseur.next() ;

            // Affiche dans le combobox le Fournisseur pour ce jeu
            QString NomFournisseur = RequeteFournisseur.value(0).toString() ;
            ui->CBx_Fournisseur->setItemText(0, NomFournisseur);

            //ui->CBx_Fournisseur->setCurrentIndex(RequeteFournisseur.value(1).toInt());
            TableauPositionIndex[3] = ui->CBx_Fournisseur->currentIndex() ;
            //qDebug() << "F_AjoutSuppModifJeux::on_Bt_OK_clicked()-La case 3 du tableau est rempli avec :" << TableauPositionIndex[3] ;
        }
    //-----------------Remplissage CBx Emplacement -----------------------------------------------------
        QSqlQuery RequeteEmplacement ;
        QString IdEmplacement = RequeteRechercheJeu.value(21).toString() ;
        if(IdEmplacement == 0)
        {
        }
        else
        {
            RequeteEmplacement.prepare("SELECT IdEmplacement, Nom FROM emplacement WHERE IdEmplacement =:IdDeEmplacement") ;
            RequeteEmplacement.bindValue(":IdDeEmplacement", IdEmplacement);
            RequeteEmplacement.exec() ;
            RequeteEmplacement.next() ;
            // Affiche dans le combobox l'emplacement pour ce jeu
            QString NomEmplacement = RequeteEmplacement.value(1).toString() ;
            ui->CBx_Emplacement->setItemText(0, NomEmplacement);

            //ui->CBx_Emplacement->setCurrentIndex(RequeteEmplacement.value(0).toInt());
            TableauPositionIndex[4] = ui->CBx_Emplacement->currentIndex() ;
        }
    //---------------Remplissage CBx MotCle1-----------------------------------------------------------------------
        QSqlQuery RequeteMotCle1 ;
        QString IdMotCle1 = RequeteRechercheJeu.value(22).toString() ;
        if(IdMotCle1 == 0)
        {
        }
        else
        {
            RequeteMotCle1.prepare("SELECT Id_MotCle, MotCle FROM motscles WHERE Id_MotCle =:IdDeMotCle") ;
            RequeteMotCle1.bindValue(":IdDeMotCle", IdMotCle1);
            RequeteMotCle1.exec() ;
            RequeteMotCle1.next() ;
            ui->CBx_MotCle1->setCurrentIndex(RequeteMotCle1.value(0).toInt());
            TableauPositionIndex[5] = ui->CBx_MotCle1->currentIndex() ;
        }
        //----------------Remplissage CBx MotCle2--------------------------------------------------------
        QSqlQuery RequeteMotCle2 ;
        QString IdMotCle2 = RequeteRechercheJeu.value(23).toString() ;
        if(IdMotCle2 == 0)
        {
        }
        else
        {

            RequeteMotCle2.prepare("SELECT Id_MotCle, MotCle FROM motscles WHERE Id_MotCle =:IdDeMotCle") ;
            RequeteMotCle2.bindValue(":IdDeMotCle", IdMotCle2);
            RequeteMotCle2.exec() ;
            RequeteMotCle2.next() ;
            ui->CBx_MotCle2->setCurrentIndex(RequeteMotCle2.value(0).toInt());
            TableauPositionIndex[6] = ui->CBx_MotCle2->currentIndex() ;
        }
     //-------------Remplissage CBx MotCle3----------------------------------------------------------------
        QSqlQuery RequeteMotCle3 ;
        QString IdMotCle3 = RequeteRechercheJeu.value(24).toString() ;
        if(IdMotCle3 == 0)
        {
        }
        else
        {
            RequeteMotCle3.prepare("SELECT Id_MotCle, MotCle FROM motscles WHERE Id_MotCle =:IdDeMotCle") ;
            RequeteMotCle3.bindValue(":IdDeMotCle", IdMotCle3);
            RequeteMotCle3.exec() ;
            RequeteMotCle3.next() ;
            ui->CBx_MotCle3->setCurrentIndex(RequeteMotCle3.value(0).toInt());
            TableauPositionIndex[7] = ui->CBx_MotCle3->currentIndex() ;
        }
    //--------------Cocher EtatInitial-----------------------------------------------------------------
        QString EtatInitial = RequeteRechercheJeu.value(25).toString() ;

        if(EtatInitial == "Neuf")
        {
            ui->RBt_Neuf->setVisible(true);
            ui->RBt_Occasion->setVisible(true);
            ui->RBt_Neuf->setChecked(true);
            ui->Lb_EtatInitial->hide();
            ui->Bt_Renseigner->setVisible(false);
        }
        else
        {
            if(EtatInitial == "Occasion")
            {
                ui->RBt_Neuf->setVisible(true);
                ui->RBt_Occasion->setVisible(true);
                ui->RBt_Occasion->setChecked(true);
                ui->Lb_EtatInitial->hide();
                ui->Bt_Renseigner->setVisible(false);
            }
            else
            {
                ui->RBt_Neuf->setVisible(false);
                ui->RBt_Occasion->setVisible(false);
                //ui->RBt_Neuf->setChecked(false);
                //ui->RBt_Occasion->setChecked(false);
                ui->Lb_EtatInitial->setVisible(true);
                ui->Lb_EtatInitial->setText("Non renseigné");
                ui->Lb_EtatInitial->setStyleSheet( "color: red" );
                ui->Bt_Renseigner->setVisible(true);
            }
        }
    //----------Affichage photo----------------------------------------------------------------------------
        QString sCheminImage ( QApplication::applicationDirPath() + "/Images/" + ui->LE_Code->text() ) ;

        QPixmap Image( sCheminImage ) ;
        ui->Lb_Photo->setPixmap( Image ) ;

        //Met l'image à l'échelle du cadre
        ui->Lb_Photo->setScaledContents( true ) ;

        //Grisee les boutons valider et annuler
        // mais autoriser la création d'un nouveau jeu ou la suppression du jeu courant
        ui->Bt_Valider->setEnabled(false);
        ui->Bt_Annuler->setEnabled(false);
        ui->Bt_Supprimer->setEnabled(true);
        ui->Bt_Ajouter->setEnabled(true);
    }
    AjoutOuModif = false ;
}
//###################################################################
/**
 * @brief Méthode qui actualise le CBx_Editeur après un ajout
 *
 */
void F_AjoutSuppModifJeux::ActualiserCBx_Editeur()
{
    ui->CBx_Editeur->clear();
    ////////////////////////////////////////////
    ///////// Remplissage CBx_Editeur /////////
    //////////////////////////////////////////

    QSqlQuery RequeteEditeur ;

    RequeteEditeur.exec("SELECT NomFournisseur, Editeur FROM fournisseursediteur WHERE Editeur = 1 ORDER BY NomFournisseur") ;

    ui->CBx_Editeur->addItem("Sélectionner un éditeur");

    while(RequeteEditeur.next())
    {
        QString NomEditeur = (RequeteEditeur.value(0).toString()) ;
        ui->CBx_Editeur->addItem(NomEditeur);
    }
    ui->CBx_Editeur->addItem("Ajouter un éditeur");
}
//###################################################################
/**
 * @brief Méthode qui actualise le CBx_Fournisseur après un ajout
 *
 */
void F_AjoutSuppModifJeux::ActualiserCBx_Fournisseur()
{
    ui->CBx_Fournisseur->clear();
    ////////////////////////////////////////////
    ///////// Remplissage CBx_Fournisseur /////
    //////////////////////////////////////////

    QSqlQuery RequeteFournisseur ;

    RequeteFournisseur.exec("SELECT NomFournisseur, Fournisseur FROM fournisseursediteur WHERE Fournisseur = 1 ORDER BY NomFournisseur") ;

    ui->CBx_Fournisseur->addItem("Sélectionner un fournisseur");
    while(RequeteFournisseur.next())
    {

        QString NomFournisseur = (RequeteFournisseur.value(0).toString()) ;
        ui->CBx_Fournisseur->addItem(NomFournisseur);
    }
    ui->CBx_Fournisseur->addItem("Ajouter un fournisseur");
}
//###################################################################
/**
 * @brief Méthode qui actualise le CBx_Statut après un ajout
 *
 */
void F_AjoutSuppModifJeux::ActualiserCBx_Statut()
{
    ui->CBx_Statut->clear();
    ////////////////////////////////////////////
    ///////// Remplissage CBx_Emplacement /////
    //////////////////////////////////////////

    QSqlQuery RequeteStatut ;

    RequeteStatut.exec("SELECT `StatutJeu` FROM `statutjeux") ;
    ui->CBx_Statut->addItem("Sélectionner un statut");

    while(RequeteStatut.next())
    {
        QString NomStatut = (RequeteStatut.value(0).toString()) ;
        ui->CBx_Statut->addItem(NomStatut);
    }

    ui->CBx_Statut->addItem("Ajouter un statut");
}
//###################################################################
/**
 * @brief Méthode qui actualise le CBx_Etat après un ajout
 *
 */
void F_AjoutSuppModifJeux::ActualiserCBX_Etat()
{
    ui->CBx_Etat->clear();
    ////////////////////////////////////////////
    ///////// Remplissage CBx_Emplacement /////
    //////////////////////////////////////////

    QSqlQuery RequeteEtat ;

    RequeteEtat.exec("SELECT `Etat` FROM `etatsjeu") ;
    ui->CBx_Etat->addItem("Sélectionner un état");

    while(RequeteEtat.next())
    {
        QString NomEtat = (RequeteEtat.value(0).toString()) ;
        ui->CBx_Etat->addItem(NomEtat);
    }
    ui->CBx_Etat->addItem("Ajouter un état");
}
//###################################################################
/**
 * @brief Méthode qui actualise le CBx_Classification après un ajout
 *
 */
void F_AjoutSuppModifJeux::ActualiserCBx_Classification()
{
    ui->CBx_Classification->clear();

    QSqlQuery RequeteClassification ;

    RequeteClassification.exec("SELECT TypeJeux, Classification FROM typejeux ORDER BY Classification ASC") ;

    ui->CBx_Classification->addItem("Sélectionner une classification");

    while(RequeteClassification.next())
    {

        QString NomClassification = (RequeteClassification.value(0).toString()) ;
        QString CodeClassification = (RequeteClassification.value(1).toString()) ;
        QString AffichageCBx = CodeClassification + ": " + NomClassification ;
        ui->CBx_Classification->addItem(AffichageCBx);
    }
    ui->CBx_Classification->addItem("Ajouter une classification");
}
//###################################################################
/**
 * @brief Méthode qui actualise le CBx emplacement après un ajout
 *
 */
void F_AjoutSuppModifJeux::ActualiserCBx_Emplacement()
{
    ui->CBx_Emplacement->clear();

    QSqlQuery RequeteEmplacement ;

    RequeteEmplacement.exec("SELECT Nom FROM emplacement GROUP BY Nom") ;
    ui->CBx_Emplacement->addItem("Sélectionner un emplacement");
    while(RequeteEmplacement.next())
    {

        QString NomEmplacement = (RequeteEmplacement.value(0).toString()) ;
        ui->CBx_Emplacement->addItem(NomEmplacement);
    }
    ui->CBx_Emplacement->addItem("Ajouter un emplacement");
}
//###################################################################
/**
 * @brief Méthode qui actualise le CBx_MotCle après un ajout
 *
 */
void F_AjoutSuppModifJeux::ActualiserCBx_MotCle()
{
    ui->CBx_MotCle1->clear();
    ui->CBx_MotCle2->clear();
    ui->CBx_MotCle3->clear();
    ////////////////////////////////////////////
    ///////// Remplissage CBx_MotCles /////////
    //////////////////////////////////////////

    QSqlQuery RequeteMotCle ;

    RequeteMotCle.exec("SELECT `MotCle` FROM `motscles") ;

    ui->CBx_MotCle1->addItem("Mot clé 1");
    ui->CBx_MotCle2->addItem("Mot clé 2");
    ui->CBx_MotCle3->addItem("Mot clé 3");

    while(RequeteMotCle.next())
    {

        QString MotCle = (RequeteMotCle.value(0).toString()) ;
        ui->CBx_MotCle1->addItem(MotCle);
        ui->CBx_MotCle2->addItem(MotCle);
        ui->CBx_MotCle3->addItem(MotCle);
    }
    ui->CBx_MotCle1->addItem("Ajouter un mot clé");
    ui->CBx_MotCle2->addItem("Ajouter un mot clé");
    ui->CBx_MotCle3->addItem("Ajouter un mot clé");
}
//###################################################################
/**
 *  Pour gérer le jeu de cache cache des boutons Valider/Annuler/Ajouter/Supprimer
 */
void F_AjoutSuppModifJeux::CacherBoutons()
{
    ui->Bt_Valider->setEnabled(true);
    ui->Bt_Annuler->setEnabled(true);
    ui->Bt_Supprimer->setEnabled(false);
    ui->Bt_Ajouter->setEnabled(false);
}
