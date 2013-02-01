//------------------------------------------------------------------------------
/** @file         f_Paiement.cpp
 *  @brief        Cette classe permet d'éffectuer une paiement
 *
 *  @author       Botherel Philippe
 *  @author       STS IRIS, Lycée Nicolas APPERT, ORVAULT (FRANCE)
 *  @since        25/05/1012
 *  @version      0.1
 *  @date         01/02/2013
 *
 *  Permet à l'utilisateur de payer un emprun ou prolongement d'emprunt par
 *  une ou plusieurs de ses cartes prépayées ou par un ou plusieurs autre
 *  moyen de paiement (espèce, chèque...)
 *
 *  Fabrication   QtCreator, LudOpen.pro

 *
 *  @bug          <25/05/1012> - <CORRIGE> - <Calcule du montant restant à payer>
 */
//------------------------------------------------------------------------------

#include <QtSql>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <math.h>
#include <fenv.h>               // Gestion des arrondis des floats

#include "f_paiement.h"
#include "ui_f_paiement.h"


/** Constructeur de F_Paiement
 *  @pre    Fenêtre principale lancée
 *  @see    NombreLignePaiement,NombreLignePaiementAutre,Prix,Reste,MembreActif,PrixCredit
 */
F_Paiement::F_Paiement(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::F_Paiement)
{
    fesetround(FE_TOWARDZERO) ;     // Partie entière
    ui->setupUi(this);
    setWindowIcon(QIcon("paiement.png"));
    //Recherche du prix unitaire de la location
    QSqlQuery RequetePrixunitaire;
    RequetePrixunitaire.exec("SELECT `UniteLocation` FROM`preferences` ");
    RequetePrixunitaire.next();
    this->PrixCredit=(round(RequetePrixunitaire.value(0).toDouble()*100.0));




    //Remplire le ComboBox des modes de paiements
    QSqlQuery RequeteMode;
    RequeteMode.exec("SELECT `NomPaiement`FROM`modepaiement`");
    while (RequeteMode.next())
    {
        ui->CBx_ModePaiement->addItem(RequeteMode.value(0).toString());
    }

//Initialise le tableau des Cartes prépayées
    this->NombreLignePaiement=0;
    ui->TW_Paiement->setColumnCount(3);
    ui->TW_Paiement->setHorizontalHeaderItem(0,new QTableWidgetItem ("Moyen de paiement"));
    ui->TW_Paiement->setHorizontalHeaderItem(1,new QTableWidgetItem ("Crédits"));

    ui->TW_Paiement->verticalHeader()->hide();

    ui->TW_Paiement->horizontalHeader()->setResizeMode( 0, QHeaderView::Stretch );
    ui->TW_Paiement->horizontalHeader()->setResizeMode( 1, QHeaderView::ResizeToContents );
    ui->TW_Paiement->setColumnHidden(2,true);

//Initialise le tableau des Autres moyens de paiement
    this->NombreLignePaiementAutre=0;
    ui->TW_PaiementAutre->setColumnCount(2);
    ui->TW_PaiementAutre->setHorizontalHeaderItem(0,new QTableWidgetItem ("Moyen de paiement"));
    ui->TW_PaiementAutre->setHorizontalHeaderItem(1,new QTableWidgetItem ("Crédits"));

    ui->TW_PaiementAutre->verticalHeader()->hide();

    ui->TW_PaiementAutre->horizontalHeader()->setResizeMode( 0, QHeaderView::Stretch );
    ui->TW_PaiementAutre->horizontalHeader()->setResizeMode( 1, QHeaderView::ResizeToContents );

//Grise le bouton OK
    QPushButton* BoutonOk;
    BoutonOk= ui->buttonBox->button(QDialogButtonBox::Ok);
    BoutonOk->setEnabled(false);
}

/** Description détaillée du destructeur
 *
 *  @see    NombreLignePaiement,NombreLignePaiementAutre,Prix,Reste,MembreActif,PrixCredit
 */
