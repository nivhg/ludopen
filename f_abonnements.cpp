//------------------------------------------------------------------------------
/** @file       f_abonnements.cpp
 *  @brief      F_Abonnements permet la gestion des abonnements dans la partie "Administration".
 *
 *  @author     NORMAND Julien
 *  @author     STS IRIS, Lycée Nicolas APPERT, ORVAULT (FRANCE)
 *  @since      01/2012
 *  @version    1.0
 *  @date       12/06/2012
 *
 *  Les options disponibles dans cette classe sont:
 *      - l'ajout d'un abonnement.
 *      - la suppression d'un abonnement.
 *      - la modification d'un abonnement.
 *      - la consultaion d'un abonnement.
 *
 *  Fabrication   Qt Creator, projet    .pro
 *
 *  @todo       Classe terminée.
 */
//------------------------------------------------------------------------------

// En-têtes standards (ATTENTION : garder toujours le meme ordre) --------------
#include <QtGui>
#include <QtSql>
#include <iostream>
using namespace std;
//------------------------------------------------------------------------------

// En-tête propre à l'objet ----------------------------------------------------
#include "f_abonnements.h"
#include "ui_f_abonnements.h"
//------------------------------------------------------------------------------

/**
 *  @brief  Constructeur de la classe.
 *
 *  @param  parent
 *  @see    MettreAJourBDD()
 */
F_Abonnements::F_Abonnements(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::F_Abonnements)
{
    QRegExp Prix("[\\d*]{1,6}\\.[\\d+]{1,2}");
    QRegExp Chiffre("[\\d*]{1,8}");
    QValidator * BlocagePrix = new QRegExpValidator(Prix, this);
    QValidator * BlocageChiffre = new QRegExpValidator(Chiffre, this);

    ui->setupUi(this);
    ui->LE_Nom->setReadOnly(true);
    ui->LE_Duree->setReadOnly(true);
    ui->LE_Prix->setReadOnly(true);
    ui->LE_Credit->setReadOnly(true);
    ui->LE_Duree->setValidator(BlocageChiffre);
    ui->LE_Credit->setValidator(BlocageChiffre);
    ui->LE_Prix->setValidator(BlocagePrix);
    ui->Bt_Annuler->hide();
    ui->Bt_Valider->hide();

    this->TbRechercheModele = new QStandardItemModel();
    ui->TbV_Recherche->setModel(this->TbRechercheModele);
    ui->TbV_Recherche->setEditTriggers(0);
    ui->TbV_Recherche->resizeColumnsToContents();

    this->MettreAJourBDD();
}

/**
 *  @brief  Destructeur de la classe.
 *
 */
F_Abonnements::~F_Abonnements()
{
    delete ui;
}

//---------------------------------------------------------------------------
// METHODEs PRIVEEs
//---------------------------------------------------------------------------
/**
 *  @brief  Fonction qui permet de mettre à jour le tableau contenant toutes les informations de la BDD.
 *
 *  @pre    Etre connecté à la BDD.
 *
 *  @test   TestAjout.pro
 */
void F_Abonnements::MettreAJourBDD()
{
    QSqlQuery RechercheTableau;
    int nNombreLigne(0);

    this->TbRechercheModele->setColumnCount(5);
    this->TbRechercheModele->setRowCount(nNombreLigne);
    this->TbRechercheModele->setHorizontalHeaderItem(0, new QStandardItem("Type"));
    this->TbRechercheModele->setHorizontalHeaderItem(1, new QStandardItem("Nom"));
    this->TbRechercheModele->setHorizontalHeaderItem(2, new QStandardItem("Prix"));
    this->TbRechercheModele->setHorizontalHeaderItem(3, new QStandardItem("Duree de Validite"));
    this->TbRechercheModele->setHorizontalHeaderItem(4, new QStandardItem("Credit Disponible"));

    ui->TbV_Recherche->setColumnWidth(1, 300);
    ui->TbV_Recherche->setColumnWidth(2, 90);
    ui->TbV_Recherche->setColumnWidth(3, 130);
    ui->TbV_Recherche->setColumnWidth(4, 130);

    RechercheTableau.exec("SELECT * FROM cartesprepayees");
    while (RechercheTableau.next())
    {
        this->TbRechercheModele->setItem(nNombreLigne, 0, new QStandardItem("Carte"));
        this->TbRechercheModele->setItem(nNombreLigne, 1, new QStandardItem(RechercheTableau.value(1).toString()));
        this->TbRechercheModele->setItem(nNombreLigne, 2, new QStandardItem(RechercheTableau.value(3).toString()));
        this->TbRechercheModele->setItem(nNombreLigne, 3, new QStandardItem(RechercheTableau.value(2).toString()));
        this->TbRechercheModele->setItem(nNombreLigne, 4, new QStandardItem(RechercheTableau.value(4).toString()));
        nNombreLigne = nNombreLigne +1;
    }

    RechercheTableau.exec("SELECT * FROM prestations");
    while (RechercheTableau.next())
    {
        this->TbRechercheModele->setItem(nNombreLigne, 0, new QStandardItem("Prestation"));
        this->TbRechercheModele->setItem(nNombreLigne, 1, new QStandardItem(RechercheTableau.value(1).toString()));
        this->TbRechercheModele->setItem(nNombreLigne, 2, new QStandardItem(RechercheTableau.value(3).toString()));
        this->TbRechercheModele->setItem(nNombreLigne, 3, new QStandardItem(RechercheTableau.value(2).toString()));
        nNombreLigne = nNombreLigne +1;
    }
}

