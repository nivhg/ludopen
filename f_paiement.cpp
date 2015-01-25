//------------------------------------------------------------------------------
/** @file         f_Paiement.cpp
 *  @brief        Cette classe permet d'éffectuer une paiement
 *
 *  @author       Botherel Philippe
 *  @author       STS IRIS, Lycée Nicolas APPERT, ORVAULT (FRANCE)
 *  @since        25/05/1012
 *  @version      0.1
 *  @date         01/03/2014
 *  @author       William
 *
 *  Permet à l'utilisateur de payer un emprun ou prolongement d'emprunt par
 *  une ou plusieurs de ses cartes prépayées ou par un ou plusieurs autre
 *  moyen de paiement (espèce, chèque...)
 *
 *  Fabrication   QtCreator, LudOpen.pro

 *
 *  @bug          <25/05/2012> - <Philippe> - <Calcule du montant restant à payer>
 *  @bug          <02/03/2014> - <William> - <Suppression de paiement dans tableau + Remise refonte du code + Modif IHM>
 *
 */
//------------------------------------------------------------------------------

#include <QtSql>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <math.h>
#include <fenv.h>               // Gestion des arrondis des floats

#include "f_paiement.h"
#include "ui_f_paiement.h"
#include "f_preferences.h"

//------------------------------------------------------------------------------
/** Constructeur de F_Paiement
 *  @pre    Fenêtre principale lancée
 *  @see    NombreLignePaiement,NombreLignePaiementAutre,Prix,Reste,MembreActif,PrixCredit
 */
F_Paiement::F_Paiement(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::F_Paiement)
{
   // Partie entière. Sert pour des problèmes d'opération où l'on n'arrive pas avoir la valeur 0 exactement avec des calculs de réels flottant
   fesetround(FE_TOWARDZERO) ;

   ui->setupUi(this);

   this->PrixCredit=(round(F_Preferences::ObtenirValeur("UniteLocation").toDouble()*100.0));

   //Remplir le ComboBox des modes de paiements
   QSqlQuery RequeteMode;
   RequeteMode.exec("SELECT NomPaiement FROM modepaiement");
   while (RequeteMode.next())
   {
     ui->CBx_ModePaiement->addItem(RequeteMode.value(0).toString());
   }

   //Initialise le tableau des Cartes prépayées
   this->NombreLignePaiement=0;
   ui->TW_PaiementCarte->setColumnCount(3);
   ui->TW_PaiementCarte->setHorizontalHeaderItem(0,new QTableWidgetItem ("Cartes pré-payées"));
   ui->TW_PaiementCarte->setHorizontalHeaderItem(1,new QTableWidgetItem ("Crédits"));

   ui->TW_PaiementCarte->verticalHeader()->hide();

   ui->TW_PaiementCarte->horizontalHeader()->setSectionResizeMode( 0, QHeaderView::Stretch );
   ui->TW_PaiementCarte->horizontalHeader()->setSectionResizeMode( 1, QHeaderView::ResizeToContents );
   ui->TW_PaiementCarte->setColumnHidden(2,true);

   //Initialise le tableau des Autres moyens de paiement
   this->NombreLignePaiementAutre=0;
   ui->TW_PaiementAutre->setColumnCount(2);
   ui->TW_PaiementAutre->setHorizontalHeaderItem(0,new QTableWidgetItem ("Moyens de paiement"));
   ui->TW_PaiementAutre->setHorizontalHeaderItem(1,new QTableWidgetItem ("Somme"));
   ui->TW_PaiementAutre->verticalHeader()->hide();
   ui->TW_PaiementAutre->horizontalHeader()->setSectionResizeMode( 0, QHeaderView::Stretch );
   ui->TW_PaiementAutre->horizontalHeader()->setSectionResizeMode( 1, QHeaderView::ResizeToContents );

   //Grise le bouton OK
   ui->Bt_OK_Annuler->button(QDialogButtonBox::Ok)->setEnabled(false);
}
//------------------------------------------------------------------------------
/** Description détaillée du destructeur
 *
 *  @see    NombreLignePaiement,NombreLignePaiementAutre,Prix,Reste,MembreActif,PrixCredit
 */
F_Paiement::~F_Paiement()
{
    delete ui;
}
//------------------------------------------------------------------------------
/** Permet d'afficher les informations du payement
 *  @pre    Connexion à la base de données
 *  @param  Somme : Pris à payer en nombre de crédits , CodeMembre : code du membre qui emprunte
 *  @retval Valeurs de retour
 */
