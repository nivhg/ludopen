#include <QtSql>
#include "f_reservation.h"
#include "ui_f_reservation.h"



/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Constructeur///////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
F_Reservation::F_Reservation(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::F_Reservation)
{
    ui->setupUi(this);
    JeuActif="";
    MembreActif="";
    setWindowIcon(QIcon("Reservation.png"));

//Initialisation du combo box des types d'emprunts
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


//Création d'un modèle pour le TableView des membres
    this->ModelMembre = new QStandardItemModel() ;
//Associe le modèl au TableView
    ui->TbV_Recherche->setModel(this->ModelMembre);
//Met le TableView en lecture seule
    ui->TbV_Recherche->setEditTriggers(0);
//Initialise le tableau avec tous les membres
    on_LE_RechercheMembre_textChanged("");



//Création d'un modèle pour le TableView des jeux empruntés
    this->ModelJeuEmpruntes = new QStandardItemModel() ;
//Associe le modèl au TableView
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


//Création d'un modèle pour le TableView des jeux réservés
   this->ModelJeuReserves = new QStandardItemModel() ;
//Associe le modèl au TableView
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


//Grise les boutons de modification des remarques du membre
    ui->Bt_ValiderRemarque->setEnabled(false);
    ui->Bt_AnnulerRemarque->setEnabled(false);
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////Déstructeur//////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
F_Reservation::~F_Reservation()
{
    delete ui;
    delete this->ModelJeuReserves;
    delete this->ModelJeuEmpruntes;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////Mise à jour de l'atribut CodeJeu//////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void F_Reservation::set_JeuActif (QString CodeJeu)
{
    this->JeuActif=CodeJeu;
}

//#######################################################################################################
//////////////////////////////////////////////MEMBRES////////////////////////////////////////////////////
//#######################################################################################################

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Rechercher un membre avec son nom//////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void F_Reservation::on_LE_RechercheMembre_textChanged(const QString &arg1)
{

    unsigned int NumeroLigne (0);
    QString Nom=arg1;

    if(Nom.size()>=2)
    {


        Nom="%"+Nom+"%";

        QSqlQuery RequeteMembre;
        NumeroLigne=0;


        RequeteMembre.prepare("SELECT  `CodeMembre`,`Nom`,Prenom,`DateNaissance` FROM`membres` WHERE `Nom` LIKE (:Nom) GROUP BY `CodeMembre`");
        RequeteMembre.bindValue(":Nom",Nom);
        RequeteMembre.exec();

        //On vide le modèle
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
                //on ajoute une nouvelle ligne du table view
                this->ModelMembre->setItem(NumeroLigne, 0, new QStandardItem(RequeteMembre.value(0).toString() ));
                this->ModelMembre->setItem(NumeroLigne, 1, new QStandardItem(RequeteMembre.value(1).toString() ));
                this->ModelMembre->setItem(NumeroLigne, 2, new QStandardItem(RequeteMembre.value(2).toString() ));
                this->ModelMembre->setItem(NumeroLigne, 3, new QStandardItem(RequeteMembre.value(3).toString() ));
                NumeroLigne++;

             }

    }
    else
    {


                QSqlQuery RequeteMembre;
                NumeroLigne =0;
                RequeteMembre.exec("SELECT  `CodeMembre`,`Nom`,Prenom,`DateNaissance` FROM`membres` GROUP BY `CodeMembre`");

                //On vide le modèle
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
                        //on ajoute une nouvelle ligne du table view
                        this->ModelMembre->setItem(NumeroLigne, 0, new QStandardItem(RequeteMembre.value(0).toString() ));
                        this->ModelMembre->setItem(NumeroLigne, 1, new QStandardItem(RequeteMembre.value(1).toString() ));
                        this->ModelMembre->setItem(NumeroLigne, 2, new QStandardItem(RequeteMembre.value(2).toString() ));
                        this->ModelMembre->setItem(NumeroLigne, 3, new QStandardItem(RequeteMembre.value(3).toString() ));
                        NumeroLigne++;

                     }
    }

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////Clic sur un membre dans le tableau///////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void F_Reservation::on_TbV_Recherche_clicked(const QModelIndex &index)
{
    ui->LE_CodeMembre->setText(this->ModelMembre->index(index.row(),0).data().toString());
    on_Bt_RechercheOK_clicked();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Pression sur entrée pour le code membre////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void F_Reservation::on_LE_CodeMembre_returnPressed()
{
    on_Bt_RechercheOK_clicked();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////Clic sur le bonton OK du code membre//////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void F_Reservation::on_Bt_RechercheOK_clicked()
{

    QString CodeMembre =ui->LE_CodeMembre->text();

// Actualise l'atribut MembreActif avec le code du membre que l'on affiche
    MembreActif=CodeMembre;

//Affiche les informations du membre
    this->AfficherMembre(CodeMembre);


}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Afficher le membre dans le volet central///////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void F_Reservation::AfficherMembre(QString CodeMembre)
{
    QSqlQuery Requete;

//Prépare la requête
    Requete.prepare("SELECT Nom,Prenom,NbreRetard,Ecarte,Remarque,NbreJeuxAutorises FROM membres WHERE CodeMembre=:CodeDuMembre");
    Requete.bindValue(":CodeDuMembre",CodeMembre);

//Execute la requête
    if (!Requete.exec())
    {

    }

    Requete.next();


//Récupère le Nom dans la base de données puis l'affiche
    QString TextTemporaire = (Requete.value(0).toString());
    ui->LE_Nom->setText(TextTemporaire);

//Récupère le Prénom dans la base de données puis l'affiche
    TextTemporaire = (Requete.value(1).toString());
    ui->LE_Prenom->setText(TextTemporaire);

//Récupère le nombre de retards dans la base de données  puis l'affiche
    TextTemporaire = (Requete.value(2).toString());
    ui->LE_NbrRetards->setText(TextTemporaire);

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

//Récupère les remarques dans la base de données puis les afficher
    TextTemporaire = (Requete.value(4).toString());
    ui->TxE_Remarque->setText(TextTemporaire);

//Grise les boutons de modification des remarques du membre
    ui->Bt_ValiderRemarque->setEnabled(false);
    ui->Bt_AnnulerRemarque->setEnabled(false);

//Récupère le nombre de jeux empruntables dans la base de données puis l'afficher
    TextTemporaire = (Requete.value(5).toString());
    ui->LE_NbrJeuxEmpruntable->setText(TextTemporaire);

//Affiche l'état de la cotisation
   //Savoir si le membre à un memmbre assosier
   QSqlQuery RequeteMembreAssocier ;
   RequeteMembreAssocier.prepare("SELECT `MembreAssocie`FROM`membres` WHERE `CodeMembre`=:CodeDuMembre AND `MembreAssocie` !=0");
   RequeteMembreAssocier.bindValue(":codeDuMembre",this->MembreActif);
   RequeteMembreAssocier.exec();
   RequeteMembreAssocier.next();
   //s'i y en a un,
   if(RequeteMembreAssocier.size()>0)
   {
       //On Affiche l'état de la cotisation du membre associé au membre actif
       AfficherEtatCotisation(RequeteMembreAssocier.value(0).toString());
   }
   //Sinon,
   else
   {
       //On affiche l'état de la cotisation du membre en cours de consultation
       AfficherEtatCotisation(this->MembreActif);
   }

//Affiche les jeux empruntés
    AfficherJeuxEnEmprunt();

//Affiche les jeux réservés
    AfficherJeuxReserve();

//Affiche le nombre de crédits restants
    CalculerCreditsRestants();

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
 */
void F_Reservation::AfficherEtatCotisation(QString CodeMembre)
{
    //Connaitre la date d'expiration de la cotisation anuelle la plus récente
    QSqlQuery RequeteCotisation ;
    RequeteCotisation.prepare("SELECT DateExpiration "
                             "FROM abonnements, membres "
                             "WHERE Prestations_IdPrestation IS NOT NULL  AND Membres_IdMembre=IdMembre AND CodeMembre=:CodeDuMembre ORDER BY DateExpiration DESC " );
    RequeteCotisation.bindValue(":CodeDuMembre",CodeMembre );


    if(!RequeteCotisation.exec())
    {
        qDebug()<<"Retour affiche Cotisation  requete état cotisation  "<<RequeteCotisation.lastError();
    }

    RequeteCotisation.next();

    QDate DateActuelle;
    DateActuelle= DateActuelle.currentDate();

    int NombreJoursRestant ;
    //Nombre de jours qu'il reste à la cotisation
    NombreJoursRestant=DateActuelle.daysTo(RequeteCotisation.value(0).toDate());

    //si le nombres de jours restant est négatif la cotisation est expirée
    if(NombreJoursRestant<0)
    {
        QString Cotisation;
        //Si le code membre est différent du membre actif, alors
        if(MembreActif!=CodeMembre)
        {
            //On précise le code du membre qui pocède la cotisation
            Cotisation="Cotisation expirée (membre "+ CodeMembre +") depuis le ";
        }
        //Sinon,
        else
        {
            //On affiche juste l'état de la cotisation
            Cotisation="Cotisation expiré";
        }
        ui->Lb_EtatCotisation->setText(Cotisation);
        ui->Lb_EtatCotisation->setStyleSheet("QLabel {color:red;}");
        ui->Lb_CotisationDate->setText(RequeteCotisation.value(0).toDate().toString("ddd d MMMM yyyy"));
        ui->Lb_CotisationDate->setStyleSheet(" QLabel{color:red;}" );
    }
    else
    {
        QString Cotisation;
        //Si le code membre est différent du membre actif, alors
        if(MembreActif!=CodeMembre)
        {
            //On précise le code du membre qui pocède la cotisation
            Cotisation="Cotisation à jours (membre "+ CodeMembre +")";
        }
        //Sinon,
        else
        {
            //On affiche juste l'état de la cotisation
            Cotisation="Cotisation à jours";

        }
        QString JourRestant ;
        JourRestant.setNum(NombreJoursRestant);
        ui->Lb_EtatCotisation->setText(Cotisation);
        ui->Lb_CotisationDate->setText("("+ JourRestant +" jours restant)");
        //Si le nombre de jonrs restant est inferieur à 30 jours, on l'affiche en orange
        if (NombreJoursRestant<31)
        {
            ui->Lb_EtatCotisation->setStyleSheet("QLabel {color:orange;}");
            ui->Lb_CotisationDate->setStyleSheet(" QLabel{color:orange;}" );
        }
        //sinon, on l'affiche ne vert
        else
        {
            ui->Lb_EtatCotisation->setStyleSheet("QLabel {color:green;}");
            ui->Lb_CotisationDate->setStyleSheet(" QLabel{color:green;}" );
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////Afficher le nombre de crédits restants d'un membre/////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/** Description détaillée de la méthode
 *  @pre    Connexion avec la base de donnée
 *  @param  Code du membre
 */
void F_Reservation::CalculerCreditsRestants()
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

    ui->LE_NbrCreditRestant->setText(Credit.setNum(CreditRestant));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////Changement de la remaque du Membre//////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void F_Reservation::on_TxE_Remarque_textChanged()
{
//Affiche les boutons de modification de la remarque du jeu
    ui->Bt_ValiderRemarque->setEnabled(true);
    ui->Bt_AnnulerRemarque->setEnabled(true);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Enregistrer la remarque du membre//////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void F_Reservation::on_Bt_ValiderRemarque_clicked()
{
    QSqlQuery RequeteMiseAJourRemarque;

//prépare le requête de mise à jour
    RequeteMiseAJourRemarque.prepare("UPDATE `membres` SET `Remarque` =:NouvelRemarque WHERE `CodeMembre`=:CodeDuMembre");

//Entre les valeurs de la reqête
    RequeteMiseAJourRemarque.bindValue(":CodeDuMembre",MembreActif);
    RequeteMiseAJourRemarque.bindValue(":NouvelRemarque",ui->TxE_Remarque->toPlainText());

//Execut la requête
    if (!RequeteMiseAJourRemarque.exec())
    {

    }

//Grise les boutons de modification de le remarque
    ui->Bt_ValiderRemarque->setEnabled(false);
    ui->Bt_AnnulerRemarque->setEnabled(false);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Annuler changement de la remaque du membre/////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void F_Reservation::on_Bt_AnnulerRemarque_clicked()
{
    QSqlQuery RequeteAnnulerRemarque;

//Prépare la requête
    RequeteAnnulerRemarque.prepare("SELECT Remarque FROM membres WHERE CodeMembre=:CodeDuMembre");
    RequeteAnnulerRemarque.bindValue(":CodeDuMembre",MembreActif);


//Execut la requête
    if (!RequeteAnnulerRemarque.exec())
    {

    }

    RequeteAnnulerRemarque.next();

//Récupère les remarques dans la base de données et les affiches
    QString TextTemporaire = (RequeteAnnulerRemarque.value(0).toString());
    ui->TxE_Remarque->setText(TextTemporaire);

//Grise les boutons de modification des remarques du membre
    ui->Bt_ValiderRemarque->setEnabled(false);
    ui->Bt_AnnulerRemarque->setEnabled(false);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Recherche des jeux en emprunt//////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void F_Reservation::AfficherJeuxEnEmprunt()
{
    QDate DateActuelle;
    DateActuelle=DateActuelle.currentDate();
    QSqlQuery RequeteIdMembre;
    unsigned int IdDuMembre (0);

//Prépare le reqête pour récupérer l'id du membre
    RequeteIdMembre.prepare("SELECT IdMembre FROM membres WHERE CodeMembre=:CodeDuMembre");
    RequeteIdMembre.bindValue(":CodeDuMembre",MembreActif);



//Execute la requête
    if (!RequeteIdMembre.exec())
    {
        qDebug()<< "F_Réservation::JeuxEnEmprunt ||| RequeteIdMembre  "<< RequeteIdMembre.lastError();
    }

    RequeteIdMembre.next();

    //Récupère l'id dans la base de données
    IdDuMembre=RequeteIdMembre.value(0).toInt();
    QSqlQuery RequeteJeuEmprunte;
    unsigned int NumeroLigne =0;
    RequeteJeuEmprunte.prepare("SELECT `DateEmprunt`,`DateRetourPrevu`,`NomJeu`,`CodeJeu` "
                               "FROM `emprunts`,`jeux`"
                               "WHERE  `DateRetour` IS NULL AND`Membres_IdMembre`=:IdDuMembre AND `IdJeux`=`Jeux_IdJeux`" );
    RequeteJeuEmprunte.bindValue(":IdDuMembre",IdDuMembre);
    if(!RequeteJeuEmprunte.exec())
    {
        qDebug()<<"F_Réservation::JeuxEnEmprunt() ||| Requette jeux en emprunt "<<RequeteJeuEmprunte.lastError();
    }

    //On vide le modèle
    this->ModelJeuEmpruntes->clear();
    //Initialise les colones du TableView des jeux empruntés
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
        this->ModelJeuEmpruntes->setItem(NumeroLigne, 0, new QStandardItem(RequeteJeuEmprunte.value(3).toString() ));
        this->ModelJeuEmpruntes->setItem(NumeroLigne, 1, new QStandardItem(RequeteJeuEmprunte.value(2).toString()));
        this->ModelJeuEmpruntes->setItem(NumeroLigne, 2, new QStandardItem(RequeteJeuEmprunte.value(0).toDate().toString("ddd d MMMM yyyy") ));
        this->ModelJeuEmpruntes->setItem(NumeroLigne, 3, new QStandardItem(RequeteJeuEmprunte.value(1).toDate().toString("ddd d MMMM yyyy") ));
        if (RequeteJeuEmprunte.value(1).toDate()>DateActuelle)
        {
            this->ModelJeuEmpruntes->setData(ModelJeuEmpruntes->index(NumeroLigne,3),QColor(Qt::green), Qt::BackgroundColorRole);
        }
        else
        {
            this->ModelJeuEmpruntes->setData(ModelJeuEmpruntes->index(NumeroLigne,3),QColor(Qt::red), Qt::BackgroundColorRole);
        }
        NumeroLigne++;

    }

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Recherche des jeux réservés//////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void F_Reservation::AfficherJeuxReserve()
{

    QSqlQuery RequeteIdMembre;
    unsigned int IdDuMembre (0);

//Prépare le reqête pour récupérer l'id du membre
    RequeteIdMembre.prepare("SELECT IdMembre FROM membres WHERE CodeMembre=:CodeDuMembre");
    RequeteIdMembre.bindValue(":CodeDuMembre",MembreActif);

//Execute la requête
    if (!RequeteIdMembre.exec())
    {
        qDebug()<<"F_Retour::JeuxReserve ||| Requete IdDuMembre "<<RequeteIdMembre.lastError();
    }

    RequeteIdMembre.next();

    //Récupère l'id dans la base de données
    IdDuMembre=RequeteIdMembre.value(0).toInt();

    QSqlQuery RequeteJeuReserve;
    unsigned int NumeroLigne =0;
    RequeteJeuReserve.prepare("SELECT `DateReservation`,`DatePrevuEmprunt`,`DatePrevuRetour`,`NomLieux`,`NomJeu`,`CodeJeu`"
                              "FROM `reservation`,`lieux` ,`jeux`"
                              "WHERE  `JeuEmprunte`=1 AND`Membres_IdMembre`=:IdMembre AND`IdLieux`=`Lieux_IdLieux` AND `IdJeux`=`Jeux_IdJeux`" );

    RequeteJeuReserve.bindValue(":IdMembre",IdDuMembre);

    if (!RequeteJeuReserve.exec())
    {
        qDebug()<<"F_Réservation::JeuxReserve ||| Requete Jeux Reservés "<<RequeteJeuReserve.lastError();
    }


    //On vide le modèle
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
            this->ModelJeuReserves->setItem(NumeroLigne, 0, new QStandardItem(RequeteJeuReserve.value(5).toString() ));
            this->ModelJeuReserves->setItem(NumeroLigne, 1, new QStandardItem(RequeteJeuReserve.value(4).toString()));
            this->ModelJeuReserves->setItem(NumeroLigne, 2, new QStandardItem(RequeteJeuReserve.value(0).toDate().toString("ddd d MMMM yyyy") ));
            this->ModelJeuReserves->setItem(NumeroLigne, 3, new QStandardItem(RequeteJeuReserve.value(1).toDate().toString("ddd d MMMM yyyy") ));
            this->ModelJeuReserves->setItem(NumeroLigne, 4, new QStandardItem(RequeteJeuReserve.value(2).toDate().toString("ddd d MMMM yyyy") ));
            this->ModelJeuReserves->setItem(NumeroLigne, 5, new QStandardItem(RequeteJeuReserve.value(3).toString() ));

            //Savoir si le jeu est disponible ou non
            QSqlQuery Requetejeu;
            Requetejeu.prepare("SELECT `IdEmprunts` FROM`emprunts`,`jeux`WHERE `CodeJeu`=:CodeDuJeu AND`Jeux_IdJeux`=`IdJeux` AND `DateRetour`IS NULL");
            Requetejeu.bindValue(":CodeDuJeu",RequeteJeuReserve.value(5).toString());
            Requetejeu.exec();
            if(Requetejeu.size()==0)
            {
                this->ModelJeuReserves->setData(ModelJeuReserves->index(NumeroLigne,3),QColor(Qt::green), Qt::BackgroundColorRole);
            }
            else
            {
                this->ModelJeuReserves->setData(ModelJeuReserves->index(NumeroLigne,3),QColor(Qt::red), Qt::BackgroundColorRole);
            }

            NumeroLigne++;

        }

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////Clic sur un jeu emprunté dans le tableau/////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void F_Reservation::on_TbV_JeuxMembres_clicked(const QModelIndex &index)
{
    qDebug()<<"F_Reservation::on_TbV_JeuxMembres_clicked";
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////Clic sur un jeu réservé dans le tableau//////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void F_Reservation::on_TbV_JeuxReserves_clicked(const QModelIndex &index)
{
    ui->bt_SuppReservation->setEnabled(true);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////Suppression d'un jeu réservé/////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void F_Reservation::on_bt_SuppReservation_clicked()
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

    //si le statut du jeux est à réservé, alors,
    if (RequeteStatut.value(0)==4)
    {
        //on met son statut à disponible
        QSqlQuery RequeteMAJStatut;
        RequeteMAJStatut.prepare("UPDATE `jeux` SET `StatutJeux_IdStatutJeux`=1 WHERE `IdJeux`=:IdDuJeu");
        RequeteMAJStatut.bindValue("IdDuJeu",RequeteIdJeu.value(0));
        if(!RequeteMAJStatut.exec())
        {
            qDebug()<< "F_Retour::on_TbV_JeuxReserve_clicked  || MAJ statut jeu s'il est réservé    "<<RequeteMAJStatut.lastError();
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
    ui->bt_SuppReservation->setEnabled(false);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Vider informations du Membre///////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void F_Reservation::ViderMembre()
{
    ui->LE_RechercheMembre->setText("");
    ui->LE_CodeMembre->setText("");
    ui->LE_Nom->setText("");
    ui->LE_Prenom->setText("");
    ui->LE_NbrRetards->setText("");
    ui->LE_NbrCreditRestant->setText("");
    ui->LE_NbrJeuxEmpruntable->setText("");
    ui->TxE_Remarque->setText("");

    //On vide le modèle des jeux déjà empruntés
    this->ModelJeuEmpruntes->clear();
    //Initialise les colones du TableView des jeux empruntés
         this->ModelJeuEmpruntes->setColumnCount(4);
         this->ModelJeuEmpruntes->setHorizontalHeaderItem(0, new QStandardItem("Code"));
         this->ModelJeuEmpruntes->setHorizontalHeaderItem(1, new QStandardItem("Nom du jeu"));
         this->ModelJeuEmpruntes->setHorizontalHeaderItem(2, new QStandardItem("Date emprunt"));
         this->ModelJeuEmpruntes->setHorizontalHeaderItem(3, new QStandardItem("Date retour"));
         ui->TbV_JeuxMembres->setColumnWidth(0,40);
         ui->TbV_JeuxMembres->setColumnWidth(1,125);
         ui->TbV_JeuxMembres->verticalHeader()->setVisible(false);

    //On vide le modèle des jeux réservés
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

}

//#######################################################################################################
//////////////////////////////////////////////JEUX///////////////////////////////////////////////////////
//#######################################################################################################



/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////Afficher les informations du jeux////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void F_Reservation::AfficherJeu()
{
//met le type d'emprunt au type par défaut
    ui->CBx_TypeEmprunt->setCurrentIndex(0);

    QSqlQuery Requete;
//Prépare la requête et entre ses valeurs
    Requete.prepare("SELECT NomJeu, StatutJeux_IdStatutJeux,`IdJeux` FROM jeux WHERE CodeJeu=:CodeDuJeu");

    Requete.bindValue(":CodeDuJeux",JeuActif);

//Execute la requête
    if (!Requete.exec())
    {

    }

    Requete.next();


//Récupère le nom du jeu et l'affiche
    ui->LE_NomJeu->setText(Requete.value(0).toString() ) ;
    setWindowTitle("Réserver "+Requete.value(0).toString());


//Récupère l'id du statut
    unsigned int IdStatut =(Requete.value(1).toInt());

    //Si le statut est "emprunté",
    if (IdStatut==3)
    {

        //alors mettre dans le line edit "disponible le" la Date du retour du jeu
        ui->Lb_Disponible->setVisible(true);
        ui->LE_DateDisponible->setVisible(true);
        QSqlQuery RequeteDateRetour;
        RequeteDateRetour.prepare("SELECT `DateRetourPrevu` FROM `emprunts` WHERE `Jeux_IdJeux`=:IdDuJeu AND `DateRetour` IS NULL");
        RequeteDateRetour.bindValue(":IdDuJeu",Requete.value(2));
        RequeteDateRetour.exec();
        RequeteDateRetour.next();
        ui->LE_DateDisponible->setText(RequeteDateRetour.value(0).toDate().toString("ddd d MMMM yyyy"));

        //mettre l'attribut de la date d'emprunt à jour
        this->DateEmprunt=RequeteDateRetour.value(0).toDate();
        //mettre la date de l'emprunt à la date la date de ratour par défaut
        ui->Cal_DateEmprunt->setMinimumDate(RequeteDateRetour.value(0).toDate());
        ui->Cal_DateEmprunt->setSelectedDate(RequeteDateRetour.value(0).toDate());
    }
    else
    {
        //sinon, cacher le label et le line edit
        ui->Lb_Disponible->setVisible(false);
        ui->LE_DateDisponible->setVisible(false);

        //mette la date de l'emprunt à aujourd'hui
        QDate DateActuelle;
        DateActuelle=DateActuelle.currentDate();
        ui->Cal_DateEmprunt->setMinimumDate(DateActuelle);
        ui->Cal_DateEmprunt->setSelectedDate(DateActuelle);
        //mettre l'attribut de la date d'emprunt à jour
        this->DateEmprunt=DateActuelle;
    }


    QSqlQuery RequeteStatut;

//Prépare la requête pour cherche le statut du jeu et l'execute
    RequeteStatut.prepare("SELECT StatutJeu FROM statutjeux WHERE IdStatutJeux=:IdStatutDuJeu");
    RequeteStatut.bindValue(":IdStatutDuJeu",IdStatut);

    if (!RequeteStatut.exec())
    {

    }

    RequeteStatut.next();

//Récupère le statut et l'affiche
    ui->LE_StatutJeu->setText(RequeteStatut.value(0).toString());

//Calcule et affichage de la date du retour

    CalculerDateRetour();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////Changement de la date d'emprunt//////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void F_Reservation::on_Cal_DateEmprunt_clicked(const QDate &Date)
{
    DecolorierJours();
    this->DateEmprunt=ui->Cal_DateEmprunt->selectedDate();
    CalculerDateRetour();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////calcule de la date de retour/////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void F_Reservation::CalculerDateRetour()
{
   // DecoloriageJours();

    QDate DateRetour;
//DateRetour prend la date de l'emprunt
    DateRetour=ui->Cal_DateEmprunt->selectedDate();

    QString TypeEmprunt ;

//On cherche le text du ComboBox qui détermine le type d'emprunt
     TypeEmprunt=ui->CBx_TypeEmprunt->currentText().left(ui->CBx_TypeEmprunt->currentText().indexOf("(")-1);


//On prépare et exécute la requête qui permet de récupérer la durée dy type d'emprunt
    QSqlQuery Requete;

    Requete.prepare("SELECT DureeEmprunt FROM `typeemprunt`WHERE TypeEmprunt=:Type");
    Requete.bindValue(":Type",TypeEmprunt);
    Requete.exec();
    Requete.next();

//La date de retour prend la date de l'emprunt plus le nombre de jour indiqué par le type d'emprunt
    DateRetour=DateRetour.addDays((Requete.value(0).toInt()));
//Grise les dates inferieurs à la date d'emprunt
    ui->Cal_DateRetour->setMinimumDate(ui->Cal_DateEmprunt->selectedDate());

//Met la date du retour dans le calendrier du retour
    ui->Cal_DateRetour->setSelectedDate(DateRetour);
    this->DateRetour=DateRetour;

    ColorierJours();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Coloriage des jours de l'emprunt///////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void F_Reservation::ColorierJours()
{

//Colorie les jours concernés par l'emprunt.
    QTextCharFormat formatDate;
    QDate DateRetour;
    QDate DateEmprunt;

    DateEmprunt=ui->Cal_DateEmprunt->selectedDate();
    DateRetour=ui->Cal_DateRetour->selectedDate();

    unsigned int JourEmprunt;
    JourEmprunt=DateEmprunt.daysTo(DateRetour);

    formatDate.setBackground(QBrush(Qt::green, Qt::SolidPattern));
    ui->Cal_DateRetour->setDateTextFormat(DateRetour,formatDate);
    for (unsigned int i=0;i<JourEmprunt;i++)
    {
        ui->Cal_DateRetour->setDateTextFormat(DateEmprunt.addDays(i),formatDate);
    }

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////Déoloriage des jours de l'emprunt///////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void F_Reservation::DecolorierJours()
{
//Décolorie les jours coloriés.
    QTextCharFormat formatDate;

    unsigned int JourEmprunt;
    JourEmprunt=this->DateEmprunt.daysTo(this->DateRetour);

    formatDate.setBackground(QBrush(Qt::white, Qt::SolidPattern));
    ui->Cal_DateRetour->setDateTextFormat(this->DateRetour,formatDate);
    for (unsigned int i=0;i<JourEmprunt;i++)
    {
        ui->Cal_DateRetour->setDateTextFormat(this->DateEmprunt.addDays(i),formatDate);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////Changement du type d'emprunt/////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void F_Reservation::on_CBx_TypeEmprunt_currentIndexChanged(int index)
{
    this->DateEmprunt=ui->Cal_DateEmprunt->selectedDate();
    this->DateRetour=ui->Cal_DateRetour->selectedDate();
    DecolorierJours();
    CalculerDateRetour();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////Changement de la date d'emprunt//////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void F_Reservation::on_Cal_DateRetour_clicked(const QDate &date)
{
    DecolorierJours();
    this->DateRetour=ui->Cal_DateRetour->selectedDate();
    ColorierJours();
}

//#######################################################################################################
/////////////////////////////////////////////Réservation/////////////////////////////////////////////////
//#######################################################################################################



/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////Réserver un jeu//////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void F_Reservation::on_Bt_ValiderReservation_clicked()

{

//S'il un membre est sellectionné
if((MembreActif)!=(""))
{
//Recherche de l'id du membre
    //Prépare la requête
    QSqlQuery RequeteIdMembre;
    RequeteIdMembre.prepare("SELECT IdMembre FROM membres WHERE CodeMembre=:CodeDuMembre");
    RequeteIdMembre.bindValue(":CodeDuMembre",MembreActif);

    //Execute la requête
    if (!RequeteIdMembre.exec())
    {
        qDebug()<<"F_Reservation::on_Bt_ValiderReservation_clicked"<<RequeteIdMembre.lastError();
    }

    RequeteIdMembre.next();

//Recherche de l'id du jeu
    //Prépare la requête
    QSqlQuery RequeteIdJeu;
    RequeteIdJeu.prepare("SELECT `IdJeux` FROM `jeux`WHERE `CodeJeu`=:CodeDuJeu");
    RequeteIdJeu.bindValue(":CodeDuJeu",JeuActif);

    //Execute la requête
    if (!RequeteIdJeu.exec())
    {
        qDebug()<<"F_Reservation::on_Bt_ValiderReservation_clicked"<<RequeteIdJeu.lastError();
    }

    RequeteIdJeu.next();

    //Date de l'emprunt
    QDate DateActuelle;
    DateActuelle=DateActuelle.currentDate();

    //Lieu de l'emprunt (site ou est le logiciel)
    unsigned int Lieu (2);

//Enregistre la réservation
    //Prépare la requête
    QSqlQuery RequeteReservation;
    RequeteReservation.prepare("insert into `reservation` (`Lieux_IdLieux`,`Membres_IdMembre`,`Jeux_IdJeux`,`DateReservation`,`DatePrevuEmprunt`,`DatePrevuRetour`) values (:IdLieu,:IdMembre,:IdJeu,:DateReservation,:DateEmprunt,:DateRetour)");
    RequeteReservation.bindValue(":IdLieu",Lieu);
    RequeteReservation.bindValue(":IdMembre",RequeteIdMembre.value(0));
    RequeteReservation.bindValue(":IdJeu",RequeteIdJeu.value(0));
    RequeteReservation.bindValue(":DateReservation",DateActuelle);
    RequeteReservation.bindValue(":DateEmprunt",ui->Cal_DateEmprunt->selectedDate());
    RequeteReservation.bindValue(":DateRetour",ui->Cal_DateRetour->selectedDate());

    //Execute la requête
    if(!RequeteReservation.exec())
    {
        qDebug()<<"F_Reservation::on_Bt_ValiderReservation_clicked "<< RequeteReservation.lastError();
    }

    //Si le jeu est "Disponible",
    if(ui->LE_StatutJeu->text()=="Disponible")
    {
        //Si oui, mettre son statut à "Réservé"
        QSqlQuery RequeteStatut;
        RequeteStatut.prepare("UPDATE `jeux`SET `StatutJeux_IdStatutJeux`=4 WHERE `CodeJeu`=:CodeDuJeu");
        RequeteStatut.bindValue(":CodeDuJeu",JeuActif);
        RequeteStatut.exec();
    }
    this->hide();
    ViderMembre();

}
//sinon
else
{
    qDebug()<<"F_Reservation::on_Bt_ValiderReservation_clicked ||| Aucun membre sellectionné !!!";
}
}





void F_Reservation::on_Bt_AnnulerReservation_clicked()
{
    DecolorierJours();
    this->hide();
}