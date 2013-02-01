//------------------------------------------------------------------------------
/** @file       f_clesttem.cpp
 *  @brief      F_CLESTTEM est une classe qui regroupe tous les ajouts, les modifications, les suppressions dans la BDD.
 *
 *  @author     NORMAND Julien
 *  @author     STS IRIS, Lycée Nicolas APPERT, ORVAULT (FRANCE)
 *  @since      07/2012
 *  @version    1.0
 *  @date       01/08/2012
 *
 *  Les options disponibles dans cette classe sont:
 *      - Jeux avec :
 *          - Etat qui dispose le numéro 0 pour les options.
 *          - Statut qui dispose le numéro 1 pour les options.
 *          - Emplacement qui dispose le numéro 2 pour les options.
 *          - Type qui dispose le numéro 3 pour les options.
 *      - Membres avec :
 *          - Type qui dispose le numéro 5 pour les options.
 *          - Titre qui dispose le numéro 6 pour les options.
 *      - Emprunt avec :
 *          - Type qui dispose le numéro 7 pour les options.
 *      - Informations de la ludothèque avec :
 *          - Lieux qui dispose le numéro 8 pour les options.
 *
 *  Fabrication   Qt Creator, projet    .pro
 *
 *  @todo       Classe presque terminé, manque la liaison avec le F_Mainwindow.
 */
//------------------------------------------------------------------------------

// En-têtes standards (ATTENTION : garder toujours le meme ordre) --------------
#include "f_clesttem.h"
#include "ui_f_clesttem.h"
//------------------------------------------------------------------------------

/**
 *  @brief  Constructeur de la classe.
 *
 *  @param  parent
 *
 *  @see    MettreAJourBDD()
 */