F_Paiement::~F_Paiement()
{
    delete ui;
}

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
    RequeteMembre.prepare("SELECT `Nom`,`Prenom` FROM `membres` WHERE `CodeMembre`=:CodeDuMembre");
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
    for(register int i=0;i<NombreLignePaiement;i++)
    {
        ui->TW_Paiement->removeRow(i);
    }
    this->NombreLignePaiement=0;


    //Vide le TableWidget des autre moyens de paiement
    for(register int i=0;i<NombreLignePaiementAutre;i++)
    {
        ui->TW_PaiementAutre->removeRow(i);
    }
    this->NombreLignePaiementAutre=0;

    //vider le ComboBox des Cartes Prépayées
    ui->CBx_Cartes->clear();

    //Remplir le ComboBox des cartes en fonction du membre
    QSqlQuery RequeteCartes;
    RequeteCartes.prepare("SELECT `NomCarte`,`CreditRestant` "
                          "FROM `abonnements`,`cartesprepayees`,`membres` "
                          "WHERE `abonnements`.`Membres_IdMembre`=`IdMembre` "
                          "AND `CodeMembre`=:CodeDuMembre "
                                   "AND `abonnements`.`CartesPrepayees_IdCarte` IS NOT NULL "
                                   "AND `abonnements`.`CreditRestant` >0 "
                                   "AND `IdCarte`=`CartesPrepayees_IdCarte`");


    RequeteCartes.bindValue("CodeDuMembre",CodeMembre);
    if (!RequeteCartes.exec())
    {
        qDebug()<<"F_Paiement::AffichePaiement  RequeteCartes "<<RequeteCartes.lastError();
    }
    while(RequeteCartes.next())
    {
        ui->CBx_Cartes->addItem(RequeteCartes.value(0).toString()+" ("+RequeteCartes.value(1).toString()+" crédits restants)");
    }

    on_CalculePrix();
}


void F_Paiement::on_Bt_Ajouter_clicked()
{
    if (ui->CBx_Cartes->currentText()=="")
    {
        return;
    }
    ui->TW_Paiement->insertRow(NombreLignePaiement);

    QSpinBox* SpinBox;
    SpinBox= new QSpinBox;

    connect(SpinBox,SIGNAL(valueChanged(int)),this,SLOT(on_CalculePrix()));

    SpinBox->setMaximum(this->Prix);
    SpinBox->setValue(this->Reste);

    ui->TW_Paiement->setItem(this->NombreLignePaiement,0,new QTableWidgetItem (ui->CBx_Cartes->currentText()));

    //Requète perméttant de retrouver la carte choisie, le nombre de crédit restant afain de limiter le SpinBox
    QSqlQuery RequeteCartes;
    RequeteCartes.prepare("SELECT IdAbonnements,CreditRestant,`NomCarte` "
                          "FROM `abonnements`,`cartesprepayees`,`membres` "
                          "WHERE `abonnements`.`Membres_IdMembre`=`IdMembre` "
                          "AND `CodeMembre`=:CodeDuMembre "
                                   "AND `abonnements`.`CartesPrepayees_IdCarte` IS NOT NULL "
                                   "AND `abonnements`.`CreditRestant` >0 "
                                   "AND `IdCarte`=`CartesPrepayees_IdCarte`");


    RequeteCartes.bindValue("CodeDuMembre",this->MembreActif);
    if (!RequeteCartes.exec())
    {
        qDebug()<<"F_Paiement::on_Bt_Ajouter_clicked  RequeteCartes "<<RequeteCartes.lastError();
    }
    for(register int i =0;i<ui->CBx_Cartes->currentIndex()+1; i++)
    {
        RequeteCartes.next();
    }

    ui->TW_Paiement->setItem(this->NombreLignePaiement,2,new QTableWidgetItem (RequeteCartes.value(0).toString()));
    if(RequeteCartes.value(1).toInt()<this->Prix)
    {
        SpinBox->setMaximum(RequeteCartes.value(1).toInt());
    }

    ui->TW_Paiement->setCellWidget(this->NombreLignePaiement,1,SpinBox);
    this->NombreLignePaiement++;

    //pour toute les ligne du tableau (sauf la dernière)
    for (register int i=0; i<this->NombreLignePaiement-1;i++)
    {
        //si l'Id de la carte éxiste déja
        int j;
        j=((QTableWidgetItem* )ui->TW_Paiement->item(i,2))->data(0).toInt()-((QTableWidgetItem* )ui->TW_Paiement->item(this->NombreLignePaiement-1,2))->data(0).toInt();

        if (((QTableWidgetItem* )ui->TW_Paiement->item(i,2))->data(0).toInt()==((QTableWidgetItem* )ui->TW_Paiement->item(this->NombreLignePaiement-1,2))->data(0).toInt())
        {
            ui->TW_Paiement->removeRow(this->NombreLignePaiement-1);
            this->NombreLignePaiement--;
        }

    }
    on_CalculePrix();
}





void F_Paiement::on_Bt_AjouterAutres_clicked()
{

    ui->TW_PaiementAutre->insertRow(NombreLignePaiementAutre);

    QDoubleSpinBox* DoubleSpinBox;
    DoubleSpinBox= new QDoubleSpinBox;
    DoubleSpinBox->setSingleStep(0.01);

    connect(DoubleSpinBox,SIGNAL(valueChanged(double)),this,SLOT(on_CalculePrix()));

    DoubleSpinBox->setMaximum(this->Prix*this->PrixCredit);
    DoubleSpinBox->setValue(ui->LE_RestAPayer->text().toFloat());

    ui->TW_PaiementAutre->setItem(this->NombreLignePaiementAutre,0,new QTableWidgetItem (ui->CBx_ModePaiement->currentText()));
    ui->TW_PaiementAutre->setCellWidget(this->NombreLignePaiementAutre,1,DoubleSpinBox);
    this->NombreLignePaiementAutre++;

    on_CalculePrix();
}

