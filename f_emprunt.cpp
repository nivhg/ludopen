//------------------------------------------------------------------------------
/** @file         f_emprunt.cpp
 *  @brief        Classe graphique permettant de gérer les emprunts
 *
 *  @author       Philippe BOTHEREL
 *  @author       STS IRIS, Lycée Nicolas APPERT, ORVAULT (FRANCE)
 *  @since        01/01/1012
 *  @version      0.1
 *  @date         29/07/1012
 *
 *  Cette classe permet de créer un emprunt, qui associe un jeu, un membre entre autre.
 *  Elle permet aussi de supprimer une réservation d'un membre.
 *
 *  Fabrication   QtCreator, LudOpen
 *
 */
//------------------------------------------------------------------------------



#include <QtSql>
#include <QStandardItemModel>
#include <QMessageBox>

#include "f_emprunt.h"
#include "ui_f_emprunt.h"
//#include "montableview.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Constructeur///////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/** Description détaillée du CONSTRUCTEUR
 *  @pre    Connexion ? la base de données, existance de la table Emprunt et de ses champs
 */
F_Emprunt::F_Emprunt(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::F_Emprunt)
{

    ui->setupUi(this);


    pPaiement=new F_Paiement;
    MembreActif="";
    JeuActif="";

//Met une date minimum pour le DateEdit du retour (la date du jour)
    QDate DateActuelle;
    DateActuelle=DateActuelle.currentDate();
    ui->DtE_Retour->setMinimumDate(DateActuelle);

     QSqlQuery Requete;
     QString TypeEmprunt;

    Requete.exec("SELECT `TypeEmprunt`,DureeEmprunt FROM `typeemprunt`");

//Tant qu'il y a des types d'emprunt dans la table TupesEmprunt,
    while(Requete.next())
        {
            //on entre un nouveau Item au ComboBox avec le nom du type d'emprunt
        TypeEmprunt=(Requete.value(0).toString())+" ("+(Requete.value(1).toString())+"jours)";
            ui->CBx_TypeEmprunt->addItem(TypeEmprunt);
        }

/*
//Création du table view des jeux réservés
    ui->TbV_JeuxReserves= new MonTableView(this,ui->bt_JeuxReserves);
    ui->TbV_JeuxReserves->setLayout(ui->Layout_JeuReserves);
     ui->TbV_JeuxReserves->setGeometry(11,380,369,175);
    ui->TbV_JeuxReserves->setVisible(true);

    connect(ui->TbV_JeuxReserves,SIGNAL(clicked(QModelIndex)),this,SLOT(on_TbV_JeuxReserves_clicked()));


*/






//Création d'un model pour le TableView des membres
    this->ModelMembre = new QStandardItemModel() ;
//Associe le mod?le au TableView
    ui->TbV_Recherche->setModel(this->ModelMembre);
//Met le TableView en lecture seule*/
     ui->TbV_Recherche->setEditTriggers(0);
//Initialise le tableau avec tous les membres
     on_LE_RechercheMembre_textChanged("");


//Création d'un mod?le pour le TableView des nouveaux emprunts
     this->ModelNewEmprunts = new QStandardItemModel() ;
//Associe le mod?le au TableView
     ui->TbV_NewEmprunt->setModel(this->ModelNewEmprunts);
//Met le TableView en lecture seule
     ui->TbV_Recherche->setEditTriggers(0);
//Initialise les colones du TableView des nouveaux emprunts

     this->ModelNewEmprunts->setColumnCount(4);
     this->ModelNewEmprunts->setHorizontalHeaderItem(0, new QStandardItem("Code"));
     this->ModelNewEmprunts->setHorizontalHeaderItem(1, new QStandardItem("Nom du jeu"));
     this->ModelNewEmprunts->setHorizontalHeaderItem(2, new QStandardItem("Date Retour"));
     this->ModelNewEmprunts->setHorizontalHeaderItem(3, new QStandardItem("Prix"));
     ui->TbV_NewEmprunt->setColumnWidth(0,40);
     ui->TbV_NewEmprunt->setColumnWidth(1,200);
     ui->TbV_NewEmprunt->setColumnWidth(3,50);

      this->NmbLigneNewEmprunts=0;




//Création d'un mod?le pour le TableView des jeux empruntés
    this->ModelJeuEmpruntes = new QStandardItemModel() ;
//Associe le mod?l au TableView
    ui->TbV_JeuxMembres->setModel(this->ModelJeuEmpruntes);
//Met le TableView en lecture seule
    ui->TbV_JeuxMembres->setEditTriggers(0);
//Initialise les colones du TableView des nouveaux emprunts
    this->ModelJeuEmpruntes->setColumnCount(4);
    this->ModelJeuEmpruntes->setHorizontalHeaderItem(0, new QStandardItem("Code"));
    this->ModelJeuEmpruntes->setHorizontalHeaderItem(1, new QStandardItem("Nom du jeu"));
    this->ModelJeuEmpruntes->setHorizontalHeaderItem(2, new QStandardItem("Date emprunt"));
    this->ModelJeuEmpruntes->setHorizontalHeaderItem(3, new QStandardItem("Date retour"));
    ui->TbV_JeuxMembres->setColumnWidth(0,40);
    ui->TbV_JeuxMembres->setColumnWidth(1,125);


//Création d'un mod?le pour le TableView des jeux empruntés
   this->ModelJeuReserves = new QStandardItemModel() ;
//Associe le mod?le au TableView
  ui->TbV_JeuxReserves->setModel(this->ModelJeuReserves);
//Met le TableView en lecture seule
   ui->TbV_JeuxReserves->setEditTriggers(0);

//Initialise les colones du TableView des nouveaux emprunts
    this->ModelJeuReserves->setColumnCount(5);
    this->ModelJeuReserves->setHorizontalHeaderItem(0, new QStandardItem("Code"));
    this->ModelJeuReserves->setHorizontalHeaderItem(1, new QStandardItem("Nom du jeu"));
    this->ModelJeuReserves->setHorizontalHeaderItem(2, new QStandardItem("Date réservation"));
    this->ModelJeuReserves->setHorizontalHeaderItem(3, new QStandardItem("Date emprunt"));
    this->ModelJeuReserves->setHorizontalHeaderItem(4, new QStandardItem("Date retour"));
    this->ModelJeuReserves->setHorizontalHeaderItem(5, new QStandardItem("lieu"));
    ui->TbV_JeuxReserves->setColumnWidth(0,40);
    ui->TbV_JeuxReserves->setColumnWidth(1,125);

}





/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////Déstructeur//////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/** Destructeur de F_Emprunt
 *  @pre    F_Emprunt existant
 */
F_Emprunt::~F_Emprunt()
{
    delete ui;
    delete this->ModelMembre;
    this->ModelMembre=NULL;
    delete this->ModelNewEmprunts;
    this->ModelNewEmprunts=NULL;
    delete this->ModelJeuEmpruntes;
    this->ModelJeuEmpruntes=NULL;
    delete this->ModelJeuReserves;
    this->ModelJeuReserves=NULL;


}

//#######################################################################################################
//////////////////////////////////////////////MEMBRES////////////////////////////////////////////////////
//#######################################################################################################


/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Rechercher un membre avec son nom//////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/** Description détaillée de la méthode
 *  @pre    Connexion avec la base de donnée
 *  @post   Table membres remplie
 *  @see    ModelMembre
 */