F_CLESTTEM::F_CLESTTEM(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::F_CLESTTEM)
{
    ui->setupUi(this);

    this->pEtatJeuAjMod = new F_PopUpCLESTTEM(0);
    this->pEtatJeuAjMod->setWindowModality(Qt::ApplicationModal);

    this->pStatutJeuAjMod = new F_PopUpCLESTTEM(1);
    this->pStatutJeuAjMod->setWindowModality(Qt::ApplicationModal);

    this->pEmplacementJeuAjMod = new F_PopUpCLESTTEM(2);
    this->pEmplacementJeuAjMod->setWindowModality(Qt::ApplicationModal);

    this->pTypeJeuAjMod = new F_PopUpCLESTTEM(3);
    this->pTypeJeuAjMod->setWindowModality(Qt::ApplicationModal);

    this->pTypeMembreAjMod = new F_PopUpCLESTTEM(5);
    this->pTypeMembreAjMod->setWindowModality(Qt::ApplicationModal);

    this->pTitreMembreAjMod = new F_PopUpCLESTTEM(6);
    this->pTitreMembreAjMod->setWindowModality(Qt::ApplicationModal);

    this->pTypeEmpruntAjMod = new F_PopUpCLESTTEM(7);
    this->pTypeEmpruntAjMod->setWindowModality(Qt::ApplicationModal);

    this->pLieuxInfoLudoAjMod = new F_PopUpCLESTTEM(8);
    this->pLieuxInfoLudoAjMod->setWindowModality(Qt::ApplicationModal);

    /*---------------------------Création des tableaux----------------------------*/
    this->TbTitreMembre = new QStandardItemModel();
    ui->TbV_TitreMembre->setModel(this->TbTitreMembre);
    ui->TbV_TitreMembre->setEditTriggers(0);
    ui->TbV_TitreMembre->resizeColumnsToContents();
    ui->TbV_TitreMembre->verticalHeader()->setVisible(false);

    this->TbTypeMembre = new QStandardItemModel();
    ui->TbV_TypeMembre->setModel(this->TbTypeMembre);
    ui->TbV_TypeMembre->setEditTriggers(0);
    ui->TbV_TypeMembre->resizeColumnsToContents();
    ui->TbV_TypeMembre->verticalHeader()->setVisible(false);

    this->TbEmplacementJeux = new QStandardItemModel();
    ui->TbV_EmplacementJeux->setModel(this->TbEmplacementJeux);
    ui->TbV_EmplacementJeux->setEditTriggers(0);
    ui->TbV_EmplacementJeux->resizeColumnsToContents();
    ui->TbV_EmplacementJeux->verticalHeader()->setVisible(false);

    this->TbEtatJeux = new QStandardItemModel();
    ui->TbV_EtatJeux->setModel(this->TbEtatJeux);
    ui->TbV_EtatJeux->setEditTriggers(0);
    ui->TbV_EtatJeux->resizeColumnsToContents();
    ui->TbV_EtatJeux->verticalHeader()->setVisible(false);

    this->TbStatutJeux = new QStandardItemModel();
    ui->TbV_StatutJeux->setModel(this->TbStatutJeux);
    ui->TbV_StatutJeux->setEditTriggers(0);
    ui->TbV_StatutJeux->resizeColumnsToContents();
    ui->TbV_StatutJeux->verticalHeader()->setVisible(false);

    this->TbTypeJeux = new QStandardItemModel();
    ui->TbV_TypeJeux->setModel(this->TbTypeJeux);
    ui->TbV_TypeJeux->setEditTriggers(0);
    ui->TbV_TypeJeux->resizeColumnsToContents();
    ui->TbV_TypeJeux->verticalHeader()->setVisible(false);

    this->TbTypeEmprunt= new QStandardItemModel();
    ui->TbV_TypeEmprunt->setModel(this->TbTypeEmprunt);
    ui->TbV_TypeEmprunt->setEditTriggers(0);
    ui->TbV_TypeEmprunt->resizeColumnsToContents();
    ui->TbV_TypeEmprunt->verticalHeader()->setVisible(false);

    this->TbLieuxLudo= new QStandardItemModel();
    ui->TbV_LieuxLudo->setModel(this->TbLieuxLudo);
    ui->TbV_LieuxLudo->setEditTriggers(0);
    ui->TbV_LieuxLudo->resizeColumnsToContents();
    ui->TbV_LieuxLudo->verticalHeader()->setVisible(false);

    /*----------------------------------Connect-----------------------------------*/
    connect(this->pTitreMembreAjMod, SIGNAL(SignalValider()), this, SLOT(slot_Valider()));
    connect(this->pTypeMembreAjMod, SIGNAL(SignalValider()), this, SLOT(slot_Valider()));
    connect(this->pEmplacementJeuAjMod, SIGNAL(SignalValider()), this, SLOT(slot_Valider()));
    connect(this->pEtatJeuAjMod, SIGNAL(SignalValider()), this, SLOT(slot_Valider()));
    connect(this->pStatutJeuAjMod, SIGNAL(SignalValider()), this, SLOT(slot_Valider()));
    connect(this->pTypeJeuAjMod, SIGNAL(SignalValider()), this, SLOT(slot_Valider()));
    connect(this->pTypeEmpruntAjMod, SIGNAL(SignalValider()), this, SLOT(slot_Valider()));
    connect(this->pLieuxInfoLudoAjMod, SIGNAL(SignalValider()), this, SLOT(slot_Valider()));

    this->MettreAJourBDD();
}

/**
 *  @brief  Destructeur de la classe.
 *
 */
F_CLESTTEM::~F_CLESTTEM()
{
    delete ui;
}

/**
 *  @brief  Slot qui récupère un signal pour la classe F_Preferences.
 *
 */
void F_CLESTTEM::slot_Valider()
{
    this->MettreAJourBDD();
}

/**
 *  @brief  Fonction qui permet de mettre à  jour le tableau contenant toutes les informations de la BDD.
 *
 */
