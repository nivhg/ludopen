/** @file         F_Retour.cpp
 *  @brief        Classe graphique permettant le retours à la ludothèque des jeux
 *
 *  @author       Botherel Philippe
 *  @author       STS IRIS, Lycée Nicolas APPERT, ORVAULT (FRANCE)
 *  @since        21/01/2012
 *  @version      0.1
 *  @date         30/01/2013 William
 *  @todo         Gérer les amandes en cas de retard
 *
 *  Cette classe permet de rendre les jeux qu'un membre à emprunté,
 *  on peut aussi supprimer une reservation de jeu entre autre
 *
 */
#include <QtSql>
#include "f_retour.h"
#include "ui_f_retour.h"
#include "fonctions_globale.h"
#include "f_preferences.h"

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
    this->MembreActif = "";
    // pas de jeu choisi
    this->JeuActif = "";
    // Amende à 0€
    this->Amende = 0.0 ;
    ui->Lb_AmendeAPayer->setStyleSheet("QLabel {color:red;}");

    //Met une date minimum pour le DateEdit de la prolongation (la date du jour)
    QDate DateDeRetourToleree;
    DateDeRetourToleree=DateDeRetourToleree.currentDate();
    ui->DtE_Prolonger->setMinimumDate(DateDeRetourToleree);

    //Grise les boutons de modification des remarques
    ui->Bt_ValiderRemarqueMembre->setEnabled(false);
    ui->Bt_AnnulerRemarqueMembre->setEnabled(false);
    ui->Bt_ValiderRemarqueJeu->setEnabled(false);
    ui->Bt_AnnulerRemarqueJeu->setEnabled(false);

    QSqlQuery RequeteEtatJeu;
    QString EtatJeu;

    //Remplissage du ComboBox de l'état du jeu à partir de la table etatjeu
    if (!RequeteEtatJeu.exec("SELECT Etat FROM etatsjeu"))
    {
        qDebug()<<"F_Retour::F_Retour || RequeteEtatJeu "<<RequeteEtatJeu.lastQuery();
    }

    //Tant qu'il y a des types états dans la table etatjeu,
    while(RequeteEtatJeu.next())
    {
        //on entre un nouveau Item au ComboBox avec le nom de l'état
        EtatJeu=(RequeteEtatJeu.value(0).toString());
        ui->CBx_Etat->addItem(EtatJeu);
    }

    QSqlQuery RequeteStatutJeu;
    QString StatutJeu;

    //Remplissage du ComboBox du statut du jeu à partir de la table statutsJeu
    if (!RequeteStatutJeu.exec("SELECT StatutJeu FROM statutjeux"))
    {
        qDebug()<<"F_Retour::F_Retour || RequeteStatutJeu "<<RequeteStatutJeu.lastQuery();
    }
    //Tant qu'il y a des types status dans la table statutsjeu,
    while(RequeteStatutJeu.next())
    {
        //on entre un nouveau Item au ComboBox avec le nom du statut
        StatutJeu=(RequeteStatutJeu.value(0).toString());
        ui->CBx_Statut->addItem(StatutJeu);
    }

    //Remplissage du ComboBox du type de prolongation du jeu à partir de la table typeemprunt
    QSqlQuery RequeteTypeEmprunt;
    QString   TypeEmprunt;

    if (!RequeteTypeEmprunt.exec("SELECT TypeEmprunt,DureeEmprunt FROM typeemprunt"))
    {
       qDebug()<<"F_Retour::F_Retour || RequeteTypeEmprunt "<<RequeteTypeEmprunt.lastQuery();
    }
   //Tant qu'il y a des types d'emprunt dans la table TupesEmprunt,
   while(RequeteTypeEmprunt.next())
   {
       //on entre un nouveau Item au ComboBox avec le nom du type d'emprunt
       TypeEmprunt=(RequeteTypeEmprunt.value(0).toString())+" ("+(RequeteTypeEmprunt.value(1).toString())+"jours)";
       ui->CBx_TypeProlongation->addItem(TypeEmprunt);
   }

   //Création d'un modèle pour le TableView des membres
   this->ModelMembre = new QStandardItemModel() ;
   //Associe le modèl au TableView
   ui->TbV_Recherche->setModel(this->ModelMembre);
   //Met le TableView en lecture seule
   ui->TbV_Recherche->setEditTriggers(0);
   // Autorise le tri pour ce tableau
   ui->TbV_Recherche->setSortingEnabled(true);
   //Initialise le tableau avec tous les membres
   this->on_LE_RechercheMembre_textChanged("");

   //Création d'un modèle pour le TableView des jeux empruntés
   this->ModelJeuEmpruntes = new QStandardItemModel() ;
   //Associe le modèl au TableView
   ui->TbV_JeuxEmprunte->setModel(this->ModelJeuEmpruntes);
   //Met le TableView en lecture seule
   ui->TbV_JeuxEmprunte->setEditTriggers(0);
   // Autorise le tri pour ce tableau
   ui->TbV_JeuxEmprunte->setSortingEnabled(true);
   //Initialise les colonnes du TableView des nouveaux emprunts
   this->ModelJeuEmpruntes->setColumnCount(4);
   this->ModelJeuEmpruntes->setHorizontalHeaderItem(0, new QStandardItem("Code"));
   this->ModelJeuEmpruntes->setHorizontalHeaderItem(1, new QStandardItem("Nom du jeu"));
   this->ModelJeuEmpruntes->setHorizontalHeaderItem(2, new QStandardItem("Date retour"));
   this->ModelJeuEmpruntes->setHorizontalHeaderItem(3, new QStandardItem("Date emprunt"));
   ui->TbV_JeuxEmprunte->setColumnWidth(0,40);
   ui->TbV_JeuxEmprunte->setColumnWidth(1,125);

   //Création d'un modèle pour le TableView des jeux réservés
   this->ModelJeuReserves = new QStandardItemModel() ;
    //Associe le modèl au TableView
   ui->TbV_JeuxReserve->setModel(this->ModelJeuReserves);
   //Met le TableView en lecture seule
   ui->TbV_JeuxReserve->setEditTriggers(0);
   // Autorise le tri pour ce tableau
   ui->TbV_JeuxReserve->setSortingEnabled(true);

   //Initialise les colonnes du TableView des jeux réservés
   this->ModelJeuReserves->setColumnCount(5);
   this->ModelJeuReserves->setHorizontalHeaderItem(0, new QStandardItem("Code"));
   this->ModelJeuReserves->setHorizontalHeaderItem(1, new QStandardItem("Nom du jeu"));
   this->ModelJeuReserves->setHorizontalHeaderItem(2, new QStandardItem("Date réservation"));
   this->ModelJeuReserves->setHorizontalHeaderItem(3, new QStandardItem("Date emprunt"));
   this->ModelJeuReserves->setHorizontalHeaderItem(4, new QStandardItem("Date retour"));
   this->ModelJeuReserves->setHorizontalHeaderItem(5, new QStandardItem("lieu"));

   ui->TbV_JeuxReserve->setColumnWidth(0,40);
   ui->TbV_JeuxReserve->setColumnWidth(1,125);

   ui->DtE_Prolonger->setDisplayFormat("dd-MM-yyyy");

   ui->LE_Caution->setVisible(false);
   ui->Lb_Caution->setVisible(false);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////Destructeur//////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
