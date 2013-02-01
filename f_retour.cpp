/** @file         F_Retour.cpp
 *  @brief        Classe graphique permettant le retours à la ludothèque des jeux
 *
 *  @author       Botherel Philippe
 *  @author       STS IRIS, Lycée Nicolas APPERT, ORVAULT (FRANCE)
 *  @since        21/01/2012
 *  @version      0.1
 *  @date         01/02/2013 William
 *
 *  Cette classe permet de rendre les jeux qu'un membre à emprunté,
 *  on peut aussi supprimer une reservation de jeu entre autre
 *
 */
#include <QtSql>
#include "f_retour.h"
#include "ui_f_retour.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Constructeur///////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/** Constructeur de F_Retour
 *  @pre    Connexion à la base de données, Table des emprunts existante
 */
F_Retour::F_Retour(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::F_Retour)
{
    ui->setupUi(this);
    // pas de membre choisi
    this->MembreActif="";
    // pas de jeu choisi
    this->JeuActif="";

    this->pPaiement= new F_Paiement;

//Met une date minimum pour le DateEdit de la prolongation (la date du jour)
    QDate DateActuelle;
    DateActuelle=DateActuelle.currentDate();
    ui->DtE_Prolonger->setMinimumDate(DateActuelle);

//Grise les boutons de modification des remarques
    ui->Bt_ValiderRemarqueMembre->setEnabled(false);
    ui->Bt_AnnulerRemarqueMembre->setEnabled(false);
    ui->Bt_ValiderRemarqueJeu->setEnabled(false);
    ui->Bt_AnnulerRemarqueJeu->setEnabled(false);

    QSqlQuery RequeteEtatJeu;
    QSqlQuery RequeteStatutJeu;
    QString EtatJeu;
    QString StatutJeu;

//Remplissage du ComboBox de l'état du jeu à partir de la table etatjeu
   RequeteEtatJeu.exec("SELECT Etat FROM `etatsjeu`");

    //Tant qu'il y a des types états dans la table etatjeu,
    while(RequeteEtatJeu.next())
        {
            //on entre un nouveau Item au ComboBox avec le nom de l'état
            EtatJeu=(RequeteEtatJeu.value(0).toString());
            ui->CBx_Etat->addItem(EtatJeu);
   }

//Remplissage du ComboBox du statut du jeu à partir de la table statutsJeu
    RequeteStatutJeu.exec("SELECT `StatutJeu` FROM `statutjeux`");

    //Tant qu'il y a des types status dans la table statutsjeu,
    while(RequeteStatutJeu.next())
    {
        //on entre un nouveau Item au ComboBox avec le nom du statut
        StatutJeu=(RequeteStatutJeu.value(0).toString());
        ui->CBx_Statut->addItem(StatutJeu);
    }


//Remplissage du ComboBox du type de prolongation du jeu à partir de la table typeemprunt
    QSqlQuery Requete;
    QString TypeEmprunt;

   Requete.exec("SELECT `TypeEmprunt`,DureeEmprunt FROM `typeemprunt`");

//Tant qu'il y a des types d'emprunt dans la table TupesEmprunt,
   while(Requete.next())
       {
           //on entre un nouveau Item au ComboBox avec le nom du type d'emprunt
       TypeEmprunt=(Requete.value(0).toString())+" ("+(Requete.value(1).toString())+"jours)";
           ui->CBx_TypeProlongation->addItem(TypeEmprunt);
       }


//Création d'un modèle pour le TableView des membres
    this->ModelMembre = new QStandardItemModel() ;
//Associe le modèl au TableView
    ui->TbV_Recherche->setModel(this->ModelMembre);
//Met le TableView en lecture seule
     ui->TbV_Recherche->setEditTriggers(0);
//Initialise le tableau avec tous les membres
     this->on_LE_RechercheMembre_textChanged("");


//Création d'un modèle pour le TableView des jeux empruntés
    this->ModelJeuEmpruntes = new QStandardItemModel() ;
//Associe le modèl au TableView
    ui->TbV_JeuxEmprunte->setModel(this->ModelJeuEmpruntes);
//Met le TableView en lecture seule
    ui->TbV_JeuxEmprunte->setEditTriggers(0);
//Initialise les colones du TableView des nouveaux emprunts
    this->ModelJeuEmpruntes->setColumnCount(4);
    this->ModelJeuEmpruntes->setHorizontalHeaderItem(0, new QStandardItem("Code"));
    this->ModelJeuEmpruntes->setHorizontalHeaderItem(1, new QStandardItem("Nom du jeu"));
    this->ModelJeuEmpruntes->setHorizontalHeaderItem(2, new QStandardItem("Date emprunt"));
    this->ModelJeuEmpruntes->setHorizontalHeaderItem(3, new QStandardItem("Date retour"));
    ui->TbV_JeuxEmprunte->setColumnWidth(0,40);
    ui->TbV_JeuxEmprunte->setColumnWidth(1,125);


//Création d'un modèle pour le TableView des jeux réservés
   this->ModelJeuReserves = new QStandardItemModel() ;
//Associe le modèl au TableView
   ui->TbV_JeuxReserve->setModel(this->ModelJeuReserves);
//Met le TableView en lecture seule
   ui->TbV_JeuxReserve->setEditTriggers(0);

//Initialise les colones du TableView des nouveaux emprunts
    this->ModelJeuReserves->setColumnCount(5);
    this->ModelJeuReserves->setHorizontalHeaderItem(0, new QStandardItem("Code"));
    this->ModelJeuReserves->setHorizontalHeaderItem(1, new QStandardItem("Nom du jeu"));
    this->ModelJeuReserves->setHorizontalHeaderItem(2, new QStandardItem("Date réservation"));
    this->ModelJeuReserves->setHorizontalHeaderItem(3, new QStandardItem("Date emprunt"));
    this->ModelJeuReserves->setHorizontalHeaderItem(4, new QStandardItem("Date retour"));
    this->ModelJeuReserves->setHorizontalHeaderItem(5, new QStandardItem("lieu"));
    ui->TbV_JeuxReserve->setColumnWidth(0,40);
    ui->TbV_JeuxReserve->setColumnWidth(1,125);

}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////Déstructeur//////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
F_Retour::~F_Retour()
{
    delete ui;
}