void F_CLESTTEM::MettreAJourBDD()
{
    QSqlQuery RechercheTableau;
    int nNombreLigne(0);

    //Affichage de la BDD dans le tableau titre qui est dans l'onglet Membres de F_CLESTTEM.
    this->TbTitreMembre->setColumnCount(2);
    this->TbTitreMembre->setRowCount(nNombreLigne);
    this->TbTitreMembre->setHorizontalHeaderItem(0, new QStandardItem("Titre Membre"));
    this->TbTitreMembre->setHorizontalHeaderItem(1, new QStandardItem("Emprunt"));
    ui->TbV_TitreMembre->setColumnWidth(0, 100);
    ui->TbV_TitreMembre->setColumnWidth(1, 65);

    RechercheTableau.exec("SELECT NomTitre, NbrJeuxEmpruntable FROM titremembre");
    while (RechercheTableau.next())
    {
        this->TbTitreMembre->setItem(nNombreLigne, 0, new QStandardItem(RechercheTableau.value(0).toString()));
        this->TbTitreMembre->setItem(nNombreLigne, 1, new QStandardItem(RechercheTableau.value(1).toString()));
        nNombreLigne = nNombreLigne+ 1;
    }
    nNombreLigne = 0;

    //Affichage de la BDD dans le tableau type qui est dans l'onglet Membres de F_CLESTTEM.
    this->TbTypeMembre->setColumnCount(1);
    this->TbTypeMembre->setRowCount(nNombreLigne);
    this->TbTypeMembre->setHorizontalHeaderItem(0, new QStandardItem("Type Membre"));
    ui->TbV_TypeMembre->setColumnWidth(0, 150);

    RechercheTableau.exec("SELECT TypeMembre FROM typemembres");
    while (RechercheTableau.next())
    {
        this->TbTypeMembre->setItem(nNombreLigne, 0, new QStandardItem(RechercheTableau.value(0).toString()));
        nNombreLigne = nNombreLigne+ 1;
    }
    nNombreLigne = 0;

    //Affichage de la BDD dans le tableau paiement qui est dans l'onglet Membres de F_CLESTTEM.
    this->TbPaiementMembre->setColumnCount(1);
    this->TbPaiementMembre->setRowCount(nNombreLigne);
    this->TbPaiementMembre->setHorizontalHeaderItem(0, new QStandardItem("Paiement"));
    ui->TbV_PaiementMembre->setColumnWidth(0, 150);

    RechercheTableau.exec("SELECT NomPaiement FROM modepaiement");
    while (RechercheTableau.next())
    {
        this->TbPaiementMembre->setItem(nNombreLigne, 0, new QStandardItem(RechercheTableau.value(0).toString()));
        nNombreLigne = nNombreLigne+ 1;
    }
    nNombreLigne = 0;

    //Affichage de la BDD dans le tableau emplacement qui est dans l'onglet Jeux de F_CLESTTEM.
    this->TbEmplacementJeux->setColumnCount(1);
    this->TbEmplacementJeux->setRowCount(nNombreLigne);
    this->TbEmplacementJeux->setHorizontalHeaderItem(0, new QStandardItem("Emplacement"));
    ui->TbV_EmplacementJeux->setColumnWidth(0, 180);

    RechercheTableau.exec("SELECT Nom FROM emplacement");
    while (RechercheTableau.next())
    {
        this->TbEmplacementJeux->setItem(nNombreLigne, 0, new QStandardItem(RechercheTableau.value(0).toString()));
        nNombreLigne = nNombreLigne+ 1;
    }
    nNombreLigne=0;

    //Affichage de la BDD dans le tableau etat qui est dans l'onglet Jeux de F_CLESTTEM.
    this->TbEtatJeux->setColumnCount(1);
    this->TbEtatJeux->setRowCount(nNombreLigne);
    this->TbEtatJeux->setHorizontalHeaderItem(0, new QStandardItem("Etat"));
    ui->TbV_EtatJeux->setColumnWidth(0, 180);

    RechercheTableau.exec("SELECT Etat FROM etatsjeu");
    while (RechercheTableau.next())
    {
        this->TbEtatJeux->setItem(nNombreLigne, 0, new QStandardItem(RechercheTableau.value(0).toString()));
        nNombreLigne = nNombreLigne+ 1;
    }
    nNombreLigne = 0;

    //Affichage de la BDD dans le tableau statut qui est dans l'onglet Jeux de F_CLESTTEM.
    this->TbStatutJeux->setColumnCount(1);
    this->TbStatutJeux->setRowCount(nNombreLigne);
    this->TbStatutJeux->setHorizontalHeaderItem(0, new QStandardItem("Statut"));
    ui->TbV_StatutJeux->setColumnWidth(0, 180);

    RechercheTableau.exec("SELECT StatutJeu FROM statutjeux");
    while (RechercheTableau.next())
    {
        this->TbStatutJeux->setItem(nNombreLigne, 0, new QStandardItem(RechercheTableau.value(0).toString()));
        nNombreLigne = nNombreLigne+ 1;
    }
    nNombreLigne = 0;

    //Affichage de la BDD dans le tableau type qui est dans l'onglet Jeux de F_CLESTTEM.
    this->TbTypeJeux->setColumnCount(2);
    this->TbTypeJeux->setRowCount(nNombreLigne);
    this->TbTypeJeux->setHorizontalHeaderItem(0, new QStandardItem("Type Jeu"));
    this->TbTypeJeux->setHorizontalHeaderItem(1, new QStandardItem("Classification"));
    ui->TbV_TypeJeux->setColumnWidth(0, 180);
    ui->TbV_TypeJeux->setColumnWidth(1, 90);

    RechercheTableau.exec("SELECT * FROM typejeux");
    while (RechercheTableau.next())
    {
        this->TbTypeJeux->setItem(nNombreLigne, 0, new QStandardItem(RechercheTableau.value(0).toString()));
        this->TbTypeJeux->setItem(nNombreLigne, 1, new QStandardItem(RechercheTableau.value(1).toString()));
        nNombreLigne = nNombreLigne+ 1;
    }
    nNombreLigne = 0;


    //Affichage de la BDD dans le tableau type qui est dans l'onglet Emprunt/Informations Ludo de F_CLESTTEM.
    this->TbTypeEmprunt->setColumnCount(1);
    this->TbTypeEmprunt->setRowCount(nNombreLigne);
    this->TbTypeEmprunt->setHorizontalHeaderItem(0, new QStandardItem("Type Emprunt"));
    this->TbTypeEmprunt->setHorizontalHeaderItem(1, new QStandardItem("Durée"));
    ui->TbV_EmplacementJeux->setColumnWidth(0, 180);
    ui->TbV_EmplacementJeux->setColumnWidth(1, 90);

    RechercheTableau.exec("SELECT TypeEmprunt, DureeEmprunt FROM typeemprunt");
    while (RechercheTableau.next())
    {
        this->TbTypeEmprunt->setItem(nNombreLigne, 0, new QStandardItem(RechercheTableau.value(0).toString()));
        this->TbTypeEmprunt->setItem(nNombreLigne, 1, new QStandardItem(RechercheTableau.value(1).toString()));
        nNombreLigne = nNombreLigne+ 1;
    }
    nNombreLigne=0;

    //Affichage de la BDD dans le tableau lieu qui est dans l'onglet Emprunt/Informations Ludo de F_CLESTTEM.
    this->TbLieuxLudo->setColumnCount(1);
    this->TbLieuxLudo->setRowCount(nNombreLigne);
    this->TbLieuxLudo->setHorizontalHeaderItem(0, new QStandardItem("Lieu"));
    ui->TbV_LieuxLudo->setColumnWidth(0, 180);

    RechercheTableau.exec("SELECT NomLieux FROM lieux");
    while (RechercheTableau.next())
    {
        this->TbLieuxLudo->setItem(nNombreLigne, 0, new QStandardItem(RechercheTableau.value(0).toString()));
        nNombreLigne = nNombreLigne+ 1;
    }
}