void F_Paiement::AfficherPaiement(unsigned int Somme,QString CodeMembre)
{
    //Affichage du nom de la fenêtre
    QString NomFenetre;
    QSqlQuery RequeteMembre;
    RequeteMembre.prepare("SELECT Nom,Prenom FROM membres WHERE CodeMembre=:CodeDuMembre");
    RequeteMembre.bindValue(":CodeDuMembre",CodeMembre);
    RequeteMembre.exec();
    RequeteMembre.next();

    NomFenetre= "Paiement de "+RequeteMembre.value(0).toString()
            +" "+RequeteMembre.value(1).toString()
            + " de "+ NomFenetre.setNum(Somme) +" crédits";

    //Affiche le nom de la fenêtre
    this->setWindowTitle(NomFenetre);

    //Initialisation des attibuts
    this->Prix=Somme;
    this->MembreActif=CodeMembre;
    this->Reste=Somme;

    //Remet les ComboBox par défaut
    ui->CBx_ModePaiement->setCurrentIndex(0);
    ui->CBx_Cartes->setCurrentIndex(0);

    //Vide le TableWidgetDes cartes prépayées
    for(register unsigned int i=0 ; i < NombreLignePaiement ; i++)
    {
        ui->TW_PaiementCarte->removeRow(i);
    }
    this->NombreLignePaiement=0;

    //Vide le TableWidget des autre moyens de paiement
    for(register unsigned int i=0 ; i < NombreLignePaiementAutre ; i++)
    {
        ui->TW_PaiementAutre->removeRow(i);
    }
    this->NombreLignePaiementAutre=0;

    //vider le ComboBox des Cartes Prépayées
    ui->CBx_Cartes->clear();

    //Remplir le ComboBox des cartes en fonction du membre
    QSqlQuery RequeteCartes;
    RequeteCartes.prepare("SELECT NomCarte,CreditRestant "
                          "FROM abonnements,cartesprepayees,membres "
                          "WHERE abonnements.Membres_IdMembre=IdMembre "
                          "AND CodeMembre=:CodeDuMembre "
                                   "AND abonnements.CartesPrepayees_IdCarte IS NOT NULL "
                                   "AND abonnements.CreditRestant>0 "
                                   "AND IdCarte=CartesPrepayees_IdCarte");


    RequeteCartes.bindValue(":CodeDuMembre",CodeMembre);
    if (!RequeteCartes.exec())
    {
        qDebug()<<"F_Paiement::AffichePaiement  RequeteCartes "<<RequeteCartes.lastQuery();
    }
    while(RequeteCartes.next())
    {
        ui->CBx_Cartes->addItem(RequeteCartes.value(0).toString()+" ("+RequeteCartes.value(1).toString()+" crédits restants)");
    }

    slot_CalculerPrix();
}
//------------------------------------------------------------------------------
/**
 * @brief Ajouter dans le tableau des cartes pré-payée à utiliser une carte choisie dans la liste
 * @pre
 * @param
 * @retval
 */
void F_Paiement::on_Bt_AjouterCartePaiement_clicked()
{
    if (ui->CBx_Cartes->currentText()=="")
    {
        return;
    }
    ui->TW_PaiementCarte->insertRow(NombreLignePaiement);

    QSpinBox* SpinBox;
    SpinBox= new QSpinBox;

    connect(SpinBox,SIGNAL(valueChanged(int)),this,SLOT(slot_CalculerPrix()));

    SpinBox->setMaximum(this->Prix);
    SpinBox->setValue(this->Reste);

    ui->TW_PaiementCarte->setItem(this->NombreLignePaiement,0,new QTableWidgetItem (ui->CBx_Cartes->currentText()));

    //Requête permettant de retrouver la carte choisie,
    // le nombre de crédit restant afin de limiter le SpinBox
    QSqlQuery RequeteCartes;
    RequeteCartes.prepare("SELECT IdAbonnements,CreditRestant,NomCarte "
                          "FROM abonnements,cartesprepayees,membres "
                          "WHERE abonnements.Membres_IdMembre=IdMembre "
                          "AND CodeMembre=:CodeDuMembre "
                                   "AND abonnements.CartesPrepayees_IdCarte IS NOT NULL "
                                   "AND abonnements.CreditRestant>0 "
                                   "AND IdCarte=CartesPrepayees_IdCarte");


    RequeteCartes.bindValue(":CodeDuMembre",this->MembreActif);
    if (!RequeteCartes.exec())
    {
        qDebug()<<"F_Paiement::on_Bt_AjouterCartePaiement_clicked  RequeteCartes "<<RequeteCartes.lastQuery();
    }
    for(register int i =0;i<ui->CBx_Cartes->currentIndex()+1; i++)
    {
        RequeteCartes.next();
    }

    ui->TW_PaiementCarte->setItem(this->NombreLignePaiement,2,new QTableWidgetItem (RequeteCartes.value(0).toString()));
    if(RequeteCartes.value(1).toInt() < this->Prix)
    {
        SpinBox->setMaximum(RequeteCartes.value(1).toInt());
    }

    ui->TW_PaiementCarte->setCellWidget(this->NombreLignePaiement,1,SpinBox);
    this->NombreLignePaiement++;

    //pour toutes les lignes du tableau (sauf la dernière)
    for (register unsigned int i=0 ; i < this->NombreLignePaiement-1 ; i++)
    {
        //si l'Id de la carte existe déjà
        if (((QTableWidgetItem* )ui->TW_PaiementCarte->item(i,2))->data(0).toInt()==((QTableWidgetItem* )ui->TW_PaiementCarte->item(this->NombreLignePaiement-1,2))->data(0).toInt())
        {
            ui->TW_PaiementCarte->removeRow(this->NombreLignePaiement-1);
            this->NombreLignePaiement--;
        }
    }
    slot_CalculerPrix();
}
//------------------------------------------------------------------------------
/**
 * @brief Ajouter dans le tableau des moyens de paiements un autre moyen genre chèque ou espèce
 * @pre
 * @param
 * @retval
 */
