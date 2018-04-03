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

   QLocale french(QLocale::French);
   this->PrixCredit=(round(french.toDouble(F_Preferences::ObtenirValeur("UniteLocation"))*100.0));

   //Remplir le ComboBox des modes de paiements
   QSqlQuery RequeteMode;
   RequeteMode.exec("SELECT NomPaiement,IdModePaiement FROM modepaiement ORDER BY IdModePaiement");
   ui->CBx_ModePaiement->addItem("");
   while (RequeteMode.next())
   {
     ui->CBx_ModePaiement->addItem(RequeteMode.value(0).toString(),RequeteMode.value(1).toInt());
   }

   //Initialise le tableau des Cartes prépayées
   this->NombreLignePaiement=0;
   ui->TW_PaiementCarte->setColumnCount(3);
   ui->TW_PaiementCarte->setHorizontalHeaderItem(COL_CARTE,new QTableWidgetItem ("Cartes pré-payées"));
   ui->TW_PaiementCarte->setHorizontalHeaderItem(COL_CREDITS,new QTableWidgetItem ("Crédits"));

   ui->TW_PaiementCarte->verticalHeader()->hide();

   ui->TW_PaiementCarte->horizontalHeader()->setSectionResizeMode( COL_CARTE, QHeaderView::Stretch );
   ui->TW_PaiementCarte->horizontalHeader()->setSectionResizeMode( COL_CREDITS, QHeaderView::ResizeToContents );
   ui->TW_PaiementCarte->setColumnHidden(2,true);

   //Initialise le tableau des Autres moyens de paiement
   this->NombreLignePaiementAutre=0;
   ui->TW_PaiementAutre->setColumnCount(6);
   ui->TW_PaiementAutre->setHorizontalHeaderItem(COLAUTRE_MODEPAIEMENT,new QTableWidgetItem ("Moyen de paiement"));
   ui->TW_PaiementAutre->setHorizontalHeaderItem(COLAUTRE_NUMCHEQUE,new QTableWidgetItem ("N° du chèque"));
   ui->TW_PaiementAutre->setHorizontalHeaderItem(COLAUTRE_BANQUE,new QTableWidgetItem ("Banque"));
   ui->TW_PaiementAutre->setHorizontalHeaderItem(COLAUTRE_EMETEUR,new QTableWidgetItem ("Nom sur le chèque"));
   ui->TW_PaiementAutre->setHorizontalHeaderItem(COLAUTRE_MONTANT,new QTableWidgetItem ("Montant du chèque"));
   ui->TW_PaiementAutre->setHorizontalHeaderItem(COLAUTRE_SOMME,new QTableWidgetItem ("Somme"));
   ui->TW_PaiementAutre->verticalHeader()->hide();
   ui->TW_PaiementAutre->horizontalHeader()->setSectionResizeMode( COLAUTRE_MODEPAIEMENT, QHeaderView::ResizeToContents );
   ui->TW_PaiementAutre->setColumnWidth(COLAUTRE_BANQUE,100);
   ui->TW_PaiementAutre->horizontalHeader()->setSectionResizeMode( COLAUTRE_NUMCHEQUE, QHeaderView::ResizeToContents );
   ui->TW_PaiementAutre->horizontalHeader()->setSectionResizeMode( COLAUTRE_EMETEUR, QHeaderView::ResizeToContents );
   ui->TW_PaiementAutre->horizontalHeader()->setSectionResizeMode( COLAUTRE_MONTANT, QHeaderView::ResizeToContents );
   ui->TW_PaiementAutre->horizontalHeader()->setSectionResizeMode( COLAUTRE_SOMME, QHeaderView::ResizeToContents );

   this->pBanqueAjMod = new F_PopUpCLESTTEM();
   this->pBanqueAjMod->setWindowModality(Qt::ApplicationModal);

   this->pRetardsAmendesAjMod = new F_PopUpCLESTTEM();
   this->pRetardsAmendesAjMod->setWindowModality(Qt::ApplicationModal);

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
void F_Paiement::AfficherPaiement(QDateTime DatePaiement, QString CodeMembre,int Somme,int TypeVentilation,QString NomTable,int IdTable,int IdFacture,bool EurosOuCredits)
{
    this->TypeVentilation=TypeVentilation;
    this->NomTable=NomTable;
    this->IdTable=IdTable;
    this->IdFacture=IdFacture;

    ui->W_AmendesRetards->setVisible(false);
    ui->Cbx_AmendesRetards->setChecked(true);

    QString NomFenetre;
    QSqlQuery RequeteMembre;
    RequeteMembre.prepare("SELECT IdMembre,Nom,Prenom FROM membres WHERE CodeMembre=:CodeDuMembre");
    RequeteMembre.bindValue(":CodeDuMembre",CodeMembre);
    RequeteMembre.exec();
    RequeteMembre.next();
    this->NomMembre=ObtenirValeurParNom(RequeteMembre,"Nom").toString();
    NomFenetre= "Paiement de "+NomMembre+" "+ObtenirValeurParNom(RequeteMembre,"Nom").toString()+
            " de "+NomFenetre.setNum(Somme) +" crédits";
    IdMembre=ObtenirValeurParNom(RequeteMembre,"IdMembre").toInt();
    //Affiche le nom de la fenêtre
    this->setWindowTitle(NomFenetre);

    //Initialisation des attibuts
    this->Prix=Somme;
    this->MembreActif=CodeMembre;
    this->Reste=Somme;

    //Remet les ComboBox par défaut
    ui->CBx_ModePaiement->setCurrentIndex(0);
    ui->CBx_Cartes->setCurrentIndex(0);

    int NbLignes;
    NbLignes=ui->TW_PaiementCarte->rowCount();
    //Vide le TableWidgetDes cartes prépayées
    for(register unsigned int i=0 ; i < NbLignes ; i++)
    {
        ui->TW_PaiementCarte->removeRow(0);
    }
    this->NombreLignePaiement=0;

    NbLignes=ui->TW_PaiementAutre->rowCount();
    //Vide le TableWidget des autre moyens de paiement
    for(register unsigned int i=0 ; i < NbLignes ; i++)
    {
        ui->TW_PaiementAutre->removeRow(0);
    }
    this->NombreLignePaiementAutre=0;

    //vider le ComboBox des Cartes Prépayées
    ui->CBx_Cartes->clear();

    if(!EurosOuCredits)
    {
        ui->Bt_AjouterCartePaiement->setDisabled(true);
        ui->CBx_Cartes->setDisabled(true);
    }
    else
    {
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
    }

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

    connect(SpinBox,SIGNAL(valueChanged(int)),this,SLOT(slot_CalculerPrix()));

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
    ui->Bt_OK_Annuler->setEnabled(true);
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
    if(ui->CBx_ModePaiement->currentIndex()==0)
    {
        return;
    }
    ui->TW_PaiementAutre->insertRow(NombreLignePaiementAutre);

    QTableWidgetItem* Item=new QTableWidgetItem (ui->CBx_ModePaiement->currentText());
    Item->setData(Qt::UserRole,ui->CBx_ModePaiement->currentData());

    ui->TW_PaiementAutre->setItem(this->NombreLignePaiementAutre,COLAUTRE_MODEPAIEMENT,Item);

    if(ui->CBx_ModePaiement->currentData().toInt()==PAIEMENT_CHEQUE)
    {
        QComboBox *ComboBoxCheque=new QComboBox();
        ComboBoxCheque->setEditable(true);

        QSqlQuery RequeteCheque;

        // Récupère tous les chèques du membre encaissés aujourd'hui et dont le montant du chèque est supérieur au paiement
        RequeteCheque.prepare("SELECT DISTINCT(IdCheque) as IdCheque, Banques_IdBanque, NomEmetteur, c.Montant as Montant,Numero FROM cheques as c "
                              "LEFT JOIN paiements as p ON IdCheque=Cheques_IdCheque WHERE DATE(DatePaiement)=DATE(NOW()) "
                              "AND c.Montant>p.Montant AND Membres_IdMembre=:IdMembre ORDER BY Numero" ) ;
        RequeteCheque.bindValue(":IdMembre",IdMembre);

        qDebug() << getLastExecutedQuery(RequeteCheque)<< RequeteCheque.lastError();
        //Exectution de la requête
        if( !RequeteCheque.exec() )
        {
            qDebug() << getLastExecutedQuery(RequeteCheque)<< RequeteCheque.lastError();
        }
        else
        {
            QString Cheque;
            int IdCheque;
            int i = 0;
            ComboBoxCheque->addItem("");
            while( RequeteCheque.next() )
            {
                // Récupère la valeurs des champs
                Cheque = ObtenirValeurParNom(RequeteCheque,"Numero").toString();
                IdCheque = ObtenirValeurParNom(RequeteCheque,"IdCheque").toInt();
                // Ajoute les infos concernant le chèque dans la partie DATA
                QVariantList vListe;
                vListe<<IdCheque<<ObtenirValeurParNom(RequeteCheque,"Banques_IdBanque").toInt()<<ObtenirValeurParNom(RequeteCheque,"NomEmetteur").toString()
                     <<ObtenirValeurParNom(RequeteCheque,"Montant").toString();
                ComboBoxCheque->addItem(Cheque);
                ComboBoxCheque->setItemData(i,vListe,Qt::UserRole);
                i++;
            }
        }
        ComboBoxCheque->setProperty("row",this->NombreLignePaiementAutre);
        ComboBoxCheque->setProperty("column",COLAUTRE_NUMCHEQUE);
        connect(ComboBoxCheque,SIGNAL(currentIndexChanged(int)),this,SLOT(on_ComboBoxCheque_currentIndexChanged(int)));
        ui->TW_PaiementAutre->setCellWidget(this->NombreLignePaiementAutre,COLAUTRE_NUMCHEQUE,ComboBoxCheque);

        QComboBox *ComboBoxBanque=new QComboBox();
        connect(ComboBoxBanque,SIGNAL(currentIndexChanged(int)),this,SLOT(on_ComboBoxBanque_currentIndexChanged(int)));
        MaJBanques(ComboBoxBanque);
        ui->TW_PaiementAutre->setCellWidget(this->NombreLignePaiementAutre,COLAUTRE_BANQUE,ComboBoxBanque);

        QLineEdit *LineEdit=new QLineEdit();
        LineEdit->setText(NomMembre);
        ui->TW_PaiementAutre->setCellWidget(this->NombreLignePaiementAutre,COLAUTRE_EMETEUR,LineEdit);
        QDoubleSpinBox* DoubleSpinBoxMontant=new QDoubleSpinBox();
        DoubleSpinBoxMontant->setProperty("row",this->NombreLignePaiementAutre);
        DoubleSpinBoxMontant->setProperty("column",COLAUTRE_MONTANT);
        ui->TW_PaiementAutre->setCellWidget(this->NombreLignePaiementAutre,COLAUTRE_MONTANT,DoubleSpinBoxMontant);
        connect(DoubleSpinBoxMontant,SIGNAL(valueChanged(double)),this,SLOT(slot_VerifMontant()));
    }

    QDoubleSpinBox* DoubleSpinBox;
    DoubleSpinBox= new QDoubleSpinBox;
    DoubleSpinBox->setSingleStep(0.01);
    DoubleSpinBox->setMaximum(this->Prix*this->PrixCredit);
    DoubleSpinBox->setValue(ui->LE_ResteAPayer->text().toFloat());
    DoubleSpinBox->setProperty("row",this->NombreLignePaiementAutre);
    DoubleSpinBox->setProperty("column",COLAUTRE_SOMME);
    connect(DoubleSpinBox,SIGNAL(valueChanged(double)),this,SLOT(slot_VerifMontant()));

    ui->TW_PaiementAutre->setCellWidget(this->NombreLignePaiementAutre,COLAUTRE_SOMME,DoubleSpinBox);
    this->NombreLignePaiementAutre++;

    CalculerPrix();
    ui->Bt_OK_Annuler->setEnabled(true);
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
    QString sReste;         //Chaîne de caractères permétant l'affichage

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

    //Récupère la valeur des  SpinBox des autres moyens de paiement
    QDoubleSpinBox* DoubleSpinBox;
    for (register unsigned int i=0; i < this->NombreLignePaiementAutre ; i++)
    {
        //on récupère un pointeur du DoubleSpinBox
        DoubleSpinBox = (QDoubleSpinBox*) ui->TW_PaiementAutre->cellWidget(i,COLAUTRE_SOMME);
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

void F_Paiement::slot_VerifMontant()
{
    QDoubleSpinBox *senderObj = (QDoubleSpinBox *) sender();
    QDoubleSpinBox *MontantChequeSBx,*SommeSBx;
    MontantChequeSBx = (QDoubleSpinBox *)ui->TW_PaiementAutre->cellWidget(senderObj->property("row").toInt(),COLAUTRE_MONTANT);
    SommeSBx = (QDoubleSpinBox *)ui->TW_PaiementAutre->cellWidget(senderObj->property("row").toInt(),COLAUTRE_SOMME);

    if(MontantChequeSBx!=0 && SommeSBx!=0)
    {
        if(MontantChequeSBx->value()>SommeSBx->value())
        {
            ui->W_AmendesRetards->setVisible(true);
        }
        else
        {
            ui->W_AmendesRetards->setVisible(false);
        }
    }
    CalculerPrix();
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
      CalculerPrix();
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
    // Vérification des champs de saisie des chèques
    for(register unsigned int i=0 ; i < NombreLignePaiementAutre ; i++)
    {
        if(ui->TW_PaiementAutre->item(i,0)->data(Qt::UserRole)==PAIEMENT_CHEQUE)
        {
            QComboBox* ComboBox;
            ComboBox = (QComboBox*) ui->TW_PaiementAutre->cellWidget(i,COLAUTRE_BANQUE);
            if(ComboBox->currentIndex()==0)
            {
                QMessageBox::critical(this,"Erreur de saisie","Merci de saisir la banque des chèques");
                return;
            }

            ComboBox = (QComboBox*) ui->TW_PaiementAutre->cellWidget(i,COLAUTRE_NUMCHEQUE);
            if(ComboBox->currentText()=="")
            {
                QMessageBox::critical(this,"Erreur de saisie","Merci de saisir le numéro des chèques");
                return;
            }
            QLineEdit* LineEdit;
            LineEdit = (QLineEdit*) ui->TW_PaiementAutre->cellWidget(i,COLAUTRE_EMETEUR);
            if(LineEdit->text()=="")
            {
                QMessageBox::critical(this,"Erreur de saisie","Merci de saisir le nom du chèque");
                return;
            }

            QDoubleSpinBox* DoubleSpinBox;
            DoubleSpinBox = (QDoubleSpinBox*) ui->TW_PaiementAutre->cellWidget(i,COLAUTRE_MONTANT);
            if(DoubleSpinBox->value()==0)
            {
                QMessageBox::critical(this,"Erreur de saisie","Merci de saisir le montant des chèques");
                return;
            }
        }
    }
    // Vérifier la saisie des amendes et retards
    if(ui->W_AmendesRetards->isVisible()&&ui->Cbx_AmendesRetards->checkState()==Qt::Checked)
    {
        if(ui->SBx_Amende->value()==0 && ui->SBx_Retard->value()==0)
        {
            QMessageBox::critical(this,"Erreur de saisie","Merci de saisir le montant de l'amende et/ou du retard");
            return;
        }
    }

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

   bool ChequeNonTrouve=true;
   for(register unsigned int i=0 ; i < NombreLignePaiementAutre ; i++)
   {
        int IdCheque=NULL;
        if(ui->TW_PaiementAutre->item(i,COLAUTRE_MODEPAIEMENT)->data(Qt::UserRole)==PAIEMENT_CHEQUE)
        {
          QComboBox* ComboBox;
          ComboBox = (QComboBox*) ui->TW_PaiementAutre->cellWidget(i,COLAUTRE_NUMCHEQUE);
          // Si l'utisateur n'a pas choisi un numéro existant, on crée le chèque
          if(ComboBox->itemText(ComboBox->currentIndex())!=ComboBox->currentText())
          {
              QSqlQuery RequeteAjoutCheque;
              RequeteAjoutCheque.prepare("INSERT INTO cheques (Banques_IdBanque,NomEmetteur,Numero,Montant) "
                                           "VALUES (:Banques_IdBanque,:NomEmetteur,:Numero,:Montant)");
              RequeteAjoutCheque.bindValue(":Numero",ComboBox->currentText());

              ComboBox = (QComboBox*) ui->TW_PaiementAutre->cellWidget(i,COLAUTRE_BANQUE);
              RequeteAjoutCheque.bindValue(":Banques_IdBanque",ComboBox->currentData());


              QLineEdit * LineEdit= (QLineEdit*) ui->TW_PaiementAutre->cellWidget(i,COLAUTRE_EMETEUR);
              RequeteAjoutCheque.bindValue(":NomEmetteur",LineEdit->text());

              QDoubleSpinBox * DoubleSpinBox= (QDoubleSpinBox*) ui->TW_PaiementAutre->cellWidget(i,COLAUTRE_MONTANT);
              RequeteAjoutCheque.bindValue(":Montant",DoubleSpinBox->value());

              if(!RequeteAjoutCheque.exec())
              {
                  qDebug() << getLastExecutedQuery( RequeteAjoutCheque) << RequeteAjoutCheque.lastError();
              }
              IdCheque=RequeteAjoutCheque.lastInsertId().toInt();
          }
          // Sinon, on mets l'ID du cheque existant pour l'ajouter au nouveau paiement
          else
          {
              QComboBox *ComboBox=(QComboBox *) ui->TW_PaiementAutre->cellWidget(i,COLAUTRE_NUMCHEQUE);
              QVariantList vList=ComboBox->itemData(ComboBox->currentIndex()).value<QVariantList> ();
          }
          QDoubleSpinBox * MontantChequeDBx= (QDoubleSpinBox*) ui->TW_PaiementAutre->cellWidget(i,COLAUTRE_MONTANT);
          QDoubleSpinBox * SommeDBx= (QDoubleSpinBox*) ui->TW_PaiementAutre->cellWidget(i,COLAUTRE_SOMME);
          // Si le montant du chèque est supérieur à la somme demandée, qu'on n'a pas encore trouvé de ce type de chèque
          // et qu'il y a un montant d'amendes et/ou retards
          if(MontantChequeDBx->value()>SommeDBx->value() && ChequeNonTrouve &&
                  ui->W_AmendesRetards->isVisible() && ui->Cbx_AmendesRetards->checkState()==Qt::Checked)
          {
              if(ui->SBx_Amende->value()!=0)
              {
                  AjouterPaiement(ui->TW_PaiementAutre->item(i,COLAUTRE_MODEPAIEMENT)->data(Qt::UserRole).toInt(),ui->SBx_Amende->value(),VENTILATION_AMENDE,IdCheque);
              }
              if(ui->SBx_Retard->value()!=0)
              {
                  AjouterPaiement(ui->TW_PaiementAutre->item(i,COLAUTRE_MODEPAIEMENT)->data(Qt::UserRole).toInt(),ui->SBx_Retard->value(),VENTILATION_RETARD,IdCheque);
              }
              ChequeNonTrouve=false;
          }
        }
        //Retrouve le montant
        QSpinBox* SpinBox;
        SpinBox = (QSpinBox*) ui->TW_PaiementAutre->cellWidget(i,COLAUTRE_SOMME);
        //Ajoute les infos du paiement dans la table paiements
        AjouterPaiement(ui->TW_PaiementAutre->item(i,COLAUTRE_MODEPAIEMENT)->data(Qt::UserRole).toInt(),SpinBox->value(),this->TypeVentilation,IdCheque);
   }
   // Fermer la fenêtre et valider le paiement
   accept();
}

void F_Paiement::AjouterPaiement(int IdModePaiement,int Montant,int TypeVentilation,int IdCheque)
{
    QSqlQuery RequeteAjoutPaiement;
    RequeteAjoutPaiement.prepare("INSERT INTO paiements (DatePaiement,Membres_IdMembre,ModePaiement_IdModePaiement,Montant,"
                               "TypeVentilation_IdTypeVentilation,NomTable,IdTable,Cheques_IdCheque,Factures_IdFacture,Remarque) "
                               "VALUES (:DatePaiement,:Membres_IdMembre,:IdModePaiement,:Montant,:IdTypeVentilation,:NomTable,"
                               ":IdTable,:Cheques_IdCheque,:Factures_IdFacture,:Remarque)");
    RequeteAjoutPaiement.bindValue(":DatePaiement",QDateTime::currentDateTime().toTime_t());
    RequeteAjoutPaiement.bindValue(":Membres_IdMembre",this->IdMembre);
    RequeteAjoutPaiement.bindValue(":IdModePaiement",IdModePaiement);
    RequeteAjoutPaiement.bindValue(":Montant",Montant);
    RequeteAjoutPaiement.bindValue(":IdTypeVentilation",TypeVentilation);
    RequeteAjoutPaiement.bindValue(":NomTable",this->NomTable);
    RequeteAjoutPaiement.bindValue(":IdTable",this->IdTable);
    RequeteAjoutPaiement.bindValue(":Cheques_IdCheque",IdCheque);
    RequeteAjoutPaiement.bindValue(":Factures_IdFacture",this->IdFacture);
    RequeteAjoutPaiement.bindValue(":Remarque",ui->LE_Remarque->text());

    if(!RequeteAjoutPaiement.exec())
    {
        qDebug()<<getLastExecutedQuery(RequeteAjoutPaiement)<<RequeteAjoutPaiement.lastError();
    }
}

void F_Paiement::on_ComboBoxBanque_currentIndexChanged(int index)
{
    QComboBox *senderObj = (QComboBox *) sender();
    if( (senderObj->count()-1) == index && index !=0 )
    {
        // S'il ne rajoute pas de de banque, on sort de la fonction
        if(this->pBanqueAjMod->Ajouter(13)==0)
        {
            return;
        }
        senderObj->blockSignals(true);
        senderObj->clear();
        MaJBanques(senderObj);
        senderObj->blockSignals(false);
    }
}

void F_Paiement::on_ComboBoxCheque_currentIndexChanged(int index)
{
    QComboBox *senderObj = (QComboBox *) sender();
    QVariantList vList=senderObj->itemData(senderObj->currentIndex()-1).value<QVariantList> ();
    qDebug()<<senderObj->currentIndex();
    QComboBox *ComboBoxBanque = (QComboBox *)ui->TW_PaiementAutre->cellWidget(senderObj->property("row").toInt(),COLAUTRE_BANQUE);
    qDebug()<<ComboBoxBanque->findData(vList.at(1));
    ComboBoxBanque->setCurrentIndex(ComboBoxBanque->findData(vList.at(1)));
    QLineEdit *LineEditEmetteur = (QLineEdit *)ui->TW_PaiementAutre->cellWidget(senderObj->property("row").toInt(),COLAUTRE_EMETEUR);
    LineEditEmetteur->setText(vList.at(2).toString());
    QDoubleSpinBox *DoubleSpinBoxMontant = (QDoubleSpinBox *)ui->TW_PaiementAutre->cellWidget(senderObj->property("row").toInt(),COLAUTRE_MONTANT);
    DoubleSpinBoxMontant->setValue(vList.at(3).toDouble());
}

void F_Paiement::MaJBanques(QComboBox *ComboBoxBanque)
{
    QSqlQuery RequeteBanque;

    // Récupère toutes les banques
    RequeteBanque.prepare("SELECT IdBanque, CONCAT(Acronyme,\" - \",NomBanque) as NomBanque FROM banques ORDER BY NomBanque" ) ;

    //Exectution de la requête
    if( !RequeteBanque.exec() )
    {
        qDebug() << getLastExecutedQuery(RequeteBanque)<< RequeteBanque.lastError();
    }
    else
    {
        QString Banque;
        int IdBanque;
        int i = 0;
        ComboBoxBanque->addItem("");
        while( RequeteBanque.next() )
        {
            // Récupère la valeurs des champs
            Banque = ObtenirValeurParNom(RequeteBanque,"NomBanque").toString();
            IdBanque = ObtenirValeurParNom(RequeteBanque,"IdBanque").toInt();
            // Ajoute une activité et son ID dans la partie DATA
            ComboBoxBanque->addItem(Banque);
            ComboBoxBanque->setItemData(i,IdBanque,Qt::UserRole);
            i++;
        }
        ComboBoxBanque->addItem("Ajouter une banque...");
    }
}

void F_Paiement::on_CBx_ModePaiement_currentIndexChanged(int index)
{
    ui->Bt_AjouterAutrePaiement->setEnabled(index!=0);
}

void F_Paiement::on_Cbx_AmendesRetards_clicked()
{
    if(ui->Cbx_AmendesRetards->checkState()==Qt::Checked)
    {
        ui->SBx_Amende->setEnabled(true);
        ui->SBx_Retard->setEnabled(true);
    }
    else
    {
        ui->SBx_Amende->setEnabled(false);
        ui->SBx_Retard->setEnabled(false);
    }
}
