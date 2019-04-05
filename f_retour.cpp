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
#include "f_paiement.h"

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
//    this->Amende = 0.0 ;
    ui->Lb_AmendeAPayer->setStyleSheet("QLabel {color:red;}");

    MaJListeMembres();

    SearchMembre = new SearchBox(this);
    SearchMembre->MAJResults(this->VecteurMembres,3);
    SearchMembre->show();
    ui->Hlay11->addWidget(SearchMembre);

    connect(SearchMembre,SIGNAL(SignalSuggestionFini()),this,SLOT(on_LE_SearchMembre_returnPressed()));

    SearchJeux = new SearchBox(this);
    ActualiserListeJeux();
    SearchJeux->show();
    ui->HLay9->addWidget(SearchJeux);

    connect(SearchJeux,SIGNAL(SignalSuggestionFini()),this,SLOT(on_LE_SearchJeux_returnPressed()));

    //Met une date minimum pour le DateEdit de la prolongation (la date du jour)
    QDate DateDeRetourToleree;
    DateDeRetourToleree=DateDeRetourToleree.currentDate();
    ui->DtE_Prolonger->setMinimumDate(DateDeRetourToleree);

    //Grise les boutons de modification des remarques
    ui->Bt_ValiderRemarqueMembre->setEnabled(false);
    ui->Bt_AnnulerRemarqueMembre->setEnabled(false);
    ui->Bt_ValiderRemarqueJeu->setEnabled(false);
    ui->Bt_AnnulerRemarqueJeu->setEnabled(false);

    //Remplissage du ComboBox du type de prolongation du jeu à partir de la table typeemprunt
    QSqlQuery RequeteTypeEmprunt;
    QString   TypeEmprunt;

    if (!RequeteTypeEmprunt.exec("SELECT TypeEmprunt,DureeEmprunt FROM typeemprunt WHERE Malle=0"))
    {
       qDebug()<<"F_Retour::F_Retour || RequeteTypeEmprunt "<<RequeteTypeEmprunt.lastQuery();
    }
   //Tant qu'il y a des types d'emprunt dans la table TypeEmprunt,
   while(RequeteTypeEmprunt.next())
   {
       //on entre un nouveau Item au ComboBox avec le nom du type d'emprunt
       TypeEmprunt=(RequeteTypeEmprunt.value(0).toString())+" ("+(RequeteTypeEmprunt.value(1).toString())+" jours)";
       ui->CBx_TypeProlongation->addItem(TypeEmprunt);
   }

   //Création d'un modèle pour le TableView des jeux empruntés
   this->ModelJeuEmpruntes = new QStandardItemModel() ;
   //Associe le modèl au TableView
   ui->Tv_JeuxEmprunte->setModel(this->ModelJeuEmpruntes);
   //Met le TableView en lecture seule
   ui->Tv_JeuxEmprunte->setEditTriggers(0);
   // Autorise le tri pour ce tableau
   ui->Tv_JeuxEmprunte->setSortingEnabled(true);
   //Initialise les colonnes du TableView des nouveaux emprunts
   this->ModelJeuEmpruntes->setColumnCount(3);
   this->ModelJeuEmpruntes->setHorizontalHeaderItem(0, new QStandardItem("Nom du jeu/malle"));
   this->ModelJeuEmpruntes->setHorizontalHeaderItem(1, new QStandardItem("Code"));
   this->ModelJeuEmpruntes->setHorizontalHeaderItem(2, new QStandardItem("Date emprunt"));
   this->ModelJeuEmpruntes->setHorizontalHeaderItem(3, new QStandardItem("Date retour"));
   ui->Tv_JeuxEmprunte->setColumnWidth(0,140);
   ui->Tv_JeuxEmprunte->setColumnWidth(1,40);

   //Création d'un modèle pour le TableView des jeux réservés
   this->ModelJeuReserves = new QStandardItemModel() ;
    //Associe le modèl au TableView
   ui->Tv_JeuxReserve->setModel(this->ModelJeuReserves);
   //Met le TableView en lecture seule
   ui->Tv_JeuxReserve->setEditTriggers(0);
   // Autorise le tri pour ce tableau
   ui->Tv_JeuxReserve->setSortingEnabled(true);

   //Initialise les colonnes du TableView des jeux réservés
   this->ModelJeuReserves->setColumnCount(5);
   this->ModelJeuReserves->setHorizontalHeaderItem(0, new QStandardItem("Nom du jeu/malle"));
   this->ModelJeuReserves->setHorizontalHeaderItem(1, new QStandardItem("Code"));
   this->ModelJeuReserves->setHorizontalHeaderItem(2, new QStandardItem("Date réservation"));
   this->ModelJeuReserves->setHorizontalHeaderItem(3, new QStandardItem("Date emprunt"));
   this->ModelJeuReserves->setHorizontalHeaderItem(4, new QStandardItem("Date retour"));
   this->ModelJeuReserves->setHorizontalHeaderItem(5, new QStandardItem("lieu"));

   ui->Tv_JeuxReserve->setColumnWidth(0,140);
   ui->Tv_JeuxReserve->setColumnWidth(1,40);

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