void F_Paiement::on_Bt_AjouterAutrePaiement_clicked()
{
    ui->TW_PaiementAutre->insertRow(NombreLignePaiementAutre);

    QDoubleSpinBox* DoubleSpinBox;
    DoubleSpinBox= new QDoubleSpinBox;
    DoubleSpinBox->setSingleStep(0.01);

    connect(DoubleSpinBox,SIGNAL(valueChanged(double)),this,SLOT(slot_CalculerPrix()));

    DoubleSpinBox->setMaximum(this->Prix*this->PrixCredit);
    DoubleSpinBox->setValue(ui->LE_ResteAPayer->text().toFloat());

    ui->TW_PaiementAutre->setItem(this->NombreLignePaiementAutre,0,new QTableWidgetItem (ui->CBx_ModePaiement->currentText()));
    ui->TW_PaiementAutre->setCellWidget(this->NombreLignePaiementAutre,1,DoubleSpinBox);
    this->NombreLignePaiementAutre++;

    slot_CalculerPrix();
}
//------------------------------------------------------------------------------
/**
 * @brief Calcul du prix à payer qui diminue à chaque fois qu'un moyen de paiement est utilisé.
 * @pre
 * @param
 * @retval
 */
void F_Paiement::slot_CalculerPrix ()
{
    int nReste (0);         //reste de crédit à payer
    nReste= this->Prix;     //Prix est le nombre de crédits à payer
    QString sReste;         //Chaîne de caractères permétant l'affichage

    //Parcours du tableau des cartes prépayées
    QSpinBox* SpinBox;
    for (register unsigned int i=0; i<this->NombreLignePaiement ; i++)
    {
        //on récupère un pointeur du SpinBox
        SpinBox = (QSpinBox*) ui->TW_PaiementCarte->cellWidget(i,1);
        //On récupère la valeur du SpinBox et la soustrai du reste
        nReste -= SpinBox->value();
        this->Reste=nReste;
    }

    //Reste à payer en € *100
    int ResteEuro (nReste*(this->PrixCredit));
    //Permet d'afficher le reste à payer
    float fReste ((nReste*this->PrixCredit)/100.0) ;

    //Récupère la valeur des  SpinBox des autres moyens de paiement
    QDoubleSpinBox* DoubleSpinBox;
    for (register unsigned int i=0; i < this->NombreLignePaiementAutre ; i++)
    {
        //on récupère un pointeur du DoubleSpinBox
        DoubleSpinBox = (QDoubleSpinBox*) ui->TW_PaiementAutre->cellWidget(i,1);
        //On récupère la valeur du SpinBox et on fait un arrondi
        int nVarEnt (round(DoubleSpinBox->value()*100.0L)) ;
        //On la soustrait du reste
        ResteEuro -=  nVarEnt ;
    }

    //On convertit le reste *100 en reste à virgule
    fReste =ResteEuro/100.0;
    //On le met dans une chaîne de charactères
    sReste.setNum(fReste);
    //On l'affiche
    ui->LE_ResteAPayer->setText(sReste);

    //si le reste est nul,
    if(ResteEuro == 0)
    {
        //On l'affiche en vert et on met le bouton Valider en cliquable
        ui->LE_ResteAPayer->setStyleSheet("QLineEdit {color:green;}");
        ui->Lb_RestAPayer->setStyleSheet("QLabel {color:green;}");
        ui->Lb_Euro->setStyleSheet("QLabel {color:green;}");
        ui->Bt_OK_Annuler->button(QDialogButtonBox::Ok)->setEnabled(true);
    }
    //Sinon,
    else
    {
        //On l'affiche en rouge et on met le bouton Valider en non-cliquable
        ui->LE_ResteAPayer->setStyleSheet("QLineEdit {color:red;}");
        ui->Lb_RestAPayer->setStyleSheet("QLabel {color:red;}");
        ui->Lb_Euro->setStyleSheet("QLabel {color:red;}");
        ui->Bt_OK_Annuler->button(QDialogButtonBox::Ok)->setEnabled(false);
    }
}
//------------------------------------------------------------------------------
/**
 * @brief Supprime du tableau des cartes pré-payée une carte choisie prédédemment
 * @pre
 * @param
 * @retval
 */