//------------------------------------------------------------------------
//--------------------------------Membres---------------------------------
//------------------------------------------------------------------------

/**********************************Titre*********************************/
/**
 *  @brief Sélectionne la ligne entière du tableau titre de membres.
 *
 *  @param index
 */
void F_CLESTTEM::on_TbV_TitreMembre_clicked(const QModelIndex &index)
{
    ui->TbV_TitreMembre->selectRow(index.row());
    this->sTitreMembre = this->TbTitreMembre->index(index.row(), 0).data().toString();
}

/**
 *  @brief Permet d'ajouter un titre d'un membre.
 *
 */
void F_CLESTTEM::on_Bt_AjouterTitreMembre_clicked()
{
    this->pTitreMembreAjMod->Ajouter();
}

/**
 *  @brief Permet de modifier un titre d'un membre.
 *
 */
void F_CLESTTEM::on_Bt_ModifierTitreMembre_clicked()
{
    QSqlQuery RequeteModifier;

    RequeteModifier.prepare("SELECT NomTitre FROM titremembre WHERE NomTitre=:NomTitre");
    RequeteModifier.bindValue(":NomTitre", this->sTitreMembre);
    RequeteModifier.exec();
    RequeteModifier.next();

    this->pTitreMembreAjMod->Modifier(RequeteModifier.value(0).toString());
}