//#######################################################################################################
//////////////////////////////////////////////MEMBRES////////////////////////////////////////////////////
//#######################################################################################################

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Rechercher un membre avec son nom//////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void F_Retour::on_LE_RechercheMembre_textChanged(const QString &arg1)
{

    unsigned int NumeroLigne (0);
    QString Nom=arg1;

    if(Nom.size()>=2)
    {


        Nom="%"+Nom+"%";

        QSqlQuery RequeteMembre;
        NumeroLigne=0;


        RequeteMembre.prepare("SELECT `CodeMembre`,`Nom`,Prenom,`DateNaissance` FROM`membres`,`emprunts`WHERE `emprunts`.DateRetour  IS NULL AND `Membres_IdMembre`=`IdMembre` AND `membres`.`Nom` LIKE (:Nom) GROUP BY `CodeMembre` ORDER BY Nom ASC ");
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
                RequeteMembre.exec("SELECT `CodeMembre`,`Nom`,Prenom,`DateNaissance` FROM`membres`,`emprunts`"
                                   " WHERE `emprunts`.DateRetour  IS NULL "
                                   " AND `Membres_IdMembre`=`IdMembre` GROUP BY `CodeMembre`");
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
////////////////////////////////////////Clic sur un membre dans le tableau///////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void F_Retour::on_TbV_Recherche_clicked(const QModelIndex &index)
{
    //Met le code du membre sellectionné dans le line édit du code du membre
    ui->LE_CodeMembre->setText(this->ModelMembre->index(index.row(),0).data().toString());
    //clic sur la recherche du membre
    on_Bt_RechercheOK_clicked();

    this->ViderJeu();
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Pression sur entrée pour le code membre////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void F_Retour::on_LE_CodeMembre_returnPressed()
{
    on_Bt_RechercheOK_clicked();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////Clic sur le bonton OK du code membre//////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void F_Retour::on_Bt_RechercheOK_clicked()
{
// Actualise l'atribut MembreActif avec le code du membre que l'on affiche
    this->MembreActif=ui->LE_CodeMembre->text();
//Affiche les informations du membre
    this->AfficherMembre(this->MembreActif);


}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Actualise les informations du membre///////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/** Actualiser les informations affichées du membre grâce au code membre de l'attribut
 *  @pre    Connexion à la base de données
 *  @see    MembreActif
 */
void F_Retour::ActualiserMembre()
{
    ui->LE_CodeMembre->setText(this->MembreActif);
    AfficherMembre(this->MembreActif);
}

/** Actualise la liste des membres ayant des emprunts
 *  Donne la liste de tous les membres ayant un retour à faire.
 */
void F_Retour::ActualiserListeEmprunteurs()
{
    this->on_LE_RechercheMembre_textChanged("");
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Afficher le membre dans le volet central///////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/** Affiche les information du membre passé en paramètre
 *  @pre    Connexion à la base de données
 *  @post   Description des postconditions nécessaires à la méthode
 *  @param  CodeMembre est le code du membre à passer en paramètre pour afficher ces informations
 */
void F_Retour::AfficherMembre(QString CodeMembre)
{
    // Actualise l'atribut MembreActif avec le code du membre que l'on affiche
    MembreActif=CodeMembre;
    ui->LE_CodeMembre->setText(CodeMembre);

    ui->LE_NbreJeuxRendre->setText("0");

    QSqlQuery Requete;

//Prépare la requête
    Requete.prepare("SELECT Nom,Prenom,NbreRetard,Ecarte,Remarque,NbreJeuxAutorises FROM membres WHERE CodeMembre=:CodeDuMembre");
    Requete.bindValue(":CodeDuMembre",CodeMembre);

//Execute la requête
    if (!Requete.exec())
    {
        qDebug()<<"F_Retour::AfficherMembre || requete info membre "<<Requete.lastError();
    }

    Requete.next();

//Récupère le Nom dans la base de données puis l'affiche
    ui->LE_NomARemplir->setText(Requete.value(0).toString());

//Récupère le Prénom dans la base de données puis l'affiche
    ui->LE_PrenomARemplir->setText(Requete.value(1).toString());

//Récupère le nombre de retards dans la base de données  puis l'affiche
    ui->LE_RetardARemplir->setText(Requete.value(2).toString());

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
    ui->Txe_RemarqueMembre->setText(Requete.value(4).toString());

//Grise les boutons de modification des remarques du membre
    ui->Bt_ValiderRemarqueMembre->setEnabled(false);
    ui->Bt_AnnulerRemarqueMembre->setEnabled(false);

//Récupère le nombre de jeux empruntables dans la base de données puis l'afficher
    ui->LE_JeuxMaxARemplir->setText(Requete.value(5).toString());

//Affiche l'état de la cotisation
       //Savoir si le membre à un memmbre assosier
       QSqlQuery RequeteMembreAssocier ;
       RequeteMembreAssocier.prepare("SELECT `MembreAssocie`FROM`membres` WHERE `CodeMembre`=:CodeDuMembre AND `MembreAssocie`!=0");
       RequeteMembreAssocier.bindValue(":codeDuMembre",this->MembreActif);
       RequeteMembreAssocier.exec();
       RequeteMembreAssocier.next();
       //s'i y en a un,
       if(RequeteMembreAssocier.size()>0)
       {
           //On Affiche l'état de la cotisation du membre associé au membre actif
           EtatCotisation(RequeteMembreAssocier.value(0).toString());
       }
       //Sinon,
       else
       {
           //On affiche l'état de la cotisation du membre en cours de consultation
           EtatCotisation(this->MembreActif);
       }

//Affiche les jeux empruntés
    JeuxEnEmprunt();
//Affiche les jeux réservés
    JeuxReserve();
//Affiche le nombre de crédits restants
    CalculerCreditsRestants();


}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////Afficher l'état de la cotisation d'un membre/////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/** Description détaillée de la méthode
 *  @pre    Connexion avec la base de donnée
 *  @param  Code du membre
 *  @return CotisationAJours
 */
void F_Retour::EtatCotisation(QString CodeMembre)
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
            Cotisation="Cotisation expirée (membre "+ CodeMembre +") depuis le";
        }
        //Sinon,
        else
        {
            //On affiche juste l'état de la cotisation
            Cotisation="Cotisation expirée depuis le ";
        }
        ui->Lb_Cotisation->setText(Cotisation);
        ui->Lb_Cotisation->setStyleSheet("QLabel {color:red;}");
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
        ui->Lb_Cotisation->setText(Cotisation);
        ui->Lb_CotisationDate->setText("("+ JourRestant +" jours restant)");
        //Si le nombre de jonrs restant est inferieur à 30 jours, on l'affiche en orange
        if (NombreJoursRestant<31)
        {
            ui->Lb_Cotisation->setStyleSheet("QLabel {color:orange;}");
            ui->Lb_CotisationDate->setStyleSheet(" QLabel{color:orange;}" );
        }
        //sinon, on l'affiche ne vert
        else
        {
            ui->Lb_Cotisation->setStyleSheet("QLabel {color:green;}");
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
void F_Retour::CalculerCreditsRestants()
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

    ui->LE_CreditARemplir->setText(Credit.setNum(CreditRestant));
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////Changement de la remaque du Membre//////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void F_Retour::on_Txe_RemarqueMembre_textChanged()
{
//Affiche les boutons de modification de la remarque du jeu
    ui->Bt_ValiderRemarqueMembre->setEnabled(true);
    ui->Bt_AnnulerRemarqueMembre->setEnabled(true);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Enregistrer la remarque du membre//////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void F_Retour::on_Bt_ValiderRemarqueMembre_clicked()
{
    QSqlQuery RequeteMiseAJourRemarqueMembre;

//prépare le requête de mise à jour
    RequeteMiseAJourRemarqueMembre.prepare("UPDATE `membres` SET `Remarque` =:NouvelRemarque WHERE `CodeMembre`=:CodeDuMembre");

//Entre les valeurs de la reqête
    RequeteMiseAJourRemarqueMembre.bindValue(":CodeDuMembre",MembreActif);
    RequeteMiseAJourRemarqueMembre.bindValue(":NouvelRemarque",ui->Txe_RemarqueMembre->toPlainText());

//Execut la requête
    if (!RequeteMiseAJourRemarqueMembre.exec())
    {

    }

//Grise les boutons de modification de le remarque
    ui->Bt_ValiderRemarqueMembre->setEnabled(false);
    ui->Bt_AnnulerRemarqueMembre->setEnabled(false);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Annuler changement de la remaque du membre/////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void F_Retour::on_Bt_AnnulerRemarqueMembre_clicked()
{
    QSqlQuery RequeteAnnulerRemarqueMembre;

//Prépare la requête
    RequeteAnnulerRemarqueMembre.prepare("SELECT Remarque FROM membres WHERE CodeMembre=:CodeDuMembre");
    RequeteAnnulerRemarqueMembre.bindValue(":CodeDuMembre",MembreActif);


//Execut la requête
    if (!RequeteAnnulerRemarqueMembre.exec())
    {

    }

    RequeteAnnulerRemarqueMembre.next();

//Récupère les remarques dans la base de données et les affiches
    QString TextTemporaire = (RequeteAnnulerRemarqueMembre.value(0).toString());
    ui->Txe_RemarqueMembre->setText(TextTemporaire);

//Grise les boutons de modification des remarques du membre
    ui->Bt_ValiderRemarqueMembre->setEnabled(false);
    ui->Bt_AnnulerRemarqueMembre->setEnabled(false);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Recherche des jeux en emprunt//////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/** affiche dans le tableau les jeux en cours d'emprunt du membre en cours de consultation
 *  @pre    connexion à la base de données, un membre est en cours de consultation
 *  @post   Description des postconditions nécessaires à la méthode
 *  @see    MembreActif
 */
void F_Retour::JeuxEnEmprunt()
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
        qDebug()<< "F_Retour::JeuxEnEmprunt ||| RequeteIdMembre  "<< RequeteIdMembre.lastError();
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
        qDebug()<<"F_Retour::JeuxEnEmprunt() ||| Requette jeux en emprunt "<<RequeteJeuEmprunte.lastError();
    }

    //On vide le modèle
    this->ModelJeuEmpruntes->clear();
    //Initialise les colones du TableView des jeux empruntés
         this->ModelJeuEmpruntes->setColumnCount(4);
         this->ModelJeuEmpruntes->setHorizontalHeaderItem(0, new QStandardItem("Code"));
         this->ModelJeuEmpruntes->setHorizontalHeaderItem(1, new QStandardItem("Nom du jeu"));
         this->ModelJeuEmpruntes->setHorizontalHeaderItem(2, new QStandardItem("Date emprunt"));
         this->ModelJeuEmpruntes->setHorizontalHeaderItem(3, new QStandardItem("Date retour"));
         ui->TbV_JeuxEmprunte->setColumnWidth(0,40);
         ui->TbV_JeuxEmprunte->setColumnWidth(1,125);
         ui->TbV_JeuxEmprunte->verticalHeader()->setVisible(false);

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
        QString NbJeux ;
        NbJeux.setNum(NumeroLigne);
        ui->LE_NbreJeuxRendre->setText(NbJeux);

    }

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Recherche des jeux réservés////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/** Affiche les jeux réservé dans le tableau
 *  @pre    Connextion à la base de données, un membre est en cours de consultaion
 *  @see    MembreActif
 */
void F_Retour::JeuxReserve()
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
        qDebug()<<"F_Retour::JeuxReserve ||| Requete Jeux Reservés "<<RequeteJeuReserve.lastError();
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
         ui->TbV_JeuxReserve->setColumnWidth(0,40);
         ui->TbV_JeuxReserve->setColumnWidth(1,125);
         ui->TbV_JeuxReserve->verticalHeader()->setVisible(false);

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

//#######################################################################################################
//////////////////////////////////////////////JEUX///////////////////////////////////////////////////////
//#######################################################################################################

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Pression sur entrée pour le code jeu///////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void F_Retour::on_LE_CodeJeu_returnPressed()
{
    on_Bt_CodeJeuOK_clicked();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Actualise les informations du jeu//////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/** Actualise les informations du jeu en consultation
 *  @pre    Connexion à la base de données, un jeu est en cours de consultation
 *  @see    afficherAttributs
 */
void F_Retour::ActualiserJeu()
{
    ui->LE_CodeJeu->setText(this->JeuActif);
    on_Bt_CodeJeuOK_clicked();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////Valider Jeu et l'afficher dans le volet de droite////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void F_Retour::on_Bt_CodeJeuOK_clicked()
{
//Récupère le code du jeu qui à été saisi
    QString CodeJeu= ui->LE_CodeJeu->text();

    //Vérification que le jeu est bien à rendre

    QSqlQuery RequeteStatut ;
    RequeteStatut.prepare("SELECT `IdStatutJeux`FROM `statutjeux`,`jeux` WHERE `IdStatutJeux`=`StatutJeux_IdStatutJeux` AND `CodeJeu`=:CodeDuJeu");
    RequeteStatut.bindValue(":CodeDuJeu",CodeJeu);
    RequeteStatut.exec();
    RequeteStatut.next();

    if(RequeteStatut.value(0)==3)
    {
        this->JeuActif =CodeJeu;
        //Affiche les informations du jeu
        AfficherJeu(CodeJeu);

        //Recherche du code du membre qui à emprunté le jeu
        QSqlQuery RequeteCodeMembre ;
        RequeteCodeMembre.prepare("SELECT `CodeMembre`"
                                  "FROM `emprunts`,`membres`,`jeux` "
                                  "WHERE CodeJeu=:CodeDuJeu AND `Jeux_IdJeux`=`IdJeux` AND`Membres_IdMembre`=`IdMembre` AND `DateRetour` IS NULL");
        RequeteCodeMembre.bindValue(":CodeDuJeu",CodeJeu);
        if(!RequeteCodeMembre.exec())
        {
            qDebug()<<"F_Retour::on_Bt_CodeJeuOK_clicked || requête code du membre qui à la jeu"<< RequeteCodeMembre.lastError();
        }
        RequeteCodeMembre.next();

        AfficherMembre(RequeteCodeMembre.value(0).toString());
    }
    else
    {
        if (ui->LE_CodeJeu->text()!="")
        {
            QMessageBox::information(this,"Jeu non emprunté","Ce jeu n'à pas été emprunté.","Ok");
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Afficher le jeu dans le volet de droite////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/** Affiche les informations du jeu passé en paramètre
 *  @pre    connexion à la base de données
 *  @param  CodeJeu est le code du jeu dont les informations sont à afficher
 *  @see    JeuActif
 */
void F_Retour::AfficherJeu(QString CodeJeu)
{
    //Vérification que le jeu est bien à rendre
    QSqlQuery RequeteStatut ;

    RequeteStatut.prepare("SELECT `IdStatutJeux`FROM `statutjeux`,`jeux` WHERE `IdStatutJeux`=`StatutJeux_IdStatutJeux` AND `CodeJeu`=:CodeDuJeu");
    RequeteStatut.bindValue(":CodeDuJeu",CodeJeu);
    RequeteStatut.exec();
    RequeteStatut.next();

    if(RequeteStatut.value(0)==3)
    {
        this->JeuActif = CodeJeu;
    }

    //dégrise le bonton rendre
    ui->Bt_RendreJeu->setEnabled(true);

    //Recherche d'une éventuelle réservation du jeux
    QSqlQuery RequeteJeu;
    RequeteJeu.prepare("SELECT`idReservation` FROM`reservation`,`jeux`"
                       "WHERE `CodeJeu`=:CodeDuJeu AND`Jeux_IdJeux`=`IdJeux` AND `JeuEmprunte`=1");
    RequeteJeu.bindValue(":CodeDuJeu",this->JeuActif);
    RequeteJeu.exec();
    RequeteJeu.next();

    if(RequeteJeu.size()>0)
    {
        //Si oui, mettre le bonton de prolongation en grisé
        ui->Bt_Prolonger->setEnabled(false);
    }
    else
    {
        //Sinon, le rendre cliquable
        ui->Bt_Prolonger->setEnabled(true);
    }

     QSqlQuery Requete;
//Prépare la requête et entre ses valeurs
     Requete.prepare("SELECT NomJeu, ContenuJeu, Remarque FROM jeux WHERE CodeJeu=:CodeDuJeu");

     Requete.bindValue(":CodeDuJeux",CodeJeu);

 //Execute la requête
     if (!Requete.exec())
     {

     }

     Requete.next();

 //Récupère le nom du jeu et l'affiche
     QString TextTemporaire = (Requete.value(0).toString());
     ui->LE_NomJeuARemplir->setText( TextTemporaire ) ;

 //Récupère le contenu de le boîte et l'affiche
     TextTemporaire=(Requete.value(1).toString()) ;
     ui->TxE_ContenuBoite->setText(TextTemporaire );

 //Récupère la remarque et l'affiche
     TextTemporaire = (Requete.value(2).toString());
     ui->TxE_RemarquesJeu->setText(TextTemporaire);

 //Grise les boutons de modification de la remarque du jeu
     ui->Bt_ValiderRemarqueJeu->setEnabled(false);
     ui->Bt_AnnulerRemarqueJeu->setEnabled(false);

 //Calcule la date du retour
     on_CBx_TypeProlongation_currentIndexChanged(0);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////Vide les informations du jeu/////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/** Vide toutes les informations du jeu qui sont affichés
 */
void F_Retour::ViderJeu()
{
    ui->LE_CodeJeu->setText("");
    ui->LE_NomJeuARemplir->setText("");
    ui->TxE_ContenuBoite->setText("");
    ui->LE_Caution->setText("");
    ui->TxE_RemarquesJeu->setText("");
    this->JeuActif="";

    //grise les bontons rendre et prolonger
    ui->Bt_RendreJeu->setEnabled(false);
    ui->Bt_Prolonger->setEnabled(false);
    //grise les bontons de la remarque du jeu
    ui->Bt_AnnulerRemarqueJeu->setEnabled(false);
    ui->Bt_ValiderRemarqueJeu->setEnabled(false);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////Changement de la remaque du jeu/////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void F_Retour::on_TxE_RemarquesJeu_textChanged()
{
//Affiche les boutons de modification de la remarque du jeu
    ui->Bt_ValiderRemarqueJeu->setEnabled(true);
    ui->Bt_AnnulerRemarqueJeu->setEnabled(true);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Enregistrer la remarque du jeu/////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void F_Retour::on_Bt_ValiderRemarqueJeu_clicked()
{
    QSqlQuery RequeteMiseAJourRemarqueJeu;

//prépare le requête de mise à jour
    RequeteMiseAJourRemarqueJeu.prepare("UPDATE `jeux` SET `Remarque` =:NouvelRemarque WHERE `CodeJeu`=:CodeDuJeu");

//Entre les valeurs de la reqête
    RequeteMiseAJourRemarqueJeu.bindValue(":CodeDuJeu",JeuActif);
    RequeteMiseAJourRemarqueJeu.bindValue(":NouvelRemarque",ui->TxE_RemarquesJeu->toPlainText());

//Execut la requête
    if (!RequeteMiseAJourRemarqueJeu.exec())
    {

    }

//Grise les boutons de modification de le remarque
    ui->Bt_ValiderRemarqueJeu->setEnabled(false);
    ui->Bt_AnnulerRemarqueJeu->setEnabled(false);


}



/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////Anuler le changement de la remarque du jeu///////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void F_Retour::on_Bt_AnnulerRemarqueJeu_clicked()
{
    QSqlQuery RequeteRemarqueJeu;

//prépare le reqête d'annulation
    RequeteRemarqueJeu.prepare("SELECT Remarque FROM jeux WHERE CodeJeu=:CodeDuJeu");
    RequeteRemarqueJeu.bindValue(":CodeDuJeu",JeuActif);

//Execute la requête
    if (!RequeteRemarqueJeu.exec())
    {

    }

    RequeteRemarqueJeu.next();

//Récupère les remarques dans la base de données
    QString TextTemporaire = (RequeteRemarqueJeu.value(0).toString());
    ui->TxE_RemarquesJeu->setText(TextTemporaire);

//grise les boutons de modification des remarques du jeu
    ui->Bt_ValiderRemarqueJeu->setEnabled(false);
    ui->Bt_AnnulerRemarqueJeu->setEnabled(false);
}


//#######################################################################################################
//////////////////////////////////////////////Retour/////////////////////////////////////////////////////
//#######################################################################################################

/////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////Rendre le jeu/////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void F_Retour::on_Bt_RendreJeu_clicked()
{
    if((JeuActif!="")&&(MembreActif!=""))
    {
        QSqlQuery RequeteRetour;
        QDate DateActuelle;
        DateActuelle=DateActuelle.currentDate();

        //Rechercher la date prévu de l'emprunt
        QSqlQuery RequeteDateRetour;
        RequeteDateRetour.prepare("SELECT `DateRetourPrevu` "
                                  "FROM `emprunts`,`jeux`"
                                  " WHERE `CodeJeu`=:CodeDuJeu AND `Jeux_IdJeux`=`IdJeux` AND `DateRetour` IS NULL");
        RequeteDateRetour.bindValue(":CodeDuJeu",this->JeuActif);
        RequeteDateRetour.exec();
        RequeteDateRetour.next();

        //Comparaison entre la date prévu du retour et la date du jour, on récupère un int
        int EcartJours (0);
        EcartJours= RequeteDateRetour.value(0).toDate().daysTo(DateActuelle);

        //si la date de retour est superieur à la date du jour, alors le jeux est rendu en retard
        if(EcartJours>0)
        {
            //Donc on incrémente le nombre de retard du membre
            //On récupère d'abord le nombre de retarde du membre
            QSqlQuery NbRetardsMembre;
            NbRetardsMembre.prepare("SELECT `NbreRetard` FROM `membres` WHERE `CodeMembre`=:CodeDuMembre");
            NbRetardsMembre.bindValue("CodeDuMembre",this->MembreActif);
            NbRetardsMembre.exec();
            NbRetardsMembre.next();

            //Puis on l'incrémente
            QSqlQuery RequeteRetard;
            RequeteRetard.prepare("UPDATE `membres` SET `NbreRetard`=:NbRetard  WHERE `CodeMembre`=:CodeDuMembre");
            RequeteRetard.bindValue(":NbRetard",NbRetardsMembre.value(0).toInt()+1);
            RequeteRetard.bindValue(":CodeDuMembre",this->MembreActif);
            RequeteRetard.exec();
        }

        //Rechercher de l'id du statut
        QSqlQuery RequeteStatut;
        RequeteStatut.prepare("SELECT  `IdStatutJeux` FROM `statutjeux` WHERE `StatutJeu`=:StatutDuJeu");
        RequeteStatut.bindValue(":StatutDuJeu",ui->CBx_Statut->currentText());
        RequeteStatut.exec();
        RequeteStatut.next();

        //Rechercher de l'id de l'état
        QSqlQuery RequeteEtat;
        RequeteEtat.prepare("SELECT  `IdEtatsJeu` FROM `etatsjeu` WHERE `Etat`=:EtatDuJeu");
        RequeteEtat.bindValue(":EtatDuJeu",ui->CBx_Etat->currentText());
        RequeteEtat.exec();
        RequeteEtat.next();


        RequeteRetour.prepare("UPDATE `emprunts`,`jeux`"
                                      "SET`DateRetour`=:DateRetour,`StatutJeux_IdStatutJeux`=:StatutDuJeu, `EtatsJeu_IdEtatsJeu`=:EtatDuJeu "
                                      "WHERE `DateRetour`IS NULL AND `Jeux_IdJeux`=`IdJeux` AND `CodeJeu`=:CodeDuJeu");


        RequeteRetour.bindValue(":DateRetour",DateActuelle);
        RequeteRetour.bindValue(":CodeDuJeu",this->JeuActif);
        RequeteRetour.bindValue(":StatutDuJeu",RequeteStatut.value(0).toString());
        RequeteRetour.bindValue(":EtatDuJeu",RequeteEtat.value(0).toString());
        if(!RequeteRetour.exec())
        {
            qDebug()<<"F_Retour::on_Bt_RendreJeu_clicked ||| requete retour "<< RequeteRetour.lastError();
        }
        qDebug("1");
        if(RequeteStatut.value(0).toString()=="Disponnible")
        {
            //Savoir si le jeu est réservé
            QSqlQuery RequeteJeu;
            RequeteJeu.prepare("SELECT`idReservation` FROM`reservation`,`jeux`"
                               "WHERE `CodeJeu`=:CodeDuJeu AND`Jeux_IdJeux`=`IdJeux` AND `JeuEmprunte`=1");
            RequeteJeu.bindValue(":CodeDuJeu",this->JeuActif);
            RequeteJeu.exec();
            RequeteJeu.next();
            qDebug()<<"dispo";

            if(RequeteJeu.size()>0)
            {
                //Si oui, mettre son statut à "réservé"
                QSqlQuery RequeteStatut;
                RequeteStatut.prepare("UPDATE `jeux`SET `StatutJeux_IdStatutJeux`=4 WHERE `CodeJeu`=:CodeDuJeu");
                RequeteStatut.bindValue(":CodeDuJeu",JeuActif);
                RequeteStatut.exec();
                int Retour (QMessageBox::information(this,"Jeu réservé","Vous devez mettre ce jeu de coté","Mis de coté","Pas mis de coté"));
                qDebug()<<Retour;
            }
        }
        ActualiserMembre();
        ViderJeu();
    }
    else
    {
        qDebug()<<"F_Retour::on_Bt_RendreJeu_clicked ||| Aucun jeu n'est sélectionné!!!";
    }
    ui->CBx_Etat->setCurrentIndex(0);
    ui->CBx_Statut->setCurrentIndex(0);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////Clic sur le tableau des jeux à rendre/////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void F_Retour::on_TbV_JeuxEmprunte_clicked(const QModelIndex &index)
{

    unsigned int LigneTableau (0);
    LigneTableau=index.row();
    ui->LE_CodeJeu->setText(this->ModelJeuEmpruntes->index(LigneTableau,0).data().toString());
    on_Bt_CodeJeuOK_clicked();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////Clic sur le tableau des jeux réservés/////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void F_Retour::on_TbV_JeuxReserve_clicked(const QModelIndex &index)
{
    ui->bt_SuppReservation->setEnabled(true);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////Suppression d'un jeu réservé/////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void F_Retour::on_bt_SuppReservation_clicked()
{

    //Récupération de l'id du jeu avec son code
    QSqlQuery RequeteIdJeu;
    RequeteIdJeu.prepare("SELECT `IdJeux` FROM `jeux` WHERE `CodeJeu`=:CodeDuJeu");
    RequeteIdJeu.bindValue(":CodeDuJeu",(this->ModelJeuReserves->index(ui->TbV_JeuxReserve->currentIndex().row(),0).data().toString()));

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

    JeuxReserve();

//Grise le bouton de suppression du tableau des réservations
    ui->bt_SuppReservation->setEnabled(false);
}

//#######################################################################################################
////////////////////////////////////////////Prolongation/////////////////////////////////////////////////
//#######################################################################################################

/////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////Calcule de la date du retour en fonction du type//////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void F_Retour::on_CBx_TypeProlongation_currentIndexChanged(int index)
{
    QDate DateRetour;
    DateRetour=DateRetour.currentDate();


    //Récupération de la date de retour prévu
    QSqlQuery RequeteDateRetour;
    RequeteDateRetour.prepare("SELECT `DateRetourPrevu`"
                                  "FROM `emprunts`,`jeux` "
                                  "WHERE `CodeJeu`=:CodeDuJeu AND `Jeux_IdJeux`=`IdJeux` AND `DateRetour` IS NULL");
    RequeteDateRetour.bindValue(":CodeDuJeu",this->JeuActif);
    RequeteDateRetour.exec();
    RequeteDateRetour.next();

    if( DateRetour<RequeteDateRetour.value(0).toDate())
    {
        DateRetour=RequeteDateRetour.value(0).toDate();
    }



QString TypeEmpunt;
    //Récupération du text du ComboBox
    TypeEmpunt=ui->CBx_TypeProlongation->currentText().left(ui->CBx_TypeProlongation->currentText().indexOf("(")-1);


//On prépare et exécute la requête qui permet de récupérer la durée du type d'emprunt
    QSqlQuery Requete;

    Requete.prepare("SELECT DureeEmprunt FROM `typeemprunt`WHERE TypeEmprunt=:TypeEmprunt");
    Requete.bindValue(":TypeEmprunt",TypeEmpunt);
    Requete.exec();
    Requete.next();

    DateRetour=DateRetour.addDays((Requete.value(0).toInt()));


    ui->DtE_Prolonger->setDate(DateRetour);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////Paye le prolongement////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void F_Retour::on_Bt_Prolonger_clicked()
{
    //Recherche le nombre de crédit à payer
    QSqlQuery RequeteCredit;
    RequeteCredit.prepare("SELECT `PrixLoc`FROM`jeux` WHERE`CodeJeu`=:CodeDuJeu");
    RequeteCredit.bindValue(":CodeDuJeu",this->JeuActif);
    RequeteCredit.exec();
    RequeteCredit.next();

    int nResultat (0);
    pPaiement->AfficherPaiement(RequeteCredit.value(0).toInt(),this->MembreActif);
    nResultat = pPaiement->exec();
    if (nResultat==1)
    {
        EmprunteJeux();
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////Prolonge le jeu/////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/** permet de prolonger le jeu qui est affiché
 *  @pre    Connexion à la base de données
 *  @see    JeuActif
 */
void F_Retour:: EmprunteJeux()
{
    //Récupération du nombre de prolongation
    QSqlQuery RequeteNbProlongation;
    RequeteNbProlongation.prepare("SELECT `NbrPrologation`, `IdEmprunts`"
                                  "FROM `emprunts`,`jeux` "
                                  "WHERE `CodeJeu`=:CodeDuJeu AND `Jeux_IdJeux`=`IdJeux` AND `DateRetour` IS NULL");
    RequeteNbProlongation.bindValue(":CodeDuJeu",this->JeuActif);
    RequeteNbProlongation.exec();
    RequeteNbProlongation.next();

    QSqlQuery RequeteProlongation;
    RequeteProlongation.prepare("UPDATE `emprunts` SET `NbrPrologation`=:NbProlongations,`DateRetourPrevu`=:DateRetour WHERE `IdEmprunts`=:IdEmprunt");
    RequeteProlongation.bindValue(":NbProlongations",RequeteNbProlongation.value(0).toInt()+1);
    RequeteProlongation.bindValue(":DateRetour",ui->DtE_Prolonger->date());
    RequeteProlongation.bindValue(":IdEmprunt",RequeteNbProlongation.value(1).toInt());
    RequeteProlongation.exec();

    ViderJeu();
    ActualiserMembre();
}
