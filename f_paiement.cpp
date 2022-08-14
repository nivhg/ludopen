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
#include "fonctions_globale.h"

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

   bool ok;
   QLocale French(QLocale::French);
   this->PrixCredit=(round(French.toDouble(F_Preferences::ObtenirValeur("UniteLocation"))*100.0));

   //Initialise le tableau des Cartes prépayées
   this->NombreLignePaiement=0;
   ui->TW_PaiementCarte->setColumnCount(3);
   ui->TW_PaiementCarte->setHorizontalHeaderItem(COL_CARTE,new QTableWidgetItem ("Cartes pré-payées"));
   ui->TW_PaiementCarte->setHorizontalHeaderItem(COL_CREDITS,new QTableWidgetItem ("Crédits"));

   ui->TW_PaiementCarte->verticalHeader()->hide();

   ui->TW_PaiementCarte->horizontalHeader()->setSectionResizeMode( COL_CARTE, QHeaderView::Stretch );
   ui->TW_PaiementCarte->horizontalHeader()->setSectionResizeMode( COL_CREDITS, QHeaderView::ResizeToContents );
   ui->TW_PaiementCarte->setColumnHidden(2,true);

   this->pBanqueAjMod = new F_PopUpCLESTTEM();
   this->pBanqueAjMod->setWindowModality(Qt::ApplicationModal);

   this->pRetardsAmendesAjMod = new F_PopUpCLESTTEM();
   this->pRetardsAmendesAjMod->setWindowModality(Qt::ApplicationModal);

   //Grise le bouton OK
   ui->Bt_OK->setEnabled(false);
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
/** Permet d'afficher les informations du paiement
 *  @pre    Connexion à la base de données
 *  @param  Somme : Prix à payer en nombre de crédits , CodeMembre : code du membre qui emprunte
 *  @retval Valeurs de retour
 */
void F_Paiement::AfficherPaiement(QDateTime DatePaiement, QString CodeMembre,int *Somme,int *retour,bool PanierIndispo)
{
    QString NomFenetre;
    QSqlQuery RequeteMembre;
    RequeteMembre.prepare("SELECT IdMembre,Nom,Prenom FROM membres WHERE CodeMembre=:CodeMembre");
    RequeteMembre.bindValue(":CodeMembre",CodeMembre);
    RequeteMembre.exec();
    RequeteMembre.next();
    this->NomMembre=ObtenirValeurParNom(RequeteMembre,"Nom").toString();
    NomFenetre= "Paiement de "+NomMembre+" "+ObtenirValeurParNom(RequeteMembre,"Nom").toString()+
            " de "+NomFenetre.setNum(*Somme)+" crédits";

    IdMembre=ObtenirValeurParNom(RequeteMembre,"IdMembre").toInt();
    //Affiche le nom de la fenêtre
    this->setWindowTitle(NomFenetre);

    //Initialisation des attibuts
    this->Prix=*Somme;
    this->MembreActif=CodeMembre;
    this->Reste=*Somme;
    this->RetourReste=Somme;
    this->Retour=retour;

    //Remet les ComboBox par défaut
    ui->CBx_Cartes->setCurrentIndex(0);

    int NbLignes;
    NbLignes=ui->TW_PaiementCarte->rowCount();
    //Vide le TableWidgetDes cartes prépayées
    for(register unsigned int i=0 ; i < NbLignes ; i++)
    {
        ui->TW_PaiementCarte->removeRow(0);
    }
    this->NombreLignePaiement=0;

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
                            "AND IdCarte=CartesPrepayees_IdCarte AND Supprimer=0");


    RequeteCartes.bindValue(":CodeDuMembre",CodeMembre);
    if (!RequeteCartes.exec())
    {
        qDebug()<<"F_Paiement::AffichePaiement  RequeteCartes "<<RequeteCartes.lastQuery();
    }
    while(RequeteCartes.next())
    {
        ui->CBx_Cartes->addItem(RequeteCartes.value(0).toString()+" ("+RequeteCartes.value(1).toString()+" crédits restants)");
    }

    ui->GB_Panier->setEnabled(!PanierIndispo);

    CalculerPrix();
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

    connect(SpinBox,SIGNAL(valueChanged(int)),this,SLOT(CalculerPrix()));

    SpinBox->setMaximum(this->Prix);
    SpinBox->setValue(this->Reste);

    ui->TW_PaiementCarte->setItem(this->NombreLignePaiement,COL_CARTE,new QTableWidgetItem (ui->CBx_Cartes->currentText()));

    //Requête permettant de retrouver la carte choisie,
    // le nombre de crédit restant afin de limiter le SpinBox
    QSqlQuery RequeteCartes;
    RequeteCartes.prepare("SELECT IdAbonnements,CreditRestant,NomCarte "
                          "FROM abonnements,cartesprepayees,membres "
                          "WHERE abonnements.Membres_IdMembre=IdMembre "
                          "AND CodeMembre=:CodeDuMembre "
                           "AND abonnements.CartesPrepayees_IdCarte IS NOT NULL "
                           "AND abonnements.CreditRestant>0 "
                           "AND IdCarte=CartesPrepayees_IdCarte AND Supprimer=0");


    RequeteCartes.bindValue(":CodeDuMembre",this->MembreActif);
    if (!RequeteCartes.exec())
    {
        qDebug()<<"F_Paiement::on_Bt_AjouterCartePaiement_clicked  RequeteCartes "<<RequeteCartes.lastQuery();
    }
    for(register int i =0;i<ui->CBx_Cartes->currentIndex()+1; i++)
    {
        RequeteCartes.next();
    }
    // On stocke l'ID des cartes pour vérifier qu'elles ne sont pas en doublons
    ui->TW_PaiementCarte->setItem(this->NombreLignePaiement,COL_IDCARTE,new QTableWidgetItem (RequeteCartes.value(0).toString()));
    if(RequeteCartes.value(1).toInt() < this->Prix)
    {
        SpinBox->setMaximum(RequeteCartes.value(1).toInt());
    }

    ui->TW_PaiementCarte->setCellWidget(this->NombreLignePaiement,COL_CREDITS,SpinBox);
    this->NombreLignePaiement++;

    //pour toutes les lignes du tableau (sauf la dernière)
    for (register unsigned int i=0 ; i < this->NombreLignePaiement-1 ; i++)
    {
        //si l'Id de la carte existe déjà
        if (((QTableWidgetItem* )ui->TW_PaiementCarte->item(i,2))->data(Qt::DisplayRole).toInt()==
                ((QTableWidgetItem* )ui->TW_PaiementCarte->item(this->NombreLignePaiement-1,COL_IDCARTE))->data(Qt::DisplayRole).toInt())
        {
            ui->TW_PaiementCarte->removeRow(this->NombreLignePaiement-1);
            this->NombreLignePaiement--;
        }
    }
    CalculerPrix();
    ui->Bt_OK->setEnabled(true);
}