/**
 *  @brief Permet de supprimer un titre d'un membre.
 *
 */
void F_CLESTTEM::on_Bt_SupprimerTitreMembre_clicked()
{
    QSqlQuery RequeteSupprimer;

    if(QMessageBox::question(this, "Suppression", "Etes-vous sur de vouloir supprimer ce titre de membre ?", "Oui", "Non") == 0)
    {
        RequeteSupprimer.prepare("DELETE FROM titremembre WHERE NomTitre=:NomTitre");
        RequeteSupprimer.bindValue(":NomTitre", this->sTitreMembre);
        RequeteSupprimer.exec();
        RequeteSupprimer.next();
    }

    this->MettreAJourBDD();
}

/**********************************Type**********************************/
/**
 *  @brief Sélectionne la ligne entière du tableau type de membres.
 *
 *  @param index
 */
void F_CLESTTEM::on_TbV_TypeMembre_clicked(const QModelIndex &index)
{
    ui->TbV_TypeMembre->selectRow(index.row());
    this->sTypeMembre = this->TbTypeMembre->index(index.row(), 0).data().toString();
}

/**
 *  @brief Permet d'ajouter un type d'un membre.
 *
 */
void F_CLESTTEM::on_Bt_AjouterTypeMembre_clicked()
{
    this->pTypeMembreAjMod->Ajouter();
}

/**
 *  @brief Permet de modifier un type d'un membre.
 *
 */
void F_CLESTTEM::on_Bt_ModifierTypeMembre_clicked()
{
    QSqlQuery RequeteModifier;

    RequeteModifier.prepare("SELECT TypeMembre FROM typemembres WHERE TypeMembre=:TypeMembre");
    RequeteModifier.bindValue(":TypeMembre", this->sTypeMembre);
    RequeteModifier.exec();
    RequeteModifier.next();

    this->pTypeMembreAjMod->Modifier(RequeteModifier.value(0).toString());
}

/**
 *  @brief Permet de supprimer un type d'un membre.
 *
 */
void F_CLESTTEM::on_Bt_SupprimerTypeMembre_clicked()
{
    QSqlQuery RequeteSupprimer;

    if(QMessageBox::question(this, "Suppression", "Etes-vous sur de vouloir supprimer ce type de membre ?", "Oui", "Non") == 0)
    {
        RequeteSupprimer.prepare("DELETE FROM typemembres WHERE TypeMembre=:TypeMembre");
        RequeteSupprimer.bindValue(":TypeMembre", this->sTypeMembre);
        RequeteSupprimer.exec();
        RequeteSupprimer.next();
    }

    this->MettreAJourBDD();
}

//------------------------------------------------------------------------
//---------------------------------Jeux-----------------------------------
//------------------------------------------------------------------------

/******************************Emplacement*******************************/
/**
 *  @brief Sélectionne la ligne entière du tableau emplacement des jeux.
 *
 *  @param index
 */
void F_CLESTTEM::on_TbV_EmplacementJeux_clicked(const QModelIndex &index)
{
    ui->TbV_EmplacementJeux->selectRow(index.row());
    this->sEmplacementJeux = this->TbEmplacementJeux->index(index.row(), 0).data().toString();
}

/**
 *  @brief Permet d'ajouter un emplacement de jeu.
 *
 */
void F_CLESTTEM::on_Bt_AjouterEmplacementJeux_clicked()
{
    this->pEmplacementJeuAjMod->Ajouter();
}

/**
 *  @brief Permet de modifier un emplacement de jeu.
 *
 */
void F_CLESTTEM::on_Bt_ModifierEmplacementJeux_clicked()
{
    QSqlQuery RequeteModifier;

    RequeteModifier.prepare("SELECT Nom FROM emplacement WHERE Nom=:Nom");
    RequeteModifier.bindValue(":Nom", this->sEmplacementJeux);
    RequeteModifier.exec();
    RequeteModifier.next();

    this->pEmplacementJeuAjMod->Modifier(RequeteModifier.value(0).toString());
}