void F_Paiement::on_Bt_SupprimerCartePaiement_clicked()
{
   //Supprime la ligne sélectionnée après avoir vérifier qu'il y ait bien un item de choisi dans le tableau
   if ( ui->TW_PaiementCarte->currentRow() >= 0 )
   {
      ui->TW_PaiementCarte->removeRow(ui->TW_PaiementCarte->currentRow());
      this->NombreLignePaiement--;

      //Grise le bouton de suppression
      ui->Bt_SupprimerCartePaiement->setEnabled(false);

      //Calcule le reste à payer
      slot_CalculerPrix();
   }
}
//------------------------------------------------------------------------------
/**
 * @brief Supprime du tableau des Autres paiements possibles un moyen de paiement choisi prédédemment
 * @pre
 * @param
 * @retval
 */
void F_Paiement::on_Bt_SupprimerAutrePaiement_clicked()
{
   //Supprime la ligne sélectionnée après avoir vérifier qu'il y ait bien un item de choisi dans le tableau
   if ( ui->TW_PaiementAutre->currentRow() >= 0 )
   {
      ui->TW_PaiementAutre->removeRow(ui->TW_PaiementAutre->currentRow());
      this->NombreLignePaiementAutre--;

      //Grise le bouton de suppression
      ui->Bt_SupprimerAutrePaiement->setEnabled(false);

      //Calcule le reste à payer
      slot_CalculerPrix();
   }
}
//------------------------------------------------------------------------------
void F_Paiement::on_TW_PaiementCarte_clicked(const QModelIndex &index)
{
    ui->Bt_SupprimerCartePaiement->setEnabled(true);
}
//------------------------------------------------------------------------------
void F_Paiement::on_TW_PaiementAutre_clicked(const QModelIndex &index)
{
    ui->Bt_SupprimerAutrePaiement->setEnabled(true);
}
//------------------------------------------------------------------------------
void F_Paiement::on_Bt_OK_Annuler_accepted()
{
   for(register unsigned int i=0 ; i < NombreLignePaiement ; i++)
   {
      //Récupère le nombre de crédits restant sur la carte
      QSqlQuery RequeteNbCredit;
      RequeteNbCredit.prepare("SELECT CreditRestant FROM abonnements WHERE IdAbonnements=:IdAbonnement");
      RequeteNbCredit.bindValue(":IdAbonnement",((QTableWidgetItem* )ui->TW_PaiementCarte->item(i,2))->data(0).toInt() );

      RequeteNbCredit.exec();
      RequeteNbCredit.next();

      //Calcule du nombre de crédit à mettre sur la carte
      QSpinBox* SpinBox;
      int CreditRestant(0);
      CreditRestant=RequeteNbCredit.value(0).toInt();
      SpinBox = (QSpinBox*) ui->TW_PaiementCarte->cellWidget(i,1);
      CreditRestant=CreditRestant - SpinBox->value();

      //Retire le nombre de crédit utilisé
      QSqlQuery RequeteMajCredit;
      RequeteMajCredit.prepare("UPDATE abonnements SET CreditRestant=:Credit WHERE IdAbonnements=:IdAbonnement");
      RequeteMajCredit.bindValue(":Credit",CreditRestant);
      RequeteMajCredit.bindValue(":IdAbonnement",((QTableWidgetItem* )ui->TW_PaiementCarte->item(i,2))->data(0).toInt());
      RequeteMajCredit.exec();
   }
   //Cacher la fenêtre du paiement
   this->hide();
}