/**   Mise  jour de la liste des membres
 *  @pre    Accès à  la base de données
 *  @retval bool
 *  @return True si tous c'est passé correctement et false si il y a une erreur
 *  @test   Voir la procédure dans le fichier associé.
 */
bool F_Retour::MaJListeMembres()
{
    QSqlQuery query ;
    QVector<QString> Membres ;
    bool bRetourner = true ;

    //Vidange du vecteur
    this->VecteurMembres.clear() ;

    QString requeteSQL;
    requeteSQL="SELECT DISTINCT Nom, Prenom, Email,Ville,IdMembre,CodeMembre FROM emprunts as e "
            "LEFT JOIN membres as m ON e.Membres_IdMembre=m.IdMembre WHERE "
            "e.DateRetour IS NULL ORDER BY Nom ASC";

    //Execute une requète sql qui retourne la liste des membres
    //Si la requète est correcte -> Remplissage du veteur VecteurMembres avec le résultat de la requète et on retourne vrai.
    if(query.exec(requeteSQL))
    {
        while(query.next())
        {
            for(int i=0;i<query.record().count();i++)
            {
                Membres.append(query.value(i).toString()) ;
            }
            this->VecteurMembres.push_back(Membres) ;
            Membres.clear();
        }
    }
    else //Sinon on affiche un message d'erreur et on retourne Faux
    {
        qDebug()<< getLastExecutedQuery(query) << query.lastError();
        bRetourner = false ;
    }
    return bRetourner ;
}

/**   Mise  jour de la liste des membres
 *  @pre    Accès à  la base de données
 *  @retval bool
 *  @return True si tous c'est passé correctement et false si il y a une erreur
 *  @test   Voir la procédure dans le fichier associé.
 */
bool F_Retour::MaJListeJeux()
{
    QSqlQuery query ;
    QVector<QString> Jeux ;
    bool bRetourner = true ;

    //Vidange du vecteur
    this->VecteurJeux.clear() ;

    QSqlQuery Requete;
    //Prépare la requête
    Requete.prepare("SELECT DISTINCT j.NomJeu,j.CodeJeu FROM emprunts as e "
                    "LEFT JOIN jeux as j ON j.IdJeux=Jeux_IdJeux WHERE "
                    "e.DateRetour IS NULL ORDER BY NomJeu");

    //Execute une requète sql qui retourne la liste des jeux
    //Si la requète est correcte -> Remplissage du veteur VecteurJeux
    // avec le résultat de la requête et on retourne vrai.
    if(!Requete.exec())
    {
        qDebug()<< getLastExecutedQuery(Requete)<<Requete.lastError();
        return false;
    }
    while(Requete.next())
    {
        for(int i=0;i<Requete.record().count();i++)
        {
            Jeux.append(Requete.value(i).toString()) ;
        }
        this->VecteurJeux.push_back(Jeux) ;
        Jeux.clear();
    }
    return bRetourner ;
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
    SearchMembre->setCurrentText(this->MembreActif);
    AfficherMembre();
}