/**
 *  @brief  Fonction qui exécute la requête SQL "UPDATE pour une prestation.
 *
 *  @pre    Etre connecté à la BDD.
 *
 *  @param  sTexte
 *  @param  nDuree
 *  @param  fPrix
 */
void F_Abonnements::MettreAJourPrestation(QString sTexte, unsigned int nDuree, float fPrix)
{
    QSqlQuery Requete;

    Requete.prepare("UPDATE prestations SET NomPrestation=:NomPrestation, DureeValidite=:DureeValidite, Prix=:Prix WHERE NomPrestation=:NomPrestationPrecedent");
    Requete.bindValue(":NomPrestationPrecedent", sNomPrestationPrecedent);
    Requete.bindValue(":NomPrestation", sTexte);
    Requete.bindValue(":DureeValidite", nDuree);
    Requete.bindValue(":Prix", fPrix);
    if(!Requete.exec())
    {
        cerr << "L'insertion dans la base de donnée a échoué" << endl;
    }
}

/**
 *  @brief  Fonction qui exécute la requête SQL "UPDATE" pour une carte prépayée.
 *
 *  @pre    Etre connecté à la BDD.
 *
 *  @param  sTexte
 *  @param  nDuree
 *  @param  fPrix
 *  @param  nCredit
 */
void F_Abonnements::MettreAJourCartePrepayee(QString sTexte, unsigned int nDuree, float fPrix, unsigned int nCredit)
{
    QSqlQuery Requete;

    Requete.prepare("UPDATE cartesprepayees SET NomCarte=:NomCarte, DureeValidite=:DureeValidite, Prix=:Prix, CreditDisponible=:CreditDisponible WHERE NomCarte=:NomCartePrecedent");
    Requete.bindValue(":NomCartePrecedent", sNomCartePrecedent);
    Requete.bindValue(":NomCarte", sTexte);
    Requete.bindValue(":DureeValidite", nDuree);
    Requete.bindValue(":Prix", fPrix);
    Requete.bindValue(":CreditDisponible", nCredit);
    if(!Requete.exec())
    {
        cerr << "L'insertion dans la base de donnée a échoué" << endl;
    }
}

/**
 *  @brief  Fonction qui exécute la requête SQL "INSERT INTO" pour une prestation.
 *
 *  @pre    Etre connecté à la BDD.
 *
 *  @param  sTexte
 *  @param  nDuree
 *  @param  fPrix
 */
void F_Abonnements::CreerPrestation(QString sTexte, unsigned int nDuree, float fPrix)
{
    QSqlQuery Requete;

    Requete.prepare("INSERT INTO prestations (NomPrestation, DureeValidite, Prix) VALUES (:NomPrestation, :DureeValidite, :Prix)");
    Requete.bindValue(":NomPrestation", sTexte);
    Requete.bindValue(":DureeValidite", nDuree);
    Requete.bindValue(":Prix", fPrix);
    if(!Requete.exec())
    {
        cerr << "L'insertion dans la base de donnée a échoué" << endl;
    }
}

/**
 *  @brief  Fonction qui exécute la requête SQL "INSERT INTO" pour une carte prépayée.
 *
 *  @pre    Etre connecté à la BDD.
 *
 *  @param  sTexte
 *  @param  nDuree
 *  @param  fPrix
 *  @param  nCredit
 */