/**
 *  @brief Permet de supprimer un emplacement de jeu.
 *
 */
void F_CLESTTEM::on_Bt_SupprimerEmplacementJeux_clicked()
{
    QSqlQuery RequeteSupprimer;

    if(QMessageBox::question(this, "Suppression", "Etes-vous sur de vouloir supprimer cette emplacement de jeu ?", "Oui", "Non") == 0)
    {
        RequeteSupprimer.prepare("DELETE FROM emplacement WHERE Nom=:Nom");
        RequeteSupprimer.bindValue(":StatutJeu", this->sEmplacementJeux);
        RequeteSupprimer.exec();
    }

    this->MettreAJourBDD();
}

/**********************************Etat**********************************/
/**
 *  @brief Sélectionne la ligne entière du tableau état des jeux.
 *
 *  @param index
 */
void F_CLESTTEM::on_TbV_EtatJeux_clicked(const QModelIndex &index)
{
    ui->TbV_EtatJeux->selectRow(index.row());
    this->sEtatJeux = this->TbEtatJeux->index(index.row(), 0).data().toString();
}

/**
 *  @brief Permet d'ajouter un état de jeu.
 *
 */
void F_CLESTTEM::on_Bt_AjouterEtatJeux_clicked()
{
    this->pEtatJeuAjMod->Ajouter();
}

/**
 *  @brief Permet de modifier un état de jeu.
 *
 */
void F_CLESTTEM::on_Bt_ModifierEtatJeux_clicked()
{
    QSqlQuery RequeteModifier;

    RequeteModifier.prepare("SELECT Etat FROM etatsjeu WHERE Etat=:Etat");
    RequeteModifier.bindValue(":Etat", this->sEtatJeux);
    RequeteModifier.exec();
    RequeteModifier.next();

    this->pEtatJeuAjMod->Modifier(RequeteModifier.value(0).toString());
}

/**
 *  @brief Permet de supprimer un état de jeu.
 *
 */
void F_CLESTTEM::on_Bt_SupprimerEtatJeux_clicked()
{
    QSqlQuery RequeteSupprimer;

    if(QMessageBox::question(this, "Suppression", "Etes-vous sur de vouloir supprimer cet état de jeu ?", "Oui", "Non") == 0)
    {
        RequeteSupprimer.prepare("DELETE FROM etatsjeu WHERE Etat=:Etat");
        RequeteSupprimer.bindValue(":Etat", this->sEtatJeux);
        RequeteSupprimer.exec();
    }

    this->MettreAJourBDD();
}

/*********************************Statut*********************************/
/**
 *  @brief Sélectionne la ligne entière du tableau statut des jeux.
 *
 *  @param index
 */
void F_CLESTTEM::on_TbV_StatutJeux_clicked(const QModelIndex &index)
{
    ui->TbV_StatutJeux->selectRow(index.row());
    this->sStatutJeux = this->TbStatutJeux->index(index.row(), 0).data().toString();
    if(index.row() < 4)
    {
        ui->Bt_ModifierStatutJeux->setEnabled(false);
        ui->Bt_SupprimerStatutJeux->setEnabled(false);
    }
    else
    {
        ui->Bt_ModifierStatutJeux->setEnabled(true);
        ui->Bt_SupprimerStatutJeux->setEnabled(true);
    }
}

/**
 *  @brief Permet d'ajouter un statut de jeu.
 *
 */
void F_CLESTTEM::on_Bt_AjouterStatutJeux_clicked()
{
    this->pStatutJeuAjMod->Ajouter();
}

/**
 *  @brief Permet de modifier un statut de jeu.
 *
 */
void F_CLESTTEM::on_Bt_ModifierStatutJeux_clicked()
{
    QSqlQuery RequeteModifier;

    RequeteModifier.prepare("SELECT StatutJeu FROM statutjeux WHERE StatutJeu=:StatutJeu");
    RequeteModifier.bindValue(":StatutJeu", this->sStatutJeux);
    RequeteModifier.exec();
    RequeteModifier.next();

    this->pStatutJeuAjMod->Modifier(RequeteModifier.value(0).toString());
}