/** Actualise la liste des membres ayant des emprunts
 *  Donne la liste de tous les membres ayant un retour à faire.
 */
void F_Retour::ActualiserListeEmprunteurs()
{
    MaJListeMembres();
    SearchMembre->MAJResults(VecteurMembres,3);
}

/** Actualise la liste des membres ayant des emprunts
 *  Donne la liste de tous les membres ayant un retour à faire.
 */
void F_Retour::ActualiserListeJeux()
{
    MaJListeJeux();
    SearchJeux->MAJResults(VecteurJeux,1);
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
    ActualiserListeJeux();
    SearchJeux->setCurrentText(this->JeuActif);
    on_LE_SearchJeux_returnPressed();
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
    if(this->MembreActif=="")
    {
        return;
    }
    SearchMembre->setCurrentText(this->MembreActif);
    ui->LE_NbreJeuxRendre->setText("0");
//    this->Amende = 0 ;

    QSqlQuery Requete;

    //Prépare la requête
    Requete.prepare("SELECT Nom,Prenom,IdMembre,Ecarte,Remarque,Email,NbrJeuxEmpruntables FROM membres "
                    "LEFT JOIN titremembre ON TitreMembre_IdTitreMembre=IdTitreMembre WHERE CodeMembre=:CodeDuMembre");
    Requete.bindValue(":CodeDuMembre",this->MembreActif);

    if (!Requete.exec())
    {
        qDebug()<<"requete info membre "<<getLastExecutedQuery(Requete)<<Requete.lastError();
        return;
    }

    Requete.next();

    IdDuMembre=ObtenirValeurParNom(Requete,"IdMembre").toInt();

    if(ObtenirValeurParNom(Requete,"Email").toString().trimmed()=="" || ObtenirValeurParNom(Requete,"Email").toString().trimmed()=="@")
    {
        QMessageBox::information(this,"Adresse email manquante","Le membre n'a pas de courriel de saisie, merci de la saisir dans l'onglet membre","OK");
    }
    //Récupère le Nom dans la base de données puis l'affiche
    ui->LE_NomARemplir->setText(Requete.value(0).toString());

    //Récupère le Prénom dans la base de données puis l'affiche
    ui->LE_PrenomARemplir->setText(Requete.value(1).toString());

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
    ui->LE_JeuxMaxARemplir->setText(ObtenirValeurParNom(Requete,"NbrJeuxEmpruntables").toString());

    //Affiche l'état de la cotisation
    //Savoir si le membre a un membre associé
    QSqlQuery RequeteMembreAssocier ;
    RequeteMembreAssocier.prepare("SELECT CodeMembre FROM membresassocies,membres WHERE "
                                  "Membres_IdMembre=:IdMembre AND Membres_IdCollectivite=IdMembre");
    RequeteMembreAssocier.bindValue(":IdMembre",IdDuMembre);

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
   //Recherche d'une éventuelle réservation du jeux
   QSqlQuery RequeteJeuReserve;
   RequeteJeuReserve.prepare("SELECT idReservation FROM reservation,jeux "
                             "WHERE CodeJeu=:JeuActif AND Jeux_IdJeux=IdJeux");
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
                             " AND CodeMembre=:CodeDuMembre AND supprimer=0 AND Prestations_IdPrestation!=5 ORDER BY DateExpiration DESC");
    RequeteCotisation.bindValue(":CodeDuMembre",CodeMembre );

    if(!RequeteCotisation.exec())
    {
        qDebug()<<"Retour affiche Cotisation  requete état cotisation  "<<RequeteCotisation.lastQuery();
    }
    qDebug()<<"Retour affiche Cotisation  requete état cotisation  "<<RequeteCotisation.lastQuery();

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
   QString AmendeAPayer,NbreJeuxARendre;
   int NbreJeuxRendre;
   // Paramètre NbreJeuxARendre renvoie la mauvaise valeur, la fonction retourne la bonne valeur
   NbreJeuxRendre=F_Emprunt::AfficherJeuxEnEmprunt(this->ModelJeuEmpruntes,this->MembreActif,true,&NbreJeuxARendre,&AmendeAPayer);
   ui->LE_NbreJeuxRendre->setText(QString::number(NbreJeuxRendre));
   ui->Lb_AmendeAPayer->setText(AmendeAPayer);
   resizeColumnsToContents(this->ModelJeuEmpruntes,ui->Tv_JeuxEmprunte);
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
    F_Emprunt::AfficherJeuxReserve(this->ModelJeuReserves,this->MembreActif,true);
    resizeColumnsToContents(this->ModelJeuReserves,ui->Tv_JeuxReserve);
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
                           " AND IdCarte=CartesPrepayees_IdCarte AND Supprimer=0");

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
    SearchJeux->blockSignals(true);
    SearchJeux->setCurrentText("");
    SearchJeux->blockSignals(false);
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
void F_Retour::on_Tv_JeuxEmprunte_clicked(const QModelIndex &index)
{
    qDebug()<<index;
    // Si c'est un jeu d'une malle, on désélectionne la sélection et on quitte la fonction
    if(index.parent().isValid())
    {
        ui->Tv_JeuxEmprunte->selectionModel()->select(ModelJeuEmpruntes->index(index.row(),0,index.parent()),QItemSelectionModel::Deselect | QItemSelectionModel::Rows);
        return;
    }
    int NbJeux=0;
    int NbMalles=0;
    QString CodePremierJeuChoisi;
    // On parcours le selectionModel pour voir ce qui a été sélectionné
    foreach(QModelIndex selectedIndex, ui->Tv_JeuxEmprunte->selectionModel()->selectedRows())
    {
        // Si une malle a été sélectionnée
        if(this->ModelJeuEmpruntes->index(selectedIndex.row(),0).data(Qt::UserRole+1)!=QVariant::Invalid)
        {
            NbMalles++;
        }
        else
        {
            if(CodePremierJeuChoisi=="")
            {
                CodePremierJeuChoisi=this->ModelJeuEmpruntes->index(selectedIndex.row(),1).data().toString();
            }
            NbJeux++;
        }
    }
    // Si on a cliqué sur une malle
    if(this->ModelJeuEmpruntes->index(index.row(),0).data(Qt::UserRole+1)!=QVariant::Invalid)
    {
        QHash<QString, QVariant> hInfosMalle=this->ModelJeuEmpruntes->item(index.row(),0)->data().value<QHash<QString, QVariant> >();
        QString ListeJeux;
        QVariant DataEnCours;
        int i=0;
        while((DataEnCours=this->ModelJeuEmpruntes->index(index.row(),0).child(i++,0).data())!=QVariant::Invalid)
        {
            ListeJeux+=DataEnCours.toString()+"\n";
        }
        iMalleActive=hInfosMalle["IdMalle"].toInt();
        ui->LE_NomJeuARemplir->setText(hInfosMalle["TypeEmprunt"].toString());
        ui->TxE_RemarquesJeu->setText(hInfosMalle["Remarque"].toString());

        //Grise les boutons de modification des remarques du membre
        ui->Bt_ValiderRemarqueJeu->setEnabled(false);
        ui->Bt_AnnulerRemarqueJeu->setEnabled(false);

        ui->LE_Caution->setText(hInfosMalle["Caution"].toString());
        ui->TxE_ContenuBoite->setText(ListeJeux);

    }
    // Si on a cliqué sur un jeu
    else
    {
        // On bloque les signals de la Searchbox pour éviter qu'elle appelle la fonction AfficherJeuxEnEmprunt qui efface la sélection
        SearchJeux->blockSignals(true);
        if(NbJeux==1)
        {
            SearchJeux->setCurrentText(CodePremierJeuChoisi);
        }
        else
        {
            SearchJeux->setCurrentText(this->ModelJeuEmpruntes->index(index.row(),1).data().toString());
        }
        SearchJeux->blockSignals(false);
        // Affiche les infos du jeu sélectionné
        this->JeuActif = SearchJeux->currentText();
        //Affiche les informations du jeu actuellement choisi dans la liste des jeux empruntés de ce membre
        AfficherDetailDuJeu();
    }
    // Si aucun jeux et malles n'a été sélectionné
    if(NbJeux==0 && NbMalles==0)
    {
        ui->Bt_RendreJeu->setEnabled(false);
    }
    else
    {
        ui->Bt_RendreJeu->setEnabled(true);
    }
    if(NbMalles>0)
    {
        this->AffichageProlongation(false);
    }
    else
    {
        this->AffichageProlongation(true);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////Affiche ou masque les contrôles liés à la prolongation/////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void F_Retour::AffichageProlongation(bool Activer)
{
    ui->Bt_Prolonger->setEnabled(Activer);
    ui->Lb_TypeProlongation->setVisible(Activer);
    ui->CBx_TypeProlongation->setVisible(Activer);
    ui->Lb_Prolonger->setVisible(Activer);
    ui->DtE_Prolonger->setVisible(Activer);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////Clic sur le tableau des jeux réservés/////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void F_Retour::on_Tv_JeuxReserve_clicked(const QModelIndex &index)
{
    //ui->Tv_JeuxReserve->selectRow(index.row());
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

    // Si le membre rends le jeu avant la date de retour prévue, on se base sur la date de retour prévue pour calculer la date de prolongation
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
/////////////////////////////////////////////////Paye le prolongement////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void F_Retour::on_Bt_Prolonger_clicked()
{
    QStringList CodesJeux;
    foreach(QModelIndex index, ui->Tv_JeuxEmprunte->selectionModel()->selectedRows())
    {
        // S'il s'agit d'une malle
        if(this->ModelJeuEmpruntes->index(index.row(),0).data(Qt::UserRole+1)==QVariant::Invalid)
        {
            CodesJeux << ModelJeuEmpruntes->item(index.row(),1)->text();
        }
    }
    // Si aucun jeux n'est trouvé, on quitte la fonction
    if(CodesJeux.count()==0)
    {
        return;
    }

    //Recherche le nombre de crédit à payer
    QSqlQuery RequeteCredit;
    RequeteCredit.prepare("SELECT SUM(PrixLoc) as Total FROM jeux WHERE CodeJeu IN ("+CodesJeux.join(",")+")");

    if (!RequeteCredit.exec())
    {
        qDebug()<<"RequeteCredit "<<getLastExecutedQuery(RequeteCredit)<<RequeteCredit.lastError();
    }
    RequeteCredit.next();

    // Création de la fenêtre du paiement
    // TODO : Regarder les jeux réservés avant de demander le paiement
    F_Paiement FenetrePaiement;
    int MontantPanier=RequeteCredit.value(0).toInt();
    int retour=0;
    FenetrePaiement.AfficherPaiement(QDateTime::currentDateTime(),this->MembreActif,&MontantPanier,&retour);
    if (FenetrePaiement.exec()==1)
    {
        QSqlQuery *RequeteProlongation=new QSqlQuery();
        RequeteProlongation->prepare("UPDATE emprunts as e LEFT JOIN jeux as j ON e.Jeux_IdJeux=j.IdJeux SET e.NbrPrologation=NbrPrologation+1,e.DateRetourPrevu=:DateRetour"
                                    " WHERE j.CodeJeu IN ("+CodesJeux.join(",")+") AND e.DateRetour IS NULL");
        RequeteProlongation->bindValue(":DateRetour",ui->DtE_Prolonger->date());

        if(retour==2)
        {
            QList<QSqlQuery *> *ListeRequetes=new QList<QSqlQuery *>();
            ListeRequetes->append(RequeteProlongation);
            emit(Signal_AjouterAuPanier("Prolongation d'emprunt(s)",this->IdDuMembre,(double) MontantPanier,VENTILATION_PRET,"emprunts",ListeRequetes));
        }
        else
        {
            if (!RequeteProlongation->exec())
            {
               qDebug()<<"RequeteProlongation "<<getLastExecutedQuery(*RequeteProlongation)<<RequeteProlongation->lastError();
            }
        }
        ViderJeu();
        ActualiserMembre();
    }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////Clic sur le bouton OK du code membre//////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void F_Retour::on_LE_SearchMembre_returnPressed()
{
   // Actualise l'attribut MembreActif avec le code du membre que l'on affiche
   this->MembreActif=SearchMembre->currentText();
   //Affiche les informations du membre
   this->AfficherMembre();
    ui->Bt_RendreJeu->setEnabled(false);
    ui->Bt_PayerAmende->setEnabled(true);
    ui->Bt_PayerRetard->setEnabled(true);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////Valider Jeu et l'afficher dans le volet de droite////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void F_Retour::on_LE_SearchJeux_returnPressed()
{
    ui->Bt_RendreJeu->setEnabled(false);
    //Récupère le code du jeu qui a été saisi
    QString CodeJeu= SearchJeux->currentText();

    //Vérification que le jeu est bien à rendre
    QSqlQuery RequeteStatut ;
    RequeteStatut.prepare("SELECT IdStatutJeux FROM statutjeux,jeux WHERE IdStatutJeux=StatutJeux_IdStatutJeux AND CodeJeu=:CodeJeu");
    RequeteStatut.bindValue(":CodeJeu",CodeJeu);

    if (!RequeteStatut.exec())
    {
       qDebug()<<"RequeteStatut "<<getLastExecutedQuery(RequeteStatut)<<RequeteStatut.lastError();
    }

    RequeteStatut.next();
    // Vérification que le jeu a bien été emprunté
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
            qDebug()<<"F_Retour::on_LE_SearchJeux_returnPressed || RequeteCodeMembre"<< RequeteCodeMembre.lastQuery();
        }
        RequeteCodeMembre.next();

        // Actualisation du membre actif
        this->MembreActif = RequeteCodeMembre.value(0).toString() ;
        AfficherMembre();

        //Savoir si le jeu est déja réservé
        QSqlQuery RequeteResa;
        RequeteResa.prepare("SELECT idReservation,DatePrevuEmprunt,DatePrevuRetour "
                            "FROM reservation LEFT JOIN jeux ON Jeux_IdJeux=IdJeux "
                            "WHERE CodeJeu=:CodeJeu");
        RequeteResa.bindValue(":CodeJeu",CodeJeu);

        if (!RequeteResa.exec())
        {
            qDebug() << "RequeteResa :" << getLastExecutedQuery(RequeteResa)<<RequeteResa.lastError();
        }
        QCalendarWidget * Calendrier=new QCalendarWidget();
        // Marque sur le calendrier les dates de réservation
        while(RequeteResa.next())
        {
           QDate date = ObtenirValeurParNom(RequeteResa,"DatePrevuEmprunt").toDate();
           QDate dateRetour = ObtenirValeurParNom(RequeteResa,"DatePrevuRetour").toDate().addDays(1);
           while(date!=dateRetour)
           {
               QTextCharFormat cf = Calendrier->dateTextFormat(date);
               cf.setFontStrikeOut(true);
               Calendrier->setDateTextFormat( date, cf );

               date=date.addDays(1);
           }
        }

        ui->DtE_Prolonger->setCalendarWidget(Calendrier);
    }
    else
    {
        if (SearchJeux->currentText()!="")
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
    if(iMalleActive!=0)
    {
        QSqlQuery RequeteMiseAJourRemarqueMalle;

        //prépare le requête de mise à jour
        RequeteMiseAJourRemarqueMalle.prepare("UPDATE malles SET Remarque =:NouvelRemarque WHERE IdMalle=:IdMalle");

        //Entre les valeurs de la requête
        RequeteMiseAJourRemarqueMalle.bindValue(":IdMalle",this->iMalleActive);
        RequeteMiseAJourRemarqueMalle.bindValue(":NouvelRemarque",ui->TxE_RemarquesJeu->toPlainText());

        if (!RequeteMiseAJourRemarqueMalle.exec())
        {
           qDebug()<< getLastExecutedQuery(RequeteMiseAJourRemarqueMalle) << RequeteMiseAJourRemarqueMalle.lastError();
        }
        AfficherJeuxEnEmprunt();
    }
    else
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
    // Si aucun membre n'est actif, on sort de la fonction
    if(MembreActif=="")
    {
        qDebug()<<"Aucun membre n'est sélectionné!!!";
        return;
    }

    foreach(QModelIndex index, ui->Tv_JeuxEmprunte->selectionModel()->selectedRows())
    {
        // S'il s'agit d'une malle
        if(this->ModelJeuEmpruntes->index(index.row(),0).data(Qt::UserRole+1)!=QVariant::Invalid)
        {
            // Récupère la QHash sur les infos de la malle
            QHash<QString, QVariant> hInfosMalle=this->ModelJeuEmpruntes->
                    item(index.row(),0)->data().value<QHash<QString, QVariant> >();
            int iIdMalle=hInfosMalle["IdMalle"].toInt();
            // Mise à jour de la table malles
            QSqlQuery RequeteRetourMalle;
            RequeteRetourMalle.prepare("UPDATE malles SET DateRetour=:DateRetour "
                                  "WHERE DateRetour IS NULL AND IdMalle=:IdMalle");

            QDateTime DateDeRetour;
            DateDeRetour=DateDeRetour.currentDateTime();
            RequeteRetourMalle.bindValue(":DateRetour",DateDeRetour);
            RequeteRetourMalle.bindValue(":IdMalle",iIdMalle);
            if(!RequeteRetourMalle.exec())
            {
                qDebug()<<"RequeteRetourMalle "<< getLastExecutedQuery(RequeteRetourMalle) << RequeteRetourMalle.lastError();
            }
            QVariant CodeEnCours;
            int j=0;
            while((CodeEnCours=this->ModelJeuEmpruntes->index(index.row(),0).child(j,1).data())
                  !=QVariant::Invalid)
            {
                RetournerJeu(CodeEnCours.toString(),this->ModelJeuEmpruntes->
                             index(index.row(),0).child(j,0).data().toString());
                j++;
            }
        }
        // Si c'est un jeu
        else
        {
            this->RetournerJeu(ModelJeuEmpruntes->item(index.row(),1)->text(),
                               ModelJeuEmpruntes->item(index.row(),0)->text());
        }
    }
    ActualiserMembre();
    ViderJeu();
    ActualiserListeJeux();
    ActualiserListeEmprunteurs();
}

void F_Retour::RetournerJeu(QString CodeJeu,QString NomJeu)
{
    QSqlQuery RequeteRetour;
    QDateTime DateDeRetourToleree;
    DateDeRetourToleree=DateDeRetourToleree.currentDateTime();

    //Rechercher la date prévu de l'emprunt
    QSqlQuery RequeteDateRetour;
    RequeteDateRetour.prepare("SELECT DateRetourPrevu, IdJeu"
                              " FROM emprunts,jeux"
                              " WHERE CodeJeu=:CodeDuJeu AND Jeux_IdJeux=IdJeux AND DateRetour IS NULL");
    RequeteDateRetour.bindValue(":CodeDuJeu",CodeJeu);

    if (!RequeteDateRetour.exec())
    {
       qDebug()<<"RequeteDateRetour "<<RequeteDateRetour.lastQuery();
    }
    RequeteDateRetour.next();

    //Comparaison entre la date prévue du retour et la date du jour, on récupère un int
    unsigned int EcartJours (0);
    EcartJours= ObtenirValeurParNom(RequeteDateRetour,"DateRetourPrevu").toDateTime().daysTo(DateDeRetourToleree);

    unsigned int IdJeu=ObtenirValeurParNom(RequeteDateRetour,"IdJeu").toInt();

    unsigned int NbJoursRetardToleres(0);
    // Calculer la date de retour avec la tolérance du nombre de jours
    NbJoursRetardToleres = F_Preferences::ObtenirValeur("JourRetard").toInt();

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
           qDebug()<<"RequeteNbRetardsMembre "<<RequeteNbRetardsMembre.lastQuery();
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

    // Mise à jour de la table emprunt
    RequeteRetour.prepare("UPDATE emprunts,jeux "
                          "SET DateRetour=:DateRetour,StatutJeux_IdStatutJeux=1 "
                          "WHERE DateRetour IS NULL AND Jeux_IdJeux=IdJeux AND CodeJeu=:CodeDuJeu");

    RequeteRetour.bindValue(":DateRetour",DateDeRetourToleree);
    RequeteRetour.bindValue(":CodeDuJeu",CodeJeu);
    if(!RequeteRetour.exec())
    {
        qDebug()<<"F_Retour::on_Bt_RendreJeu_clicked => RequeteRetour "<< RequeteRetour.lastQuery();
    }

    //Savoir si le jeu est réservé
    QSqlQuery RequeteJeu;
    RequeteJeu.prepare("SELECT idReservation,IdMembre,Email FROM reservation LEFT JOIN jeux ON Jeux_IdJeux=IdJeux LEFT JOIN membres ON "
                       "Membres_IdMembre=IdMembre WHERE CodeJeu=:CodeDuJeu");
    RequeteJeu.bindValue(":CodeDuJeu",CodeJeu);

    if(!RequeteJeu.exec())
    {
        qDebug()<<"RequeteJeu "<< RequeteJeu.lastQuery();
    }

    RequeteJeu.next();

    if(RequeteJeu.size()>0)
    {
        // On affiche le dialog demandant si le jeu a été mis de coté ou non
        D_ResaMisDeCote D_ResaMisDeCote(this,CodeJeu,NomJeu,ObtenirValeurParNom(RequeteJeu,"IdMembre").toInt(),IdJeu,
                                        ObtenirValeurParNom(RequeteJeu,"Email").toString());
        D_ResaMisDeCote.exec() ;
    }
}

////////////////////////////////////////////Suppression d'un jeu réservé/////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void F_Retour::on_bt_SuppReservation_clicked()
{
    if(F_Emprunt::SupprimerReservation(ui->Tv_JeuxReserve,this->ModelJeuReserves,this->MembreActif))
    {
        //Grise le bouton de suppression du tableau des réservations
        ui->bt_SuppReservation->setEnabled(false);
        // Efface les info sur le jeu actuellement affiché
        this->ViderJeu();
    }
}

void F_Retour::on_Bt_ToutSelectionner_clicked()
{
    // Sélection de toutes les lignes (les child des malles ne seront pas sélectionnées)
    for(int i=0;i<this->ModelJeuEmpruntes->rowCount();i++)
    {
        ui->Tv_JeuxEmprunte->selectionModel()->select(
                    this->ModelJeuEmpruntes->item(i,0)->index(),
                    QItemSelectionModel::Select | QItemSelectionModel::Rows);
    }
    ui->Bt_RendreJeu->setEnabled(true);
    this->AffichageProlongation(true);
    //On appelle cette fonction pour quel calcul si il faut activer ou non le bouton prolonger (uniquement si pas de malle)
    on_Tv_JeuxEmprunte_clicked(this->ModelJeuEmpruntes->item(0,0)->index());
}

void F_Retour::on_Bt_ToutDeselectionner_clicked()
{
    ui->Tv_JeuxEmprunte->selectionModel()->clear();
    ui->Bt_RendreJeu->setEnabled(false);
    this->AffichageProlongation(false);
}

void F_Retour::on_Bt_PayerAmende_clicked()
{
    bool ok;
    double MontantAmende = QInputDialog::getDouble(this, tr("QInputDialog::getText()"),
                                         tr("Montant de l'amende à mettre au panier:"),0, 0,2147483647, 2, &ok);
    if (!ok) return;
    emit(Signal_AjouterAuPanier("Amende",IdDuMembre,MontantAmende,VENTILATION_AMENDE,"",NULL));
}

void F_Retour::on_Bt_PayerRetard_clicked()
{
    bool ok;
    double MontantAmende = QInputDialog::getDouble(this, tr("QInputDialog::getText()"),
                                         tr("Montant du retard à mettre au panier:"), 0, 0,2147483647, 2, &ok);
    if (!ok) return;
    emit(Signal_AjouterAuPanier("Amende",IdDuMembre,MontantAmende,VENTILATION_RETARD,"",NULL));
}