void F_Abonnements::CreerCartePrepayee(QString sTexte, unsigned int nDuree, float fPrix, unsigned int nCredit)
{
    QSqlQuery Requete;

    Requete.prepare("INSERT INTO cartesprepayees (NomCarte, DureeValidite, Prix, CreditDisponible) VALUES (:NomCarte, :DureeValidite, :Prix, :CreditDisponible)");
    Requete.bindValue(":NomCarte", sTexte);
    Requete.bindValue(":DureeValidite", nDuree);
    Requete.bindValue(":Prix", fPrix);
    Requete.bindValue(":CreditDisponible", nCredit);
    if(!Requete.exec())
    {
        cerr << "L'insertion dans la base de données a échoué" << endl;
    }
}

/**
 *  @brief  Fonction qui permet de mettre les informations des abonnements dans les lines edits.
 *
 *  @pre    Etre connecté à la BDD.
 *
 *  @param  sTexte
 */
void F_Abonnements::AfficherDetailAbonnement(QString sTexte)
{
    QSqlQuery Requete;

    Requete.prepare("SELECT NomCarte, DureeValidite, Prix, CreditDisponible FROM cartesprepayees WHERE NomCarte=:NomCarte");
    Requete.bindValue(":NomCarte", sTexte);
    Requete.exec();
    Requete.next();
    if (Requete.isValid()==true)
    {
        ui->RBt_CartePrepayee->setChecked(true);
        ui->LE_Credit->setEnabled(true);
    }
    else
    {
        Requete.prepare("SELECT NomPrestation, DureeValidite, Prix FROM prestations WHERE NomPrestation=:NomPrestation");
        Requete.bindValue(":NomPrestation", sTexte);
        Requete.exec();
        Requete.next();
        if(Requete.isValid()==true)
        {
            ui->RBt_Prestation->setChecked(true);
            ui->LE_Credit->setEnabled(false);
        }
    }

    QString NomCarte = Requete.value(0).toString();
    QString Duree = Requete.value(1).toString();
    QString Prix = Requete.value(2).toString();
    QString Credit = Requete.value(3).toString();

    ui->LE_Nom->setText(NomCarte);
    ui->LE_Duree->setText(Duree);
    ui->LE_Prix->setText(Prix);
    ui->LE_Credit->setText(Credit);
}

/**
 *  @brief   Fonction qui exécute la requête SQL "DELETE".
 *
 *  @pre     Etre connecté à la BDD.
 *
 *  @param   sTexte
 *  @param   nCredit
 */
void F_Abonnements::SupprimerAbonnement(QString sTexte, int nCredit)
{
    QSqlQuery Requete;

    Requete.prepare("DELETE FROM cartesprepayees WHERE NomCarte=:NomCarte AND CreditDisponible=:CreditDisponible");
    Requete.bindValue(":NomCarte", sTexte);
    Requete.bindValue(":CreditDisponible", nCredit);
    Requete.exec();
    Requete.next();
    if (Requete.isValid()!=true)
    {
        Requete.prepare("DELETE FROM prestations WHERE NomPrestation=:NomPrestation");
        Requete.bindValue(":NomPrestation", sTexte);
        Requete.exec();
        Requete.next();
    }
}

/**
 *  @brief  Fonction du clic sur le bouton "Recherche". Permet de faire une recherche depuis la BDD.
 *
 */
void F_Abonnements::on_Bt_Recherche_clicked()
{
    this->AfficherDetailAbonnement(ui->LE_Recherche->text());
}

/**
 *  @brief  Sélectionne la ligne entière du tableau.
 *
 *  @param  index
 */
void F_Abonnements::on_TbV_Recherche_clicked(const QModelIndex &index)
{
    ui->TbV_Recherche->selectRow(index.row());

    this->AfficherDetailAbonnement(this->TbRechercheModele->index(index.row(),1).data().toString());
}

/**
 *  @brief  Fonction du clic sur le bouton "Ajouter". Permet d'ajouter un abonnement.
 *
 */