/**
 *  @brief Permet de supprimer un statut de jeu.
 *
 */
void F_CLESTTEM::on_Bt_SupprimerStatutJeux_clicked()
{
    QSqlQuery RequeteSupprimer;

    if(QMessageBox::question(this, "Suppression", "Etes-vous sur de vouloir supprimer ce statut de jeu ?", "Oui", "Non") == 0)
    {
        RequeteSupprimer.prepare("DELETE FROM statutjeux WHERE StatutJeu=:StatutJeu");
        RequeteSupprimer.bindValue(":StatutJeu", this->sStatutJeux);
        RequeteSupprimer.exec();
    }

    this->MettreAJourBDD();
}

/**********************************Type**********************************/
/**
 *  @brief Sélectionne la ligne entière du tableau type des jeux.
 *
 *  @param index
 */
void F_CLESTTEM::on_TbV_TypeJeux_clicked(const QModelIndex &index)
{
    ui->TbV_TypeJeux->selectRow(index.row());
    this->sTypeJeux = this->TbTypeJeux->index(index.row(), 0).data().toString();
}

/**
 *  @brief Permet d'ajouter un type de jeu.
 *
 */
void F_CLESTTEM::on_Bt_AjouterTypeJeux_clicked()
{
    this->pTypeJeuAjMod->Ajouter();
}

/**
 *  @brief Permet de modifier un type de jeu.
 *
 */
void F_CLESTTEM::on_Bt_ModifierTypeJeux_clicked()
{
    QSqlQuery RequeteModifier;

    RequeteModifier.prepare("SELECT TypeJeux FROM typejeux WHERE TypeJeux=:TypeJeux");
    RequeteModifier.bindValue(":TypeJeux", this->sTypeJeux);
    RequeteModifier.exec();
    RequeteModifier.next();

    this->pTypeJeuAjMod->Modifier(RequeteModifier.value(0).toString());
}

/**
 *  @brief Permet de supprimer un type de jeu.
 *
 */
void F_CLESTTEM::on_Bt_SupprimerTypeJeux_clicked()
{
    QSqlQuery RequeteSupprimer;

    if(QMessageBox::question(this, "Suppression", "Etes-vous sur de vouloir supprimer ce type de jeu ?", "Oui", "Non") == 0)
    {
        RequeteSupprimer.prepare("DELETE FROM typejeux WHERE TypeJeux=:TypeJeux");
        RequeteSupprimer.bindValue(":NomTitre", this->sTypeJeux);
        RequeteSupprimer.exec();
    }

    this->MettreAJourBDD();
}

//------------------------------------------------------------------------
//--------------------------------Emprunt---------------------------------
//------------------------------------------------------------------------

/**********************************Type**********************************/
/**
 *  @brief Sélectionne la ligne entière du tableau type d'emprunt.
 *
 *  @param index
 */
void F_CLESTTEM::on_TbV_TypeEmprunt_clicked(const QModelIndex &index)
{
    ui->TbV_TypeEmprunt->selectRow(index.row());
    this->sTypeEmprunt = this->TbTypeEmprunt->index(index.row(), 0).data().toString();
}

/**
 *  @brief Permet d'ajouter un type d'emprunt.
 *
 */
void F_CLESTTEM::on_Bt_AjouterTypeEmprunt_clicked()
{
    this->pTypeEmpruntAjMod->Ajouter();
}

/**
 *  @brief Permet de modifier un type d'emprunt.
 *
 */
void F_CLESTTEM::on_Bt_ModifierTypeEmprunt_clicked()
{
    QSqlQuery RequeteModifier;

    RequeteModifier.prepare("SELECT TypeEmprunt FROM typeemprunt WHERE TypeEmprunt=:TypeEmprunt");
    RequeteModifier.bindValue(":TypeEmprunt", this->sTypeEmprunt);
    RequeteModifier.exec();
    RequeteModifier.next();

    this->pTypeEmpruntAjMod->Modifier(RequeteModifier.value(0).toString());
}

/**
 *  @brief Permet de supprimer un type d'emprunt.
 *
 */