void F_Emprunt::on_LE_RechercheMembre_textChanged(const QString &arg1)
{

    unsigned int NbrJeuxEmprunte_Reserve (0);
    QString Nom=arg1;

    //Si il y plus de 2 caract?res dans le nom du membre recherché, alors
    if(Nom.size()>=2)
    {


        Nom="%"+Nom+"%";

        QSqlQuery RequeteMembre;
        NbrJeuxEmprunte_Reserve=0;

        //Rechercher dans la table membres, les membre qui conti?nent dans leur nom le suite de caract?res présente dans le line édit du nom
        RequeteMembre.prepare("SELECT  `CodeMembre`,`Nom`,Prenom,`DateNaissance` FROM`membres` WHERE `Nom` LIKE (:Nom) ORDER BY Nom ASC");
        RequeteMembre.bindValue(":Nom",Nom);
        RequeteMembre.exec();


        //On vide le mod?le
        this->ModelMembre->clear();
        //Indique le nombes de colones puis leurs noms
        this->ModelMembre->setColumnCount(4);
        this->ModelMembre->setHorizontalHeaderItem(0, new QStandardItem("Code"));
        this->ModelMembre->setHorizontalHeaderItem(1, new QStandardItem("Nom"));
        this->ModelMembre->setHorizontalHeaderItem(2, new QStandardItem("Prénom"));
        this->ModelMembre->setHorizontalHeaderItem(3, new QStandardItem("Date de naissance"));
        //Impose une taille aux colones
        ui->TbV_Recherche->setColumnWidth(0,40);
        ui->TbV_Recherche->setColumnWidth(3,40);

        //Tant qu'il y a des membres dans la table membres,
        while(RequeteMembre.next())
            {
                //on ajoute une nouvelle ligne du table view et la remplie avec les informations du membre
                this->ModelMembre->setItem(NbrJeuxEmprunte_Reserve, 0, new QStandardItem(RequeteMembre.value(0).toString() ));
                this->ModelMembre->setItem(NbrJeuxEmprunte_Reserve, 1, new QStandardItem(RequeteMembre.value(1).toString() ));
                this->ModelMembre->setItem(NbrJeuxEmprunte_Reserve, 2, new QStandardItem(RequeteMembre.value(2).toString() ));
                this->ModelMembre->setItem(NbrJeuxEmprunte_Reserve, 3, new QStandardItem(RequeteMembre.value(3).toString() ));
                NbrJeuxEmprunte_Reserve++;

             }

    }
    //sinon,
    else
    {

                //On Affiche tout les membre :
                QSqlQuery RequeteMembre;
                NbrJeuxEmprunte_Reserve =0;
                RequeteMembre.exec("SELECT  `CodeMembre`,`Nom`,Prenom,`DateNaissance` FROM`membres` ORDER BY Nom ASC");

                //On vide le mod?le
                this->ModelMembre->clear();
                //Indique le nombes de colones puis leurs noms
                this->ModelMembre->setColumnCount(4);
                this->ModelMembre->setHorizontalHeaderItem(0, new QStandardItem("Code"));
                this->ModelMembre->setHorizontalHeaderItem(1, new QStandardItem("Nom"));
                this->ModelMembre->setHorizontalHeaderItem(2, new QStandardItem("Prénom"));
                this->ModelMembre->setHorizontalHeaderItem(3, new QStandardItem("Date de naissance"));
                //impose une taille aux colones
                ui->TbV_Recherche->setColumnWidth(0,40);
                ui->TbV_Recherche->setColumnWidth(3,40);
                ui->TbV_Recherche->verticalHeader()->setVisible(false);

                //Tant qu'il y a des membres dans la table membres,
                while(RequeteMembre.next())
                    {
                        //on ajoute une nouvelle ligne du table view et la emplie avec le membre
                        this->ModelMembre->setItem(NbrJeuxEmprunte_Reserve, 0, new QStandardItem(RequeteMembre.value(0).toString() ));
                        this->ModelMembre->setItem(NbrJeuxEmprunte_Reserve, 1, new QStandardItem(RequeteMembre.value(1).toString() ));
                        this->ModelMembre->setItem(NbrJeuxEmprunte_Reserve, 2, new QStandardItem(RequeteMembre.value(2).toString() ));
                        this->ModelMembre->setItem(NbrJeuxEmprunte_Reserve, 3, new QStandardItem(RequeteMembre.value(3).toString() ));
                        NbrJeuxEmprunte_Reserve++;

                     }
    }

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////Clic sur un membre dans le tableau//////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/** Description détaillée de la méthode
 *  @pre    Connexion avec la base de donnée, tableau des membre remplie
 *  @post
 *  @param
 *  @retval
 *  @return
 *  @test
 *  @see    ModelMembre
 */
void F_Emprunt::on_TbV_Recherche_clicked(const QModelIndex &index)
{
    //Met le code du membre sellectionné dans le line édit du code du membre
    ui->LE_CodeMembre->setText(this->ModelMembre->index(index.row(),0).data().toString());
    //clic sur la recherche du membre
    on_Bt_RechercheOK_clicked();
    ui->TbV_Recherche->selectRow(index.row());
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Pression sur entrée pour le code membre////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/** Description détaillée de la méthode
*  @pre    Connexion avec la base de donnée, line édit du code membre rempli
*  @post
*  @param
*  @retval
*  @return
*  @test
*  @see
*/
void F_Emprunt::on_LE_CodeMembre_returnPressed()
{
    //"clic" sur la recherche du membre
    on_Bt_RechercheOK_clicked();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Clic sur le bonton OK du code membre///////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/** Description détaillée de la méthode
 *  @pre    Connexion avec la base de donnée, line édit du code membre rempli
 *  @post
 *  @param
 *  @retval
 *  @return
 *  @test
 *  @see    ModelMembre
 */
void F_Emprunt::on_Bt_RechercheOK_clicked()
{
    //S'il y ? encore des emprunts ? payer
    if(this->NmbLigneNewEmprunts != 0)
    {
        // affichage d'un message d'erreur
        QMessageBox::warning(this,"Emprunt non réglé","Attention ! \nVous n'avez payé les emprunts.","Ok");
        ui->LE_CodeMembre->setText(this->MembreActif);
        return;
    }
    QString CodeMembre =ui->LE_CodeMembre->text();

// Actualise l'atribut MembreActif avec le code du membre que l'on affiche
    MembreActif=CodeMembre;
//Affiche les informations du membre
    this->AfficherMembre(CodeMembre);

//Vide les informations du jeu et met l'attribut "jeux actif" a vide
    ViderJeu();
    this->JeuActif="";
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Actualiser le membre en cours de consultation//////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/** Description détaillée de la méthode
 *  @pre    Connexion avec la base de donnée, un membre déj? sellectionné
 *  @post
 *  @param
 *  @retval
 *  @return
 *  @test
 *  @see    MembreActif
 */
void F_Emprunt::ActualiserMembre()
{
    AfficherMembre(this->MembreActif);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Afficher le membre dans le volet central///////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/** Description détaillée de la méthode
 *  @pre    Connexion avec la base de donnée
 *  @post
 *  @param  Code du membre
 *  @retval
 *  @return
 *  @test
 *  @see    MembreActif
 */
void F_Emprunt::AfficherMembre(QString CodeMembre)
{

    this->MembreActif=CodeMembre;

    if(CodeMembre!="")
    {
        QSqlQuery Requete;

    //Grise les boutons de modification des remarques du membre
        ui->Bt_ValiderRemarques->setEnabled(false);
        ui->Bt_AnnulerRemarques->setEnabled(false);


    //Prépare la requête
        Requete.prepare("SELECT Nom,Prenom,NbreRetard,Ecarte,Remarque,NbreJeuxAutorises,IdMembre FROM membres WHERE CodeMembre=:CodeDuMembre");
        Requete.bindValue(":CodeDuMembre",CodeMembre);

    //Execute la requête
        if (!Requete.exec())
        {

        }

        Requete.next();


    //Récup?re le Nom dans la base de données puis l'affiche
        ui->Le_NomARemplir->setText(Requete.value(0).toString());


    //Récup?re le Prénom dans la base de données puis l'affiche
        ui->Le_PrenomARemplir->setText(Requete.value(1).toString());

    //Récup?re le nombre de retards dans la base de données  puis l'affiche
        ui->Le_RetardARemplir->setText(Requete.value(2).toString());

    //Regarde dans la base de données si le membre est écarté
        bool MembreEcarte = (Requete.value(3).toBool());
        if (MembreEcarte)
        {
            //s'il l'est, on affiche en rouge que le membre est écarté
            ui->Lb_MembreEcarte->setStyleSheet("QLabel {color:red;}");
            ui->Lb_MembreEcarte->setText("Membre écarté ");
        }
        else
        {
           //s'il ne l'est pas, on affiche rien
           ui->Lb_MembreEcarte->setText(" ");
        }

    //Récup?re les remarques dans la base de données puis les affiches
        ui->TxE_Remarques->setText(Requete.value(4).toString());

    //Grise les boutons de modification des remarques du membre
        ui->Bt_ValiderRemarques->setEnabled(false);
        ui->Bt_AnnulerRemarques->setEnabled(false);

    //Grise les bouons de suppression des tableau des jeux
           ui->bt_NewEmprunt->setEnabled(false);
           ui->bt_JeuxReserves->setEnabled(false);

    //Récup?re le nombre de jeux empruntables dans la base de données puis l'affiche
           ui->Le_NbreJeuxEmpr->setText(Requete.value(5).toString());


    //Affiche l'état de la cotisation
           //Savoir si le membre ? un memmbre assosier
           QSqlQuery RequeteMembreAssocier ;
           RequeteMembreAssocier.prepare("SELECT `MembreAssocie`FROM`membres` WHERE `CodeMembre`=:CodeDuMembre AND `MembreAssocie` !=0");
           RequeteMembreAssocier.bindValue(":codeDuMembre",this->MembreActif);
           RequeteMembreAssocier.exec();
           RequeteMembreAssocier.next();
           //s'i y en a un,
           if(RequeteMembreAssocier.size()>0)
           {
               //On Affiche l'état de la cotisation du membre associé au membre actif
               this->EtatCotisationMembre= AfficherEtatCotisation(RequeteMembreAssocier.value(0).toString());
           }
           //Sinon,
           else
           {
               //On affiche l'état de la cotisation du membre en cours de consultation
              this->EtatCotisationMembre= AfficherEtatCotisation(this->MembreActif);
           }




    //Affiche les jeux empruntés
        AfficherJeuxEnEmprunt();
    //Affiche les jeux réservés
        AfficherJeuxReserve();
    //Affiche le nombre de crédits restants
        CalculerCreditsRestants();
    }



}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////Afficher l'état de la cotisation d'un membre/////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/** Description détaillée de la méthode
 *  @pre    Connexion avec la base de donnée
 *  @post
 *  @param  Code du membre
 *  @retval
 *  @return CotisationAJours
 *  @test
 *  @see
 */
bool F_Emprunt::AfficherEtatCotisation(QString CodeMembre)
{
    bool EtatDeLaCotisation;
    //Connaitre la date d'expiration de la cotisation anuelle la plus récente
    QSqlQuery RequeteCotisation ;
    RequeteCotisation.prepare("SELECT DateExpiration "
                             "FROM abonnements, membres "
                             "WHERE Prestations_IdPrestation IS NOT NULL  AND Membres_IdMembre=IdMembre AND CodeMembre=:CodeDuMembre ORDER BY DateExpiration DESC " );
    RequeteCotisation.bindValue(":CodeDuMembre",CodeMembre );


    if(!RequeteCotisation.exec())
    {
        qDebug()<<"Emprunt affiche info membre  requete état cotisation  "<<RequeteCotisation.lastError();
    }

    RequeteCotisation.next();

    QDate DateActuelle;
    DateActuelle= DateActuelle.currentDate();

    int NombreJoursRestant ;
    //Nombre de jours qu'il reste ? la cotisation
    NombreJoursRestant=DateActuelle.daysTo(RequeteCotisation.value(0).toDate());

    //si le nombres de jours restant est négatif la cotisation est expirée
    if(NombreJoursRestant<0)
    {
        QString Cotisation;
        //Si le code membre est différent du membre actif, alors
        if(MembreActif!=CodeMembre)
        {
            //On précise le code du membre qui poc?de la cotisation
            Cotisation="Cotisation expirée (membre "+ CodeMembre +") depuis le ";
        }
        //Sinon,
        else
        {
            //On affiche juste l'état de la cotisation
            Cotisation="Cotisation expirée depuis le";
        }
        ui->Lb_Cotisation->setText(Cotisation);
        ui->Lb_Cotisation->setStyleSheet("QLabel {color:red;}");
        ui->Lb_CotisationARemplir->setText(RequeteCotisation.value(0).toDate().toString("ddd d MMMM yyyy"));
        ui->Lb_CotisationARemplir->setStyleSheet(" QLabel{color:red;}" );
        EtatDeLaCotisation=false;
    }
    else
    {
        QString Cotisation;
        //Si le code membre est différent du membre actif, alors
        if(MembreActif!=CodeMembre)
        {
            //On précise le code du membre qui poc?de la cotisation
            Cotisation="Cotisation ? jours (membre "+ CodeMembre +")";
        }
        //Sinon,
        else
        {
            //On affiche juste l'état de la cotisation
            Cotisation="Cotisation ? jours";

        }
        EtatDeLaCotisation=true;
        QString JourRestant ;
        JourRestant.setNum(NombreJoursRestant);
        ui->Lb_Cotisation->setText(Cotisation);
        ui->Lb_CotisationARemplir->setText("("+ JourRestant +" jours restant)");
        //Si le nombre de jonrs restant est inferieur ? 30 jours, on l'affiche en orange
        if (NombreJoursRestant<31)
        {
            ui->Lb_Cotisation->setStyleSheet("QLabel {color:orange;}");
            ui->Lb_CotisationARemplir->setStyleSheet(" QLabel{color:orange;}" );
        }
        //sinon, on l'affiche ne vert
        else
        {
            ui->Lb_Cotisation->setStyleSheet("QLabel {color:green;}");
            ui->Lb_CotisationARemplir->setStyleSheet(" QLabel{color:green;}" );
        }
    }
    return EtatDeLaCotisation;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Changement de la remarque du membre////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/** Description détaillée de la méthode
 *  @pre    Connexion avec la base de donnée, un membre est actif dans la classe
 *  @post
 *  @param
 *  @retval
 *  @return
 *  @test
 *  @see
 */
void F_Emprunt::on_TxE_Remarques_textChanged()
{
//Affiche les boutons de modification de la remarque du membre
    ui->Bt_ValiderRemarques->setEnabled(true);
    ui->Bt_AnnulerRemarques->setEnabled(true);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Enregister la remarque du membre///////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/** Description détaillée de la méthode
 *  @pre    Connexion avec la base de donnée, un membre est actif dans la classe, la remarque ? été modifiée
 *  @post
 *  @param
 *  @retval
 *  @return
 *  @test
 *  @see    MembreActif
 */
void F_Emprunt::on_Bt_ValiderRemarques_clicked()
{
    QSqlQuery Requete;

//prépare le requête de mise ? jour
    Requete.prepare("UPDATE `membres` SET `Remarque` =:NouvelRemarque WHERE `CodeMembre`=:CodeDuMembre");

//Entre les valeurs de la reqête
    Requete.bindValue(":CodeDuMembre",MembreActif);
    Requete.bindValue(":NouvelRemarque",ui->TxE_Remarques->toPlainText());

 //Execut la requête
    if (!Requete.exec())
    {

    }

//Grise les boutons de modification de le remarque
    ui->Bt_ValiderRemarques->setEnabled(false);
    ui->Bt_AnnulerRemarques->setEnabled(false);

}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Annuler changement de la remaque du membre/////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/** Description détaillée de la méthode
*  @pre    Connexion avec la base de donnée, membre actif dans la classe, remarque modifiée
*  @post
*  @param
*  @retval
*  @return
*  @test
*  @see    MembreActif
*/
void F_Emprunt::on_Bt_AnnulerRemarques_clicked()
{
    QSqlQuery Requete;



    Requete.prepare("SELECT Remarque FROM membres WHERE CodeMembre=:CodeDuMembre");
    Requete.bindValue(":CodeDuMembre",MembreActif);


//Execut la requête
    if (!Requete.exec())
    {

    }

    Requete.next();

//Récup?re les remarques dans la base de données et les affiches
    QString TextTemporaire = (Requete.value(0).toString());
    ui->TxE_Remarques->setText(TextTemporaire);

//Grise les boutons de modification des remarques du membre
    ui->Bt_ValiderRemarques->setEnabled(false);
    ui->Bt_AnnulerRemarques->setEnabled(false);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////Recherche du nombre de crédits restants//////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/** Description détaillée de la méthode
 *  @pre    Connexion avec la base de donnée, un membre est actif dans la classe
 *  @post
 *  @param
 *  @retval
 *  @return
 *  @test
 *  @see    MembreActif, ModelJeuEmpruntes
 */
void F_Emprunt::CalculerCreditsRestants()
{
    int CreditRestant(0);

    //rechercher les crédits restants dans toutes les cartes prépayées du membre
    QSqlQuery RequeteCartes;
    RequeteCartes.prepare("SELECT `CreditRestant` "
                          "FROM `abonnements`,`cartesprepayees`,`membres` "
                          "WHERE `abonnements`.`Membres_IdMembre`=`IdMembre` "
                                   "AND `CodeMembre`=:CodeDuMembre "
                                   "AND `abonnements`.`CartesPrepayees_IdCarte` IS NOT NULL "
                                   "AND `abonnements`.`CreditRestant` >0 "
                                   "AND `IdCarte`=`CartesPrepayees_IdCarte`");


    RequeteCartes.bindValue("CodeDuMembre",this->MembreActif);
    RequeteCartes.exec();

    while(RequeteCartes.next())
    {
        CreditRestant=CreditRestant+RequeteCartes.value(0).toInt();
    }
    QString Credit;

    ui->Le_CreditRestantARemplir->setText(Credit.setNum(CreditRestant));
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Recherche des jeux en emprunt//////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/** Description détaillée de la méthode
 *  @pre    Connexion avec la base de donnée, un membre est actif dans la classe
 *  @post
 *  @param
 *  @retval
 *  @return
 *  @test
 *  @see    MembreActif, ModelJeuEmpruntes
 */
void F_Emprunt::AfficherJeuxEnEmprunt()
{
    QDate DateActuelle;
    QSqlQuery RequeteIdMembre;
    unsigned int IdDuMembre (0);

    DateActuelle=DateActuelle.currentDate();

//Prépare le reqête pour récupérer l'id du membre
    RequeteIdMembre.prepare("SELECT IdMembre FROM membres WHERE CodeMembre=:CodeDuMembre");
    RequeteIdMembre.bindValue(":CodeDuMembre",MembreActif);

//Execute la requête
    if (!RequeteIdMembre.exec())
    {
        qDebug()<< "F_Emprunt::JeuxEnEmprunt ||| RequeteIdMembre  "<< RequeteIdMembre.lastError();
    }

    RequeteIdMembre.next();

    //Récup?re l'id dans la base de données
    IdDuMembre=RequeteIdMembre.value(0).toInt();

    QSqlQuery RequeteJeuEmprunte;
    unsigned int NbrJeuxEmprunte_Reserve =0;
    RequeteJeuEmprunte.prepare("SELECT `DateEmprunt`,`DateRetourPrevu`,NomJeu,CodeJeu FROM `emprunts`,jeux WHERE `Membres_IdMembre`=:IdDuMembre AND`IdJeux`=`Jeux_IdJeux` AND `DateRetour` IS NULL");
    RequeteJeuEmprunte.bindValue(":IdDuMembre",IdDuMembre);
    if(!RequeteJeuEmprunte.exec())
    {
        qDebug()<<"JeuxEnEmprunt() ||| Requette jeux en emprunt "<<RequeteJeuEmprunte.lastError();
    }



    //On vide le mod?le
    this->ModelJeuEmpruntes->clear();
    //Initialise les colones du TableView des nouveaux emprunts
         this->ModelJeuEmpruntes->setColumnCount(4);
         this->ModelJeuEmpruntes->setHorizontalHeaderItem(0, new QStandardItem("Code"));
         this->ModelJeuEmpruntes->setHorizontalHeaderItem(1, new QStandardItem("Nom du jeu"));
         this->ModelJeuEmpruntes->setHorizontalHeaderItem(2, new QStandardItem("Date emprunt"));
         this->ModelJeuEmpruntes->setHorizontalHeaderItem(3, new QStandardItem("Date retour"));
         ui->TbV_JeuxMembres->setColumnWidth(0,40);
         ui->TbV_JeuxMembres->setColumnWidth(1,125);
         ui->TbV_JeuxMembres->verticalHeader()->setVisible(false);

    //Tant qu'il y a des jeux,
    while(RequeteJeuEmprunte.next())
    {
        //on ajoute une nouvelle ligne du table view
        this->ModelJeuEmpruntes->setItem(NbrJeuxEmprunte_Reserve, 0, new QStandardItem(RequeteJeuEmprunte.value(3).toString() ));
        this->ModelJeuEmpruntes->setItem(NbrJeuxEmprunte_Reserve, 1, new QStandardItem(RequeteJeuEmprunte.value(2).toString()));
        this->ModelJeuEmpruntes->setItem(NbrJeuxEmprunte_Reserve, 2, new QStandardItem(RequeteJeuEmprunte.value(0).toDate().toString("ddd d MMMM yyyy") ));
        this->ModelJeuEmpruntes->setItem(NbrJeuxEmprunte_Reserve, 3, new QStandardItem(RequeteJeuEmprunte.value(1).toDate().toString("ddd d MMMM yyyy") ));

        if (RequeteJeuEmprunte.value(1).toDate()>DateActuelle)
        {
            this->ModelJeuEmpruntes->setData(ModelJeuEmpruntes->index(NbrJeuxEmprunte_Reserve,3),QColor(Qt::green), Qt::BackgroundColorRole);
        }
        else
        {
            this->ModelJeuEmpruntes->setData(ModelJeuEmpruntes->index(NbrJeuxEmprunte_Reserve,3),QColor(Qt::red), Qt::BackgroundColorRole);
        }
        NbrJeuxEmprunte_Reserve++;

    }

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Recherche des jeux réservés////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/** Description détaillée de la méthode
 *  @pre    Connexion avec la base de donnée, un membre est actif dans la classe
 *  @post
 *  @param
 *  @retval
 *  @return
 *  @test
 *  @see    MembreActif, ModelJeuReserves
 */
void F_Emprunt::AfficherJeuxReserve()
{

    QSqlQuery RequeteIdMembre;
    unsigned int IdDuMembre (0);

//Prépare le reqête pour récupérer l'id du membre
    RequeteIdMembre.prepare("SELECT IdMembre FROM membres WHERE CodeMembre=:CodeDuMembre");
    RequeteIdMembre.bindValue(":CodeDuMembre",MembreActif);

//Execute la requête
    if (!RequeteIdMembre.exec())
    {
        qDebug()<<"F_Emprunt::JeuxReserve ||| Requete IdDuMembre "<<RequeteIdMembre.lastError();
    }

    RequeteIdMembre.next();

    //Récup?re l'es remarques'id dans la base de données
    IdDuMembre=RequeteIdMembre.value(0).toInt();

    QSqlQuery RequeteJeuReserve;
    unsigned int NbrJeuxEmprunte_Reserve =0;
    RequeteJeuReserve.prepare("SELECT `DateReservation`,`DatePrevuEmprunt`,`DatePrevuRetour`,`NomLieux`,`NomJeu`,`CodeJeu`"
                              "FROM `reservation`,`lieux` ,`jeux`"
                              "WHERE  `JeuEmprunte`=1 AND`Membres_IdMembre`=:IdMembre AND`IdLieux`=`Lieux_IdLieux` AND `IdJeux`=`Jeux_IdJeux`" );

    RequeteJeuReserve.bindValue(":IdMembre",IdDuMembre);

    if (!RequeteJeuReserve.exec())
    {
        qDebug()<<"F_Emprunt::JeuxReserve ||| Requete Jeux Reservés "<<RequeteJeuReserve.lastError();
    }


    //On vide le mod?le
    this->ModelJeuReserves->clear();
    //Initialise les colones du TableView des nouveaux emprunts
         this->ModelJeuReserves->setColumnCount(5);
         this->ModelJeuReserves->setHorizontalHeaderItem(0, new QStandardItem("Code"));
         this->ModelJeuReserves->setHorizontalHeaderItem(1, new QStandardItem("Nom du jeu"));
         this->ModelJeuReserves->setHorizontalHeaderItem(2, new QStandardItem("Date réservation"));
         this->ModelJeuReserves->setHorizontalHeaderItem(3, new QStandardItem("Date emprunt"));
         this->ModelJeuReserves->setHorizontalHeaderItem(4, new QStandardItem("Date retour"));
         this->ModelJeuReserves->setHorizontalHeaderItem(5, new QStandardItem("lieu"));
         ui->TbV_JeuxReserves->setColumnWidth(0,40);
         ui->TbV_JeuxReserves->setColumnWidth(1,125);
         ui->TbV_JeuxReserves->verticalHeader()->setVisible(false);

    //Tant qu'il y a des membres dans la table membres,
    while(RequeteJeuReserve.next())
        {

            //on ajoute une nouvelle ligne du table view
            this->ModelJeuReserves->setItem(NbrJeuxEmprunte_Reserve, 0, new QStandardItem(RequeteJeuReserve.value(5).toString() ));
            this->ModelJeuReserves->setItem(NbrJeuxEmprunte_Reserve, 1, new QStandardItem(RequeteJeuReserve.value(4).toString()));
            this->ModelJeuReserves->setItem(NbrJeuxEmprunte_Reserve, 2, new QStandardItem(RequeteJeuReserve.value(0).toDate().toString("ddd d MMMM yyyy") ));
            this->ModelJeuReserves->setItem(NbrJeuxEmprunte_Reserve, 3, new QStandardItem(RequeteJeuReserve.value(1).toDate().toString("ddd d MMMM yyyy") ));
            this->ModelJeuReserves->setItem(NbrJeuxEmprunte_Reserve, 4, new QStandardItem(RequeteJeuReserve.value(2).toDate().toString("ddd d MMMM yyyy") ));
            this->ModelJeuReserves->setItem(NbrJeuxEmprunte_Reserve, 5, new QStandardItem(RequeteJeuReserve.value(3).toString() ));

            //Savoir si le jeu est disponible ou non
            QSqlQuery Requetejeu;
            Requetejeu.prepare("SELECT `IdEmprunts` FROM`emprunts`,`jeux`WHERE `CodeJeu`=:CodeDuJeu AND`Jeux_IdJeux`=`IdJeux` AND `DateRetour`IS NULL");
            Requetejeu.bindValue(":CodeDuJeu",RequeteJeuReserve.value(5).toString());
             Requetejeu.exec();
            if(Requetejeu.size()==0)
            {
                this->ModelJeuReserves->setData(ModelJeuReserves->index(NbrJeuxEmprunte_Reserve,3),QColor(Qt::green), Qt::BackgroundColorRole);
            }
            else
            {
                this->ModelJeuReserves->setData(ModelJeuReserves->index(NbrJeuxEmprunte_Reserve,3),QColor(Qt::red), Qt::BackgroundColorRole);
            }


            NbrJeuxEmprunte_Reserve++;

        }

}




//#######################################################################################################
//////////////////////////////////////////////JEUX///////////////////////////////////////////////////////
//#######################################################################################################


/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Pression sur Entrée lors du code du jeu////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/** Description détaillée de la méthode
 *  @pre    Connexion avec la base de donnée, un code de jeu ? été saisi
 *  @post
 *  @param
 *  @retval
 *  @return
 *  @test
 *  @see
 */
void F_Emprunt::on_LE_CodeJeu_returnPressed()
{
    on_Bt_OK_clicked();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////Actualise les informations du jeu////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/** Description détaillée de la méthode
 *  @pre    Connexion avec la base de donnée, un jeu est actif dans la classe
 *  @post
 *  @param
 *  @retval
 *  @return
 *  @test
 *  @see    JeuActif
 */
void F_Emprunt::ActualiserJeu()
{
    if (this->JeuActif!="")
    {
        ui->LE_CodeJeu->setText(this->JeuActif);
        on_Bt_OK_clicked();
    }

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////Valider Jeu et l'afficher dans le volet de droite////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/** Description détaillée de la méthode
 *  @pre    Connexion avec la base de donnée, un jeu est actif dans la classe
 *  @post
 *  @param
 *  @retval
 *  @return
 *  @test
 *  @see    JeuActif, MembreActif
 */
void F_Emprunt::on_Bt_OK_clicked()
{
//Vérification qu'il y ? un membre sélectionné.
    QSqlQuery RequeteMembre;
    RequeteMembre.prepare("SELECT `CodeMembre` FROM `membres` WHERE `CodeMembre`=:CodeDuMembre");
    RequeteMembre.bindValue(":CodeDuMembre",this->MembreActif);
    RequeteMembre.exec();
    RequeteMembre.next();
    if(RequeteMembre.size()==0)
    {
        //On envoi un message d'erreur
        QMessageBox::warning(this,"Pas de membre","Attention ! \nVous n'avez sélectionné aucun membre.","Ok");
        return;
    }


//Récup?re le code du jeu qui ? été saisi
    this->JeuActif= ui->LE_CodeJeu->text();

    //Si le code entré est nul
    if(this->JeuActif=="")
    {
        //On envoi un message d'erreur
        QMessageBox::warning(this,"Pas de jeu","Attention ! \nVous n'avez entré aucun jeu.","Ok");
        return;
    }


    QSqlQuery Requete;
//Prépare la requête et entre ses valeurs
    Requete.prepare("SELECT NomJeu, ContenuJeu, PrixLoc, Caution, Remarque, StatutJeux_IdStatutJeux, EtatsJeu_idEtatsJeu, `IdJeux` FROM jeux WHERE CodeJeu=:CodeDuJeu");

    Requete.bindValue(":CodeDuJeux",this->JeuActif);

//Execute la requête
    if (!Requete.exec())
    {

    }

    Requete.next();

//Récup?re le nom du jeu et l'affiche
    QString TextTemporaire = (Requete.value(0).toString());
    ui->Le_NomJeuARemplir->setText( TextTemporaire ) ;

//Récup?re le contenu de le boîte et l'affiche
    TextTemporaire=(Requete.value(1).toString()) ;
    ui->TxE_ContenuBoite->setText(TextTemporaire );

//Récup?re le prix de l'emprunt et l'affiche
    TextTemporaire = (Requete.value(2).toString());
    ui->Le_PrixEmpruntARemplir->setText(TextTemporaire);

//Récup?re le prix de la caution et l'affiche
    TextTemporaire = (Requete.value(3).toString());
    ui->Le_PrixCautionARemplir->setText(TextTemporaire);

//Récup?re la remarque et l'affiche
    TextTemporaire = (Requete.value(4).toString());
    ui->TxE_RemarquesJeu->setText(TextTemporaire);

//Récup?re l'id du statut
    unsigned int IdStatut =(Requete.value(5).toInt());

//Grise les boutons de modification de la remarque du jeu
    ui->Bt_ValiderRemarquesJeu->setEnabled(false);
    ui->Bt_AnnulerRemarquesJeu->setEnabled(false);

//Récup?re l'id de l'état
    unsigned int IdEtat =(Requete.value(6).toInt());


    QSqlQuery RequeteStatut;

//Prépare la requête pour cherche le statut du jeu et l'execute
    RequeteStatut.prepare("SELECT StatutJeu FROM statutjeux WHERE IdStatutJeux=:IdStatutDuJeu");
    RequeteStatut.bindValue(":IdStatutDuJeu",IdStatut);

    if (!RequeteStatut.exec())
    {

    }

    RequeteStatut.next();

    //si le statut est "Disponible", alors
    if (IdStatut==1)
    {
        //Le metre en vert
        ui->Le_StatutJeuARemplir->setStyleSheet("QLineEdit {color:green;}");
        ui->Lb_StatutJeu->setStyleSheet("QLabel {color:green;}");
        //Met le bouton "Ajouté" en cliquable
        ui->Bt_Ajouter->setEnabled(true);
    }
    else
    {
        //si le statut est "Résevé", alors
        if (IdStatut==4)
        {
        //Rechercher si la personne qui ? emprunté est ce membre
            //Prépare le reqête pour récupérer l'id du membre
            QSqlQuery RequeteIdMembre;
            RequeteIdMembre.prepare("SELECT IdMembre FROM membres WHERE CodeMembre=:CodeDuMembre");
            RequeteIdMembre.bindValue(":CodeDuMembre",this->MembreActif);


            //Execute la requête
            if (!RequeteIdMembre.exec())
            {
                qDebug()<<"F_Emprunt::on_Bt_Ajouter_clicked ||| requette IdMembre  "<<RequeteIdMembre.lastError() ;
            }

            RequeteIdMembre.next();

            //Recherche de l'id du membre qui ? réservé le jeu
            QSqlQuery RequeteJeuReserve;
            RequeteJeuReserve.prepare("SELECT Membres_IdMembre FROM `reservation` WHERE  `JeuEmprunte`=1 AND `Jeux_IdJeux`=:IdDuJeu");
            RequeteJeuReserve.bindValue(":IdDuJeu",Requete.value(7));

            if (!RequeteJeuReserve.exec())
            {

            }

            RequeteJeuReserve.next();

            //si l'id du membre est le même qie celui du réserveur,
            if(RequeteJeuReserve.value(0)==RequeteIdMembre.value(0))
            {
                //Le metre en vert
                ui->Le_StatutJeuARemplir->setStyleSheet("QLineEdit {color:green;}");
                ui->Lb_StatutJeu->setStyleSheet("QLabel {color:green;}");
                //Met le bouton "Ajouté" en cliquable
                ui->Bt_Ajouter->setEnabled(true);
            }
            else
            {
                //Le metre en rouge
                ui->Le_StatutJeuARemplir->setStyleSheet("QLineEdit {color:red;}");
                ui->Lb_StatutJeu->setStyleSheet("QLabel {color:red;}");
                //Met le bouton "Ajouté" en cliquable
                ui->Bt_Ajouter->setEnabled(false);
            }

        }
        else
        {
            //Le metre en rouge
            ui->Le_StatutJeuARemplir->setStyleSheet("QLineEdit {color:red;}");
            ui->Lb_StatutJeu->setStyleSheet("QLabel {color:red;}");
            //Met le bouton "Ajouté" en cliquable
            ui->Bt_Ajouter->setEnabled(false);

        }
    }

//Récup?re le statut et l'affiche
    TextTemporaire = (RequeteStatut.value(0).toString());
    ui->Le_StatutJeuARemplir->setText(TextTemporaire);

    QSqlQuery RequeteEtat;

//Prépare la requête pour cherche l'état du jeu et l'execute
    RequeteEtat.prepare("SELECT `Etat` FROM `etatsjeu` WHERE `idEtatsJeu`=:IdEtatDuJeu");
    RequeteEtat.bindValue(":IdEtatDuJeu",IdEtat);

    if (!RequeteEtat.exec())
    {

    }

    RequeteEtat.next();

    //Récup?re l'état et l'affiche
    TextTemporaire = (RequeteEtat.value(0).toString());
    ui->Le_EtatJeuARemplir->setText(TextTemporaire);

//met le focus sur le bouton "Ajouter"
    ui->Bt_Ajouter->setFocus();
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Changement du type d'emprunt///////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/** Description détaillée de la méthode
 *  @pre    Connexion avec la base de donnée
 *  @post
 *  @param  Index du ComboBox
 *  @retval
 *  @return
 *  @test
 *  @see
 */
void F_Emprunt::on_CBx_TypeEmprunt_currentIndexChanged(int index)
{
//Calcule la nouvelle date du retour
    this->CalculerDateRetour();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////Calcule de la Date de retout /////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/** Description détaillée de la méthode
 *  @pre    Connexion avec la base de donnée
 *  @post
 *  @param
 *  @retval
 *  @return
 *  @test
 *  @see
 */
void F_Emprunt::CalculerDateRetour()
{
    QDate DateRetour;
 //DateRetour prend la date actuelle
    DateRetour=DateRetour.currentDate();

    QString TypeEmprunt ;

//On cherque l'id du ComboBox qui détermine le type d'emprunt
    TypeEmprunt=ui->CBx_TypeEmprunt->currentText().left(ui->CBx_TypeEmprunt->currentText().indexOf("(")-1);

//On prépare et exécute la requête qui permet de récupérer la durée dy type d'emprunt
    QSqlQuery Requete;

    Requete.prepare("SELECT DureeEmprunt FROM typeemprunt WHERE TypeEmprunt=:TypeDeLEmprunt");
    Requete.bindValue(":TypeDeLEmprunt",TypeEmprunt);
    Requete.exec();
    Requete.next();

    DateRetour=DateRetour.addDays(Requete.value(0).toInt());


    ui->DtE_Retour->setDate(DateRetour);

}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////Changement de la remaque du jeu/////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/** Description détaillée de la méthode
 *  @pre    Connexion avec la base de donnée, un jeu est actif dans la classe
 *  @post
 *  @param
 *  @retval
 *  @return
 *  @test
 *  @see
 */
void F_Emprunt::on_TxE_RemarquesJeu_textChanged()
{
//Affiche les boutons de modification de la remarque du jeu
    ui->Bt_ValiderRemarquesJeu->setEnabled(true);
    ui->Bt_AnnulerRemarquesJeu->setEnabled(true);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Enregistrer la remarque du jeu/////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/** Description détaillée de la méthode
 *  @pre    Connexion avec la base de donnée, un jeu est actif dans la classe, la remarque ? été modifiée
 *  @post
 *  @param
 *  @retval
 *  @return
 *  @test
 *  @see    JeuActif
 */
void F_Emprunt::on_Bt_ValiderRemarquesJeu_clicked()
{
    QSqlQuery Requete;



//prépare le requête de mise ? jour
    Requete.prepare("UPDATE `jeux` SET `Remarque` =:NouvelRemarque WHERE `CodeJeu`=:CodeDuJeu");

//Entre les valeurs de la reqête
    Requete.bindValue(":CodeDuJeu",JeuActif);
    Requete.bindValue(":NouvelRemarque",ui->TxE_RemarquesJeu->toPlainText());

//Execut la requête
    if (!Requete.exec())
    {

    }

//Grise les boutons de modification de le remarque
    ui->Bt_ValiderRemarquesJeu->setEnabled(false);
    ui->Bt_AnnulerRemarquesJeu->setEnabled(false);


}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////Anuler le changement de la remarque du jeu///////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/** Description détaillée de la méthode
 *  @pre    Connexion avec la base de donnée, un jeu est actif dans la classe, la remarque ? été modifiée
 *  @post
 *  @param
 *  @retval
 *  @return
 *  @test
 *  @see    JeuActif
 */
void F_Emprunt::on_Bt_AnnulerRemarquesJeu_clicked()
{
    QSqlQuery Requete;


//prépare le reqête d'annulation
    Requete.prepare("SELECT Remarque FROM jeux WHERE CodeJeu=:CodeDuJeu");
    Requete.bindValue(":CodeDuJeu",JeuActif);

//Execute la requête
    if (!Requete.exec())
    {

    }

    Requete.next();

//Récup?re les remarques dans la base de données
    QString TextTemporaire = (Requete.value(0).toString());
    ui->TxE_RemarquesJeu->setText(TextTemporaire);

//grise les boutons de modification des remarques du jeu
    ui->Bt_ValiderRemarquesJeu->setEnabled(false);
    ui->Bt_AnnulerRemarquesJeu->setEnabled(false);

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Vider les informations du jeu//////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/** Description détaillée de la méthode
 *  @pre    Connexion avec la base de donnée, des informations d'un jeu sont affichées
 *  @post
 *  @param
 *  @retval
 *  @return
 *  @test
 *  @see
 */
void F_Emprunt::ViderJeu()
{
    ui->LE_CodeJeu->setText("");
    ui->Le_NomJeuARemplir->setText("");
    ui->Le_StatutJeuARemplir->setText("");
    ui->Lb_StatutJeu->setStyleSheet("QLabel {color:black;}");
    ui->Le_EtatJeuARemplir->setText("");
    ui->Le_PrixEmpruntARemplir->setText("");
    ui->Le_PrixCautionARemplir->setText("");
    ui->TxE_ContenuBoite->setText("");
    ui->TxE_RemarquesJeu->setText("");
    ui->Bt_AnnulerRemarquesJeu->setEnabled(false);
    ui->Bt_ValiderRemarquesJeu->setEnabled(false);

    ui->Bt_Ajouter->setEnabled(false);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////Clic sur un jeu chez le membre dans le tableau///////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/** Description détaillée de la méthode
 *  @pre    Connexion avec la base de donnée, un menbre est actif dans la classe, il ? des jeux chez lui
 *  @post
 *  @param
 *  @retval
 *  @return
 *  @test
 *  @see
 */
void F_Emprunt::on_TbV_JeuxMembres_clicked(const QModelIndex &index)
{
    ui->TbV_JeuxMembres->selectRow(index.row());
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////Clic sur un jeu réservé dans le tableau//////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/** Description détaillée de la méthode
 *  @pre    Connexion avec la base de donnée, un membre est actif dans la classe, il ? des jeux réservés
 *  @post
 *  @param  index du tableau
 *  @retval
 *  @return
 *  @test
 *  @see    ModelJeuReserves
 */
void F_Emprunt::on_TbV_JeuxReserves_clicked(const QModelIndex &index)
{
    ui->TbV_JeuxReserves->selectRow(index.row());
    ui->bt_JeuxReserves->setEnabled(true);
    //Met le code du jeu sellectionné dans le line édit du code du membre
    ui->LE_CodeJeu->setText(this->ModelJeuReserves->index(index.row(),0).data().toString());
    //clic sur la recherche du jeu
    on_Bt_OK_clicked();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////Clic sur un nouvel emprunt dans le tableau///////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/** Description détaillée de la méthode
 *  @pre    Connexion avec la base de donnée, un membre est actif dans la classe, il y a un nouvel emprunt
 *  @post
 *  @param  index du tableau
 *  @retval
 *  @return
 *  @test
 *  @see
 */
void F_Emprunt::on_TbV_NewEmprunt_clicked(const QModelIndex &index)

{
    ui->TbV_NewEmprunt->selectRow(index.row());
    ui->bt_NewEmprunt->setEnabled(true);
/*
    QDate dates;

    dates=dates.currentDate() ;
    dates=dates.addDays(-30);
    for (register int i=1; i<100 ;i++)
    {
        dates=dates.addDays(1);
        QSqlQuery req ;
        req.prepare("INSERT INTO `abonnements`(`Prestations_IdPrestation`,`ModePaiement_IdModePaiement`,`CartesPrepayees_IdCarte`,`Membres_IdMembre`,`DateSouscription`,`DateExpiration`,`CreditRestant`)"
                    " VALUES ( 3,1,NULL,:Membre,NULL,:Date,NULL)");
        req.bindValue(":Membre",i);
        req.bindValue(":Date",dates);
        if(!req.exec())
        {

            qDebug()<< req.lastError();
        }

    }

    dates=dates.currentDate() ;
    dates=dates.addDays(-30);
    for (register int i=100; i<200 ;i++)
    {
        dates=dates.addDays(1);
        QSqlQuery req ;
        req.prepare("INSERT INTO `abonnements`(`Prestations_IdPrestation`,`ModePaiement_IdModePaiement`,`CartesPrepayees_IdCarte`,`Membres_IdMembre`,`DateSouscription`,`DateExpiration`,`CreditRestant`)"
                    " VALUES ( 3,1,NULL,:Membre,NULL,:Date,NULL)");
        req.bindValue(":Membre",i);
        req.bindValue(":Date",dates);
        if(!req.exec())
        {

            qDebug()<< req.lastError();
        }

    }

    dates=dates.currentDate() ;
    dates=dates.addDays(-30);
    for (register int i=200; i<300 ;i++)
    {
        dates=dates.addDays(1);
        QSqlQuery req ;
        req.prepare("INSERT INTO `abonnements`(`Prestations_IdPrestation`,`ModePaiement_IdModePaiement`,`CartesPrepayees_IdCarte`,`Membres_IdMembre`,`DateSouscription`,`DateExpiration`,`CreditRestant`)"
                    " VALUES ( 3,1,NULL,:Membre,NULL,:Date,NULL)");
        req.bindValue(":Membre",i);
        req.bindValue(":Date",dates);
        if(!req.exec())
        {

            qDebug()<< req.lastError();
        }

    }

    dates=dates.currentDate() ;
    dates=dates.addDays(-30);
    for (register int i=300; i<400 ;i++)
    {
        dates=dates.addDays(1);
        QSqlQuery req ;
        req.prepare("INSERT INTO `abonnements`(`Prestations_IdPrestation`,`ModePaiement_IdModePaiement`,`CartesPrepayees_IdCarte`,`Membres_IdMembre`,`DateSouscription`,`DateExpiration`,`CreditRestant`)"
                    " VALUES ( 3,1,NULL,:Membre,NULL,:Date,NULL)");
        req.bindValue(":Membre",i);
        req.bindValue(":Date",dates);
        if(!req.exec())
        {

            qDebug()<< req.lastError();
        }

    }

    dates=dates.currentDate() ;
    dates=dates.addDays(-30);
    for (register int i=400; i<500 ;i++)
    {
        dates=dates.addDays(1);
        QSqlQuery req ;
        req.prepare("INSERT INTO `abonnements`(`Prestations_IdPrestation`,`ModePaiement_IdModePaiement`,`CartesPrepayees_IdCarte`,`Membres_IdMembre`,`DateSouscription`,`DateExpiration`,`CreditRestant`)"
                    " VALUES ( 3,1,NULL,:Membre,NULL,:Date,NULL)");
        req.bindValue(":Membre",i);
        req.bindValue(":Date",dates);
        if(!req.exec())
        {

            qDebug()<< req.lastError();
        }

    }

    dates=dates.currentDate() ;
    dates=dates.addDays(-30);
    for (register int i=500; i<600 ;i++)
    {
        dates=dates.addDays(1);
        QSqlQuery req ;
        req.prepare("INSERT INTO `abonnements`(`Prestations_IdPrestation`,`ModePaiement_IdModePaiement`,`CartesPrepayees_IdCarte`,`Membres_IdMembre`,`DateSouscription`,`DateExpiration`,`CreditRestant`)"
                    " VALUES ( 3,1,NULL,:Membre,NULL,:Date,NULL)");
        req.bindValue(":Membre",i);
        req.bindValue(":Date",dates);
        if(!req.exec())
        {

            qDebug()<< req.lastError();
        }

    }

    dates=dates.currentDate() ;
    dates=dates.addDays(-30);
    for (register int i=600; i<700 ;i++)
    {
        dates=dates.addDays(1);
        QSqlQuery req ;
        req.prepare("INSERT INTO `abonnements`(`Prestations_IdPrestation`,`ModePaiement_IdModePaiement`,`CartesPrepayees_IdCarte`,`Membres_IdMembre`,`DateSouscription`,`DateExpiration`,`CreditRestant`)"
                    " VALUES ( 3,1,NULL,:Membre,NULL,:Date,NULL)");
        req.bindValue(":Membre",i);
        req.bindValue(":Date",dates);
        if(!req.exec())
        {

            qDebug()<< req.lastError();
        }

    }

    dates=dates.currentDate() ;
    dates=dates.addDays(-30);
    for (register int i=700; i<800 ;i++)
    {
        dates=dates.addDays(1);
        QSqlQuery req ;
        req.prepare("INSERT INTO `abonnements`(`Prestations_IdPrestation`,`ModePaiement_IdModePaiement`,`CartesPrepayees_IdCarte`,`Membres_IdMembre`,`DateSouscription`,`DateExpiration`,`CreditRestant`)"
                    " VALUES ( 3,1,NULL,:Membre,NULL,:Date,NULL)");
        req.bindValue(":Membre",i);
        req.bindValue(":Date",dates);
        if(!req.exec())
        {

            qDebug()<< req.lastError();
        }

    }

    dates=dates.currentDate() ;
    dates=dates.addDays(-30);
    for (register int i=800; i<900 ;i++)
    {
        dates=dates.addDays(1);
        QSqlQuery req ;
        req.prepare("INSERT INTO `abonnements`(`Prestations_IdPrestation`,`ModePaiement_IdModePaiement`,`CartesPrepayees_IdCarte`,`Membres_IdMembre`,`DateSouscription`,`DateExpiration`,`CreditRestant`)"
                    " VALUES ( 3,1,NULL,:Membre,NULL,:Date,NULL)");
        req.bindValue(":Membre",i);
        req.bindValue(":Date",dates);
        if(!req.exec())
        {

            qDebug()<< req.lastError();
        }

    }

    dates=dates.currentDate() ;
    dates=dates.addDays(-30);
    for (register int i=900; i<1000 ;i++)
    {
        dates=dates.addDays(1);
        QSqlQuery req ;
        req.prepare("INSERT INTO `abonnements`(`Prestations_IdPrestation`,`ModePaiement_IdModePaiement`,`CartesPrepayees_IdCarte`,`Membres_IdMembre`,`DateSouscription`,`DateExpiration`,`CreditRestant`)"
                    " VALUES ( 3,1,NULL,:Membre,NULL,:Date,NULL)");
        req.bindValue(":Membre",i);
        req.bindValue(":Date",dates);
        if(!req.exec())
        {

            qDebug()<< req.lastError();
        }

    }*/
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////Suppression d'un jeu réservé/////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/** Description détaillée de la méthode
 *  @pre    Connexion avec la base de donnée, un jeu est actif dans la classe, il y ? un jeu réservé
 *  @post
 *  @param
 *  @retval
 *  @return
 *  @test
 *  @see    ModelJeuReserves
 */
void F_Emprunt::on_bt_JeuxReserves_clicked()
{
    //Récupération de l'id du jeu avec son code
    QSqlQuery RequeteIdJeu;
    RequeteIdJeu.prepare("SELECT `IdJeux` FROM `jeux` WHERE `CodeJeu`=:CodeDuJeu");
    RequeteIdJeu.bindValue(":CodeDuJeu",(this->ModelJeuReserves->index(ui->TbV_JeuxReserves->currentIndex().row(),0).data().toString()));

    RequeteIdJeu.exec();
    RequeteIdJeu.next();

    //Connaitre le statut du jeu
    QSqlQuery RequeteStatut;
    RequeteStatut.prepare("SELECT `StatutJeux_IdStatutJeux` FROM `jeux` WHERE `IdJeux`=:IdDuJeu");
    RequeteStatut.bindValue(":IdDuJeu",RequeteIdJeu.value(0));

    RequeteStatut.exec();
    RequeteStatut.next();

    //si le statut du jeux est ? réservé, alors,
    if (RequeteStatut.value(0)==4)
    {
        //on met son statut ? disponible
        QSqlQuery RequeteMAJStatut;
        RequeteMAJStatut.prepare("UPDATE `jeux` SET `StatutJeux_IdStatutJeux`=1 WHERE `IdJeux`=:IdDuJeu");
        RequeteMAJStatut.bindValue("IdDuJeu",RequeteIdJeu.value(0));
        if(!RequeteMAJStatut.exec())
        {
            qDebug()<< "on_bt_JeuxReserves_clicked  || MAJ statut jeu s'il est réservé    "<<RequeteMAJStatut.lastError();
        }

    }

    //suppression de la réservation
    QSqlQuery RequeteSupp;
    RequeteSupp.prepare("DELETE FROM `reservation` WHERE `Jeux_IdJeux`=:IdDuJeu AND `JeuEmprunte`=1");
    RequeteSupp.bindValue(":IdDuJeu",RequeteIdJeu.value(0));

    RequeteSupp.exec();
    RequeteSupp.next();

    AfficherJeuxReserve();

//Grise le bouton de suppression du tableau des réservations
    ui->bt_JeuxReserves->setEnabled(false);


}

//#######################################################################################################
//////////////////////////////////////////////Emprunt////////////////////////////////////////////////////
//#######################################################################################################


/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////Ajouter le jeu aux nouveaux emprunts/////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/** Description détaillée de la méthode
 *  @pre    Connexion avec la base de donnée, un jeu et un membre est actif dans la classe,
 *  @post
 *  @param
 *  @retval
 *  @return
 *  @test
 *  @see    JeuActif, MembreActif, ModelNewEmprunts, NouveauEmprunts
 */
void F_Emprunt::on_Bt_Ajouter_clicked()
{

    //Si la cotisation n'est pas ? jour, alors
    if(EtatCotisationMembre==false)
    {
        //Afficher un message indiquant que la cotisation n'est plus ? jour et empécher l'ajout du jeu aux nouveau emprunts
        //QMessageBox::question(this,"Cotisation expirée","Attention ! \nLa cotisation n'est plus ? jour, il faut la renouvler pour pouvoir emprunter ? nouveau.","Ok");
        //QMessageBox::warning(this,"Cotisation expirée","Attention ! \nLa cotisation n'est plus ? jour, il faut la renouvler pour pouvoir emprunter ? nouveau.","Ok");
        if(QMessageBox::critical(this,"Cotisation expirée","Attention ! \nLa cotisation n'est plus ? jour, voulez-vous quand même emprunter le jeu?","Emprunter","Annuler"))
        {
            return;

        }
    }

    //Savoir combien de jeux sont en cours d'emprunt :
    QSqlQuery RequeteNbJeuEmprunte;
    int NbrJeuxEmprunte_Reserve =0;
    RequeteNbJeuEmprunte.prepare("SELECT `Jeux_IdJeux`"
                               "FROM `emprunts`,`membres`"
                               "WHERE  `DateRetour` IS NULL AND`Membres_IdMembre`=`IdMembre`AND `CodeMembre`=:CodeDuMembre" );
    RequeteNbJeuEmprunte.bindValue(":CodeDuMembre",this->MembreActif);
    if(!RequeteNbJeuEmprunte.exec())
    {
        qDebug()<<"F_Emprunt::on_Bt_Ajouter_clicked() ||| Requette jeux en emprunt "<<RequeteNbJeuEmprunte.lastError();
    }

    //Tant qu'il y a des jeux,
    while(RequeteNbJeuEmprunte.next())
    {
        NbrJeuxEmprunte_Reserve++;
    }

    //Savoir combien de jeux sont réservés :
    QSqlQuery RequeteNbJeuReserve;
    RequeteNbJeuReserve.prepare("SELECT `idReservation`"
                                "FROM `reservation`,`membres`"
                                "WHERE  `JeuEmprunte` =1 AND`Membres_IdMembre`=`IdMembre`AND `CodeMembre`=:CodeDuMembre" );
    RequeteNbJeuReserve.bindValue(":CodeDuMembre",this->MembreActif);
    if(!RequeteNbJeuReserve.exec())
    {
        qDebug()<<"F_Emprunt::on_Bt_Ajouter_clicked() ||| Requette jeux en emprunt "<<RequeteNbJeuReserve.lastError();
    }

    //Tant qu'il y a des jeux,
    while(RequeteNbJeuReserve.next())
    {
        NbrJeuxEmprunte_Reserve++;
    }

    //on ajoute le nombre de nouveaux emprunts
    NbrJeuxEmprunte_Reserve=NbrJeuxEmprunte_Reserve+NouveauEmprunts.size();


    //Met le bonton "Valider les emprunts" en cliquable
    ui->Bt_ValiderEmprunt->setEnabled(true);

    QSqlQuery RequeteIdJeu;
    unsigned int IdDuJeu (0);

//Prépare le reqête pour récupérer l'id du jeu
    RequeteIdJeu.prepare("SELECT IdJeux FROM jeux WHERE CodeJeu=:CodeDuJeu");
    RequeteIdJeu.bindValue(":CodeDuJeu",this->JeuActif);

//Execute la requête
    if (!RequeteIdJeu.exec())
    {
        qDebug()<<"F_Emprunt::on_Bt_Ajouter_clicked ||| requette IdJeu  "<<(RequeteIdJeu.lastError()) ;
    }

    RequeteIdJeu.next();


    //Récup?re l'es remarques'id dans la base de données
    IdDuJeu=RequeteIdJeu.value(0).toInt();


    QSqlQuery RequeteIdMembre;
    unsigned int IdDuMembre (0);

//Prépare le reqête pour récupérer l'id du membre
    RequeteIdMembre.prepare("SELECT IdMembre FROM membres WHERE CodeMembre=:CodeDuMembre");
    RequeteIdMembre.bindValue(":CodeDuMembre",this->MembreActif);

//Execute la requête
    if (!RequeteIdMembre.exec())
    {
        qDebug()<<"F_Emprunt::on_Bt_Ajouter_clicked ||| requette IdMembre  "<<RequeteIdMembre.lastError() ;
    }

    RequeteIdMembre.next();


    //Récup?re l'id dans la base de données
    IdDuMembre=RequeteIdMembre.value(0).toInt();

//Si le jeux est disponible
    if (ui->Le_StatutJeuARemplir->text()=="Disponible")
    {
        //Si le nombre de jeux plus le nombre de réservation dépasse le nombre de jeux autoriser,
        if(ui->Le_NbreJeuxEmpr->text().toInt()<NbrJeuxEmprunte_Reserve+1)
        {
            QString Message ;
            Message ="Déj? "+Message.setNum(NbrJeuxEmprunte_Reserve )+" jeux empruntés ou réservés.\nVoulez-vous quand même emprunter le jeu ? ";
            if (QMessageBox::question(this,"Nombre maximum de jeux dépassé !",Message,"Oui","Non")==1)
            {
                return;
            }
        }

        Emprunts Emprunt;
        //Récupération de la date du jour
        QDate DateActuelle;
        DateActuelle=DateActuelle.currentDate();

        //Remplissage de l'emprunt dans le Vecteur Emprunt
        Emprunt.idJeu= IdDuJeu;
        Emprunt.idMembre= IdDuMembre;
        Emprunt.DateRetourPrevu= ui->DtE_Retour->date();
        Emprunt.DateEmprunt= DateActuelle;
        Emprunt.idTypeEmprunt= ui->CBx_TypeEmprunt->currentIndex()+1;
        Emprunt.PrixCaution= ui->Le_PrixCautionARemplir->text().toInt();
        Emprunt.PrixEmprunt= ui->Le_PrixEmpruntARemplir->text().toInt();
        this->NouveauEmprunts.push_back(Emprunt);

        ModelNewEmprunts->setItem(this->NmbLigneNewEmprunts,0, new QStandardItem(ui->LE_CodeJeu->text()));
        ModelNewEmprunts->setItem(this->NmbLigneNewEmprunts,1, new QStandardItem(ui->Le_NomJeuARemplir->text()));
        ModelNewEmprunts->setItem(this->NmbLigneNewEmprunts,2, new QStandardItem( this->NouveauEmprunts[this->NmbLigneNewEmprunts].DateRetourPrevu.toString("ddd d MMMM yyyy") ));
        ModelNewEmprunts->setItem(this->NmbLigneNewEmprunts,3, new QStandardItem(ui->Le_PrixEmpruntARemplir->text()));

        this->NmbLigneNewEmprunts++;
        ViderJeu();

        //Mettre le statut du jeux ? "En emprunt"
        QSqlQuery RequeteStatut;

        RequeteStatut.prepare("UPDATE `jeux`SET `StatutJeux_IdStatutJeux`=2 WHERE `IdJeux`=:IdDuJeu");
        RequeteStatut.bindValue(":IdDuJeu",IdDuJeu);

        if (!RequeteStatut.exec())
        {
            qDebug()<<"F_Emprunt::on_Bt_Ajouter_clicked ||| requette statut  "<<(RequeteStatut.lastError()) ;
        }

    }
    else
    {
        //si le jeu est réservé,
        if(ui->Le_StatutJeuARemplir->text()=="Réservé")
        {
            //Recherche de l'id du membre qui ? réservé le jeu
            QSqlQuery RequeteJeuReserve;
            RequeteJeuReserve.prepare("SELECT Membres_IdMembre FROM `reservation` WHERE  `JeuEmprunte`=1 AND `Jeux_IdJeux`=:IdDuJeu");
            RequeteJeuReserve.bindValue(":IdDuJeu",IdDuJeu);

            if (!RequeteJeuReserve.exec())
            {
                qDebug()<<"F_Emprunt::on_Bt_Ajouter_clicked ||| requette jeu   "<< RequeteJeuReserve.lastError() ;
            }

            RequeteJeuReserve.next();

            //si l'id du membre est le même qie celui du réserveur,
            if(RequeteJeuReserve.value(0)==IdDuMembre)
            {
                //Si le nombre de jeux plus le nombre de réservation dépasse le nombre de jeux autoriser,
                if(ui->Le_NbreJeuxEmpr->text().toInt()<NbrJeuxEmprunte_Reserve)
                {
                    QString Message ;
                    Message ="Déj? "+Message.setNum(NbrJeuxEmprunte_Reserve )+" jeux empruntés ou réservés.\nVoulez-vous quand même emprunter le jeu ? ";
                    if (QMessageBox::question(this,"Nombre maximum de jeux dépassé !",Message,"Oui","Non")==1)
                    {
                        return;
                    }
                }

                Emprunts Emprunt;
                //Récupération de la date du jour
                QDate DateActuelle;
                DateActuelle=DateActuelle.currentDate();

                //Remplissage de l'emprunt dans le Vecteur Emprunt
                Emprunt.idJeu= IdDuJeu;
                Emprunt.idMembre= IdDuMembre;
                Emprunt.DateRetourPrevu= ui->DtE_Retour->date();
                Emprunt.DateEmprunt= DateActuelle;
                Emprunt.idTypeEmprunt= ui->CBx_TypeEmprunt->currentIndex()+1;
                Emprunt.PrixCaution= ui->Le_PrixCautionARemplir->text().toInt();
                Emprunt.PrixEmprunt= ui->Le_PrixEmpruntARemplir->text().toInt();
                this->NouveauEmprunts.push_back(Emprunt);

                ModelNewEmprunts->setItem(this->NmbLigneNewEmprunts,0, new QStandardItem(ui->LE_CodeJeu->text()));
                ModelNewEmprunts->setItem(this->NmbLigneNewEmprunts,1, new QStandardItem(ui->Le_NomJeuARemplir->text()));
                ModelNewEmprunts->setItem(this->NmbLigneNewEmprunts,2, new QStandardItem( this->NouveauEmprunts[this->NmbLigneNewEmprunts].DateRetourPrevu.toString("ddd d MMMM yyyy") ));
                ModelNewEmprunts->setItem(this->NmbLigneNewEmprunts,3, new QStandardItem(ui->Le_PrixEmpruntARemplir->text()));

                this->NmbLigneNewEmprunts++;
                ViderJeu();


                //Mettre le statut du jeux ? "En emprunt"
                QSqlQuery RequeteStatut;

                RequeteStatut.prepare("UPDATE `jeux`SET `StatutJeux_IdStatutJeux`=2 WHERE `IdJeux`=:IdDuJeu");
                RequeteStatut.bindValue(":IdDuJeu",IdDuJeu);

                if (!RequeteStatut.exec())
                {
                    qDebug()<<"F_Emprunt::on_Bt_Ajouter_clicked ||| requette statut  "<<(RequeteStatut.lastError()) ;
                }

                //Mettre la réservation ? Jeuemprunté
                QSqlQuery RequeteJeuEmprunte;

                RequeteJeuEmprunte.prepare("UPDATE `reservation`SET `JeuEmprunte`=0 WHERE `Jeux_IdJeux`=:IdDuJeu AND `JeuEmprunte`=1");
                RequeteJeuEmprunte.bindValue(":IdDuJeu",IdDuJeu);


                if (!RequeteJeuEmprunte.exec())
                {
                    qDebug()<<"F_Emprunt::on_Bt_Ajouter_clicked ||| requette jeu emprunté  "<<(RequeteJeuEmprunte.lastError()) ;

                }
                AfficherJeuxReserve();
            }
            else
            {
                qDebug()<<"F_Emprunt::on_Bt_Ajouter_clicked ||| Jeux non disponible!!!!";
            }
        }
        else
        {
            qDebug()<<"F_Emprunt::on_Bt_Ajouter_clicked ||| Jeux non disponible!!!!";
        }
    }

    //Calcule du nombre de crédits ? demander
    int NbCredits (0);
    for(register int i=0;i<this->NmbLigneNewEmprunts;i++)
    {
        NbCredits=NbCredits+ this->ModelNewEmprunts->index(i,3).data().toInt();
    }

    //Si le le prix des nouveaux emprunts est plus ch?re que les crédits restants, alors
    if (NbCredits>ui->Le_CreditRestantARemplir->text().toInt())
    {
        //mettre en rouge le nombre de crédits restants
        ui->Le_CreditRestantARemplir->setStyleSheet("QLineEdit {color:red;}");
        ui->Lb_CreditRestant->setStyleSheet("QLabel {color:red;}");
    }
    //sinon,
    else
    {
        //Le mettre en noir
        ui->Le_CreditRestantARemplir->setStyleSheet("QLineEdit {color:black;}");
        ui->Lb_CreditRestant->setStyleSheet("QLabel {color:black;}");
    }
    ui->LE_CodeJeu->setFocus();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Valider les Nouveau emprunts///////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/** Description détaillée de la méthode
 *  @pre    Connexion avec la base de donnée, un membre est actif dans la classe, un(des) a(ont) été fait(ent)
 *  @post
 *  @param
 *  @retval
 *  @return
 *  @test
 *  @see    NouveauEmprunts, ModelNewEmprunts, NmbLigneNewEmprunts
 */
void F_Emprunt::on_Bt_ValiderEmprunt_clicked()
{
    //Calcule du nombre de crédits ? demander
    int NbCredits (0);
    for(register int i=0;i<this->NmbLigneNewEmprunts;i++)
    {
        NbCredits=NbCredits+ this->ModelNewEmprunts->index(i,3).data().toInt();
    }

    pPaiement->setWindowModality(Qt::ApplicationModal);
    pPaiement->AfficherPaiement(NbCredits,this->MembreActif);
    int nResultat (0);

    nResultat = pPaiement->exec();

    if (nResultat==1)
    {
        EmprunterJeux();
    }

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////Emprunte les nouveau Jeux:://////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/** Description détaillée de la méthode
 *  @pre    Connexion avec la base de donnée, un membre est actif dans la classe, un(des) a(ont) été fait(ent)
 *  @post
 *  @param
 *  @retval
 *  @return
 *  @test
 *  @see    NouveauEmprunts, ModelNewEmprunts, NmbLigneNewEmprunts
 */
void F_Emprunt::EmprunterJeux()
{
    //Met le bonton "Valider les emprunts" en non-cliquable
    ui->Bt_ValiderEmprunt->setEnabled(false);
    //Met le bonton  de suppression des nouveaux emprunt en non-cliquable
    ui->bt_NewEmprunt->setEnabled(false);


    for( register int i=0;i<NouveauEmprunts.size();i++ )
    {
        //Création de l'emprunt
        QSqlQuery RequeteEmprunt;

        RequeteEmprunt.prepare("INSERT INTO emprunts"
                               "(Jeux_IdJeux,Membres_IdMembre,TypeEmprunt_IdTypeEmprunt,"
                                "ModePaiement_IdModePaiement,DateEmprunt,DateRetourPrevu,NbrPrologation,Cautionnement,MontantCaution)"
                               "VALUES (:Jeux_IdJeux,:Membres_IdMembre,:TypeEmprunt_IdTypeEmprunt,:ModePaiement_IdModePaiement,"
                                ":DateEmprunt,:DateRetourPrevu,:NbrPrologation,:Cautionnement,:MontantCaution)");

        RequeteEmprunt.bindValue(":Jeux_IdJeux",NouveauEmprunts[i].idJeu);
        RequeteEmprunt.bindValue(":Membres_IdMembre",NouveauEmprunts[i].idMembre);
        RequeteEmprunt.bindValue(":TypeEmprunt_IdTypeEmprunt",NouveauEmprunts[i].idTypeEmprunt);
        RequeteEmprunt.bindValue(":ModePaiement_IdModePaiement",1);
        RequeteEmprunt.bindValue(":DateEmprunt",NouveauEmprunts[i].DateEmprunt);
        RequeteEmprunt.bindValue(":DateRetourPrevu",NouveauEmprunts[i].DateRetourPrevu);
        RequeteEmprunt.bindValue(":NbrPrologation",0);
        if ((NouveauEmprunts[i].PrixCaution )==0)
        {
            RequeteEmprunt.bindValue(":Cautionnement",0);
        }
        else
        {
            RequeteEmprunt.bindValue(":Cautionnement",1);

        }
        RequeteEmprunt.bindValue(":MontantCaution",NouveauEmprunts[i].PrixCaution);
        if (!RequeteEmprunt.exec())
        {
            qDebug()<<"F_Emprunt::Emprunter "<<RequeteEmprunt.lastError().text();
        }

        //Mettre le statut du jeux ? "Emprunté"
        QSqlQuery RequeteStatut;

        RequeteStatut.prepare("UPDATE `jeux`SET `StatutJeux_IdStatutJeux`=3 WHERE `IdJeux`=:IdDuJeu");
        RequeteStatut.bindValue(":IdDuJeu",NouveauEmprunts[i].idJeu);
        RequeteStatut.exec();

        //savoir si le jeu est ? réservé
        QSqlQuery RequeteResa;
        RequeteResa.prepare("SELECT `idReservation` FROM `reservation` WHERE `Jeux_IdJeux`=:IdDuJeu");
        RequeteResa.bindValue(":IdDuJeu",NouveauEmprunts[i].idJeu);
        RequeteResa.exec();
        RequeteResa.next();

        if (RequeteResa.size()!=0)
        {
            QSqlQuery RequeteMajResa;
            RequeteMajResa.prepare("UPDATE `reservation` SET `JeuEmprunte`=0 WHERE `idReservation`=:IdResa");
            RequeteMajResa.bindValue(":IdResa",RequeteResa.value(0));
            RequeteMajResa.exec();
        }



    }

    //Vider le tableau des nouveaux emprunts
    NouveauEmprunts.clear();
    ModelNewEmprunts->clear();
    //Initialise les colones du TableView des nouveaux emprunts
    this->ModelNewEmprunts->setColumnCount(4);
    this->ModelNewEmprunts->setHorizontalHeaderItem(0, new QStandardItem("Code"));
    this->ModelNewEmprunts->setHorizontalHeaderItem(1, new QStandardItem("Nom du jeu"));
    this->ModelNewEmprunts->setHorizontalHeaderItem(2, new QStandardItem("Date Retour"));
    this->ModelNewEmprunts->setHorizontalHeaderItem(3, new QStandardItem("Prix Emprunt"));
    ui->TbV_NewEmprunt->setColumnWidth(0,40);
    ui->TbV_NewEmprunt->setColumnWidth(1,100);

    this->NmbLigneNewEmprunts=0;


    AfficherJeuxEnEmprunt();
    AfficherJeuxReserve();

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////Suppression d'un nouvel emprunt//////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/** Description détaillée de la méthode
 *  @pre    Connexion avec la base de donnée,un nouvel emprunt est présent
 *  @post
 *  @param
 *  @retval
 *  @return
 *  @test
 *  @see    ModelNewEmprunts,NouveauEmprunts
 */
void F_Emprunt::on_bt_NewEmprunt_clicked()
{
    //Mettre le statut du jeu ? "Disponible"
    QSqlQuery RequeteStatut;
    RequeteStatut.prepare("UPDATE `jeux`SET `StatutJeux_IdStatutJeux`=1 WHERE `CodeJeu`=:CodeDuJeu");
    RequeteStatut.bindValue(":CodeDuJeu",(this->ModelNewEmprunts->index(ui->TbV_NewEmprunt->currentIndex().row(),0).data().toString()));
    RequeteStatut.exec();

    //Supprimer la ligne du vecteur
    this->NouveauEmprunts.remove(ui->TbV_NewEmprunt->currentIndex().row());
    this->NmbLigneNewEmprunts--;

    //Supprime la ligne du tableau
    this->ModelNewEmprunts->removeRow(ui->TbV_NewEmprunt->currentIndex().row());

//Grise le bouton de suppression des nouveau emprunts
    ui->bt_NewEmprunt->setEnabled(false);
}