void F_Paiement::on_CalculePrix ()
{
    int nReste (0);         //reste de crédit à payer
    nReste= this->Prix;     //Prix est le nombre de crédits à payer
    QString sReste;         //Chaîne de caractères permétant l'affichage

    //Parcourt du tableau des cates prépayées
    QSpinBox* SpinBox;
    for (register int i=0; i<this->NombreLignePaiement;i++)
    {
        //on récupère un pointeur du SpinBox
        SpinBox = (QSpinBox*) ui->TW_Paiement->cellWidget(i,1);
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
    for (register int i=0; i<this->NombreLignePaiementAutre;i++)
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
    ui->LE_RestAPayer->setText(sReste);

    QPushButton* BoutonOk;
    //si le reste est nul,
    if(ResteEuro==0)
    {
        //On l'affiche en vert et on met le bouton Valider en cliquable
        ui->LE_RestAPayer->setStyleSheet("QLineEdit {color:green;}");
        ui->Lb_RestAPayer->setStyleSheet("QLabel {color:green;}");
        ui->Lb_Euro->setStyleSheet("QLabel {color:green;}");
        //ui->Bt_Valider->setEnabled(true);
        BoutonOk= ui->buttonBox->button (QDialogButtonBox::Ok);
        BoutonOk->setEnabled(true);
    }
    //Sinon,
    else
    {
        //On l'affiche en rouge et on met le bouton Valider en non-cliquable
        ui->LE_RestAPayer->setStyleSheet("QLineEdit {color:red;}");
        ui->Lb_RestAPayer->setStyleSheet("QLabel {color:red;}");
        ui->Lb_Euro->setStyleSheet("QLabel {color:red;}");
        // ui->Bt_Valider->setEnabled(false);
        BoutonOk= ui->buttonBox->button(QDialogButtonBox::Ok);
        BoutonOk->setEnabled(false);
    }
}

void F_Paiement::on_Bt_Supprimer_clicked()
{
    //Supprime la ligne sellectionnée
    ui->TW_Paiement->removeRow(ui->TW_Paiement->currentRow());
    this->NombreLignePaiement--;

    //Grise le bouton de suppression
    ui->Bt_Supprimer->setEnabled(false);

    //Calcule le reste à payer
    on_CalculePrix();

}


void F_Paiement::on_Bt_SupprimerAutres_clicked()
{
    //Supprime la ligne sellectionnée
    ui->TW_PaiementAutre->removeRow(ui->TW_PaiementAutre->currentRow());
    this->NombreLignePaiementAutre--;

    //Grise le bouton de suppression
    ui->Bt_SupprimerAutres->setEnabled(false);

    //Calcule le reste à payer
    on_CalculePrix();

}


void F_Paiement::on_TW_Paiement_clicked(const QModelIndex &index)
{
    ui->Bt_Supprimer->setEnabled(true);
}

void F_Paiement::on_TW_PaiementAutre_clicked(const QModelIndex &index)
{
    ui->Bt_SupprimerAutres->setEnabled(true);
}


void F_Paiement::on_buttonBox_clicked(QAbstractButton *button)
{

    if(button->text()=="OK")
    {
        for(register int i=0;i<NombreLignePaiement;i++)
        {
            //Réqupère le nombre de crédits restant sur la carte
            QSqlQuery RequeteNbCredit;
            RequeteNbCredit.prepare("SELECT `CreditRestant` FROM `abonnements` WHERE `IdAbonnements`=:IdAbonnement");
            RequeteNbCredit.bindValue(":IdAbonnement",((QTableWidgetItem* )ui->TW_Paiement->item(i,2))->data(0).toInt() );

            RequeteNbCredit.exec();
            RequeteNbCredit.next();

            //Calcule du nombre de crédit à mettre sur la carte
            QSpinBox* SpinBox;
            int CreditRestant(0);
            CreditRestant=RequeteNbCredit.value(0).toInt();
            SpinBox = (QSpinBox*) ui->TW_Paiement->cellWidget(i,1);
            CreditRestant=CreditRestant - SpinBox->value();

            //Retire le nombre de crédit utilisé
            QSqlQuery RequeteMajCredit;
            RequeteMajCredit.prepare("UPDATE `abonnements` SET `CreditRestant`=:Credit WHERE `IdAbonnements`=:IdAbonnement");
            RequeteMajCredit.bindValue(":Credit",CreditRestant);
            RequeteMajCredit.bindValue(":IdAbonnement",((QTableWidgetItem* )ui->TW_Paiement->item(i,2))->data(0).toInt());
            RequeteMajCredit.exec();



        }
        //Cacher la fenêtre du payement
        this->hide();
    }
}