F_Retour::~F_Retour()
{
    delete ui;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////Prolonge le jeu/////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/** permet de prolonger le jeu qui est affiché
 *  @pre    Connexion à la base de données
 *  @see    JeuActif
 */
void F_Retour:: EmprunterJeux()
{
    //Récupération du nombre de prolongation
    QSqlQuery RequeteNbProlongation;
    RequeteNbProlongation.prepare("SELECT NbrPrologation, IdEmprunts"
                                  " FROM emprunts,jeux"
                                  " WHERE CodeJeu=:CodeDuJeu AND Jeux_IdJeux=IdJeux AND DateRetour IS NULL");
    RequeteNbProlongation.bindValue(":CodeDuJeu",this->JeuActif);

    if (!RequeteNbProlongation.exec())
    {
       qDebug()<<"F_Retour::EmprunterJeux || RequeteNbProlongation "<<RequeteNbProlongation.lastQuery();
    }
    RequeteNbProlongation.next();

    QSqlQuery RequeteProlongation;
    RequeteProlongation.prepare("UPDATE emprunts SET NbrPrologation=:NbProlongations,DateRetourPrevu=:DateRetour"
                                " WHERE IdEmprunts=:IdEmprunt");
    RequeteProlongation.bindValue(":NbProlongations",RequeteNbProlongation.value(0).toInt()+1);
    RequeteProlongation.bindValue(":DateRetour",ui->DtE_Prolonger->date());
    RequeteProlongation.bindValue(":IdEmprunt",RequeteNbProlongation.value(1).toInt());

    if (!RequeteProlongation.exec())
    {
       qDebug()<<"F_Retour::EmprunterJeux || RequeteProlongation "<<RequeteProlongation.lastQuery();
    }

    ViderJeu();
    ActualiserMembre();
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
    AfficherMembre();
}

/** Actualise la liste des membres ayant des emprunts
 *  Donne la liste de tous les membres ayant un retour à faire.
 */
void F_Retour::ActualiserListeEmprunteurs()
{
    this->on_LE_RechercheMembre_textChanged("");
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
//////////////////////////////////////////Afficher le membre dans le volet central///////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/** Affiche les information du membre passé en paramètre
 *  @pre    Connexion à la base de données
 *  @post   Description des postconditions nécessaires à la méthode
 *  @param  CodeMembre est le code du membre à passer en paramètre pour afficher ces informations
 */
void F_Retour::AfficherMembre()
{
    ui->LE_CodeMembre->setText(this->MembreActif);
    ui->LE_NbreJeuxRendre->setText("0");
    this->Amende = 0 ;

    QSqlQuery Requete;

    //Prépare la requête
    Requete.prepare("SELECT Nom,Prenom,NbreRetard,Ecarte,Remarque,NbreJeuxAutorises,IdMembre FROM membres WHERE CodeMembre=:CodeDuMembre");
    Requete.bindValue(":CodeDuMembre",this->MembreActif);

    if (!Requete.exec())
    {
        qDebug()<<"F_Retour::AfficherMembre || requete info membre "<<Requete.lastQuery();
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
    RequeteMembreAssocier.prepare("SELECT CodeMembre FROM membresassocies,membres WHERE "
                                  "Membres_IdMembre=:IdMembre AND Membres_IdCollectivite=IdMembre");
    RequeteMembreAssocier.bindValue(":IdMembre",ObtenirValeurParNom(Requete,"IdMembre").toString());

    if (!RequeteMembreAssocier.exec())
    {
       qDebug()<<"F_Retour::AfficherMembre || RequeteMembreAssocier "<<RequeteMembreAssocier.lastQuery();
    }
    RequeteMembreAssocier.next();
    //s'i y en a un,
    if(RequeteMembreAssocier.size() > 0)
    {
        //On Affiche l'état de la cotisation du membre associé au membre actif
        AfficherEtatCotisation(ObtenirValeurParNom(RequeteMembreAssocier,"CodeMembre").toString());
    }
    else
    {
        //On affiche l'état de la cotisation du membre en cours de consultation
        AfficherEtatCotisation(this->MembreActif);
    }

    //Affiche les jeux empruntés de membre
    AfficherJeuxEnEmprunt();
    //Affiche les jeux réservés de membre
    AfficherJeuxEnReservation();
    //Affiche le nombre de crédits restants
    CalculerCreditsRestants();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Afficher le jeu dans le volet de droite////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/** Affiche les informations du jeu passé en paramètre
 *  @pre    connexion à la base de données
 *  @param  CodeJeu est le code du jeu dont les informations sont à afficher
 *  @see    JeuActif
 */
void F_Retour::AfficherDetailDuJeu()
{
   //dégrise le bouton rendre
   ui->Bt_RendreJeu->setEnabled(true);

   //Recherche d'une éventuelle réservation du jeux
   QSqlQuery RequeteJeuReserve;
   RequeteJeuReserve.prepare("SELECT idReservation FROM reservation,jeux "
                             "WHERE CodeJeu=:JeuActif AND Jeux_IdJeux=IdJeux AND JeuEmprunte=1");
   RequeteJeuReserve.bindValue(":JeuActif",this->JeuActif);

   if (!RequeteJeuReserve.exec())
   {
      qDebug()<<"F_Retour::AfficherDetailDuJeu => RequeteJeuReserve "<<RequeteJeuReserve.lastQuery();
   }
   //qDebug()<<"F_Retour::AfficherDetailDuJeu => RequeteJeuReserve "<<RequeteJeuReserve.executedQuery();
   RequeteJeuReserve.next();

   if(RequeteJeuReserve.size()>0)
   {
      //Si oui, mettre le bouton de prolongation en grisé
      ui->Bt_Prolonger->setEnabled(false);
   }
   else
   {
      //Sinon, le rendre cliquable
      ui->Bt_Prolonger->setEnabled(true);
   }

   QSqlQuery RequeteDetailJeu;
   //Prépare la requête et entre ses valeurs
   RequeteDetailJeu.prepare("SELECT NomJeu,ContenuJeu,Remarque FROM jeux WHERE CodeJeu=:JeuActif");
   RequeteDetailJeu.bindValue(":JeuActif",this->JeuActif);

   if (!RequeteDetailJeu.exec())
   {
      qDebug()<<"F_Retour::AfficherDetailDuJeu => RequeteDetailJeu "<<RequeteDetailJeu.lastQuery();
   }

   RequeteDetailJeu.next();

   //Récupère le nom du jeu et l'affiche
   QString TextTemporaire = (RequeteDetailJeu.value(0).toString());
   ui->LE_NomJeuARemplir->setText( TextTemporaire ) ;

   //Récupère le contenu de la boîte et l'affiche
   TextTemporaire=(RequeteDetailJeu.value(1).toString()) ;
   ui->TxE_ContenuBoite->setText(TextTemporaire );

   //Récupère la remarque et l'affiche
   TextTemporaire = (RequeteDetailJeu.value(2).toString());
   ui->TxE_RemarquesJeu->setText(TextTemporaire);

   //Grise les boutons de modification de la remarque du jeu
   ui->Bt_ValiderRemarqueJeu->setEnabled(false);
   ui->Bt_AnnulerRemarqueJeu->setEnabled(false);

   //Calcule la date du retour
   on_CBx_TypeProlongation_currentIndexChanged(0);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////Afficher l'état de la cotisation d'un membre/////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/** Description détaillée de la méthode
 *  @pre    Connexion avec la base de donnée
 *  @param  Code du membre
 *  @return CotisationAJours
 */
void F_Retour::AfficherEtatCotisation(QString CodeMembre)
{
    //Connaitre la date d'expiration de la cotisation annuelle la plus récente
    QSqlQuery RequeteCotisation ;
    RequeteCotisation.prepare("SELECT DateExpiration FROM abonnements, membres"
                             " WHERE Prestations_IdPrestation IS NOT NULL AND Membres_IdMembre=IdMembre"
                             " AND CodeMembre=:CodeDuMembre ORDER BY DateExpiration DESC ");
    RequeteCotisation.bindValue(":CodeDuMembre",CodeMembre );

    if(!RequeteCotisation.exec())
    {
        qDebug()<<"Retour affiche Cotisation  requete état cotisation  "<<RequeteCotisation.lastQuery();
    }

    RequeteCotisation.next();

    QDate DateDeRetourToleree;
    DateDeRetourToleree= DateDeRetourToleree.currentDate();

    int NombreJoursRestant ;
    //Nombre de jours qu'il reste à la cotisation
    NombreJoursRestant=DateDeRetourToleree.daysTo(RequeteCotisation.value(0).toDate());

    //si le nombres de jours restant est négatif la cotisation est expirée
    if(NombreJoursRestant<0)
    {
        QString Cotisation;
        //Si le code membre est différent du membre actif, alors
        if(MembreActif!=CodeMembre)
        {
            //On précise le code du membre qui pocède la cotisation
            Cotisation="<b>Cotisation expirée (membre "+ CodeMembre +") depuis le</b>";
        }
        //Sinon,
        else
        {
            //On affiche juste l'état de la cotisation
            Cotisation="<b>Cotisation expirée depuis le </b>";
        }
        ui->Lb_Cotisation->setText(Cotisation);
        ui->Lb_Cotisation->setStyleSheet("QLabel {color:red;}");
        ui->Lb_CotisationDate->setText("<b>"+RequeteCotisation.value(0).toDate().toString("dd-MM-yyyy")+"</b>");
        ui->Lb_CotisationDate->setStyleSheet(" QLabel{color:red;}" );
    }
    else
    {
        QString Cotisation;
        //Si le code membre est différent du membre actif, alors
        if(MembreActif!=CodeMembre)
        {
            //On précise le code du membre qui pocède la cotisation
            Cotisation="<b>Cotisation à jours (membre "+ CodeMembre +")</b>";
        }
        //Sinon,
        else
        {
            //On affiche juste l'état de la cotisation
            Cotisation="<b>Cotisation à jours</b>";

        }
        QString JourRestant ;
        JourRestant.setNum(NombreJoursRestant);
        ui->Lb_Cotisation->setText(Cotisation);
        ui->Lb_CotisationDate->setText("<b>("+ JourRestant +" jours restant)</b>");
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
//////////////////////////////////////////Affichage des jeux en emprunt//////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/** affiche dans le tableau les jeux en cours d'emprunt du membre en cours de consultation
 *  @pre    connexion à la base de données, un membre est en cours de consultation
 *  @post   Description des postconditions nécessaires à la méthode
 *  @see    MembreActif
 */
void F_Retour::AfficherJeuxEnEmprunt()
{
   //qDebug()<< "F_Retour::AfficherJeuxEnEmprunt======================= DEBUT ====================  ";
   QSqlQuery RequeteIdMembre;
   unsigned int IdDuMembre (0);

   //Prépare le requête pour récupérer l'id du membre
   RequeteIdMembre.prepare("SELECT IdMembre FROM membres WHERE CodeMembre=:CodeDuMembre");
   RequeteIdMembre.bindValue(":CodeDuMembre",this->MembreActif);

   if (!RequeteIdMembre.exec())
   {
      qDebug()<< "F_Retour::AfficherJeuxEnEmprunt =>  RequeteIdMembre  "<< RequeteIdMembre.lastQuery();
   }
   // si il y a un membre correspondant à cette requête
   if ( RequeteIdMembre.next() )
   {
      //Récupère l'id dans la base de données
      qDebug()<< "F_Retour::AfficherJeuxEnEmprunt =>  IdDuMembre  "<< RequeteIdMembre.value(0).toInt();
      IdDuMembre=RequeteIdMembre.value(0).toInt();
      QSqlQuery RequeteJeuEmprunte;

      RequeteJeuEmprunte.prepare("SELECT DateEmprunt,DateRetourPrevu,NomJeu,CodeJeu"
                               " FROM emprunts,jeux"
                               " WHERE DateRetour IS NULL AND Membres_IdMembre=:IdDuMembre AND IdJeux=Jeux_IdJeux" );
      RequeteJeuEmprunte.bindValue(":IdDuMembre",IdDuMembre);
      if(!RequeteJeuEmprunte.exec())
      {
         qDebug()<<"F_Retour::AfficherJeuxEnEmprunt() =>  Requête jeux en emprunt "<<RequeteJeuEmprunte.lastQuery();
      }

      qDebug()<<"F_Retour::AfficherJeuxEnEmprunt() =>  Requête jeux en emprunt "<<RequeteJeuEmprunte.lastQuery();
      //On vide le modèle
      this->ModelJeuEmpruntes->clear();
      //Initialise les colonnes du TableView des jeux empruntés
      this->ModelJeuEmpruntes->setColumnCount(4);
      this->ModelJeuEmpruntes->setHorizontalHeaderItem(0, new QStandardItem("Code"));
      this->ModelJeuEmpruntes->setHorizontalHeaderItem(1, new QStandardItem("Nom du jeu"));
      this->ModelJeuEmpruntes->setHorizontalHeaderItem(2, new QStandardItem("Date retour"));
      this->ModelJeuEmpruntes->setHorizontalHeaderItem(3, new QStandardItem("Date emprunt"));
      ui->TbV_JeuxEmprunte->setColumnWidth(0,40);
      ui->TbV_JeuxEmprunte->setColumnWidth(1,125);
      ui->TbV_JeuxEmprunte->verticalHeader()->setVisible(false);

      unsigned int NumeroLigne = 0;
      QDate DateDeRetourPrevue ;
      unsigned int NbDeSemainesDeRetard = 0;

      //Tant qu'il y a des jeux en emprunt
      while(RequeteJeuEmprunte.next())
      {
         //on ajoute une nouvelle ligne du table view
         this->ModelJeuEmpruntes->setItem(NumeroLigne, 0, new QStandardItem(RequeteJeuEmprunte.value(3).toString()));
         this->ModelJeuEmpruntes->setItem(NumeroLigne, 1, new QStandardItem(RequeteJeuEmprunte.value(2).toString()));
         this->ModelJeuEmpruntes->setItem(NumeroLigne, 2, new QStandardItem(RequeteJeuEmprunte.value(1).toDate().toString("dd-MM-yyyy") ));
         this->ModelJeuEmpruntes->setItem(NumeroLigne, 3, new QStandardItem(RequeteJeuEmprunte.value(0).toDate().toString("dd-MM-yyyy") ));

         // Calculer la date de retour possible avec la tolérance du nombre de jours
         DateDeRetourPrevue = RequeteJeuEmprunte.value(1).toDate();

         //qDebug()<<"F_Retour::AfficherJeuxEnEmprunt() => Date de retour Toleree=" << DateDeRetourToleree << "jours toleres=" << RequeteNbJoursRetardToleres.value(0).toInt() ;
         //qDebug()<<"F_Retour::AfficherJeuxEnEmprunt() => Nb de jours =" << DateDeRetourPrevue.daysTo( DateDeRetourToleree );

         // Ligne en rouge si retard, sinon en vert
         if ( DateDeRetourPrevue.daysTo( QDate::currentDate() ) < F_Preferences::ObtenirValeur("JourRetard").toInt() )
         {  // Pas de retard
            this->ModelJeuEmpruntes->setData(ModelJeuEmpruntes->index(NumeroLigne,0),QColor(Qt::green), Qt::BackgroundColorRole);
            this->ModelJeuEmpruntes->setData(ModelJeuEmpruntes->index(NumeroLigne,1),QColor(Qt::green), Qt::BackgroundColorRole);
            this->ModelJeuEmpruntes->setData(ModelJeuEmpruntes->index(NumeroLigne,2),QColor(Qt::green), Qt::BackgroundColorRole);
            this->ModelJeuEmpruntes->setData(ModelJeuEmpruntes->index(NumeroLigne,3),QColor(Qt::green), Qt::BackgroundColorRole);
         }
         else
         {  // En retard
            this->ModelJeuEmpruntes->setData(ModelJeuEmpruntes->index(NumeroLigne,0),QColor(Qt::red), Qt::BackgroundColorRole);
            this->ModelJeuEmpruntes->setData(ModelJeuEmpruntes->index(NumeroLigne,1),QColor(Qt::red), Qt::BackgroundColorRole);
            this->ModelJeuEmpruntes->setData(ModelJeuEmpruntes->index(NumeroLigne,2),QColor(Qt::red), Qt::BackgroundColorRole);
            this->ModelJeuEmpruntes->setData(ModelJeuEmpruntes->index(NumeroLigne,3),QColor(Qt::red), Qt::BackgroundColorRole);

            // Calculer l'amende à payer
            int NbJours=DateDeRetourPrevue.daysTo( QDate::currentDate() );
            int JoursToleres=F_Preferences::ObtenirValeur("JourRetard").toInt();
            if(JoursToleres==0)
            {
                NbDeSemainesDeRetard=0;
            }
            else
            {
                NbDeSemainesDeRetard = NbJours / JoursToleres ;
            }
            this->Amende=this->Amende+F_Preferences::ObtenirValeur("PrixAmende").toFloat() * NbDeSemainesDeRetard ;
            //qDebug()<<"F_Retour::AfficherJeuxEnEmprunt() => Amende=" << this->Amende << " NbDeSemainesDeRetard=" << NbDeSemainesDeRetard ;
         }
         NumeroLigne++;
         // Affichage du nombre de jeux à rendre
         QString NbJeux ;
         NbJeux.setNum(NumeroLigne);
         ui->LE_NbreJeuxRendre->setText(NbJeux);
      }
      // Afficher l'amende à payer
      if (this->Amende > 0)
      {
         QString Amende ;
         Amende.setNum(this->Amende);
         ui->Lb_AmendeAPayer->setText("Amende de "+Amende+"€ ");
      }
      else
      {
         ui->Lb_AmendeAPayer->clear();
      }
   }
   //qDebug()<< "F_Retour::AfficherJeuxEnEmprunt======================= FIN ====================  ";
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Affichage des jeux réservés////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/** Affiche les jeux réservés dans le tableau
 *  @pre    Connextion à la base de données, un membre est en cours de consultaion
 *  @see    MembreActif
 */
void F_Retour::AfficherJeuxEnReservation()
{

    QSqlQuery RequeteIdMembre;
    unsigned int IdDuMembre (0);

   //Prépare le reqête pour récupérer l'id du membre
    RequeteIdMembre.prepare("SELECT IdMembre FROM membres WHERE CodeMembre=:CodeDuMembre");
    RequeteIdMembre.bindValue(":CodeDuMembre",MembreActif);

   //Execute la requête
    if (!RequeteIdMembre.exec())
    {
        qDebug()<<"F_Retour::JeuxReserve =>  Requete IdDuMembre "<<RequeteIdMembre.lastQuery();
    }

    RequeteIdMembre.next();

    //Récupère l'id dans la base de données
    IdDuMembre=RequeteIdMembre.value(0).toInt();

    QSqlQuery RequeteJeuReserve;
    unsigned int NumeroLigne =0;
    RequeteJeuReserve.prepare("SELECT DateReservation,DatePrevuEmprunt,DatePrevuRetour,NomLieux,NomJeu,CodeJeu"
                              " FROM reservation,lieux ,jeux"
                              " WHERE JeuEmprunte=1 AND Membres_IdMembre=:IdMembre AND IdLieux=Lieux_IdLieuxReservation AND IdJeux=Jeux_IdJeux");

    RequeteJeuReserve.bindValue(":IdMembre",IdDuMembre);

    if (!RequeteJeuReserve.exec())
    {
        qDebug()<<"F_Retour::JeuxReserve =>  Requête Jeux Reservés "<<RequeteJeuReserve.lastQuery();
    }

    //On vide le modèle
    this->ModelJeuReserves->clear();
    //Initialise les colonnes du TableView des nouveaux emprunts
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
        this->ModelJeuReserves->setItem(NumeroLigne, 2, new QStandardItem(RequeteJeuReserve.value(0).toDate().toString("dd-MM-yyyy") ));
        this->ModelJeuReserves->setItem(NumeroLigne, 3, new QStandardItem(RequeteJeuReserve.value(1).toDate().toString("dd-MM-yyyy") ));
        this->ModelJeuReserves->setItem(NumeroLigne, 4, new QStandardItem(RequeteJeuReserve.value(2).toDate().toString("dd-MM-yyyy") ));
        this->ModelJeuReserves->setItem(NumeroLigne, 5, new QStandardItem(RequeteJeuReserve.value(3).toString() ));

        //Savoir si le jeu est disponible ou non
        QSqlQuery RequeteJeu;
        RequeteJeu.prepare("SELECT IdEmprunts FROM emprunts,jeux WHERE CodeJeu=:CodeDuJeu AND Jeux_IdJeux=IdJeux AND DateRetour IS NULL");
        RequeteJeu.bindValue(":CodeDuJeu",RequeteJeuReserve.value(5).toString());

         if (!RequeteJeu.exec())
         {
             qDebug()<<"F_Retour::JeuxReserve =>  RequeteJeu "<<RequeteJeu.lastQuery();
         }

        if(RequeteJeu.size()==0)
        {
            this->ModelJeuReserves->setData(ModelJeuReserves->index(NumeroLigne,2),QColor(Qt::green), Qt::BackgroundColorRole);
        }
        else
        {
            this->ModelJeuReserves->setData(ModelJeuReserves->index(NumeroLigne,2),QColor(Qt::red), Qt::BackgroundColorRole);
        }

        NumeroLigne++;
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
    RequeteCartes.prepare("SELECT CreditRestant"
                          " FROM abonnements,cartesprepayees,membres"
                          " WHERE abonnements.Membres_IdMembre=IdMembre"
                                   " AND CodeMembre=:CodeDuMembre"
                                   " AND abonnements.CartesPrepayees_IdCarte IS NOT NULL"
                                   " AND abonnements.CreditRestant>0"
                                   " AND IdCarte=CartesPrepayees_IdCarte");

    RequeteCartes.bindValue(":CodeDuMembre",this->MembreActif);

    if (!RequeteCartes.exec())
    {
       qDebug()<<"F_Retour::AfficherMembre || RequeteCartes "<<RequeteCartes.lastQuery();
    }

    while(RequeteCartes.next())
    {
        CreditRestant=CreditRestant+RequeteCartes.value(0).toInt();
    }
    QString Credit;

    ui->LE_CreditARemplir->setText(Credit.setNum(CreditRestant));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////Vide les informations du jeu/////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/** Vide toutes les informations du jeu actuel qui sont affichés
 */
void F_Retour::ViderJeu()
{
    ui->LE_CodeJeu->setText("");
    ui->LE_NomJeuARemplir->setText("");
    ui->TxE_ContenuBoite->setText("");
    ui->LE_Caution->setText("");
    ui->TxE_RemarquesJeu->setText("");
    this->JeuActif="";

    //grise les boutons rendre et prolonger
    ui->Bt_RendreJeu->setEnabled(false);
    ui->Bt_Prolonger->setEnabled(false);
    //grise les boutons de la remarque du jeu
    ui->Bt_AnnulerRemarqueJeu->setEnabled(false);
    ui->Bt_ValiderRemarqueJeu->setEnabled(false);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Pression sur entrée pour le code jeu///////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void F_Retour::on_LE_CodeJeu_returnPressed()
{
    on_Bt_CodeJeuOK_clicked();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Rechercher un membre avec son nom//////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void F_Retour::on_LE_RechercheMembre_textChanged(const QString &arg1)
{
    unsigned int NumeroLigne (0);
    QString Nom=arg1;
    QSqlQuery RequeteMembre;

    if(Nom.size()>=2)
    {
        Nom="%"+Nom+"%";
        RequeteMembre.prepare("SELECT CodeMembre,Nom,Prenom FROM membres,emprunts"
                              " WHERE emprunts.DateRetour IS NULL AND Membres_IdMembre=IdMembre"
                              " AND membres.Nom LIKE (:Nom) GROUP BY CodeMembre ORDER BY Nom ASC ");

        RequeteMembre.bindValue(":Nom",Nom);

        if (!RequeteMembre.exec())
        {
           qDebug()<<"F_Retour::on_LE_RechercheMembre_textChanged || RequeteMembre "<<RequeteMembre.lastQuery();
        }

        //On vide le modèle
        this->ModelMembre->clear();
        //Indique le nombes de colonnes puis leurs noms
        this->ModelMembre->setColumnCount(3);
        this->ModelMembre->setHorizontalHeaderItem(0, new QStandardItem("Code"));
        this->ModelMembre->setHorizontalHeaderItem(1, new QStandardItem("Nom"));
        this->ModelMembre->setHorizontalHeaderItem(2, new QStandardItem("Prénom"));
        //Impose une taille aux colonnes
        ui->TbV_Recherche->setColumnWidth(0,40);

        //Tant qu'il y a des membres dans la table membres,
        while(RequeteMembre.next())
        {
            //on ajoute une nouvelle ligne du table view
            this->ModelMembre->setItem(NumeroLigne, 0, new QStandardItem(RequeteMembre.value(0).toString() ));
            this->ModelMembre->setItem(NumeroLigne, 1, new QStandardItem(RequeteMembre.value(1).toString() ));
            this->ModelMembre->setItem(NumeroLigne, 2, new QStandardItem(RequeteMembre.value(2).toString() ));
            NumeroLigne++;
        }
    }
    else
    {
        if (!RequeteMembre.exec("SELECT CodeMembre,Nom,Prenom FROM membres,emprunts"
                                " WHERE emprunts.DateRetour IS NULL"
                                " AND Membres_IdMembre=IdMembre GROUP BY CodeMembre"))
        {
           qDebug()<<"F_Retour::on_LE_RechercheMembre_textChanged || RequeteMembre "<<RequeteMembre.lastQuery();
        }

        //On vide le modèle
        this->ModelMembre->clear();
        //Indique le nombres de colonnes puis leurs noms
        this->ModelMembre->setColumnCount(3);
        this->ModelMembre->setHorizontalHeaderItem(0, new QStandardItem("Code"));
        this->ModelMembre->setHorizontalHeaderItem(1, new QStandardItem("Nom"));
        this->ModelMembre->setHorizontalHeaderItem(2, new QStandardItem("Prénom"));
        //impose une taille aux colonnes
        ui->TbV_Recherche->setColumnWidth(0,40);
        ui->TbV_Recherche->verticalHeader()->setVisible(false);

        //Tant qu'il y a des membres dans la table membres,
        while(RequeteMembre.next())
        {
            //on ajoute une nouvelle ligne du table view
            this->ModelMembre->setItem(NumeroLigne, 0, new QStandardItem(RequeteMembre.value(0).toString() ));
            this->ModelMembre->setItem(NumeroLigne, 1, new QStandardItem(RequeteMembre.value(1).toString() ));
            this->ModelMembre->setItem(NumeroLigne, 2, new QStandardItem(RequeteMembre.value(2).toString() ));
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
/////////////////////////////////////////Changement de la remarque du Membre//////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void F_Retour::on_Txe_RemarqueMembre_textChanged()
{
//Affiche les boutons de modification de la remarque du jeu
    ui->Bt_ValiderRemarqueMembre->setEnabled(true);
    ui->Bt_AnnulerRemarqueMembre->setEnabled(true);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////Changement de la remarque du jeu/////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void F_Retour::on_TxE_RemarquesJeu_textChanged()
{
    //Affiche les boutons de modification de la remarque du jeu
    ui->Bt_ValiderRemarqueJeu->setEnabled(true);
    ui->Bt_AnnulerRemarqueJeu->setEnabled(true);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////Clic sur le tableau des jeux à rendre/////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void F_Retour::on_TbV_JeuxEmprunte_clicked(const QModelIndex &index)
{
    unsigned int LigneTableau (0);
    LigneTableau=index.row();
    ui->LE_CodeJeu->setText(this->ModelJeuEmpruntes->index(LigneTableau,0).data().toString());
    // Affiche les infos du jeu sélectionné
    on_Bt_CodeJeuOK_clicked();
    ui->TbV_JeuxEmprunte->selectRow(LigneTableau);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////Clic sur le tableau des jeux réservés/////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void F_Retour::on_TbV_JeuxReserve_clicked(const QModelIndex &index)
{
    ui->TbV_JeuxReserve->selectRow(index.row());
    ui->bt_SuppReservation->setEnabled(true);
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
    RequeteDateRetour.prepare("SELECT DateRetourPrevu"
                                  " FROM emprunts,jeux"
                                  " WHERE CodeJeu=:CodeDuJeu AND Jeux_IdJeux=IdJeux AND DateRetour IS NULL");
    RequeteDateRetour.bindValue(":CodeDuJeu",this->JeuActif);

    if (!RequeteDateRetour.exec())
    {
       qDebug()<<"F_Retour::on_CBx_TypeProlongation_currentIndexChanged =>  RequeteDateRetour "<<RequeteDateRetour.lastQuery();
    }
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

    Requete.prepare("SELECT DureeEmprunt FROM typeemprunt WHERE TypeEmprunt=:TypeEmprunt");
    Requete.bindValue(":TypeEmprunt",TypeEmpunt);

    if (!Requete.exec())
    {
       qDebug()<<"F_Retour::on_CBx_TypeProlongation_currentIndexChanged =>  Requete "<<Requete.lastQuery();
    }
    Requete.next();

    DateRetour=DateRetour.addDays((Requete.value(0).toInt()));

    ui->DtE_Prolonger->setDate(DateRetour);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Pression sur entrée pour le code membre////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void F_Retour::on_LE_CodeMembre_returnPressed()
{
    on_Bt_RechercheOK_clicked();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////Paye le prolongement////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void F_Retour::on_Bt_Prolonger_clicked()
{
    //Recherche le nombre de crédit à payer
    QSqlQuery RequeteCredit;
    RequeteCredit.prepare("SELECT PrixLoc FROM jeux WHERE CodeJeu=:CodeDuJeu");
    RequeteCredit.bindValue(":CodeDuJeu",this->JeuActif);

    if (!RequeteCredit.exec())
    {
       qDebug()<<"F_Retour::on_Bt_Prolonger_clicked =>  RequeteCredit "<<RequeteCredit.lastQuery();
    }
    RequeteCredit.next();

    int nResultat (0);
    // Création de la fenêtre du paiement
    F_Paiement FenetrePaiement;
    FenetrePaiement.AfficherPaiement(RequeteCredit.value(0).toInt(),this->MembreActif);
    nResultat = FenetrePaiement.exec();
    if (nResultat==1)
    {
        EmprunterJeux();
    }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////Clic sur le bouton OK du code membre//////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void F_Retour::on_Bt_RechercheOK_clicked()
{
   // Actualise l'attribut MembreActif avec le code du membre que l'on affiche
   this->MembreActif=ui->LE_CodeMembre->text();
   //Affiche les informations du membre
   this->AfficherMembre();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////Valider Jeu et l'afficher dans le volet de droite////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void F_Retour::on_Bt_CodeJeuOK_clicked()
{
  //Récupère le code du jeu qui a été saisi
    QString CodeJeu= ui->LE_CodeJeu->text();

    //Vérification que le jeu est bien à rendre
    QSqlQuery RequeteStatut ;
    RequeteStatut.prepare("SELECT IdStatutJeux FROM statutjeux,jeux WHERE IdStatutJeux=StatutJeux_IdStatutJeux AND CodeJeu=:CodeJeu");
    RequeteStatut.bindValue(":CodeJeu",CodeJeu);

    if (!RequeteStatut.exec())
    {
       qDebug()<<"F_Retour::on_Bt_CodeJeuOK_clicked =>  RequeteStatut "<<RequeteStatut.lastQuery();
    }

    RequeteStatut.next();

    if(RequeteStatut.value(0)==3)
    {
        this->JeuActif = CodeJeu;
        //Affiche les informations du jeu actuellement choisi dans la liste des jeux empruntés de ce membre
        AfficherDetailDuJeu();

        //Recherche du code du membre qui a emprunté le jeu
        QSqlQuery RequeteCodeMembre ;
        RequeteCodeMembre.prepare("SELECT CodeMembre"
                                  " FROM emprunts,membres,jeux"
                                  " WHERE CodeJeu=:CodeDuJeu AND Jeux_IdJeux=IdJeux AND Membres_IdMembre=IdMembre AND DateRetour IS NULL");
        RequeteCodeMembre.bindValue(":CodeDuJeu",CodeJeu);
        if(!RequeteCodeMembre.exec())
        {
            qDebug()<<"F_Retour::on_Bt_CodeJeuOK_clicked || RequeteCodeMembre"<< RequeteCodeMembre.lastQuery();
        }
        RequeteCodeMembre.next();

        // Actualisation du membre actif
        this->MembreActif = RequeteCodeMembre.value(0).toString() ;
        AfficherMembre();
    }
    else
    {
        if (ui->LE_CodeJeu->text()!="")
        {
            QMessageBox::information(this,"Jeu non emprunté","Ce jeu n'a pas été emprunté.\nVous ne pouvez pas le rendre.","Ok");
        }
    }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Enregistrer la remarque du membre//////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void F_Retour::on_Bt_ValiderRemarqueMembre_clicked()
{
   QSqlQuery RequeteMiseAJourRemarqueMembre;

   //prépare le requête de mise à jour
   RequeteMiseAJourRemarqueMembre.prepare("UPDATE membres SET Remarque =:NouvelRemarque WHERE CodeMembre=:CodeDuMembre");

   //Entre les valeurs de la reqête
   RequeteMiseAJourRemarqueMembre.bindValue(":CodeDuMembre",MembreActif);
   RequeteMiseAJourRemarqueMembre.bindValue(":NouvelRemarque",ui->Txe_RemarqueMembre->toPlainText());

   if (!RequeteMiseAJourRemarqueMembre.exec())
   {
      qDebug()<<"F_Retour::on_Bt_ValiderRemarqueMembre_clicked || RequeteMiseAJourRemarqueMembre "<< RequeteMiseAJourRemarqueMembre.lastQuery();
   }

   //Grise les boutons de modification de le remarque
   ui->Bt_ValiderRemarqueMembre->setEnabled(false);
   ui->Bt_AnnulerRemarqueMembre->setEnabled(false);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Annuler changement de la remarque du membre/////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void F_Retour::on_Bt_AnnulerRemarqueMembre_clicked()
{
   QSqlQuery RequeteAnnulerRemarqueMembre;

   //Prépare la requête
   RequeteAnnulerRemarqueMembre.prepare("SELECT Remarque FROM membres WHERE CodeMembre=:CodeDuMembre");
   RequeteAnnulerRemarqueMembre.bindValue(":CodeDuMembre",MembreActif);

   if (!RequeteAnnulerRemarqueMembre.exec())
   {
      qDebug()<<"F_Retour::on_Bt_AnnulerRemarqueMembre_clicked || RequeteAnnulerRemarqueMembre "<< RequeteAnnulerRemarqueMembre.lastQuery();
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
//////////////////////////////////////////Enregistrer la remarque du jeu/////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void F_Retour::on_Bt_ValiderRemarqueJeu_clicked()
{
   QSqlQuery RequeteMiseAJourRemarqueJeu;

   //prépare le requête de mise à jour
   RequeteMiseAJourRemarqueJeu.prepare("UPDATE jeux SET Remarque =:NouvelRemarque WHERE CodeJeu=:CodeDuJeu");

   //Entre les valeurs de la requête
   RequeteMiseAJourRemarqueJeu.bindValue(":CodeDuJeu",JeuActif);
   RequeteMiseAJourRemarqueJeu.bindValue(":NouvelRemarque",ui->TxE_RemarquesJeu->toPlainText());

   if (!RequeteMiseAJourRemarqueJeu.exec())
   {
      qDebug()<<"F_Retour::on_Bt_ValiderRemarqueJeu_clicked || RequeteMiseAJourRemarqueJeu "<< RequeteMiseAJourRemarqueJeu.lastQuery();
   }

   //Grise les boutons de modification de le remarque
   ui->Bt_ValiderRemarqueJeu->setEnabled(false);
   ui->Bt_AnnulerRemarqueJeu->setEnabled(false);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////Annuler le changement de la remarque du jeu///////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void F_Retour::on_Bt_AnnulerRemarqueJeu_clicked()
{
   QSqlQuery RequeteRemarqueJeu;

   //prépare le reqête d'annulation
   RequeteRemarqueJeu.prepare("SELECT Remarque FROM jeux WHERE CodeJeu=:CodeDuJeu");
   RequeteRemarqueJeu.bindValue(":CodeDuJeu",JeuActif);

   if (!RequeteRemarqueJeu.exec())
   {
      qDebug()<<"F_Retour::on_Bt_AnnulerRemarqueJeu_clicked || RequeteRemarqueJeu "<< RequeteRemarqueJeu.lastQuery();
   }

   RequeteRemarqueJeu.next();

   //Récupère les remarques dans la base de données
   QString TextTemporaire = (RequeteRemarqueJeu.value(0).toString());
   ui->TxE_RemarquesJeu->setText(TextTemporaire);

   //grise les boutons de modification des remarques du jeu
   ui->Bt_ValiderRemarqueJeu->setEnabled(false);
   ui->Bt_AnnulerRemarqueJeu->setEnabled(false);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////Rendre le jeu/////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void F_Retour::on_Bt_RendreJeu_clicked()
{
    if((JeuActif!="")&&(MembreActif!=""))
    {
        QSqlQuery RequeteRetour;
        QDate DateDeRetourToleree;
        DateDeRetourToleree=DateDeRetourToleree.currentDate();

        //Rechercher la date prévu de l'emprunt
        QSqlQuery RequeteDateRetour;
        RequeteDateRetour.prepare("SELECT DateRetourPrevu"
                                  " FROM emprunts,jeux"
                                  " WHERE CodeJeu=:CodeDuJeu AND Jeux_IdJeux=IdJeux AND DateRetour IS NULL");
        RequeteDateRetour.bindValue(":CodeDuJeu",this->JeuActif);

        if (!RequeteDateRetour.exec())
        {
           qDebug()<<"F_Retour::on_Bt_RendreJeu_clicked =>  RequeteDateRetour "<<RequeteDateRetour.lastQuery();
        }
        RequeteDateRetour.next();

        //Comparaison entre la date prévue du retour et la date du jour, on récupère un int
        unsigned int EcartJours (0);
        EcartJours= RequeteDateRetour.value(0).toDate().daysTo(DateDeRetourToleree);

        //
        // TO DO Ajouter un paramètre qui est le nombre de jour de retard toléré avant de compter une amande
        //
        // obtenir le nombre de jours tolérés pour un retard
        DateDeRetourToleree=DateDeRetourToleree.currentDate();
        unsigned int NbJoursRetardToleres(0);
        // Calculer la date de retour avec la tolérance du nombre de jours
        NbJoursRetardToleres = F_Preferences::ObtenirValeur("JourRetard").toInt();
        qDebug()<<"F_Retour::on_Bt_RendreJeu_clicked() => NbJoursRetardToleres=" << NbJoursRetardToleres ;

        //si la date de retour est supérieure à la date du jour, alors le jeux est rendu en retard
        if(EcartJours > NbJoursRetardToleres)
        {
            //Donc on incrémente le nombre de retard du membre
            //On récupère d'abord le nombre de retard du membre
            QSqlQuery RequeteNbRetardsMembre;
            RequeteNbRetardsMembre.prepare("SELECT NbreRetard FROM membres WHERE CodeMembre=:CodeDuMembre");
            RequeteNbRetardsMembre.bindValue(":CodeDuMembre",this->MembreActif);

            if (!RequeteNbRetardsMembre.exec())
            {
               qDebug()<<"F_Retour::on_Bt_RendreJeu_clicked =>  RequeteNbRetardsMembre "<<RequeteNbRetardsMembre.lastQuery();
            }
            RequeteNbRetardsMembre.next();

            //Puis on l'incrémente
            QSqlQuery RequeteRetard;
            RequeteRetard.prepare("UPDATE membres SET NbreRetard=:NbRetard WHERE CodeMembre=:CodeDuMembre");
            RequeteRetard.bindValue(":NbRetard",RequeteNbRetardsMembre.value(0).toInt()+1);
            RequeteRetard.bindValue(":CodeDuMembre",this->MembreActif);

            if (!RequeteRetard.exec())
            {
               qDebug()<<"F_Retour::on_Bt_RendreJeu_clicked =>  RequeteRetard "<<RequeteRetard.lastQuery();
            }
        }

        //Rechercher de l'id du statut du jeu
        QSqlQuery RequeteStatut;
        RequeteStatut.prepare("SELECT IdStatutJeux FROM statutjeux WHERE StatutJeu=:StatutDuJeu");
        RequeteStatut.bindValue(":StatutDuJeu",ui->CBx_Statut->currentText());
        if (!RequeteStatut.exec())
        {
           qDebug()<<"F_Retour::on_Bt_RendreJeu_clicked => RequeteStatut "<<RequeteStatut.lastQuery();
        }
        RequeteStatut.next();

        //Rechercher de l'id de l'état
        QSqlQuery RequeteEtat;
        RequeteEtat.prepare("SELECT IdEtatsJeu FROM etatsjeu WHERE Etat=:EtatDuJeu");
        RequeteEtat.bindValue(":EtatDuJeu",ui->CBx_Etat->currentText());
        if(!RequeteEtat.exec())
        {
            qDebug()<<"F_Retour::on_Bt_RendreJeu_clicked => RequeteEtat "<< RequeteEtat.lastQuery();
        }
        RequeteEtat.next();

        // Mise à jour de la table emprunt
        RequeteRetour.prepare("UPDATE emprunts,jeux "
                              "SET DateRetour=:DateRetour,StatutJeux_IdStatutJeux=:StatutDuJeu,EtatsJeu_IdEtatsJeu=:EtatDuJeu "
                              "WHERE DateRetour IS NULL AND Jeux_IdJeux=IdJeux AND CodeJeu=:CodeDuJeu");

        RequeteRetour.bindValue(":DateRetour",DateDeRetourToleree);
        RequeteRetour.bindValue(":CodeDuJeu",this->JeuActif);
        RequeteRetour.bindValue(":StatutDuJeu",RequeteStatut.value(0));
        RequeteRetour.bindValue(":EtatDuJeu",RequeteEtat.value(0));
        if(!RequeteRetour.exec())
        {
            qDebug()<<"F_Retour::on_Bt_RendreJeu_clicked => RequeteRetour "<< RequeteRetour.lastQuery();
        }

        //if(RequeteStatut.value(0) == 1 ) // "statut 1 = Disponnible"
        {
            //Savoir si le jeu est réservé
            //
            // TO DO rendre le jeu dispo à la réservation
            //
            QSqlQuery RequeteJeu;
            RequeteJeu.prepare("SELECT idReservation FROM reservation,jeux"
                               " WHERE CodeJeu=:CodeDuJeu AND Jeux_IdJeux=IdJeux");
            RequeteJeu.bindValue(":CodeDuJeu",this->JeuActif);

            if(!RequeteJeu.exec())
            {
                qDebug()<<"F_Retour::on_Bt_RendreJeu_clicked => RequeteJeu "<< RequeteJeu.lastQuery();
            }

            RequeteJeu.next();

            if(RequeteJeu.size()>0)
            {
                //Modifier la réservation de ce jeu pour qu'il soit marqué "Disponible" pour l'emprunt
                QSqlQuery RequeteJeuEmprunte;
                RequeteJeuEmprunte.prepare("UPDATE reservation FROM reservation,jeux SET JeuEmprunte=1"
                                           " WHERE CodeJeu=:CodeDuJeu AND Jeux_IdJeux=IdJeux");
                RequeteJeuEmprunte.bindValue(":CodeDuJeu",this->JeuActif);
                if (!RequeteJeuEmprunte.exec())
                {
                    qDebug()<<"F_Retour::on_Bt_Ajouter_clicked || RequeteJeuEmprunte "<<RequeteJeuEmprunte.lastQuery() ;
                }
                //
                // TODO message avec le nom de la personne qui a réservé
                //
                QMessageBox::information(this,"Ce jeu est réservé !","Vous devriez mettre ce jeu de coté.","Mis de coté","Pas mis de coté");

            }
        }
        ActualiserMembre();
        ViderJeu();
    }
    else
    {
        qDebug()<<"F_Retour::on_Bt_RendreJeu_clicked =>  Aucun jeu n'est sélectionné!!!";
    }
    ui->CBx_Etat->setCurrentIndex(0);
    ui->CBx_Statut->setCurrentIndex(0);
}


////////////////////////////////////////////Suppression d'un jeu réservé/////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void F_Retour::on_bt_SuppReservation_clicked()
{

    //Récupération de l'id du jeu avec son code
    QSqlQuery RequeteIdJeu;
    RequeteIdJeu.prepare("SELECT IdJeux FROM jeux WHERE CodeJeu=:CodeDuJeu");
    RequeteIdJeu.bindValue(":CodeDuJeu",(this->ModelJeuReserves->index(ui->TbV_JeuxReserve->currentIndex().row(),0).data().toString()));

    if (!RequeteIdJeu.exec())
    {
        qDebug()<<"F_Retour::on_bt_SuppReservation_clicked || RequeteIdJeu "<<RequeteIdJeu.lastQuery() ;
    }
    RequeteIdJeu.next();

    //Connaitre le statut du jeu
    QSqlQuery RequeteStatut;
    RequeteStatut.prepare("SELECT StatutJeux_IdStatutJeux FROM jeux WHERE IdJeux=:IdDuJeu");
    RequeteStatut.bindValue(":IdDuJeu",RequeteIdJeu.value(0));

    if (!RequeteStatut.exec())
    {
        qDebug()<<"F_Retour::on_bt_SuppReservation_clicked || RequeteStatut "<<RequeteStatut.lastQuery() ;
    }
    RequeteStatut.next();

    // Abandonné par WS car les réservations ne se gèrent que dans la table des réservations
    // le jeu ne change pas de statut. Il est soit dispo à la ludo, soit emprunté par un adhérent !
    /*
    //si le statut du jeux est à réservé, alors,
    if (RequeteStatut.value(0)==4)
    {
        //on met son statut à disponible
        QSqlQuery RequeteMAJStatut;
        RequeteMAJStatut.prepare("UPDATE jeux SET StatutJeux_IdStatutJeux=1 WHERE IdJeux=:IdDuJeu");
        RequeteMAJStatut.bindValue(":IdDuJeu",RequeteIdJeu.value(0));
        if(!RequeteMAJStatut.exec())
        {
            qDebug()<< "F_Retour::on_TbV_JeuxReserve_clicked || RequeteMAJStatut "<<RequeteMAJStatut.lastQuery();
        }
    }
    */

    //suppression de la réservation
    QSqlQuery RequeteSupp;
    RequeteSupp.prepare("DELETE FROM reservation WHERE Jeux_IdJeux=:IdDuJeu");
    RequeteSupp.bindValue(":IdDuJeu",RequeteIdJeu.value(0));

    if (!RequeteSupp.exec())
    {
        qDebug()<<"F_Retour::on_bt_SuppReservation_clicked || RequeteSupp "<<RequeteSupp.lastQuery() ;
    }
    RequeteSupp.next();

    AfficherJeuxEnReservation();

    //Grise le bouton de suppression du tableau des réservations
    ui->bt_SuppReservation->setEnabled(false);
}