void F_Abonnements::on_Bt_Ajouter_clicked()
{
    this->bEstUnNouvelAbo = true;

    ui->RBt_CartePrepayee->setEnabled(true);
    ui->RBt_Prestation->setEnabled(true);
    ui->RBt_CartePrepayee->setAutoExclusive(false);
    ui->RBt_Prestation->setAutoExclusive(false);
    ui->RBt_CartePrepayee->setChecked(false);
    ui->RBt_Prestation->setChecked(false);
    ui->RBt_CartePrepayee->setAutoExclusive(true);
    ui->RBt_Prestation->setAutoExclusive(true);

    ui->LE_Nom->setReadOnly(false);
    ui->LE_Duree->setReadOnly(false);
    ui->LE_Prix->setReadOnly(false);
    ui->LE_Credit->setReadOnly(false);

    ui->LE_Nom->clear();
    ui->LE_Duree->clear();
    ui->LE_Prix->clear();
    ui->LE_Credit->clear();
    ui->LE_Nom->setFocus(Qt::OtherFocusReason);

    ui->Bt_Annuler->show();
    ui->Bt_Valider->show();
    ui->Bt_Ajouter->hide();
    ui->Bt_Modifier->hide();
    ui->Bt_Supprimer->hide();
}

/**
 *  @brief  Fonction du clic sur le bouton "Modifier". Permet de modifier un abonnement.
 *
 */
void F_Abonnements::on_Bt_Modifier_clicked()
{
    //-----Selection pour la fontion void F_Abonnements::on_Bt_Valider_clicked() au niveau de la BDD-----
    this->bEstUnNouvelAbo = false;

    ui->RBt_CartePrepayee->setEnabled(false);
    ui->RBt_Prestation->setEnabled(false);

    ui->LE_Nom->setReadOnly(false);
    ui->LE_Duree->setReadOnly(false);
    ui->LE_Prix->setReadOnly(false);
    ui->LE_Credit->setReadOnly(false);
    ui->LE_Nom->setFocus(Qt::OtherFocusReason);

    if(ui->RBt_CartePrepayee->isChecked() == true)
    {
        sNomCartePrecedent = ui->LE_Nom->text();
    }
    else if (ui->RBt_Prestation->isChecked() == true)
    {
        sNomPrestationPrecedent = ui->LE_Nom->text();
    }

    ui->Bt_Annuler->show();
    ui->Bt_Valider->show();
    ui->Bt_Ajouter->hide();
    ui->Bt_Modifier->hide();
    ui->Bt_Supprimer->hide();
}

/**
 *  @brief  Fonction de clic sur le bouton "Valider". Permet d'enregister dans la BDD, les actions effectuées.
 *
 *  @pre    Les champs des lines edits doivent être remplis, et etre connecter à la BDD et avoir cliqué sur le bouton "Ajouter" ou "Modifier".
 *
 *  @test   TestAjout.pro
 *  @see    MettreAJourBDD()
 */
