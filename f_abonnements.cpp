//------------------------------------------------------------------------------
/** @file       f_abonnements.cpp
 *  @brief      F_Abonnements permet la gestion des abonnements dans la partie "Administration".
 *
 *  @author     NORMAND Julien
 *  @author     STS IRIS, Lycée Nicolas APPERT, ORVAULT (FRANCE)
 *  @since      01/2012
 *  @version    1.0
 *  @date       06/03/2014
 *
 *  Les options disponibles dans cette classe sont:
 *      - l'ajout d'un abonnement.
 *      - la suppression d'un abonnement.
 *      - la modification d'un abonnement.
 *      - la consultaion d'un abonnement.
 *
 *  Fabrication   Qt Creator, projet    .pro
 *
 *  @bug        <Création de 2 abonnements avec le même nom> <corrigé> <William> <06/03/2014>
 *  @todo       Classe terminée.
 */
//------------------------------------------------------------------------------
#include <QtWidgets>
#include <QtSql>
using namespace std;
//------------------------------------------------------------------------------
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

    ui->RBt_CartePrepayee->setEnabled(false);
    ui->RBt_Prestation->setEnabled(false);

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
   unsigned nNombreLigne(0);

   this->TbRechercheModele->setColumnCount(5);
   //this->TbRechercheModele->setRowCount(nNombreLigne);
   this->TbRechercheModele->setHorizontalHeaderItem(0, new QStandardItem("Type"));
   this->TbRechercheModele->setHorizontalHeaderItem(1, new QStandardItem("Nom"));
   this->TbRechercheModele->setHorizontalHeaderItem(2, new QStandardItem("Prix"));
   this->TbRechercheModele->setHorizontalHeaderItem(3, new QStandardItem("Durée de validité"));
   this->TbRechercheModele->setHorizontalHeaderItem(4, new QStandardItem("Crédit disponible"));

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
      nNombreLigne++;
   }

   RechercheTableau.exec("SELECT * FROM prestations");
   while (RechercheTableau.next())
   {
      this->TbRechercheModele->setItem(nNombreLigne, 0, new QStandardItem("Prestation"));
      this->TbRechercheModele->setItem(nNombreLigne, 1, new QStandardItem(RechercheTableau.value(1).toString()));
      this->TbRechercheModele->setItem(nNombreLigne, 2, new QStandardItem(RechercheTableau.value(3).toString()));
      this->TbRechercheModele->setItem(nNombreLigne, 3, new QStandardItem(RechercheTableau.value(2).toString()));
      nNombreLigne++;
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
        qDebug()<< "F_Abonnements::MettreAJourPrestation : " << Requete.lastQuery();
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
        qDebug()<< "F_Abonnements::MettreAJourCartePrepayee" << Requete.lastQuery();
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

    Requete.prepare("INSERT INTO prestations (NomPrestation,DureeValidite,Prix) VALUES (:NomPrestation, :DureeValidite, :Prix)");
    Requete.bindValue(":NomPrestation", sTexte);
    Requete.bindValue(":DureeValidite", nDuree);
    Requete.bindValue(":Prix", fPrix);
    if(!Requete.exec())
    {
        qDebug()<< "F_Abonnements::CreerPrestation" << Requete.lastQuery();
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
        qDebug()<< "F_Abonnements::CreerCartePrepayee" << Requete.lastQuery();
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

    Requete.prepare("SELECT NomCarte,DureeValidite,Prix,CreditDisponible FROM cartesprepayees WHERE NomCarte=:NomCarte");
    Requete.bindValue(":NomCarte", sTexte);
    if(!Requete.exec())
    {
        qDebug()<<"F_Abonnements::AfficherDetailAbonnement =>  Requette "<<Requete.lastQuery();
    }
    Requete.next();
    if (Requete.isValid()==true)
    {
        ui->RBt_CartePrepayee->setChecked(true);
        ui->LE_Credit->setEnabled(true);
    }
    else
    {
        Requete.prepare("SELECT NomPrestation,DureeValidite,Prix FROM prestations WHERE NomPrestation=:NomPrestation");
        Requete.bindValue(":NomPrestation", sTexte);
        if(!Requete.exec())
        {
            qDebug()<<"F_Abonnements::AfficherDetailAbonnement =>  Requette " << Requete.lastQuery();
        }
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
   QSqlQuery RequeteSupprimerAbonnement;

   // C'est une carte pré-payée
   if(ui->RBt_CartePrepayee->isChecked() == true)
   {
      // TO DO
      // Vérifier que la carte n'est pas utilisé par des adhérents.
      // Si oui, demander quoi faire et proposer de leur affecté une autre prestation
      RequeteSupprimerAbonnement.prepare("DELETE FROM cartesprepayees WHERE NomCarte=:NomCarte AND CreditDisponible=:CreditDisponible");
      RequeteSupprimerAbonnement.bindValue(":NomCarte", sTexte);
      RequeteSupprimerAbonnement.bindValue(":CreditDisponible", nCredit);
      if(!RequeteSupprimerAbonnement.exec())
      {
         qDebug()<<"F_Abonnements::SupprimerAbonnement =>  RequetteSupprimerCarte "<<RequeteSupprimerAbonnement.lastQuery();
      }
   }
   // C'est une prestation
   else
   {
      RequeteSupprimerAbonnement.prepare("DELETE FROM prestations WHERE NomPrestation=:NomPrestation");
      RequeteSupprimerAbonnement.bindValue(":NomPrestation", sTexte);
      if(!RequeteSupprimerAbonnement.exec())
      {
         qDebug()<<"F_Abonnements::SupprimerAbonnement =>  RequetteSupprimerPrestation "<<RequeteSupprimerAbonnement.lastQuery();
      }
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

    ui->TbV_Recherche->setEnabled(false);

    ui->LE_Credit->setEnabled(false);
    ui->LE_Nom->setEnabled(false);
    ui->LE_Duree->setEnabled(false);
    ui->LE_Prix->setEnabled(false);

    ui->LE_Nom->clear();
    ui->LE_Duree->clear();
    ui->LE_Prix->clear();
    ui->LE_Credit->clear();
    ui->LE_Nom->setFocus(Qt::OtherFocusReason);

    ui->Bt_Ajouter->hide();
    ui->Bt_Modifier->hide();
    ui->Bt_Supprimer->hide();

    ui->Bt_Annuler->show();
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

   // savoir s'il s'agit d'une carte prépayée ou d'une prestation.
   if(ui->RBt_CartePrepayee->isChecked() == true)
   {
      // C'est une carte pré-payée
      nCreditValide = ui->LE_Credit->text().toInt();
      // SI-SINON pour savoir si les champs sont vides. Seul le prix peut être nul
      if ( sTexteValide.isEmpty()==true || nDureeValide==0 || nCreditValide==0 )
      {
         QMessageBox::critical(this, "Erreur de saisie", "Les informations de cette carte pré-payée ne sont pas toutes remplies.");
      }
      else
      {
         // savoir s'il s'agit d'un nouvel abonnement ou pas.
         if(bEstUnNouvelAbo == true)
         {
            // Vérification si cette carte existe dans la BDD
            QSqlQuery RequeteCarteExistante;
            if(!RequeteCarteExistante.exec("SELECT COUNT(*) FROM cartesprepayees WHERE NomCarte='"+sTexteValide+"'") )
            {
                qDebug()<< "F_Abonnements::on_Bt_Valider_clicked() : Requete cartesprepayees" << RequeteCarteExistante.lastQuery() << RequeteCarteExistante.lastQuery();
            }
            else
            {
               // récupérer le nombre de carte ayant le même nom
               RequeteCarteExistante.next();
               if (RequeteCarteExistante.value(0).toInt() > 0)
               {
                  QMessageBox::critical(this, "Carte existant déjà", "Une carte portant le même nom existe déjà. Merci de changer le nom.");
                  this->ui->LE_Nom->setFocus();
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
               ui->Bt_Annuler->hide();
               ui->Bt_Valider->hide();
               ui->Bt_Ajouter->show();
               ui->Bt_Modifier->show();
               ui->Bt_Supprimer->show();
               ui->TbV_Recherche->setEnabled(true);
               ui->RBt_CartePrepayee->setEnabled(false);
               ui->RBt_Prestation->setEnabled(false);
            }
         }
      }
   }
   // C'est une prestation
   else
   {
      if (ui->RBt_Prestation->isChecked() == true)
      {
         // savoir si des champs sont vides.
         if (sTexteValide.isEmpty()==true || nDureeValide==0 || fPrixValide==0.0)
         {
           QMessageBox::critical(this, "Erreur de saisie", "Les informations de cette prestation ne sont pas toutes remplies.");
         }
         else
         {
            // savoir s'il s'agit d'un nouvel abonnement ou pas.
            if(bEstUnNouvelAbo == true)
            {
               // Vérification si cette prestation existe dans la BDD
               QSqlQuery RequetePrestationExistante;
               if(!RequetePrestationExistante.exec("SELECT COUNT(*) FROM prestations WHERE NomPrestation='"+sTexteValide+"'"))
               {
                  qDebug()<< "F_Abonnements::on_Bt_Valider_clicked() : Requete prestations" << RequetePrestationExistante.lastQuery() << RequetePrestationExistante.lastQuery();
               }
               else
               {
                  // récupérer le nombre de Prestation ayant le même nom
                  RequetePrestationExistante.next();
                  if (RequetePrestationExistante.value(0).toInt() > 0)
                  {
                     QMessageBox::critical(this, "Prestation existant déjà", "Une prestation portant le même nom existe déjà. Merci de changer le nom.");
                     this->ui->LE_Nom->setFocus();
                  }
                  else  // C'est bien une nouvelle prestation
                  {
                     this->CreerPrestation(sTexteValide, nDureeValide, fPrixValide);
                     ui->Bt_Annuler->hide();
                     ui->Bt_Valider->hide();
                     ui->Bt_Ajouter->show();
                     ui->Bt_Modifier->show();
                     ui->Bt_Supprimer->show();
                     ui->TbV_Recherche->setEnabled(true);
                     ui->RBt_CartePrepayee->setEnabled(false);
                     ui->RBt_Prestation->setEnabled(false);
                  }
               }
            }
            else
            {
               if(bEstUnNouvelAbo == false)
               {
                  this->MettreAJourPrestation(sTexteValide, nDureeValide, fPrixValide);
                  ui->Bt_Annuler->hide();
                  ui->Bt_Valider->hide();
                  ui->Bt_Ajouter->show();
                  ui->Bt_Modifier->show();
                  ui->Bt_Supprimer->show();
                  ui->TbV_Recherche->setEnabled(true);
                  ui->RBt_CartePrepayee->setEnabled(false);
                  ui->RBt_Prestation->setEnabled(false);
               }
            }
         }
      }
      else
      {
         // aucun radio bouton de cocher.
         if(ui->RBt_CartePrepayee->isChecked()==false && ui->RBt_Prestation->isChecked()==false)
         {
            QMessageBox::information(this, "Erreur", "Merci de choisir soit une carte pré-payée soit une prestation.");
         }
      }
    }

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
      this->MettreAJourBDD();
   }
}

/**
 *  @brief  Fonction du clic sur le bouton "Annuler". Permet d'annuler toutes les actions effectuées.
 *
 *  @pre    Avoir cliquez sur le bouton "Ajouter" ou "Modifier".
 */
void F_Abonnements::on_Bt_Annuler_clicked()
{
    ui->RBt_CartePrepayee->setEnabled(false);
    ui->RBt_Prestation->setEnabled(false);
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

    ui->TbV_Recherche->setEnabled(true);
}

/**
 *  @brief  Permet de bloquer le line edit "Credit Disponible".
 *
 */
void F_Abonnements::on_RBt_Prestation_clicked()
{
   ui->LE_Nom->setReadOnly(false);
   ui->LE_Duree->setReadOnly(false);
   ui->LE_Prix->setReadOnly(false);
   ui->LE_Credit->setReadOnly(false);

   ui->LE_Credit->setEnabled(false);
   ui->LE_Nom->setEnabled(true);
   ui->LE_Duree->setEnabled(true);
   ui->LE_Prix->setEnabled(true);

   ui->Bt_Valider->show();
}

/**
 *  @brief  Permet de débloquer le line edit "Credit Disponible".
 *
 */
void F_Abonnements::on_RBt_CartePrepayee_clicked()
{
   ui->LE_Nom->setReadOnly(false);
   ui->LE_Duree->setReadOnly(false);
   ui->LE_Prix->setReadOnly(false);
   ui->LE_Credit->setReadOnly(false);

   ui->LE_Credit->setEnabled(true);
   ui->LE_Nom->setEnabled(true);
   ui->LE_Duree->setEnabled(true);
   ui->LE_Prix->setEnabled(true);

   ui->Bt_Valider->show();
}

//---------------------------------------------------------------------------