void F_CLESTTEM::on_Bt_SupprimerTypeEmprunt_clicked()
{
    QSqlQuery RequeteSupprimer;

    if(QMessageBox::question(this, "Suppression", "Etes-vous sur de vouloir supprimer ce type d'emprunt ?", "Oui", "Non") == 0)
    {
        RequeteSupprimer.prepare("DELETE FROM typeemprunt WHERE TypeEmprunt=:TypeEmprunt");
        RequeteSupprimer.bindValue(":TypeEmprunt", this->sTypeEmprunt);
        RequeteSupprimer.exec();
    }

    this->MettreAJourBDD();
}

//------------------------------------------------------------------------
//----------------------------Information Ludo----------------------------
//------------------------------------------------------------------------

/*********************************Lieux**********************************/
/**
 *  @brief Sélectionne la ligne entière du tableau lieu de la ludothèque.
 *
 *  @param index
 */
void F_CLESTTEM::on_TbV_LieuxLudo_clicked(const QModelIndex &index)
{
    ui->TbV_LieuxLudo->selectRow(index.row());
    this->sLieuxLudo = this->TbLieuxLudo->index(index.row(), 0).data().toString();
}

/**
 *  @brief Permet d'ajouter un lieu de la ludothèque.
 *
 */
void F_CLESTTEM::on_Bt_AjouterLieuxLudo_clicked()
{
     this->pLieuxInfoLudoAjMod->Ajouter();
}

/**
 *  @brief Permet de modifier un lieu de la ludothèque.
 *
 */
void F_CLESTTEM::on_Bt_ModifierLieuxLudo_clicked()
{
    QSqlQuery RequeteModifier;

    RequeteModifier.prepare("SELECT NomLieux FROM lieux WHERE NomLieux=:NomLieux");
    RequeteModifier.bindValue(":NomLieux", this->sLieuxLudo);
    RequeteModifier.exec();
    RequeteModifier.next();

    this->pLieuxInfoLudoAjMod->Modifier(RequeteModifier.value(0).toString());
}

/**
 *  @brief Permet de supprimer un lieu de la ludothèque.
 *
 */
void F_CLESTTEM::on_Bt_SpupprimerLieuxLudo_clicked()
{
    QSqlQuery RequeteSupprimer;

    if(QMessageBox::question(this, "Suppression", "Etes-vous sur de vouloir supprimer ce lieu de l'info ludo ?", "Oui", "Non") == 0)
    {
        RequeteSupprimer.prepare("DELETE FROM lieux WHERE NomLieux=:NomLieux");
        RequeteSupprimer.bindValue(":NomLieux", this->sLieuxLudo);
        RequeteSupprimer.exec();
    }

    this->MettreAJourBDD();
}

void F_CLESTTEM::on_TbV_PaiementMembre_clicked(const QModelIndex &index)
{
    ui->TbV_PaiementMembre->selectRow(index.row());
    this->sPaiementMembre = this->TbPaiementMembre->index(index.row(), 0).data().toString();
}

void F_CLESTTEM::on_Bt_AjouterPaiementMembre_clicked()
{
    this->pPaiementMembreAjMod->Ajouter();
}

void F_CLESTTEM::on_Bt_ModifierPaiementMembre_clicked()
{
    QSqlQuery RequeteModifier;

    RequeteModifier.prepare("SELECT NomPaiement FROM modepaiement WHERE NomPaiement=:NomPaiement");
    RequeteModifier.bindValue(":NomPaiement", this->sPaiementMembre);
    RequeteModifier.exec();
    RequeteModifier.next();

    this->pPaiementMembreAjMod->Modifier(RequeteModifier.value(0).toString());
}

void F_CLESTTEM::on_Bt_SupprimerPaiementMembre_clicked()
{
    QSqlQuery RequeteSupprimer;

    if(QMessageBox::question(this, "Suppression", "Etes-vous sur de vouloir supprimer ce paiement ?", "Oui", "Non") == 0)
    {
        RequeteSupprimer.prepare("DELETE FROM modepaiement WHERE NomPaiement=:NomPaiement");
        RequeteSupprimer.bindValue(":NomPaiement", this->sPaiementMembre);
        RequeteSupprimer.exec();
        RequeteSupprimer.next();
    }

    this->MettreAJourBDD();
}