void F_Abonnements::on_Bt_Valider_clicked()
{
    QString sTexteValide;
    unsigned int nDureeValide;
    float fPrixValide;
    unsigned int nCreditValide;

    // Récupération des valeurs des LineEdit dans des variables.
    sTexteValide = ui->LE_Nom->text();
    nDureeValide = ui->LE_Duree->text().toInt();
    fPrixValide = ui->LE_Prix->text().toFloat();

    // SI-SINON pour savoir s'il s'agit d'une carte prépayée ou d'une prestation.
    if(ui->RBt_CartePrepayee->isChecked() == true)
    {
        // C'est une carte pré-payée
        nCreditValide = ui->LE_Credit->text().toInt();
        // SI-SINON pour savoir si les champs sont vides. Seul le prix peut ?tre nul
        if (sTexteValide.isEmpty()==true || nDureeValide==0 || nCreditValide==0)
        {
            QMessageBox::critical(this, "Message Erreur", "Les informations ne sont pas toutes remplies.");
        }
        else
        {
            // SI-SINON pour savoir s'il s'agit d'un nouvel abonnement ou pas.
            if(bEstUnNouvelAbo == true)
            {
                // Vérification si cette carte existe dans la BDD
                QSqlQuery Requete;

                Requete.prepare("SELECT COUNT(*) FROM  cartesprepayees WHERE NomCarte=:NomCarte");
                Requete.bindValue(":NomCarte", sTexteValide);
                if(!Requete.exec())
                {
                    cerr << "F_Abonnements::on_Bt_Valider_clicked() = Erreur d'acc?s ? la BDD" << endl;
                }
                else
                {
                    if (Requete.value(0).toInt() > 0)
                    {
                         QMessageBox::critical(this, "Donnée existante", "Une carte portant le m?me nom existe déj?. Merci de changer le nom.");
                    }
                    else  // C'est bien une nouvelle carte
                    {
                        this->CreerCartePrepayee(sTexteValide, nDureeValide, fPrixValide, nCreditValide);
                    }
                }
            }
            else
            {
                if(bEstUnNouvelAbo == false)
                {
                    this->MettreAJourCartePrepayee(sTexteValide, nDureeValide, fPrixValide, nCreditValide);
                }
            }
            ui->Bt_Annuler->hide();
            ui->Bt_Valider->hide();
            ui->Bt_Ajouter->show();
            ui->Bt_Modifier->show();
            ui->Bt_Supprimer->show();
        }
    }
    else
    {
        if (ui->RBt_Prestation->isChecked() == true)
        {
            // SI-SINON pour savoir si les champs sont vides.
            if (sTexteValide.isEmpty()==true || nDureeValide==0 || fPrixValide==0.0)
            {
                QMessageBox::critical(this, "Message Erreur", "Impossible d'inscrire dans la base de donnee, les champs ne sont pas tous remplis.");
            }
            else
            {
                // SI-SINON pour savoir s'il s'agit d'un nouvel abonnement ou pas.
                if(bEstUnNouvelAbo == true)
                {
                    this->CreerPrestation(sTexteValide, nDureeValide, fPrixValide);
                }
                else
                {
                    if(bEstUnNouvelAbo == false)
                    {
                        this->MettreAJourPrestation(sTexteValide, nDureeValide, fPrixValide);
                    }
                }
                ui->Bt_Annuler->hide();
                ui->Bt_Valider->hide();
                ui->Bt_Ajouter->show();
                ui->Bt_Modifier->show();
                ui->Bt_Supprimer->show();
            }
        }
        else
        {
            // SI pour erreur car aucun radio bouton de cocher.
            if(ui->RBt_CartePrepayee->isChecked()==false && ui->RBt_Prestation->isChecked()==false)
            {
                QMessageBox::critical(this, "Message Erreur", "Impossible d'inscrire dans la base de donnee, les champs ne sont pas tous remplis.");
            }
        }
    }

    ui->RBt_CartePrepayee->setEnabled(true);
    ui->RBt_Prestation->setEnabled(true);

    // Appel de la fonction pour actualiser le tableau.
    this->MettreAJourBDD();
}

/**
 *  @brief  Fonction du clic sur le bouton "Supprimer". Permet de supprimer un abonnement.
 *
 *  @see    MettreAJourBDD()
 */
void F_Abonnements::on_Bt_Supprimer_clicked()
{
    if(QMessageBox::question(this, "Suppression", "Etes-vous sur de vouloir supprimer cet abonnement ?", "Oui", "Non") == 0)
    {
        this->SupprimerAbonnement(ui->LE_Nom->text(), ui->LE_Credit->text().toInt());
    }

    this->MettreAJourBDD();
}

/**
 *  @brief  Fonction du clic sur le bouton "Annuler". Permet d'annuler toutes les actions effectuées.
 *
 *  @pre    Avoir cliquez sur le bouton "Ajouter" ou "Modifier".
 */
void F_Abonnements::on_Bt_Annuler_clicked()
{
    ui->RBt_CartePrepayee->setEnabled(true);
    ui->RBt_Prestation->setEnabled(true);
    ui->RBt_CartePrepayee->setAutoExclusive(false);
    ui->RBt_Prestation->setAutoExclusive(false);
    ui->RBt_CartePrepayee->setChecked(false);
    ui->RBt_Prestation->setChecked(false);
    ui->RBt_CartePrepayee->setAutoExclusive(true);
    ui->RBt_Prestation->setAutoExclusive(true);

    ui->LE_Credit->setEnabled(true);
    ui->LE_Nom->clear();
    ui->LE_Duree->clear();
    ui->LE_Prix->clear();
    ui->LE_Credit->clear();

    ui->Bt_Annuler->hide();
    ui->Bt_Valider->hide();
    ui->Bt_Ajouter->show();
    ui->Bt_Modifier->show();
    ui->Bt_Supprimer->show();
}

/**
 *  @brief  Permet de bloquer le line edit "Credit Disponible".
 *
 */
void F_Abonnements::on_RBt_Prestation_clicked()
{
    ui->LE_Credit->setEnabled(false);
}

/**
 *  @brief  Permet de débloquer le line edit "Credit Disponible".
 *
 */
void F_Abonnements::on_RBt_CartePrepayee_clicked()
{
    ui->LE_Credit->setEnabled(true);
}

//---------------------------------------------------------------------------