//------------------------------------------------------------------------------
/**
 * @brief Calcul du prix à payer qui diminue à chaque fois qu'un moyen de paiement est utilisé.
 * @pre
 * @param
 * @retval
 */
void F_Paiement::CalculerPrix ()
{
    int nReste (0);         //reste de crédit à payer
    nReste= this->Prix;     //Prix est le nombre de crédits à payer
    QString sReste;         //Chaîne de caractères permettant l'affichage

    //Parcours du tableau des cartes prépayées
    QSpinBox* SpinBox;
    for (register unsigned int i=0; i<this->NombreLignePaiement ; i++)
    {
        //on récupère un pointeur du SpinBox
        SpinBox = (QSpinBox*) ui->TW_PaiementCarte->cellWidget(i,COL_CREDITS);
        //On récupère la valeur du SpinBox et la soustrai du reste
        nReste -= SpinBox->value();
        this->Reste=nReste;
    }

    //Reste à payer en € *100
    int ResteEuro (nReste*(this->PrixCredit));
    //Permet d'afficher le reste à payer
    float fReste ((nReste*this->PrixCredit)/100.0) ;

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
        ui->Bt_OK->setEnabled(true);
    }
    //Sinon,
    else
    {
        //On l'affiche en rouge et on met le bouton Valider en non-cliquable
        ui->LE_ResteAPayer->setStyleSheet("QLineEdit {color:red;}");
        ui->Lb_RestAPayer->setStyleSheet("QLabel {color:red;}");
        ui->Lb_Euro->setStyleSheet("QLabel {color:red;}");
        ui->Bt_OK->setEnabled(false);
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
      CalculerPrix();
   }
}

//------------------------------------------------------------------------------
void F_Paiement::on_TW_PaiementCarte_clicked(const QModelIndex &index)
{
    ui->Bt_SupprimerCartePaiement->setEnabled(true);
}

void F_Paiement::on_Bt_Annuler_clicked()
{
    this->setResult(0);
    this->reject();
}

void F_Paiement::on_Bt_OK_clicked()
{
    for(register unsigned int i=0 ; i < NombreLignePaiement ; i++)
    {
       //Récupère le nombre de crédits restant sur la carte
       QSqlQuery RequeteNbCredit;
       RequeteNbCredit.prepare("SELECT CreditRestant FROM abonnements WHERE IdAbonnements=:IdAbonnement");
       RequeteNbCredit.bindValue(":IdAbonnement",((QTableWidgetItem* )ui->TW_PaiementCarte->item(i,COL_IDCARTE))->data(0).toInt() );

       RequeteNbCredit.exec();
       RequeteNbCredit.next();

       //Calcule du nombre de crédit à mettre sur la carte
       QSpinBox* SpinBox;
       int CreditRestant(0);
       CreditRestant=RequeteNbCredit.value(0).toInt();
       SpinBox = (QSpinBox*) ui->TW_PaiementCarte->cellWidget(i,COL_CREDITS);
       CreditRestant=CreditRestant - SpinBox->value();

       //Retire le nombre de crédit utilisé
       QSqlQuery RequeteMajCredit;
       RequeteMajCredit.prepare("UPDATE abonnements SET CreditRestant=:Credit WHERE IdAbonnements=:IdAbonnement");
       RequeteMajCredit.bindValue(":Credit",CreditRestant);
       RequeteMajCredit.bindValue(":IdAbonnement",((QTableWidgetItem* )ui->TW_PaiementCarte->item(i,COL_IDCARTE))->data(0).toInt());
       RequeteMajCredit.exec();
    }
    this->setResult(1);
    this->accept();
}

void F_Paiement::on_Bt_AjoutPanier_clicked()
{
    *RetourReste=this->Reste;
    *Retour=2;
    this->setResult(0);
    this->accept();
}
