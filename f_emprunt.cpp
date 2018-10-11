//------------------------------------------------------------------------------
/** @file         f_emprunt.cpp
 *  @brief        Classe graphique permettant de gérer les emprunts
 *
 *  @author       Philippe BOTHEREL
 *  @author       STS IRIS, Lycée Nicolas APPERT, ORVAULT (FRANCE)
 *  @since        01/01/1012
 *  @version      0.1
 *  @date         29/01/1013
 *  @author       William SOREL
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

#include "searchbox.h"
#include "f_emprunt.h"
#include "ui_f_emprunt.h"
#include "fonctions_globale.h"
#include "f_preferences.h"
#include "f_paiement.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Constructeur///////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/** Description détaillée du CONSTRUCTEUR
 *  @pre    Connexion  à la base de données, existance de la table Emprunt et de ses champs
 */
F_Emprunt::F_Emprunt(int iMode, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::F_Emprunt)
{
    ui->setupUi(this);

    this->pPaiement=new F_Paiement;
    this->pMalles=new F_Malles;
    // Si clic dans le bouton réservé, affiche la liste des réservation avec le jeu sélectionné
    connect( this->pMalles, SIGNAL( Signal_Clic_Emprunter( int ) ), this, SLOT( slot_Clic_Emprunter(int) )) ;

    MembreActif="";
    JeuActif="";
    iMalleActive=0;
    bRegle=false;
    this->iIdReservation=0;
    //ui->TbV_Recherche->setVisible(false);
    ui->Lb_Cotisation->setText("");
    ui->Lb_CotisationARemplir->setText("");
    ui->Lb_MembreEcarte->setText("");

    //Met une date minimum pour le DateEdit du retour (la date du jour)
    QDateTime DateActuelle;
    DateActuelle=DateActuelle.currentDateTime();
    ui->DtE_Retour->setMinimumDate(DateActuelle.date());

    //Met une date minimum pour le DateEdit du départ (la date du jour)
    ui->DtE_Depart->blockSignals(true);
    ui->DtE_Depart->setMinimumDate(DateActuelle.date());
    ui->DtE_Depart->setDateTime(DateActuelle);
    ui->DtE_Depart->blockSignals(false);

    MaJListeMembres();

    SearchMembre = new SearchBox(this);
    SearchMembre->MAJResults(this->VecteurMembres,3);
    SearchMembre->show();
    ui->horizontalLayout->addWidget(SearchMembre);

    connect(SearchMembre,SIGNAL(SignalSuggestionFini()),this,SLOT(on_LE_SearchMembre_MembreTrouve()));

    SearchJeux = new SearchBox(this);
    this->iMode=iMode;
    if(this->iMode==MODE_MALLES)
    {
        this->ActualiserListeJeux();
        ui->Bt_Reserver->setText("Réserver Malle");
        ui->Bt_CalendrierMalles->setEnabled(false);
        ui->Lb_TypeEmprunt->setText("Type de malle");
        ui->Lb_NewEmprunt->setText("Nombre de jeux à valider (en crédit) :");
    }
    else
    {
        ui->Bt_Regle->setVisible(false);
        ui->Bt_AjoutNonAdherent->setVisible(false);
        ui->Bt_Reserver->setText("Réserver");
        ui->Bt_CalendrierMalles->setVisible(false);
    }
    on_rB_Mode_Emprunt_toggled(true);
    SearchJeux->show();
    SearchJeux->setEnabled(false);

    ui->Hlay16->addWidget(SearchJeux);

    connect(SearchJeux->lineEdit(), SIGNAL( returnPressed() ), this, SLOT( on_LE_SearchJeux_returnPressed() ) );
    connect(SearchJeux,SIGNAL(SignalSuggestionFini()),this,SLOT(on_LE_SearchJeux_jeuTrouve()));


    QSqlQuery Requete;
    // Tous les lieux sauf Internet
    Requete.exec("SELECT IdLieux,NomLieux FROM lieux WHERE IdLieux!=1");

    while(Requete.next())
    {
       ui->CBx_Retrait->addItem(ObtenirValeurParNom(Requete,"NomLieux").toString(),ObtenirValeurParNom(Requete,"IdLieux").toInt());
    }

    //Création d'un modèle pour le TableView des nouveaux emprunts
    this->ModeleEmpruntsAValider = new QStandardItemModel() ;
    //Associe le modèle au TableView
    ui->TbV_EmpruntAValider->setModel(this->ModeleEmpruntsAValider);
    //Met le TableView en lecture seule
    ui->TbV_EmpruntAValider->setEditTriggers(0);
    // Autorise le tri pour ce tableau
    ui->TbV_EmpruntAValider->setSortingEnabled(true);

    this->EffacerJeuAValider();

    this->NbLignesEmpruntsAValider=0;

    //Création d'un modèle pour le TableView des jeux empruntés
    this->ModeleJeuxEmpruntes = new QStandardItemModel() ;
    //Associe le mod7l au TableView
    ui->Tv_JeuxMembres->setModel(this->ModeleJeuxEmpruntes);
    //Met le TableView en lecture seule
    ui->Tv_JeuxMembres->setEditTriggers(0);
    // Autorise le tri pour ce tableau
    ui->Tv_JeuxMembres->setSortingEnabled(true);
    //Initialise les colones du TableView des nouveaux emprunts
    this->ModeleJeuxEmpruntes->setColumnCount(4);
    this->ModeleJeuxEmpruntes->setHorizontalHeaderItem(0, new QStandardItem("Nom du jeu/malle"));
    this->ModeleJeuxEmpruntes->setHorizontalHeaderItem(1, new QStandardItem("Code"));
    this->ModeleJeuxEmpruntes->setHorizontalHeaderItem(2, new QStandardItem("Date emprunt"));
    this->ModeleJeuxEmpruntes->setHorizontalHeaderItem(3, new QStandardItem("Date retour"));
    ui->Tv_JeuxMembres->setColumnWidth(0,140);
    ui->Tv_JeuxMembres->setColumnWidth(1,40);

    //Création d'un modèle pour le TableView des jeux empruntés
    this->ModeleJeuxReserves = new QStandardItemModel() ;
    //Associe le modèle au TableView
    ui->Tv_JeuxReserves->setModel(this->ModeleJeuxReserves);
    //Met le TableView en lecture seule
    ui->Tv_JeuxReserves->setEditTriggers(0);
    // Autorise le tri pour ce tableau
    ui->Tv_JeuxReserves->setSortingEnabled(true);

    //Initialise les colones du TableView des nouveaux emprunts
    this->ModeleJeuxReserves->setColumnCount(5);
    this->ModeleJeuxReserves->setHorizontalHeaderItem(0, new QStandardItem("Nom du jeu/malle"));
    this->ModeleJeuxReserves->setHorizontalHeaderItem(1, new QStandardItem("Code"));
    this->ModeleJeuxReserves->setHorizontalHeaderItem(2, new QStandardItem("Date emprunt"));
    this->ModeleJeuxReserves->setHorizontalHeaderItem(3, new QStandardItem("Date retour"));
    this->ModeleJeuxReserves->setHorizontalHeaderItem(4, new QStandardItem("lieu"));
    this->ModeleJeuxReserves->setHorizontalHeaderItem(5, new QStandardItem("Date réservation"));
    ui->Tv_JeuxReserves->setColumnWidth(0,140);
    ui->Tv_JeuxReserves->setColumnWidth(1,40);
    ui->Tv_JeuxReserves->setColumnWidth(2,100);
    ui->Tv_JeuxReserves->setColumnWidth(3,100);
    ui->Tv_JeuxReserves->setColumnWidth(4,100);

    this->iNbEmpruntEnCours=0;
    this->iNbJeuxEmpruntables=0;
    this->iNbNouveauxEmprunts=0;
    ActualiserTypeEmprunt(0);

    // Bloque la saisie de code jeu à emprunter et du bouton OK tant que pas d'adhérent sélectionné
    ui->DtE_Retour->setDisplayFormat("dd/MM/yyyy hh:mm");
    ui->DtE_Depart->setDisplayFormat("dd/MM/yyyy hh:mm");
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
    delete this->ModeleEmpruntsAValider;
    this->ModeleEmpruntsAValider=NULL;
    delete this->ModeleJeuxEmpruntes;
    this->ModeleJeuxEmpruntes=NULL;
    delete this->ModeleJeuxReserves;
    this->ModeleJeuxReserves=NULL;
}

/**   Mise  jour de la liste des membres
 *  @pre    Accès à  la base de données
 *  @retval bool
 *  @return True si tous c'est passé correctement et false si il y a une erreur
 *  @test   Voir la procédure dans le fichier associé.
 */
bool F_Emprunt::MaJListeMembres(bool AfficherContact)
{
    QSqlQuery query ;
    QVector<QString> Membres ;
    bool bRetourner = true ;

    //Vidange du vecteur
    this->VecteurMembres.clear() ;

    QString requeteSQL;
    requeteSQL="SELECT Nom, Prenom, Email,Ville,IdMembre,CodeMembre FROM membres ";
    if(!AfficherContact)
    {
        requeteSQL+="WHERE TypeMembres_IdTypeMembres!=4 ";
    }
    requeteSQL+="ORDER BY Nom ASC";

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
        qDebug()<< "F_Emprunt::MaJListeMembres() : Erreur de connexion avec la base de donnée !" << endl ;
        bRetourner = false ;
    }
    return bRetourner ;
}

/**   Mise  jour de la liste des jeux
 *  @pre    Accès à  la base de données
 *  @retval bool
 *  @return True si tous c'est passé correctement et false si il y a une erreur
 *  @test   Voir la procédure dans le fichier associé.
 */
QVector<QVector <QString> > F_Emprunt::MaJListeJeux(QString filtre)
{
    QSqlQuery query ;
    QVector<QString> Jeux ;

    //Vidange du vecteur
    this->VecteurJeux.clear() ;

    QSqlQuery Requete;
    Requete.prepare("SELECT DISTINCT NomJeu,CodeJeu FROM jeux as j "+filtre+" ORDER BY NomJeu");

    if(!Requete.exec())
    {
        qDebug()<< getLastExecutedQuery(Requete)<<Requete.lastError();
    }
    else
    {
        //Execute une requète sql qui retourne la liste des jeux
        //Si la requète est correcte -> Remplissage du veteur VecteurJeux
        // avec le résultat de la requête et on retourne vrai.
        while(Requete.next())
        {
            for(int i=0;i<Requete.record().count();i++)
            {
                Jeux.append(Requete.value(i).toString()) ;
            }
            this->VecteurJeux.push_back(Jeux) ;
            Jeux.clear();
        }
    }
    return this->VecteurJeux;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Vider les informations du jeu//////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *  @pre    Connexion avec la base de données, des informations d'un jeu sont affichées
 *  @post
 *  @param
 *  @retval
 *  @return
 *  @test
 *  @see
 */
void F_Emprunt::ViderJeu()
{
    ui->Le_NomJeuARemplir->setText("");
    ui->Le_StatutJeuARemplir->setText("");
    ui->Lb_StatutJeu->setStyleSheet("QLabel {color:black;}");
    ui->Le_PrixEmpruntARemplir->setText("");
    ui->TxE_RemarquesJeu->setText("");
    ui->Bt_AnnulerRemarquesJeu->setEnabled(false);
    ui->Bt_ValiderRemarquesJeu->setEnabled(false);
    ui->Bt_AjouterJeu->setEnabled(false);
    // Plus de jeu actif sélectionné
    this->JeuActif = "" ;
}

//! Actualise le combobox des types de malles
void F_Emprunt::ActualiserTypeEmprunt(int iTitreMembre=0)
{
    //On prépare et exécute la requête qui permet de récupérer les infos sur le type d'emprunt
    QSqlQuery Requete;
    QString Srequete="SELECT * FROM typeemprunt WHERE MALLE="+QString::number(this->iMode==MODE_MALLES);

    if(iTitreMembre!=0)
    {
        Srequete+=" AND TitreMembre_IdTitreMembre=:titremembre";
        Requete.prepare(Srequete);
        Requete.bindValue(":titremembre",iTitreMembre);
    }
    else
    {
        Requete.prepare(Srequete);
    }
    if ( ! Requete.exec() )
    {
        qDebug()<<getLastExecutedQuery(Requete);
        return;
    }
    ui->CBx_TypeEmprunt->clear();
    QHash<QString, QVariant> HashRecord;
    QString TypeEmprunt,TypeEmpruntDefaut;
    while(Requete.next())
    {
        for(int i=0;i<Requete.record().count();i++)
        {
            QString tmp=Requete.record().fieldName(i);
            int valeur=Requete.value(i).toInt();
            HashRecord[Requete.record().fieldName(i)]=Requete.value(i).toInt();
            HashTypeEmprunt[ObtenirValeurParNom(Requete,"IdTypeEmprunt").toInt()]=HashRecord;
        }
        TypeEmprunt=ObtenirValeurParNom(Requete,"TypeEmprunt").toString()+" ("+
                ObtenirValeurParNom(Requete,"DureeEmprunt").toString()+" jours)";
        ui->CBx_TypeEmprunt->addItem(TypeEmprunt,ObtenirValeurParNom(Requete,"IdTypeEmprunt").toInt());
        if(ObtenirValeurParNom(Requete,"TypeEmprunt")==F_Preferences::ObtenirValeur("TypeEmpruntDefaut"))
        {
            TypeEmpruntDefaut=TypeEmprunt;
        }
    }
    ui->CBx_TypeEmprunt->setCurrentText(TypeEmpruntDefaut);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////Actualise les informations du jeu////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *  @pre    Connexion avec la base de données, un jeu est actif dans la classe
 *  @post
 *  @param
 *  @retval
 *  @return
 *  @test
 *  @see    JeuActif
 */
void F_Emprunt::ActualiserJeu()
{
    if (this->JeuActif != "")
    {
        SearchJeux->setCurrentText(this->JeuActif);
        on_LE_SearchJeux_jeuTrouve();
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////Actualise la liste des jeux////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *  @pre    Connexion avec la base de données, un jeu est actif dans la classe
 *  @post
 *  @param
 *  @retval
 *  @return
 *  @test
 *  @see    JeuActif
 */
void F_Emprunt::ActualiserListeJeux()
{
    QString ArgMAJListeJeux;
    // Si il n'y a pas de jeux spéciaux empruntable, on les exclue de la requête
    if(HashTypeEmprunt[ui->CBx_TypeEmprunt->currentData().toInt()]["NbJeuxSpeciauxEmpruntable"]==0)
    {
        ArgMAJListeJeux=F_Preferences::ObtenirValeur("FiltreJeuxSpeciauxNomChamps")+"!="+
                F_Preferences::ObtenirValeur("FiltreJeuxSpeciauxValeur");
    }
    // Si il n'y a pas des jeux autres que les jeux spéciaux empruntable, on les exclue de la requête
    else if(HashTypeEmprunt[ui->CBx_TypeEmprunt->currentData().toInt()]["NbAutresJeuxEmpruntable"]==0)
    {
        ArgMAJListeJeux=F_Preferences::ObtenirValeur("FiltreJeuxSpeciauxNomChamps")+"="+
                F_Preferences::ObtenirValeur("FiltreJeuxSpeciauxValeur");
    }
    // Pour les malles, on exclue les jeux qui sont empruntés et réservés aux dates définies
    if(this->iMode==MODE_MALLES)
    {
        if(ArgMAJListeJeux!="")
        {
            ArgMAJListeJeux+=" AND ";
        }
        ArgMAJListeJeux="LEFT JOIN reservation as r ON r.Jeux_IdJeux=IdJeux "
                "LEFT JOIN emprunts as e ON e.Jeux_IdJeux=IdJeux WHERE "+ArgMAJListeJeux+
                " IdJeux NOT IN (SELECT Jeux_IdJeux FROM reservation as r WHERE DATE(r.DatePrevuEmprunt)<='"+
                ui->DtE_Retour->date().toString("yyyy-MM-dd")+"' AND DATE(r.DatePrevuRetour)>='"+
                ui->DtE_Depart->date().toString("yyyy-MM-dd")+"' OR r.DatePrevuRetour IS NULL)"+
                +" AND IdJeux NOT IN (SELECT Jeux_IdJeux FROM emprunts as e WHERE DATE(e.DateEmprunt)<='"+
                ui->DtE_Retour->date().toString("yyyy-MM-dd")+"' AND DATE(e.DateRetourPrevu)>='"+
                ui->DtE_Depart->date().toString("yyyy-MM-dd")+"' AND e.DateRetour IS NULL OR e.DateRetourPrevu IS NULL)";
    }
    else if(ArgMAJListeJeux!="")
    {
        ArgMAJListeJeux=" WHERE "+ArgMAJListeJeux;
    }
    SearchJeux->MAJResults(MaJListeJeux(ArgMAJListeJeux),2);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Recherche des jeux réservés////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *  @pre    Connexion avec la base de données, un membre est actif dans la classe
 *  @post
 *  @param
 *  @retval
 *  @return
 *  @test
 *  @see    MembreActif, ModeleJeuxReserves
 */
void F_Emprunt::AfficherMalle(int iIdMalle)
{
    this->iMalleActive=iIdMalle;
    this->EffacerJeuAValider();
    QSqlQuery Requete;
    Requete.prepare("SELECT r.idReservation,m.Membres_IdMembre, CodeMembre,Jeux_IdJeux, CodeJeu, "
                    "m.TypeEmprunt_IdTypeEmprunt as IdTypeEmprunt,"
                    "m.DateRetourPrevu as DateRetourPrevu, NomJeu,PrixLoc,m.Regle FROM reservation as r "
                    "LEFT JOIN malles as m ON m.IdMalle=Malles_IdMalle "
                    "LEFT JOIN membres as me ON m.Membres_IdMembre=me.IdMembre LEFT JOIN jeux as j ON r.Jeux_IdJeux=IdJeux "
                    "WHERE IdMalle="+QString::number(iIdMalle));

    //Exécute la requête
    if (!Requete.exec())
    {
        qDebug()<<"F_Malles::AfficherCalendrier => RequeteJeux : "<< getLastExecutedQuery(Requete)<<Requete.lastError();
    }
            qDebug()<<"F_Malles::AfficherCalendrier => RequeteJeux : "<< getLastExecutedQuery(Requete)<<Requete.lastError();

    bool MembrePasCharger=true;
    while(Requete.next())
    {
        if(MembrePasCharger)
        {
            this->bRegle=ObtenirValeurParNom(Requete,"Regle").toBool();
            AfficherMembre(ObtenirValeurParNom(Requete,"CodeMembre").toString());
            MembrePasCharger=false;
            for(int i=0;i<ui->CBx_TypeEmprunt->count();i++)
            {
                qDebug()<<ui->CBx_TypeEmprunt->itemData(i).toInt();
                if(ui->CBx_TypeEmprunt->itemData(i).toInt()==ObtenirValeurParNom(Requete,"IdTypeEmprunt").toInt())
                {
                    ui->CBx_TypeEmprunt->setCurrentIndex(i);
                    break;
                }
            }

        }
        AjouterJeuAValider(ObtenirValeurParNom(Requete,"Membres_IdMembre").toInt(),ObtenirValeurParNom(Requete,"Jeux_IdJeux").toInt(),
                           ObtenirValeurParNom(Requete,"idReservation").toInt(),
                           ObtenirValeurParNom(Requete,"CodeJeu").toInt(),ObtenirValeurParNom(Requete,"IdTypeEmprunt").toInt(),
                           ObtenirValeurParNom(Requete,"DateRetourPrevu").toDate(),ObtenirValeurParNom(Requete,"PrixLoc").toInt(),
                           ObtenirValeurParNom(Requete,"NomJeu").toString(),
                           ObtenirValeurParNom(Requete,
                           F_Preferences::ObtenirValeur("FiltreJeuxSpeciauxNomChamps")).toString()==F_Preferences::ObtenirValeur("FiltreJeuxSpeciauxValeur"));
    }
    ui->Bt_Reserver->setText("Modifier Malle");
    ui->Bt_Reserver->setEnabled(true);
    ui->Bt_Emprunter->setEnabled(true);
    ui->rB_Mode_Emprunt->setChecked(true);
    this->AfficherEtatPaiement();
    this->AfficherNbEmpruntsEnCours();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Affiche l'état du paiement de la malle/////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *  @pre    Connexion avec la base de données, un membre est actif dans la classe
 *  @post
 *  @param
 *  @retval
 *  @return
 *  @test
 *  @see    MembreActif, ModeleJeuxReserves
 */
void F_Emprunt::AfficherEtatPaiement()
{
    if(this->bRegle)
    {
        ui->Bt_Regle->setText("Malle réglée");
        ui->Bt_Regle->setIcon(QIcon(":/Valider.png"));
    }
    else
    {
        ui->Bt_Regle->setText("En attente de paiement");
        ui->Bt_Regle->setIcon(QIcon(":/Supprimer.png"));
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Recherche des jeux réservés////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *  @pre    Connexion avec la base de données, un membre est actif dans la classe
 *  @post
 *  @param
 *  @retval
 *  @return
 *  @test
 *  @see    MembreActif, ModeleJeuxReserves
 */
void F_Emprunt::AfficherJeuxReserve(QStandardItemModel *ModeleJeuxReserves,QString MembreActif,
                                    bool bRetour)
{
    QSqlQuery RequeteIdMembre;
    unsigned int IdDuMembre (0);

    //Prépare le requête pour récupérer l'id du membre
    RequeteIdMembre.prepare("SELECT IdMembre FROM membres WHERE CodeMembre=:CodeDuMembre");
    RequeteIdMembre.bindValue(":CodeDuMembre",MembreActif);

    //Exécute la requête
    if (!RequeteIdMembre.exec())
    {
        qDebug()<<"F_Emprunt::JeuxReserve => Requete IdDuMembre "<<RequeteIdMembre.lastQuery();
    }

    RequeteIdMembre.next();

    //Récupère les remarques'id dans la base de données
    IdDuMembre=RequeteIdMembre.value(0).toInt();

    QSqlQuery RequeteJeuReserve;
    unsigned int  NumeroLigne =0;
    RequeteJeuReserve.prepare("SELECT r.idReservation,t.TypeEmprunt,m.DateReservation as mDateReservation, "
                              "m.DatePrevuEmprunt as mDatePrevuEmprunt,m.DateRetourPrevu as mDateRetourPrevu,"
                              "r.DateReservation,r.DatePrevuEmprunt,r.DatePrevuRetour,NomLieux,NomJeu,m.IdMalle,"
                              "CodeJeu FROM reservation as r LEFT JOIN malles as m ON r.Malles_IdMalle=m.IdMalle "
                              "LEFT JOIN typeemprunt as t ON m.TypeEmprunt_IdTypeEmprunt=t.IdTypeEmprunt "
                              "LEFT JOIN jeux ON IdJeux=Jeux_IdJeux LEFT JOIN lieux ON IdLieux=Lieux_IdLieuxReservation "
                              "WHERE r.Membres_IdMembre=:IdMembre");

    RequeteJeuReserve.bindValue(":IdMembre",IdDuMembre);

    if (!RequeteJeuReserve.exec())
    {
        qDebug()<<"Requete Jeux Reservés "<<getLastExecutedQuery(RequeteJeuReserve)<<RequeteJeuReserve.lastError();
    }

    //On vide le modèle
    ModeleJeuxReserves->removeRows(0,ModeleJeuxReserves->rowCount());

    // Compteur de ligne parente dans le table view
    int iCptTableView=0;
    //Tant qu'il y a des jeux réservés
    while(RequeteJeuReserve.next())
    {
        QStandardItem *DeuxiemeItem=new QStandardItem(ObtenirValeurParNom(RequeteJeuReserve,"CodeJeu").toString());
        /*qDebug()<<DeuxiemeItem->flags();
        DeuxiemeItem->setFlags(DeuxiemeItem->flags() & ~Qt::ItemIsSelectable & ~Qt::ItemIsEnabled);
        qDebug()<<DeuxiemeItem->flags();*/
        // On stocke l'Id de réservation dans la première case de la ligne du jeu
        DeuxiemeItem->setData(ObtenirValeurParNom(RequeteJeuReserve,"idReservation").toInt());
        // Liste des valeurs à faire apparaitre dans le modele
        QList<QStandardItem *> List;
        List<<new QStandardItem(ObtenirValeurParNom(RequeteJeuReserve,"NomJeu").toString() )
           <<DeuxiemeItem
         <<new QStandardItem(ObtenirValeurParNom(RequeteJeuReserve,"DatePrevuEmprunt").toDate().toString("dd-MM-yyyy"))
        <<new QStandardItem(ObtenirValeurParNom(RequeteJeuReserve,"DatePrevuRetour").toDate().toString("dd-MM-yyyy"))
        <<new QStandardItem(ObtenirValeurParNom(RequeteJeuReserve,"NomLieux").toString())
        <<new QStandardItem(ObtenirValeurParNom(RequeteJeuReserve,"DateReservation").toDate().toString("dd-MM-yyyy"));
        // S'il s'agit d'un jeu d'une malle
        if(ObtenirValeurParNom(RequeteJeuReserve,"IdMalle").toInt()!=0)
        {
            int iIdMalle=ObtenirValeurParNom(RequeteJeuReserve,"IdMalle").toInt();
            bool MalleNonTrouve=true;
            // On recherche si la malle existe déjà dans le modele
            for(int i=0;i<ModeleJeuxReserves->rowCount();i++)
            {
                if(iIdMalle==ModeleJeuxReserves->item(i,0)->data())
                {
                    ModeleJeuxReserves->item(i,0)->appendRow(List);
                    MalleNonTrouve=false;
                    break;
                }
            }
            if(MalleNonTrouve)
            {
                QStandardItem *parent = new QStandardItem(ObtenirValeurParNom(RequeteJeuReserve,"TypeEmprunt").toString() );
                parent->setData(iIdMalle);
                //on ajoute une nouvelle ligne du table view
                parent->appendRow(List);
                ModeleJeuxReserves->setItem( iCptTableView, 0, parent);
                ModeleJeuxReserves->setItem( iCptTableView, 5, new QStandardItem(ObtenirValeurParNom(RequeteJeuReserve,"mDateReservation").toDate().toString("dd-MM-yyyy")));
                ModeleJeuxReserves->setItem( iCptTableView, 2, new QStandardItem(ObtenirValeurParNom(RequeteJeuReserve,"mDatePrevuEmprunt").toDate().toString("dd-MM-yyyy")));
                ModeleJeuxReserves->setItem( iCptTableView++, 3, new QStandardItem(ObtenirValeurParNom(RequeteJeuReserve,"mDateRetourPrevu").toDate().toString("dd-MM-yyyy")));
            }
        }
        else
        {
            //on ajoute une nouvelle ligne du table view
            for(int i=0;i<List.count();i++)
            {
                ModeleJeuxReserves->setItem( iCptTableView, i,List.at(i));
            }
            iCptTableView++;

            //Savoir si le jeu est disponible ou non
            QSqlQuery Requetejeu;
            Requetejeu.prepare("SELECT IdEmprunts FROM emprunts,jeux WHERE CodeJeu=:CodeDuJeu AND Jeux_IdJeux=IdJeux AND DateRetour IS NULL");
            Requetejeu.bindValue(":CodeDuJeu",RequeteJeuReserve.value(5).toString());

            if ( ! Requetejeu.exec() )
            {
                qDebug()<<"Requetejeu "<<Requetejeu.lastQuery();
            }

            if(Requetejeu.size()==0)
            {
                ModeleJeuxReserves->setData(ModeleJeuxReserves->index( iCptTableView-1,2),QColor(Qt::green), Qt::BackgroundColorRole);
            }
            else
            {
                ModeleJeuxReserves->setData(ModeleJeuxReserves->index( iCptTableView-1,2),QColor(Qt::red), Qt::BackgroundColorRole);
            }
        }

        NumeroLigne++;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Actualiser le membre en cours de consultation//////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *  @pre    Connexion avec la base de données, un membre déjà sélectionné
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
/**
 *  @pre    Connexion avec la base de données
 *  @post
 *  @param  Code du membre
 *  @retval
 *  @return
 *  @test
 *  @see    MembreActif
 */
void F_Emprunt::AfficherMembre(QString CodeMembre)
{
    this->MembreActif = CodeMembre;

    if(CodeMembre == "")
    {
        return;
    }
    //Grise les boutons de modification des remarques du membre
    ui->Bt_ValiderRemarques->setEnabled(false);
    ui->Bt_AnnulerRemarques->setEnabled(false);

    //Prépare la requête
    QSqlQuery Requete;
    Requete.prepare("SELECT Nom,Prenom,NbreRetard,Ecarte,Remarque,IdMembre,TitreMembre_IdTitreMembre,t.NbrJeuxEmpruntables,"
                    "(a.Prestations_IdPrestation IS NULL AND a.CartesPrepayees_IdCarte IS NULL) as NonAdherent FROM membres as m "
                    "LEFT JOIN titremembre as t ON m.TitreMembre_IdTitreMembre=t.IdTitreMembre  LEFT JOIN abonnements as a ON a.Membres_IdMembre=m.IdMembre "
                    "WHERE CodeMembre=:CodeDuMembre");
    Requete.bindValue(":CodeDuMembre",CodeMembre);
    if ( ! Requete.exec() )
    {
        qDebug()<<"F_Emprunt::AfficherMembre => Requete " << Requete.lastQuery();
    }
    Requete.next();

    //Récupère le Nom dans la base de données puis l'affiche
    ui->Le_NomARemplir->setText(ObtenirValeurParNom(Requete,"Nom").toString());

    //Récupère le Prénom dans la base de données puis l'affiche
    ui->Le_PrenomARemplir->setText(ObtenirValeurParNom(Requete,"Prenom").toString());

    //Regarde dans la base de données si le membre est écarté
    bool MembreEcarte = (ObtenirValeurParNom(Requete,"Ecarte").toBool());
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

    //Récupère si le membre est adhérent ou pas dans la base de données
    this->NonAdherent=ObtenirValeurParNom(Requete,"NonAdherent").toBool();

    //Récupère les remarques dans la base de données puis les affiches
    ui->TxE_Remarques->setText(ObtenirValeurParNom(Requete,"Remarque").toString());

    //Grise les boutons de modification des remarques du membre
    ui->Bt_ValiderRemarques->setEnabled(false);
    ui->Bt_AnnulerRemarques->setEnabled(false);

    //Grise les boutons de suppression des tableau des jeux
    ui->Bt_SupprimerEmpruntAValider->setEnabled(false);
    ui->Bt_SupprimerReservation->setEnabled(false);

    //Récupère le nombre de jeux empruntables dans la base de données puis l'affiche
    this->iNbJeuxEmpruntables=ObtenirValeurParNom(Requete,"NbrJeuxEmpruntables").toInt();

    //Affiche l'état de la cotisation
    //Savoir si le membre  a un membre associer
    QSqlQuery RequeteMembreAssocier ;
    RequeteMembreAssocier.prepare("SELECT CodeMembre FROM membresassocies,membres WHERE "
                                  "Membres_IdMembre=:IdMembre AND Membres_IdCollectivite=IdMembre");
    RequeteMembreAssocier.bindValue(":IdMembre",ObtenirValeurParNom(Requete,"IdMembre").toString());
    if ( ! RequeteMembreAssocier.exec() )
    {
        qDebug()<<"F_Emprunt::AfficherMembre => RequeteMembreAssocier " << RequeteMembreAssocier.lastQuery();
    }

    // si il y a un membre associé,
    if ( RequeteMembreAssocier.next() )
    {
        //On Affiche l'état de la cotisation du membre associé au membre actif
        this->EtatCotisationMembre= AfficherEtatCotisation(ObtenirValeurParNom(RequeteMembreAssocier,"CodeMembre").toString());
    }
    else
    {
        //On affiche l'état de la cotisation du membre en cours de consultation
        this->EtatCotisationMembre= AfficherEtatCotisation(this->MembreActif);
    }
    // Affiche les jeux empruntés
    this->iNbEmpruntEnCours=AfficherJeuxEnEmprunt(this->ModeleJeuxEmpruntes,this->MembreActif,false,0,0);
    // Affiche les jeux réservés
    AfficherJeuxReserve(this->ModeleJeuxReserves,this->MembreActif,false);
    ui->Bt_SupprimerReservation->setEnabled(false);
    // Affiche le nombre de crédits restants
    CalculerCreditsRestants();
    // Affiche en bas à droite le nombre d'emprunt
    AfficherNbEmpruntsEnCours();

    ActualiserTypeEmprunt(ObtenirValeurParNom(Requete,"TitreMembre_IdTitreMembre").toInt());
    ActualiserListeJeux();

    resizeColumnsToContents(this->ModeleJeuxEmpruntes,ui->Tv_JeuxMembres);
    resizeColumnsToContents(this->ModeleJeuxReserves,ui->Tv_JeuxReserves);
    ui->CBx_TypeEmprunt->setEnabled(true);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////Emprunter les nouveaux Jeux //////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *  @pre    Connexion avec la base de données, un membre est actif dans la classe, un(des) a(ont) été fait(ent)
 *  @post
 *  @param
 *  @retval
 *  @return
 *  @test
 *  @see    NouveauEmprunts, ModeleEmpruntsAValider, NbLignesEmpruntsAValider
 */
void F_Emprunt::EmprunterJeux()
{
    QString IdMalle=NULL;

    if(this->iMode==MODE_MALLES)
    {
        //Création de l'emprunt dans la table des malles
        QSqlQuery RequeteMalle;
        RequeteMalle.prepare("INSERT INTO malles "
                               "(TypeEmprunt_IdTypeEmprunt,Membres_IdMembre,DateEmprunt,DateRetourPrevu,Regle) "
                               "VALUES (:TypeEmprunt_IdTypeEmprunt,:Membres_IdMembre,:DateEmprunt,:DateRetourPrevu,:Regle)");

        RequeteMalle.bindValue(":TypeEmprunt_IdTypeEmprunt",ui->CBx_TypeEmprunt->currentData().toInt());
        RequeteMalle.bindValue(":Membres_IdMembre",NouveauEmprunts[0].idMembre);
        RequeteMalle.bindValue(":DateEmprunt",ui->DtE_Depart->date());
        RequeteMalle.bindValue(":DateRetourPrevu",ui->DtE_Retour->date());
        RequeteMalle.bindValue(":Regle",true);
        if (!RequeteMalle.exec())
        {
            qDebug()<<"F_Emprunt::EmprunterJeux "<<getLastExecutedQuery(RequeteMalle)<<RequeteMalle.lastError();
        }
        IdMalle=RequeteMalle.lastInsertId().toString();
        this->bRegle=false;
        this->AfficherEtatPaiement();
    }
    // Traiter chaque jeu en cours d'emprunt
    for( register int i=0 ; i < NouveauEmprunts.size() ; i++ )
    {
        //Création de l'emprunt dans la table des emprunts
        QSqlQuery RequeteEmprunt;

        RequeteEmprunt.prepare("INSERT INTO emprunts "
                               "(Jeux_IdJeux,Membres_IdMembre,TypeEmprunt_IdTypeEmprunt,"
                                "DateEmprunt,DateRetourPrevu,NbrPrologation,Cautionnement,MontantCaution,Malles_IdMalle) "
                               "VALUES (:Jeux_IdJeux,:Membres_IdMembre,:TypeEmprunt_IdTypeEmprunt,"
                                ":DateEmprunt,:DateRetourPrevu,:NbrPrologation,:Cautionnement,:MontantCaution,:Malles_IdMalle)");

        RequeteEmprunt.bindValue(":Jeux_IdJeux",NouveauEmprunts[i].idJeu);
        RequeteEmprunt.bindValue(":Membres_IdMembre",NouveauEmprunts[i].idMembre);
        RequeteEmprunt.bindValue(":TypeEmprunt_IdTypeEmprunt",NouveauEmprunts[i].idTypeEmprunt);
        RequeteEmprunt.bindValue(":DateEmprunt",NouveauEmprunts[i].DateEmprunt);
        RequeteEmprunt.bindValue(":DateRetourPrevu",NouveauEmprunts[i].DateRetourPrevu);
        RequeteEmprunt.bindValue(":NbrPrologation",0);
        RequeteEmprunt.bindValue(":Malles_IdMalle",IdMalle);
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
            qDebug()<<"F_Emprunt::EmprunterJeux "<<RequeteEmprunt.lastQuery();
        }

        //Mettre le statut du jeux à "Emprunté"
        QSqlQuery RequeteStatut;
        RequeteStatut.prepare("UPDATE jeux SET StatutJeux_IdStatutJeux=3 WHERE IdJeux=:IdDuJeu");
        RequeteStatut.bindValue(":IdDuJeu",NouveauEmprunts[i].idJeu);
        if (!RequeteStatut.exec())
        {
            qDebug()<<"F_Emprunt::EmprunterJeux => requête statut  " << RequeteStatut.lastQuery() ;
        }

        //Virer la résa une fois que le jeu réservé a été emprunté
        //savoir si le jeu en cours de validation d'emprunt était réservé
        QSqlQuery RequeteSuppressionResa;
        RequeteSuppressionResa.prepare("DELETE FROM reservation WHERE idReservation=:IdResa");
        RequeteSuppressionResa.bindValue(":IdResa",NouveauEmprunts[i].idReservation);
        RequeteSuppressionResa.exec();
        if ( ! RequeteSuppressionResa.exec() )
        {
            qDebug()<<"F_Emprunt::EmprunterJeux => RequeteSuppressionResa=" << RequeteSuppressionResa.lastQuery();
        }
    }

    EffacerJeuAValider();
    this->iNbEmpruntEnCours=AfficherJeuxEnEmprunt(this->ModeleJeuxEmpruntes,this->MembreActif,false,0,0);
    AfficherJeuxReserve(this->ModeleJeuxReserves,this->MembreActif,false);
    ui->Bt_SupprimerReservation->setEnabled(false);
    //Met le bouton "Valider les emprunts" en non-cliquable
    ui->Bt_Emprunter->setEnabled(false);
    //Met le bouton  de suppression des nouveaux emprunts en non-cliquable
    ui->Bt_SupprimerEmpruntAValider->setEnabled(false);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////Efface les jeux à valider/////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *  @pre    Connexion avec la base de données
 *  @post
 *  @param
 *  @retval
 *  @return
 *  @test
 *  @see
 */
void F_Emprunt::EffacerJeuAValider()
{
    //Vider le tableau des nouveaux emprunts
    NouveauEmprunts.clear();
    ModeleEmpruntsAValider->clear();
    //Initialise les colones du TableView des nouveaux emprunts
    this->ModeleEmpruntsAValider->setColumnCount(4);
    this->ModeleEmpruntsAValider->setHorizontalHeaderItem(0, new QStandardItem("Nom du jeu"));
    this->ModeleEmpruntsAValider->setHorizontalHeaderItem(1, new QStandardItem("Code"));
    this->ModeleEmpruntsAValider->setHorizontalHeaderItem(2, new QStandardItem("Date Retour"));
    this->ModeleEmpruntsAValider->setHorizontalHeaderItem(3, new QStandardItem("Prix Emprunt"));
    //Initialise les colones du TableView des nouveaux emprunts
    ui->TbV_EmpruntAValider->setColumnWidth(0,200);
    ui->TbV_EmpruntAValider->setColumnWidth(1,40);
    ui->TbV_EmpruntAValider->setColumnWidth(3,50);

    this->NbLignesEmpruntsAValider=0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////Afficher l'état de la cotisation d'un membre/////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *  @pre    Connexion avec la base de données
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
    RequeteCotisation.prepare("SELECT DateExpiration FROM abonnements,membres "
                             "WHERE Prestations_IdPrestation IS NOT NULL AND Membres_IdMembre=IdMembre "
                             "AND CodeMembre=:CodeDuMembre AND Supprimer=0 ORDER BY DateExpiration DESC " );
    RequeteCotisation.bindValue(":CodeDuMembre",CodeMembre );
    if ( ! RequeteCotisation.exec())
    {
        qDebug()<<"F_Emprunt::AfficherEtatCotisation => RequeteCotisation : "<<RequeteCotisation.lastQuery();
    }

    RequeteCotisation.next();

    QDate DateActuelle;
    DateActuelle= DateActuelle.currentDate();

    int NombreJoursRestant ;
    //Nombre de jours qu'il reste  à la cotisation
    NombreJoursRestant=DateActuelle.daysTo(RequeteCotisation.value(0).toDate());

    //si le nombres de jours restant est négatif la cotisation est expirée
    if(NombreJoursRestant<0)
    {
        QString Cotisation;
        //Si le code membre est différent du membre actif, alors
        if(MembreActif!=CodeMembre)
        {
            //On précise le code du membre qui possède la cotisation
            Cotisation="<b>Cotisation expirée (membre "+ CodeMembre +") depuis le </b>";
        }
        //Sinon,
        else
        {
            //On affiche juste l'état de la cotisation
            Cotisation="<b>Cotisation expirée depuis le </b>";
        }
        ui->Lb_Cotisation->setText(Cotisation);
        ui->Lb_Cotisation->setStyleSheet("QLabel {color:red;}");
        ui->Lb_CotisationARemplir->setText("<b>"+RequeteCotisation.value(0).toDate().toString("dd-MM-yyyy")+"</b>");
        ui->Lb_CotisationARemplir->setStyleSheet(" QLabel{color:red;}" );
        EtatDeLaCotisation=false;
    }
    else
    {
        QString Cotisation;
        //Si le code membre est différent du membre actif, alors
        if(MembreActif!=CodeMembre)
        {
            //On précise le code du membre qui possède la cotisation
            Cotisation="<b>Cotisation à jours (membre "+ CodeMembre +")</b>";
        }
        //Sinon,
        else
        {
            //On affiche juste l'état de la cotisation
            Cotisation="<b>Cotisation à jours</b>";
        }
        EtatDeLaCotisation=true;
        QString JourRestant ;
        JourRestant.setNum(NombreJoursRestant);
        ui->Lb_Cotisation->setText(Cotisation);
        ui->Lb_CotisationARemplir->setText("<b>("+ JourRestant +" jours restant)</b>");
        //Si le nombre de jours restants est inférieur à 30 jours, on l'affiche en orange
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
////////////////////////////////////////Recherche du nombre de crédits restants//////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *  @pre    Connexion avec la base de données, un membre est actif dans la classe
 *  @post
 *  @param
 *  @retval
 *  @return
 *  @test
 *  @see    MembreActif, ModeleJeuxEmpruntes
 */
void F_Emprunt::CalculerCreditsRestants()
{
    int CreditRestant(0);

    //rechercher les crédits restants dans toutes les cartes prépayées du membre
    QSqlQuery RequeteCartes;
    RequeteCartes.prepare("SELECT CreditRestant FROM abonnements,cartesprepayees,membres "
                          "WHERE abonnements.Membres_IdMembre=IdMembre "
                          "AND CodeMembre=:CodeDuMembre "
                          "AND abonnements.CartesPrepayees_IdCarte IS NOT NULL "
                          "AND abonnements.CreditRestant >0 "
                          "AND IdCarte=CartesPrepayees_IdCarte AND Supprimer=0");
    RequeteCartes.bindValue(":CodeDuMembre",this->MembreActif);
    if ( ! RequeteCartes.exec())
    {
        qDebug()<<"F_Emprunt::CalculerCreditsRestants => RequeteCartes : " << RequeteCartes.lastQuery();
    }

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
/**
 *  @pre    Connexion avec la base de données, un membre est actif dans la classe
 *  @post
 *  @param
 *  @retval
 *  @return
 *  @test
 *  @see    MembreActif, ModeleJeuxEmpruntes
 */
int F_Emprunt::AfficherJeuxEnEmprunt(QStandardItemModel *ModeleJeuxEmpruntes,QString MembreActif,
                                      bool bRetour,QString *NbreJeuxRendre,QString *AmendeAPayer)
{
    int iNbEmpruntEnCours=0;
    QDate DateActuelle;
    QSqlQuery RequeteIdMembre;
    int Amende=0;
    unsigned int IdDuMembre (0);

    DateActuelle=DateActuelle.currentDate();

    //Prépare le requête pour récupérer l'id du membre
    RequeteIdMembre.prepare("SELECT IdMembre FROM membres WHERE CodeMembre=:CodeDuMembre");
    RequeteIdMembre.bindValue(":CodeDuMembre",MembreActif);

    //Exécute la requête
    if (!RequeteIdMembre.exec())
    {
        qDebug()<< "RequeteIdMembre  "<< getLastExecutedQuery(RequeteIdMembre)<<RequeteIdMembre.lastError();
    }

    RequeteIdMembre.next();

    //Récupère l'id dans la base de données
    IdDuMembre=RequeteIdMembre.value(0).toInt();

    QSqlQuery RequeteJeuEmprunte;
    unsigned int  NumeroLigne =0;
    RequeteJeuEmprunte.prepare("SELECT TypeEmprunt,t.Caution,e.DateEmprunt,e.DateRetourPrevu,NomJeu,CodeJeu,IdMalle,j.PrixLoc,"
                               "m.DateEmprunt as mDateEmprunt,m.DateRetourPrevu as mDateRetourPrevu,m.Remarque FROM emprunts as e "
                               "LEFT JOIN membres as me ON e.Membres_IdMembre=me.IdMembre "
                               "LEFT JOIN jeux as j ON IdJeux=Jeux_IdJeux "
                               "LEFT JOIN malles as m ON e.Malles_IdMalle=m.IdMalle "
                               "LEFT JOIN typeemprunt as t ON m.TypeEmprunt_IdTypeEmprunt=t.IdTypeEmprunt "
                               "WHERE me.CodeMembre=:CodeDuMembre AND IdJeux=Jeux_IdJeux AND e.DateRetour IS NULL");
    RequeteJeuEmprunte.bindValue(":CodeDuMembre",MembreActif);
    if( ! RequeteJeuEmprunte.exec())
    {
        qDebug()<<"RequeteJeuEmprunte : "<<getLastExecutedQuery(RequeteJeuEmprunte)<<RequeteJeuEmprunte.lastError();
    }

    //On vide le modèle
    ModeleJeuxEmpruntes->removeRows(0,ModeleJeuxEmpruntes->rowCount());

    unsigned int NbDeSemainesDeRetard = 0;
    int iCptTableView=0;
    //Tant qu'il y a des jeux,
    while(RequeteJeuEmprunte.next())
    {
        // Liste des valeurs à faire apparaitre dans le modele
        QList<QStandardItem *> List;
        List<<new QStandardItem(ObtenirValeurParNom(RequeteJeuEmprunte,"NomJeu").toString() )
           <<new QStandardItem(ObtenirValeurParNom(RequeteJeuEmprunte,"CodeJeu").toString())
          <<new QStandardItem(ObtenirValeurParNom(RequeteJeuEmprunte,"DateEmprunt").toDate().toString("dd-MM-yyyy") )
         <<new QStandardItem(ObtenirValeurParNom(RequeteJeuEmprunte,"DateRetourPrevu").toDate().toString("dd-MM-yyyy") );
        // S'il s'agit d'un jeu d'une malle
        if(ObtenirValeurParNom(RequeteJeuEmprunte,"IdMalle").toInt()!=0)
        {
            int iIdMalle=ObtenirValeurParNom(RequeteJeuEmprunte,"IdMalle").toInt();
            bool MalleNonTrouve=true;
            // On recherche si la malle existe déjà dans le modele
            for(int i=0;i<ModeleJeuxEmpruntes->rowCount();i++)
            {
                QHash<QString, QVariant> hInfosMalle=ModeleJeuxEmpruntes->item(i,0)->data().value<QHash<QString, QVariant> >();
                if(iIdMalle==hInfosMalle["IdMalle"].toInt())
                {
                    ModeleJeuxEmpruntes->item(i,0)->appendRow(List);
                    MalleNonTrouve=false;
                    break;
                }
            }
            if(MalleNonTrouve)
            {
                QStandardItem *parent = new QStandardItem(ObtenirValeurParNom(RequeteJeuEmprunte,"TypeEmprunt").toString() );

                QSqlRecord record = RequeteJeuEmprunte.record();
                // Liste interne à vListe qui contient toutes les infos sur la malle
                QHash<QString, QVariant> list;
                for(int i=0; i < record.count(); i++)
                {
                    list[record.fieldName(i)] = record.value(i);
                }
                parent->setData(list);
                //on ajoute une nouvelle ligne du table view
                parent->appendRow(List);
                ModeleJeuxEmpruntes->setItem( iCptTableView, 0, parent);
                ModeleJeuxEmpruntes->setItem( iCptTableView, 2, new QStandardItem(ObtenirValeurParNom(RequeteJeuEmprunte,"mDateEmprunt").toDate().toString("dd-MM-yyyy")));
                ModeleJeuxEmpruntes->setItem( iCptTableView, 3, new QStandardItem(ObtenirValeurParNom(RequeteJeuEmprunte,"mDateRetourPrevu").toDate().toString("dd-MM-yyyy")));
                /*if(bRetour)
                {
                    QStandardItem * item=new QStandardItem();
                    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
                    item->setCheckState(Qt::Unchecked);
                    ModeleJeuxEmpruntes->setItem(iCptTableView, 4, item );
                }*/
                iCptTableView++;
            }
        }
        else
        {
            iNbEmpruntEnCours+=ObtenirValeurParNom(RequeteJeuEmprunte,"PrixLoc").toInt();
            //on ajoute une nouvelle ligne du table view
            for(int i=0;i<List.count();i++)
            {
                ModeleJeuxEmpruntes->setItem( iCptTableView, i,List.at(i));
            }
            QDate DateDeRetourPrevue=ObtenirValeurParNom(RequeteJeuEmprunte,"DateRetourPrevu").toDate();
            if(bRetour)
            {
                /*QStandardItem * item=new QStandardItem();
                item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
                item->setCheckState(Qt::Unchecked);
                ModeleJeuxEmpruntes->setItem(iCptTableView, 4, item );*/
                // Ligne en rouge si retard, sinon en vert
                if ( DateDeRetourPrevue.daysTo( QDate::currentDate() ) < F_Preferences::ObtenirValeur("JourRetard").toInt() )
                {  // Pas de retard
                   ModeleJeuxEmpruntes->setData(ModeleJeuxEmpruntes->index(iCptTableView,0),QColor(Qt::green), Qt::BackgroundColorRole);
                   ModeleJeuxEmpruntes->setData(ModeleJeuxEmpruntes->index(iCptTableView,1),QColor(Qt::green), Qt::BackgroundColorRole);
                   ModeleJeuxEmpruntes->setData(ModeleJeuxEmpruntes->index(iCptTableView,2),QColor(Qt::green), Qt::BackgroundColorRole);
                   ModeleJeuxEmpruntes->setData(ModeleJeuxEmpruntes->index(iCptTableView,3),QColor(Qt::green), Qt::BackgroundColorRole);
                }
                else
                {  // En retard
                   ModeleJeuxEmpruntes->setData(ModeleJeuxEmpruntes->index(iCptTableView,0),QColor(Qt::red), Qt::BackgroundColorRole);
                   ModeleJeuxEmpruntes->setData(ModeleJeuxEmpruntes->index(iCptTableView,1),QColor(Qt::red), Qt::BackgroundColorRole);
                   ModeleJeuxEmpruntes->setData(ModeleJeuxEmpruntes->index(iCptTableView,2),QColor(Qt::red), Qt::BackgroundColorRole);
                   ModeleJeuxEmpruntes->setData(ModeleJeuxEmpruntes->index(iCptTableView,3),QColor(Qt::red), Qt::BackgroundColorRole);

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
                   Amende=Amende+F_Preferences::ObtenirValeur("PrixAmende").toFloat() * NbDeSemainesDeRetard ;
                   //qDebug()<<"F_Retour::AfficherJeuxEnEmprunt() => Amende=" << this->Amende << " NbDeSemainesDeRetard=" << NbDeSemainesDeRetard ;
                }
                NumeroLigne++;
                // Affichage du nombre de jeux à rendre
                NbreJeuxRendre->setNum(NumeroLigne);
            }
            else
            {
                if (DateDeRetourPrevue>DateActuelle)
                {
                    ModeleJeuxEmpruntes->setData(ModeleJeuxEmpruntes->
                                 index( NumeroLigne,3),
                                 QColor(Qt::green), Qt::BackgroundColorRole);
                }
                else
                {
                    ModeleJeuxEmpruntes->setData(ModeleJeuxEmpruntes->
                                 index( NumeroLigne,3),
                                 QColor(Qt::red), Qt::BackgroundColorRole);
                }
            }
            iCptTableView++;
        }
        NumeroLigne++;
    }
    if(bRetour)
    {
        AmendeAPayer->clear();
        // Afficher l'amende à payer
        if (Amende > 0)
        {
           AmendeAPayer->setNum(Amende);
        }
    }
    return iNbEmpruntEnCours;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Pression sur Entrée lors du code du jeu////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/** Mettre à jour l'affichage du nombre de jeu en cours d'emprunt aujourd'hui et le nombre maxi empruntable aujourdh'ui
 *  @pre
 *  @post
 */
void F_Emprunt::AfficherNbEmpruntsEnCours()
{
    if(this->iMode==MODE_MALLES)
    {
        ui->Lb_NbEmpruntPossibleAujourdhui->setText(
                    QString::number(this->HashTypeEmprunt[ui->CBx_TypeEmprunt->currentData().toInt()]["TotalCreditsMalle"].toInt()));
    }
    else
    {
        QString NbEmpruntPossibleAujourdhui ;
        NbEmpruntPossibleAujourdhui.setNum(iNbJeuxEmpruntables-iNbEmpruntEnCours);
        ui->Lb_NbEmpruntPossibleAujourdhui->setText( NbEmpruntPossibleAujourdhui );
    }

    iNbNouveauxEmprunts=0;

    // Calcul le nombre de jeux spéciaux et normaux ainsi que le nombre de crédit totaux
    for(int i=0;i<ModeleEmpruntsAValider->rowCount();i++)
    {
        iNbNouveauxEmprunts+=this->ModeleEmpruntsAValider->data(ModeleEmpruntsAValider->index(i,3)).toInt();
    }
    ui->Lb_NbNouveauEmprunt->setText( QString::number(iNbNouveauxEmprunts ));
}

//#######################################################################################################
//////////////////////////////////////////////JEUX///////////////////////////////////////////////////////
//#######################################################################################################

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Pression sur Entrée lors du code du jeu////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *  @pre    Connexion avec la base de données, un code de jeu a été saisi
 *  @post
 *  @param
 *  @retval
 *  @return
 *  @test
 *  @see
 */
void F_Emprunt::on_LE_CodeJeu_returnPressed()
{
    on_LE_SearchJeux_jeuTrouve();
}

void F_Emprunt::on_LE_SearchMembre_MembreTrouve()
{
    this->AfficherMembre(SearchMembre->currentText());
    SearchJeux->setEnabled(true);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Changement du type d'emprunt///////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *  @pre    Connexion avec la base de données
 *  @post
 *  @param  Index du ComboBox
 *  @retval
 *  @return
 *  @test
 *  @see
 */
void F_Emprunt::on_CBx_TypeEmprunt_currentIndexChanged(int index)
{
    if(this->iMode==MODE_MALLES)
    {
        ui->Bt_CalendrierMalles->setEnabled(true);
        //Récupère le nombre de jeux empruntables dans la base de données puis l'affiche
        QString TotalJeuxEmpruntable=HashTypeEmprunt[ui->CBx_TypeEmprunt->itemData(index).toInt()]["TotalJeuxEmpruntable"].toString();
        ui->Lb_NbEmpruntPossibleAujourdhui->setText(TotalJeuxEmpruntable );
        iNbJeuxEmpruntables=TotalJeuxEmpruntable.toInt();
    }
    // Si la sélection ne concerne pas la première ligne vide, on active la recherche des jeux
    //SearchJeux->setEnabled(index!=0);

    this->ActualiserListeJeux();

    int duree=HashTypeEmprunt[ui->CBx_TypeEmprunt->itemData(index).toInt()]["DureeEmprunt"].toInt();

    QDate DateRetour=ui->DtE_Depart->date();
    DateRetour=DateRetour.addDays(duree);

    ui->DtE_Retour->setDate(DateRetour);

    this->AfficherNbEmpruntsEnCours();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Changement de la remarque du membre////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *  @pre    Connexion avec la base de données, un membre est actif dans la classe
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
/////////////////////////////////////////Changement de la remaque du jeu/////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *  @pre    Connexion avec la base de données, un jeu est actif dans la classe
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
////////////////////////////////////Clic sur un jeu chez le membre dans le tableau///////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *  @pre    Connexion avec la base de données, un menbre est actif dans la classe, il a des jeux chez lui
 *  @post
 *  @param
 *  @retval
 *  @return
 *  @test
 *  @see
 */
void F_Emprunt::on_Tv_JeuxMembres_clicked(const QModelIndex &index)
{
    //ui->Tv_JeuxMembres->selectRow(index.row());
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////Clic sur un jeu réservé dans le tableau//////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *  @pre    Connexion avec la base de données, un membre est actif dans la classe, il a des jeux réservés
 *  @post
 *  @param  index du tableau
 *  @retval
 *  @return
 *  @test
 *  @see    ModeleJeuxReserves
 */
void F_Emprunt::on_Tv_JeuxReserves_clicked(const QModelIndex &index)
{
    // Si c'est un jeu d'une malle, on quitte la fonction
    if(index.parent().isValid())
    {
        ui->Bt_SupprimerReservation->setEnabled(false);
        return;
    }
    // Si on a cliqué sur une malle
    if(this->ModeleJeuxReserves->index(index.row(),0).data(Qt::UserRole+1)!=QVariant::Invalid)
    {
        // Demander si on veut procéder à l'emprunt de la malle
        if( ! QMessageBox::warning(0,"Procéder à l'emprunt de la malle","Souhaitez-vous procéder à l'emprunt de la malle ?","Oui","Annuler") )
        {
            emit(Signal_Reservation_Malle(this->ModeleJeuxReserves->index(index.row(),0).data(Qt::UserRole+1).toInt()));
        }
        ui->Bt_SupprimerReservation->setEnabled(true);
    }
    // Si on a cliqué sur un jeu
    else
    {
        this->iIdReservation=this->ModeleJeuxReserves->index(index.row(),1).data(Qt::UserRole+1).toInt();
        // On repasse en mode emprunt
        ui->rB_Mode_Emprunt->setChecked(true);
        // On remets le type d'emprunt à la valeur par défaut
        ui->CBx_TypeEmprunt->setCurrentIndex(0);
        ui->Bt_SupprimerReservation->setEnabled(true);
        //Met le code du jeu sellectionné dans le line édit du code du membre
        SearchJeux->blockSignals(true);
        SearchJeux->setCurrentText(this->ModeleJeuxReserves->index(index.row(),1).data().toString());
        SearchJeux->blockSignals(false);
        this->on_LE_SearchJeux_jeuTrouve();
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////Clic sur un nouvel emprunt dans le tableau///////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *  @pre    Connexion avec la base de données, un membre est actif dans la classe, il y a un nouvel emprunt
 *  @post
 *  @param  index du tableau
 *  @retval
 *  @return
 *  @test
 *  @see
 */
void F_Emprunt::on_TbV_EmpruntAValider_clicked(const QModelIndex &index)
{
    ui->TbV_EmpruntAValider->selectRow(index.row());
    ui->Bt_SupprimerEmpruntAValider->setEnabled(true);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////Suppression d'un jeu réservé/////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *  @pre    Connexion avec la base de données, un jeu est actif dans la classe, il y a un jeu réservé
 *  @post
 *  @param
 *  @retval
 *  @return
 *  @test
 *  @see    ModeleJeuxReserves
 */
void F_Emprunt::on_Bt_SupprimerReservation_clicked()
{
    if(this->SupprimerReservation(ui->Tv_JeuxReserves,this->ModeleJeuxReserves,this->MembreActif))
    {
        //Grise le bouton de suppression du tableau des réservations
        ui->Bt_SupprimerReservation->setEnabled(false);
        // Efface les info sur le jeu actuellement affiché
        this->ViderJeu();
    }
}

bool F_Emprunt::SupprimerReservation(QTreeView *Tv_JeuxReserves,QStandardItemModel *ModeleJeuxReserves,QString MembreActif)
{
    bool IsMalle=true;
    QString Message;
    int i=0;
    QVariant DataEnCours;
    // Si une malle a été sélectionnée
    if(ModeleJeuxReserves->index(Tv_JeuxReserves->currentIndex().row(),0).data(Qt::UserRole+1)!=QVariant::Invalid)
    {
        QString ListeJeux;
        while((DataEnCours=ModeleJeuxReserves->index(Tv_JeuxReserves->currentIndex().row(),0).child(i++,0).data())!=QVariant::Invalid)
        {
            ListeJeux+=DataEnCours.toString()+"\n";
        }
        Message="Voulez-vous supprimer cette réservation de "+
                ModeleJeuxReserves->index(Tv_JeuxReserves->currentIndex().row(),0).data().toString()+
                " ?\nListe des jeux :\n"+ListeJeux;
    }
    else
    {
        IsMalle=false;
        Message="Voulez-vous supprimer cette réservation ?\nNom du jeu :"+
        ModeleJeuxReserves->index(Tv_JeuxReserves->currentIndex().row(),0).data().toString();
    }
    // Demander confirmation pour la suppression de cette réservation
    if( ! QMessageBox::warning(0,"Suppression d'une réservation !",Message,"Supprimer cette réservation","Laisser") )
    {
        if(IsMalle)
        {
            //suppression de la réservation de malle
            QSqlQuery RequeteSupp;
            RequeteSupp.prepare("DELETE FROM malles WHERE IdMalle=:IdMalle");
            RequeteSupp.bindValue(":IdMalle",ModeleJeuxReserves->index(Tv_JeuxReserves->currentIndex().row(),0).data(Qt::UserRole+1).toInt());

            if (!RequeteSupp.exec())
            {
                qDebug()<< getLastExecutedQuery(RequeteSupp) <<RequeteSupp.lastError();
            }
            qDebug()<< getLastExecutedQuery(RequeteSupp) <<RequeteSupp.lastError();

            i=0;
            while((DataEnCours=ModeleJeuxReserves->index(Tv_JeuxReserves->currentIndex().row(),0).child(i++,1).data(Qt::UserRole+1))!=QVariant::Invalid)
            {
                F_Emprunt::SuppressionReservation(DataEnCours.toInt());
            }
        }
        else
        {
            F_Emprunt::SuppressionReservation(ModeleJeuxReserves->index(Tv_JeuxReserves->currentIndex().row(),1).data(Qt::UserRole+1).toInt());
        }
        AfficherJeuxReserve(ModeleJeuxReserves,MembreActif,false);
        return true;
    }
    return false;
}

void F_Emprunt::SuppressionReservation(int iIdReservation)
{
    //suppression de la réservation
    QSqlQuery RequeteSupp;
    RequeteSupp.prepare("DELETE FROM reservation WHERE idReservation=:idReservation");
    RequeteSupp.bindValue(":idReservation",iIdReservation);

    if (!RequeteSupp.exec())
    {
        qDebug()<< getLastExecutedQuery(RequeteSupp) <<RequeteSupp.lastError();
    }

    RequeteSupp.next();
}

//#######################################################################################################
//////////////////////////////////////////////Emprunt////////////////////////////////////////////////////
//#######################################################################################################


/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////Ajouter le jeu aux nouveaux emprunts/////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *  @pre    Connexion avec la base de données, un jeu et un membre est actif dans la classe,
 *  @post
 *  @param
 *  @retval
 *  @return
 *  @test
 *  @see    JeuActif, MembreActif, ModeleEmpruntsAValider, NouveauEmprunts
 */
void F_Emprunt::on_Bt_AjouterJeu_clicked()
{
    //Si la cotisation n'est pas à jour
    if(EtatCotisationMembre==false)
    {
        //Afficher un message indiquant que la cotisation n'est plus à jour et empêcher l'ajout du jeu aux nouveaux emprunts
        if(QMessageBox::critical(this,"Cotisation expirée","Attention ! \nLa cotisation n'est plus à jour.\nIl faut renouveller la cotisation de cet adhérent.\nVoulez-vous quand même autoriser l'emprunt de jeu ?","Autoriser","Refuser"))
        {
            return;
        }
    }
    QSqlQuery RequeteIdJeu;
    unsigned int IdDuJeu (0);

    //Prépare le requête pour récupérer l'id du jeu et son prix
    RequeteIdJeu.prepare("SELECT * FROM jeux WHERE CodeJeu=:CodeDuJeu");
    RequeteIdJeu.bindValue(":CodeDuJeu",this->JeuActif);

    //Exécute la requête
    if (!RequeteIdJeu.exec())
    {
        qDebug()<<"RequeteIdJeu : "<< RequeteIdJeu.lastQuery() ;
    }

    RequeteIdJeu.next();

    //Récupère l'es remarques'id dans la base de données
    IdDuJeu=ObtenirValeurParNom(RequeteIdJeu,"IdJeux").toInt();

    QSqlQuery RequeteIdMembre;
    unsigned int IdDuMembre (0);

    //Prépare le requête pour récupérer l'id du membre
    RequeteIdMembre.prepare("SELECT IdMembre FROM membres WHERE CodeMembre=:CodeDuMembre");
    RequeteIdMembre.bindValue(":CodeDuMembre",this->MembreActif);

    //Exécute la requête
    if (!RequeteIdMembre.exec())
    {
        qDebug()<<"RequeteIdMembre : "<< RequeteIdMembre.lastQuery() ;
    }

    RequeteIdMembre.next();

    //Récupère l'id dans la base de données
    IdDuMembre=RequeteIdMembre.value(0).toInt();
    // Nombre de jeux spéciaux a valider
    int NbJeuxSpeciauxAValider=0;
    // Nombre d'autres jeux a valider
    int NbAutresJeuxAValider=0;
    // Nombre de crédit à demander
    int NbCredits (0);

    //Si le jeu est disponible ou si on est en mode réservation
    if (ui->Le_StatutJeuARemplir->text()=="Disponible"||ui->rB_Mode_Resa->isChecked())
    {
        bool Verification=false;
        QString Message ;
        if(this->iMode==MODE_MALLES)
        {
            // Calcul le nombre de jeux spéciaux et normaux ainsi que le nombre de crédit totaux
            for(int i=0;i<ModeleEmpruntsAValider->rowCount();i++)
            {
                if(ModeleEmpruntsAValider->data(ModeleEmpruntsAValider->index(i,0),Qt::UserRole).toBool())
                {
                    NbJeuxSpeciauxAValider++;
                }
                else
                {
                    NbAutresJeuxAValider++;
                }
                NbCredits+=this->ModeleEmpruntsAValider->data(ModeleEmpruntsAValider->index(i,3)).toInt();
                qDebug()<<NbCredits;
            }
            // Si le nombre de jeux spéciaux est égal ou supérieur au nombre de jeux spéciaux max de la malle, on affiche un message d'avertissement
            if(NbJeuxSpeciauxAValider!=0 && NbJeuxSpeciauxAValider>=
                    HashTypeEmprunt[ui->CBx_TypeEmprunt->currentData().toInt()]["NbJeuxSpeciauxEmpruntable"].toInt())
            {
                Verification=true;
                Message ="Déjà "+Message.setNum( NbJeuxSpeciauxAValider )+" "+
                        F_Preferences::ObtenirValeur("LibelleJeuxSpeciaux")+
                        " dans le panier!\nVoulez-vous quand même autoriser l'emprunt de ce jeu ?";
            }
            // Si le nombre d'autres jeux est égal ou supérieur au nombre d'autres jeux max de la malle, on affiche un message d'avertissement
            else if(NbAutresJeuxAValider!=0 && NbAutresJeuxAValider>=
                    HashTypeEmprunt[ui->CBx_TypeEmprunt->currentData().toInt()]["NbAutresJeuxEmpruntable"].toInt())
            {
                Verification=true;
                Message ="Déjà "+Message.setNum( NbAutresJeuxAValider )+" "+
                        F_Preferences::ObtenirValeur("LibelleAutresJeux")+
                        " dans le panier!\nVoulez-vous quand même autoriser l'emprunt de ce jeu ?";
            }
            // Si le nombre de crédit est égal ou supérieur au nombre de crédit max de la malle, on affiche un message d'avertissement
            else if(NbCredits>=HashTypeEmprunt[ui->CBx_TypeEmprunt->currentData().toInt()]["TotalCreditsMalle"].toInt())
            {
                Verification=true;
                Message ="Déjà "+Message.setNum( NbCredits )+" crédits dans le panier!\nVoulez-vous quand même autoriser l'emprunt de ce jeu ?";
            }

        }
        //Si le nombre de jeux que possède ce membre dépasse le nombre de jeux autorisé,
        else
        {
            int NbrTotalDeJeuxDejaSurCeCompteAdherent=this->iNbEmpruntEnCours+this->iNbNouveauxEmprunts+ui->Le_PrixEmpruntARemplir->text().toInt();
            if(iNbJeuxEmpruntables < NbrTotalDeJeuxDejaSurCeCompteAdherent)
            {
                Verification=true;
                Message ="Déjà "+Message.setNum( NbrTotalDeJeuxDejaSurCeCompteAdherent )+" jeux empruntés !\n"
                        "Voulez-vous quand même autoriser l'emprunt de ce jeu ?";
            }
        }
        //Si le nombre de jeux que possède ce membre dépasse le nombre de jeux autorisé,
        if(Verification && QMessageBox::question(this,"Nombre maximum de jeux/crédits dépassé !",Message,
                                      "Autoriser","Refuser")==1)
        {
            return;  // Mettre fin à cette fonction pour empêcher l'emprunt
        }

        if(!VerifJeuReserve())
        {
            return;
        }
        AjouterJeuAValider(IdDuMembre,IdDuJeu,this->iIdReservation,this->JeuActif.toInt(),ui->CBx_TypeEmprunt->currentData().toInt(),
                           ui->DtE_Retour->date(),
                           ui->Le_PrixEmpruntARemplir->text().toInt(),ui->Le_NomJeuARemplir->text(),ObtenirValeurParNom(RequeteIdJeu,
                           F_Preferences::ObtenirValeur("FiltreJeuxSpeciauxNomChamps")).toString()==
                           F_Preferences::ObtenirValeur("FiltreJeuxSpeciauxValeur"));
    }

    //Calcule du nombre de crédits à demander
    for(register int i=0 ; i <= this->NbLignesEmpruntsAValider ; i++)
    {
        // certains jeux coutent plus de crédit que d'autres pour être empruntés
        NbCredits=NbCredits+ this->ModeleEmpruntsAValider->index(i,2).data().toInt();
    }

    //Si le le prix des nouveaux emprunts est plus cher que les crédits restants, alors
    if ( NbCredits > ui->Le_CreditRestantARemplir->text().toInt() )
    {
        //mettre en rouge le nombre de crédits restants
        ui->Le_CreditRestantARemplir->setStyleSheet("QLineEdit {color:red;}");
        ui->Lb_CreditRestant->setStyleSheet("QLabel {color:red;}");
    }
    else
    {
        //Le mettre en noir
        ui->Le_CreditRestantARemplir->setStyleSheet("QLineEdit {color:black;}");
        ui->Lb_CreditRestant->setStyleSheet("QLabel {color:black;}");
    }
    // Prêt pour permettre un autre emprunt pour cette personne
    SearchJeux->setFocus();
    if(this->iMode==MODE_MALLES)
    {
        ui->CBx_TypeEmprunt->setEnabled(false);
        ui->DtE_Depart->setEnabled(false);
        ui->DtE_Retour->setEnabled(false);
    }
    this->ViderJeu();
    // Affiche en bas à droite le nombre d'emprunt
    AfficherNbEmpruntsEnCours();
    ui->TbV_EmpruntAValider->resizeColumnsToContents();
    //Met le bouton "Valider les emprunts" et "réserver" en cliquable
    ui->Bt_Emprunter->setEnabled(true);
    ui->Bt_Reserver->setEnabled(true);
    this->SearchMembre->setEnabled(false);
    this->SearchJeux->clear();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/** AJouter un jeu à valider
 *  @pre    Connexion avec la base de données, un membre est actif dans la classe, un(des) a(ont) été fait(ent)
 *  @post
 *  @param
 *  @retval
 *  @return
 *  @test
 *  @see
 */
void F_Emprunt::AjouterJeuAValider(int iIdMembre, int iIdJeu, int iIdReservation, int iCodeJeu,int iTypeEmprunt,QDate dDateRetourPrevu,
                                   int iPrixEmprunt,QString sNomJeu,bool bJeuSpecial)
{
    Emprunts Emprunt;
    //Récupération de la date du jour
    QDateTime DateActuelle;
    DateActuelle=DateActuelle.currentDateTime();

    //Remplissage de l'emprunt dans le Vecteur Emprunt
    Emprunt.idJeu= iIdJeu;
    Emprunt.idMembre= iIdMembre;
    Emprunt.DateRetourPrevu= ui->DtE_Retour->dateTime();
    Emprunt.DateEmprunt= DateActuelle;
    Emprunt.idTypeEmprunt= iTypeEmprunt;
    Emprunt.idReservation= iIdReservation;
    Emprunt.PrixEmprunt= iPrixEmprunt;
    Emprunt.PrixCaution= this->PrixCaution;
    this->NouveauEmprunts.push_back(Emprunt);
    ModeleEmpruntsAValider->setItem(this->NbLignesEmpruntsAValider,0, new QStandardItem(sNomJeu));
    ModeleEmpruntsAValider->setItem(this->NbLignesEmpruntsAValider,1, new QStandardItem(QString::number(iCodeJeu)));
    ModeleEmpruntsAValider->setItem(this->NbLignesEmpruntsAValider,2, new QStandardItem( this->NouveauEmprunts[this->NbLignesEmpruntsAValider].DateRetourPrevu.toString("dd-MM-yyyy") ));
    ModeleEmpruntsAValider->setItem(this->NbLignesEmpruntsAValider,3, new QStandardItem(QString::number(iPrixEmprunt)));
    // Stocke l'info de savoir si le jeu est un jeu spécial ou pas
    ModeleEmpruntsAValider->setData(ModeleEmpruntsAValider->index(this->NbLignesEmpruntsAValider,0),QVariant(bJeuSpecial),Qt::UserRole);
    this->NbLignesEmpruntsAValider++;

    //Mettre le statut du jeux à "Emprunt à valider"
    QSqlQuery RequeteStatut;
    RequeteStatut.prepare("UPDATE jeux SET StatutJeux_IdStatutJeux=2 WHERE IdJeux=:IdDuJeu");
    RequeteStatut.bindValue(":IdDuJeu",iIdJeu);

    if ( ! RequeteStatut.exec())
    {
        qDebug()<<"RequeteStatut "<< RequeteStatut.lastQuery() ;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/** Valider les réservations
 *  @pre    Connexion avec la base de données, un membre est actif dans la classe, un(des) a(ont) été fait(ent)
 *  @post
 *  @param
 *  @retval
 *  @return
 *  @test
 *  @see
 */
void F_Emprunt::on_Bt_Reserver_clicked()
{
    QDateTime DateActuelle;
    DateActuelle=DateActuelle.currentDateTime();
    QString IdMalle;

    if(this->iMode==MODE_MALLES)
    {
        int NbCredits (0);
        bool EurosOuCredits=true;
        int TypeVentilation;

        //Calcule du nombre de crédits à demander
        if(this->NonAdherent)
        {
            NbCredits=HashTypeEmprunt[ui->CBx_TypeEmprunt->currentData().toInt()]["PrixNonAdherent"].toInt();
        }
        else
        {
            NbCredits=HashTypeEmprunt[ui->CBx_TypeEmprunt->currentData().toInt()]["PrixAdherent"].toInt();
        }
        EurosOuCredits=HashTypeEmprunt[ui->CBx_TypeEmprunt->currentData().toInt()]["EurosOuCredits"].toBool();
        TypeVentilation=VENTILATION_MALLES;

        // Si la malle n'a pas été réglée
        if(!this->bRegle)
        {
            bool regle=QMessageBox::question(this,"Procéder au paiement ?","Souhaitez-vous procéder au paiement ?",
                                             "Oui","Non")==0;
            if (regle)
            {
                pPaiement->setWindowModality(Qt::ApplicationModal);
                pPaiement->AfficherPaiement(QDateTime::currentDateTime(),this->MembreActif,NbCredits,TypeVentilation,NULL,
                                            NULL,NULL,EurosOuCredits,NULL);
                int nResultat (0);

                nResultat = pPaiement->exec();
                // Si paiement NOK, on sort de la fonction
                if(nResultat!=1)
                {
                    return;
                }
                this->bRegle=true;
                AfficherEtatPaiement();
            }
        }

        // Si il ne s'agit pas de mettre à jour une malle
        if(this->iMalleActive==0)
        {
            //Création de la malle
            QSqlQuery RequeteMalle;
            RequeteMalle.prepare("INSERT INTO malles (TypeEmprunt_IdTypeEmprunt,Membres_IdMembre,DateReservation,"
                                 "DatePrevuEmprunt,DateRetourPrevu,Regle) VALUES (:TypeEmprunt_IdTypeEmprunt,"
                                 ":Membres_IdMembre,:DateReservation,"
                                 ":DatePrevuEmprunt,:DateRetourPrevu,:Regle)");

            RequeteMalle.bindValue(":TypeEmprunt_IdTypeEmprunt",ui->CBx_TypeEmprunt->currentData().toInt());
            RequeteMalle.bindValue(":Membres_IdMembre",this->NouveauEmprunts[0].idMembre);
            RequeteMalle.bindValue(":DateReservation",DateActuelle);
            RequeteMalle.bindValue(":DatePrevuEmprunt",ui->DtE_Depart->dateTime());
            RequeteMalle.bindValue(":DateRetourPrevu",ui->DtE_Retour->dateTime());
            RequeteMalle.bindValue(":Regle",this->bRegle);
            if (!RequeteMalle.exec())
            {
                qDebug()<<"F_Emprunt::EmprunterJeux "<<getLastExecutedQuery(RequeteMalle)<<RequeteMalle.lastError();
            }
            IdMalle=RequeteMalle.lastInsertId().toString();
        }
        else
        {
            IdMalle=QString::number(this->iMalleActive);
            QSqlQuery Requete;
            Requete.prepare("DELETE FROM reservation WHERE Malles_IdMalle="+IdMalle);
            if (!Requete.exec())
            {
                qDebug()<<getLastExecutedQuery(Requete)<<Requete.lastError();
            }
            //Mise à jour de la malle
            QSqlQuery RequeteMalle;
            RequeteMalle.prepare("UPDATE malles SET TypeEmprunt_IdTypeEmprunt=:TypeEmprunt_IdTypeEmprunt,"
                                 "Membres_IdMembre=:Membres_IdMembre,DateReservation=:DateReservation,"
                                 "DatePrevuEmprunt=:DatePrevuEmprunt,DateRetourPrevu=:DateRetourPrevu,Regle=:Regle "
                                 "WHERE IdMalle=:IdMalle");

            RequeteMalle.bindValue(":TypeEmprunt_IdTypeEmprunt",ui->CBx_TypeEmprunt->currentData().toInt());
            RequeteMalle.bindValue(":Membres_IdMembre",this->NouveauEmprunts[0].idMembre);
            RequeteMalle.bindValue(":DateReservation",DateActuelle);
            RequeteMalle.bindValue(":DatePrevuEmprunt",ui->DtE_Depart->dateTime());
            RequeteMalle.bindValue(":DateRetourPrevu",ui->DtE_Retour->dateTime());
            RequeteMalle.bindValue(":Regle",this->bRegle);
            RequeteMalle.bindValue(":IdMalle",IdMalle);
            if (!RequeteMalle.exec())
            {
                qDebug()<<getLastExecutedQuery(RequeteMalle)<<RequeteMalle.lastError();
            }
        }
        // Traiter chaque jeu en cours d'emprunt
        for( register int i=0 ; i < NouveauEmprunts.size() ; i++ )
        {
            //Création de l'emprunt dans la table des emprunts
            QSqlQuery RequeteReservation;
            RequeteReservation.prepare("INSERT INTO reservation (Lieux_IdLieuxReservation,Membres_IdMembre,"
                                       "Jeux_IdJeux,DateReservation,DatePrevuEmprunt,DatePrevuRetour,"
                                       "Lieux_IdLieuxRetrait,ConfirmationReservation,Malles_IdMalle) "
                                       "values (:Lieux_IdLieuxReservation,:IdMembre,:IdJeu,:DateReservation,:DatePrevuEmprunt,:DateRetour,"
                                       ":IdLieuRetrait,1,:Malles_IdMalle)");
            RequeteReservation.bindValue(":Lieux_IdLieuxReservation",F_Preferences::ObtenirValeur("IdLieux").toInt());
            RequeteReservation.bindValue(":IdMembre",NouveauEmprunts[i].idMembre);
            RequeteReservation.bindValue(":IdJeu",NouveauEmprunts[i].idJeu);
            RequeteReservation.bindValue(":IdLieuRetrait",ui->CBx_Retrait->currentData().toInt());
            RequeteReservation.bindValue(":DateReservation",DateActuelle);
            RequeteReservation.bindValue(":DatePrevuEmprunt",ui->DtE_Depart->dateTime());
            RequeteReservation.bindValue(":DateRetour",ui->DtE_Retour->dateTime());
            if(this->iMode==MODE_MALLES)
            {
                RequeteReservation.bindValue(":Malles_IdMalle",IdMalle);
            }

            //Execute la requête
            if(!RequeteReservation.exec())
            {
                qDebug()<<getLastExecutedQuery(RequeteReservation)<<RequeteReservation.lastError();
            }
            //Mettre le statut du jeux à "Disponible"
            QSqlQuery RequeteStatut;
            RequeteStatut.prepare("UPDATE jeux SET StatutJeux_IdStatutJeux=1 WHERE IdJeux=:IdDuJeu");
            RequeteStatut.bindValue(":IdDuJeu",NouveauEmprunts[i].idJeu);
            if (!RequeteStatut.exec())
            {
                qDebug()<<"requête statut  " << getLastExecutedQuery(RequeteStatut)<<RequeteStatut.lastError();
            }
            qDebug()<<"requête statut  " << getLastExecutedQuery(RequeteStatut)<<RequeteStatut.lastError();

        }
        ui->Bt_Reserver->setText("Réserver Malle");
        this->iMalleActive=0;
        ui->DtE_Depart->setEnabled(true);
        ui->DtE_Retour->setEnabled(true);
    }
    else
    {
        if(!VerifJeuReserve())
        {
            return;
        }
        //Création de l'emprunt dans la table des emprunts
        QSqlQuery RequeteReservation;
        RequeteReservation.prepare("INSERT INTO reservation (Lieux_IdLieuxReservation,Membres_IdMembre,"
                                   "Jeux_IdJeux,DateReservation,DatePrevuEmprunt,DatePrevuRetour,"
                                   "Lieux_IdLieuxRetrait,ConfirmationReservation,Malles_IdMalle) "
                                   "values (:Lieux_IdLieuxReservation,(SELECT IdMembre FROM membres WHERE CodeMembre=:CodeMembre),"
                                   "(SELECT IdJeux FROM jeux WHERE CodeJeu=:CodeJeu),:DateReservation,:DatePrevuEmprunt,:DateRetour,"
                                   ":IdLieuRetrait,1,:Malles_IdMalle)");
        RequeteReservation.bindValue(":Lieux_IdLieuxReservation",F_Preferences::ObtenirValeur("IdLieux").toInt());
        RequeteReservation.bindValue(":CodeMembre",this->MembreActif);
        RequeteReservation.bindValue(":CodeJeu",this->JeuActif);
        RequeteReservation.bindValue(":IdLieuRetrait",ui->CBx_Retrait->currentData().toInt());
        RequeteReservation.bindValue(":DateReservation",DateActuelle);
        RequeteReservation.bindValue(":DatePrevuEmprunt",ui->DtE_Depart->dateTime());
        RequeteReservation.bindValue(":DateRetour",ui->DtE_Retour->dateTime());

        //Execute la requête
        if(!RequeteReservation.exec())
        {
            qDebug()<<getLastExecutedQuery(RequeteReservation)<<RequeteReservation.lastError();
        }
        qDebug()<<getLastExecutedQuery(RequeteReservation)<<RequeteReservation.lastError();
    }
    this->AfficherJeuxReserve(this->ModeleJeuxReserves,this->MembreActif,false);
    this->SearchMembre->setEnabled(true);
    ui->CBx_TypeEmprunt->setEnabled(true);
    ui->DtE_Depart->setEnabled(true);
    ui->DtE_Retour->setEnabled(true);
    ui->Bt_Emprunter->setEnabled(false);
    ui->Bt_Reserver->setEnabled(false);
    this->bRegle=false;
    AfficherEtatPaiement();
    EffacerJeuAValider();
    AfficherNbEmpruntsEnCours();
    ActualiserListeJeux();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/** Valider les nouveaux emprunts
 *  @pre    Connexion avec la base de données, un membre est actif dans la classe, un(des) a(ont) été fait(ent)
 *  @post
 *  @param
 *  @retval
 *  @return
 *  @test
 *  @see    NouveauEmprunts, ModeleEmpruntsAValider, NbLignesEmpruntsAValider
 */
void F_Emprunt::on_Bt_Emprunter_clicked()
{
    //Calcule du nombre de crédits à demander
    int NbCredits (0);
    bool EurosOuCredits=true;
    int TypeVentilation;
    if(this->iMode==MODE_MALLES)
    {
        if(this->NonAdherent)
        {
            NbCredits=HashTypeEmprunt[ui->CBx_TypeEmprunt->currentData().toInt()]["PrixNonAdherent"].toInt();
        }
        else
        {
            NbCredits=HashTypeEmprunt[ui->CBx_TypeEmprunt->currentData().toInt()]["PrixAdherent"].toInt();
        }
        EurosOuCredits=HashTypeEmprunt[ui->CBx_TypeEmprunt->currentData().toInt()]["EurosOuCredits"].toBool();
        TypeVentilation=VENTILATION_MALLES;
    }
    else
    {
        for(register int i=0 ; i < this->NbLignesEmpruntsAValider ; i++)
        {
            NbCredits = NbCredits + this->ModeleEmpruntsAValider->index(i,3).data().toInt();
        }
        TypeVentilation=VENTILATION_PRET;
    }

    int nResultat (0);
    // Si la malle a été réglée
    if(this->bRegle&&this->iMode==MODE_MALLES)
    {
        nResultat=1;
    }
    else
    {
        pPaiement->setWindowModality(Qt::ApplicationModal);
        pPaiement->AfficherPaiement(QDateTime::currentDateTime(),this->MembreActif,NbCredits,TypeVentilation,NULL,
                                    NULL,NULL,EurosOuCredits,NULL);

        nResultat = pPaiement->exec();
    }

    // Si paiement nok, on quitte la fonction
    if (nResultat == 0)
    {
        return;
    }
    this->EmprunterJeux();

    // Réactualiser le nombre de crédits restant pour le communiquer à l'adhérent.
    this->CalculerCreditsRestants();
    if ( ui->Le_CreditRestantARemplir->text().toInt() <= 0 )
    {
        //mettre en rouge le nombre de crédits restants
        ui->Le_CreditRestantARemplir->setStyleSheet("QLineEdit {color:red;}");
        ui->Lb_CreditRestant->setStyleSheet("QLabel {color:red;}");
    }
    else
    {
        //Le mettre en noir
        ui->Le_CreditRestantARemplir->setStyleSheet("QLineEdit {color:black;}");
        ui->Lb_CreditRestant->setStyleSheet("QLabel {color:black;}");
    }
    ui->CBx_TypeEmprunt->setEnabled(true);
    this->SearchMembre->setEnabled(true);
    ui->CBx_TypeEmprunt->setEnabled(true);
    ui->DtE_Depart->setEnabled(true);
    ui->DtE_Retour->setEnabled(true);
    ui->Bt_Emprunter->setEnabled(false);
    ui->Bt_Reserver->setEnabled(false);
    this->bRegle=false;
    AfficherEtatPaiement();
    ActualiserListeJeux();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/** Suppression d'un nouvel emprunt
 *  @pre    Connexion avec la base de données,un nouvel emprunt est présent
 *  @post
 *  @param
 *  @retval
 *  @return
 *  @test
 *  @see    ModeleEmpruntsAValider,NouveauEmprunts
 */
void F_Emprunt::on_Bt_SupprimerEmpruntAValider_clicked()
{
    //Mettre le statut du jeu à "Disponible"
    QSqlQuery RequeteStatut;
    RequeteStatut.prepare("UPDATE jeux SET StatutJeux_IdStatutJeux=1 WHERE CodeJeu=:CodeDuJeu");
    RequeteStatut.bindValue( ":CodeDuJeu" , this->ModeleEmpruntsAValider->index(ui->TbV_EmpruntAValider->currentIndex().row(),1).data().toString() );
    if ( ! RequeteStatut.exec())
    {
        qDebug()<<"F_Emprunt::on_Bt_SupprimerEmpruntAValider_clicked => RequeteStatut : "<< RequeteStatut.lastQuery() ;
    }

    // Si ce jeu était un jeu réservé par cet adhérent
    //Recherche de l'id du jeu à modifier
    int IdDuJeu = this->NouveauEmprunts[ui->TbV_EmpruntAValider->currentIndex().row()].idJeu ;
    QSqlQuery RequeteJeuReserve;
    RequeteJeuReserve.prepare("SELECT Membres_IdMembre FROM reservation WHERE JeuEmprunte=0 AND Jeux_IdJeux=:IdDuJeu");
    RequeteJeuReserve.bindValue(":IdDuJeu",IdDuJeu);
    if (!RequeteJeuReserve.exec())
    {
        qDebug()<<"F_Emprunt::on_Bt_SupprimerEmpruntAValider_clicked => RequeteJeuReserve : "<< RequeteJeuReserve.lastQuery() ;
    }
    RequeteJeuReserve.next();
    // Si ce jeu était bien réservé
    if ( RequeteJeuReserve.size() !=0 )
    {
        //Modifier la réservation de ce jeu pour qu'il soit marqué "Disponible"
        //et apparaisse à nouveau dans le tableau des jeux réservés pour cet adhérent
        QSqlQuery RequeteJeuEmprunte ;
        RequeteJeuEmprunte.prepare("UPDATE reservation SET JeuEmprunte=1 WHERE Jeux_IdJeux=:IdDuJeu");
        RequeteJeuEmprunte.bindValue(":IdDuJeu",IdDuJeu);
        if (!RequeteJeuEmprunte.exec())
        {
            qDebug()<<"F_Emprunt::on_Bt_SupprimerEmpruntAValider_clicked => RequeteJeuEmprunte : "<< RequeteJeuEmprunte.lastQuery() ;

        }
        AfficherJeuxReserve(this->ModeleJeuxReserves,this->MembreActif,false);
        ui->Bt_SupprimerReservation->setEnabled(false);
    }

    //Supprimer la ligne du vecteur
    this->NouveauEmprunts.remove(ui->TbV_EmpruntAValider->currentIndex().row());
    this->NbLignesEmpruntsAValider--;

    //Supprime la ligne du tableau
    this->ModeleEmpruntsAValider->removeRow(ui->TbV_EmpruntAValider->currentIndex().row());

    // Si plus de nouvel emprunt car tous supprimés,
    if ( this->NouveauEmprunts.count() == 0)
    {
        //Grise le bouton de validation des nouveaux emprunts
        ui->Bt_Emprunter->setEnabled(false);
        ui->Bt_Reserver->setEnabled(false);
        ui->Bt_Reserver->setText("Reserver Malle");
        ui->CBx_TypeEmprunt->setEnabled(true);
        this->SearchMembre->setEnabled(true);
        ui->Bt_SupprimerEmpruntAValider->setEnabled(false);
        ui->CBx_TypeEmprunt->setEnabled(true);
        ui->DtE_Depart->setEnabled(true);
        ui->DtE_Retour->setEnabled(true);
        this->bRegle=false;
        AfficherEtatPaiement();
    }
    // Affiche en bas à droite le nombre d'emprunt
    AfficherNbEmpruntsEnCours();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Enregister la remarque du membre///////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *  @pre    Connexion avec la base de données, un membre est actif dans la classe, la remarque a été modifiée
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

    //prépare le requête de mise à jour
    Requete.prepare("UPDATE membres SET Remarque=:NouvelRemarque WHERE CodeMembre=:CodeDuMembre");

    //Entre les valeurs de la requête
    Requete.bindValue(":CodeDuMembre",MembreActif);
    Requete.bindValue(":NouvelRemarque",ui->TxE_Remarques->toPlainText());

    if (!Requete.exec())
    {
        qDebug()<<"F_Emprunt::on_Bt_ValiderRemarques_clicked => Requete : "<< Requete.lastQuery() ;

    }
    //Grise les boutons de modification de la remarque
    ui->Bt_ValiderRemarques->setEnabled(false);
    ui->Bt_AnnulerRemarques->setEnabled(false);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Annuler changement de la remaque du membre/////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
*  @pre    Connexion avec la base de données, membre actif dans la classe, remarque modifiée
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
    if (!Requete.exec())
    {
        qDebug()<<"F_Emprunt::on_Bt_AnnulerRemarques_clicked => Requete : "<< Requete.lastQuery() ;

    }

    Requete.next();

    //Récupère les remarques dans la base de données et les affiche
    QString TextTemporaire = (Requete.value(0).toString());
    ui->TxE_Remarques->setText(TextTemporaire);

    //Grise les boutons de modification des remarques du membre
    ui->Bt_ValiderRemarques->setEnabled(false);
    ui->Bt_AnnulerRemarques->setEnabled(false);
}

void F_Emprunt::on_LE_SearchJeux_returnPressed()
{
    if(SearchJeux->currentText()!="")
    {
        on_Bt_AjouterJeu_clicked();
        SearchJeux->setCurrentText("");
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////Valider Jeu et l'afficher dans le volet de droite////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *  @pre    Connexion avec la base de données, un jeu est actif dans la classe
 *  @post
 *  @param
 *  @retval
 *  @return
 *  @test
 *  @see    JeuActif, MembreActif
 */
void F_Emprunt::on_LE_SearchJeux_jeuTrouve()
{
    //Vérification qu'il y a un membre sélectionné.
    QSqlQuery RequeteMembre;
    RequeteMembre.prepare("SELECT CodeMembre FROM membres WHERE CodeMembre=:CodeDuMembre");
    RequeteMembre.bindValue(":CodeDuMembre",this->MembreActif);
    if (!RequeteMembre.exec())
    {
        qDebug()<<"F_Emprunt::on_Bt_OK_clicked => RequeteMembre : "<< RequeteMembre.lastQuery() ;

    }
    RequeteMembre.next();
    if(RequeteMembre.size()==0)
    {
        //On envoie un message d'erreur
        QMessageBox::warning(this,"Pas de membre choisi...","Attention !\nVous n'avez sélectionné aucun membre.","Ok");
        return;
    }

    //Récupère le code du jeu qui a été saisi
    this->JeuActif= SearchJeux->currentText();

    //Si le code entré est nul
    if(this->JeuActif=="")
    {
        //On envoi un message d'erreur
        QMessageBox::warning(this,"Pas de jeu","Attention ! \nVous n'avez entré aucun jeu.","Ok");
        return;
    }

    QSqlQuery Requete;
    //Prépare la requête et entre ses valeurs
    Requete.prepare("SELECT NomJeu,ContenuJeu,PrixLoc,Caution,Remarque,StatutJeux_IdStatutJeux,"
                    "EtatsJeu_idEtatsJeu,IdJeux,StatutJeu FROM jeux as j "
                    "LEFT JOIN statutjeux as s ON s.IdStatutJeux=j.StatutJeux_IdStatutJeux WHERE CodeJeu=:CodeDuJeu");
    Requete.bindValue(":CodeDuJeu",this->JeuActif);
    if (!Requete.exec())
    {
        qDebug()<<"F_Emprunt::on_Bt_OK_clicked => Requete : "<< Requete.lastQuery();

    }
    Requete.next();

    //Récupère le nom du jeu et l'affiche
    ui->Le_NomJeuARemplir->setText(ObtenirValeurParNom(Requete,"NomJeu").toString() ) ;

    //Récupère le prix de la caution et l'enregistre dans PrixCaution
    PrixCaution=Requete.value(1).toInt();

    //Récupère le prix de l'emprunt et l'affiche
    ui->Le_PrixEmpruntARemplir->setText(ObtenirValeurParNom(Requete,"PrixLoc").toString());

    //Récupère la remarque et l'affiche
    ui->TxE_RemarquesJeu->setText(ObtenirValeurParNom(Requete,"Remarque").toString());

    ui->TxE_ContenuBoite->setText(ObtenirValeurParNom(Requete,"ContenuJeu").toString());

    //Grise les boutons de modification de la remarque du jeu
    ui->Bt_ValiderRemarquesJeu->setEnabled(false);
    ui->Bt_AnnulerRemarquesJeu->setEnabled(false);

    QDateTime DateActuelle;
    DateActuelle= DateActuelle.currentDateTime();
    ui->DtE_Depart->setTime(DateActuelle.time());

    //Savoir si le jeu est déja réservé
    QCalendarWidget * Calendrier=new QCalendarWidget();
    QCalendarWidget * Calendrier2=new QCalendarWidget();
    QSqlQuery RequeteResa;
    RequeteResa.prepare("SELECT idReservation,ConfirmationReservation,DatePrevuEmprunt,DatePrevuRetour "
                        "FROM reservation LEFT JOIN jeux ON Jeux_IdJeux=IdJeux "
                        "WHERE CodeJeu=:CodeJeu AND idReservation!=:idReservation");
    RequeteResa.bindValue(":CodeJeu",this->JeuActif);
    RequeteResa.bindValue(":idReservation",this->iIdReservation);

    if (!RequeteResa.exec())
    {
        qDebug() << "RequeteResa :" << getLastExecutedQuery(RequeteResa)<<RequeteResa.lastError();
    }
    /*if(RequeteResa.size()>0 && this->iMode!=MODE_MALLES)
    {
       QMessageBox::warning(this, "Attention !",
           "Une ou plusieurs réservations ont déjà été faite sur ce jeu.\n"
           "Merci de prévenir l'adhérent que le jeu risque de ne pas être disponible "
           "à la date souhaitée.");
    }*/
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

           QTextCharFormat cf2 = Calendrier2->dateTextFormat(date);
           cf2.setFontStrikeOut(true);
           Calendrier2->setDateTextFormat( date, cf2 );

           date=date.addDays(1);
       }
    }

    ui->DtE_Depart->setCalendarWidget(Calendrier);
    ui->DtE_Retour->setCalendarWidget(Calendrier2);

    //Récupère l'id de l'état
    unsigned int IdEtat = ObtenirValeurParNom(Requete,"EtatsJeu_idEtatsJeu").toInt();

    //Récupère l'id du statut
    unsigned int IdStatut = ObtenirValeurParNom(Requete,"StatutJeux_IdStatutJeux").toInt();
    QString NomStatut=ObtenirValeurParNom(Requete,"StatutJeu").toString();

    //suivant le statut
    switch(IdStatut)
    {
        //si le statut est marqué comme "Emprunt à valider", demander s'il faut changer ce statut pour le rendre disponible.
        case 2:
        {
            // Vérifier si le jeu n'est pas déjà dans la liste des emprunts du jour
            // TO DO

            // Afficher un message indiquant que ce jeu n'est pas en statut disponible
            if( ! QMessageBox::warning(this,"Jeu en cours d'emprunt !","Le jeu "+ Requete.value(0).toString()+" est marqué comme 'en cours d'emprunt'.\nPeut être ce jeu est en ce moment en attente d'emprunt sur un autre poste.\nSi c'est une erreur, voulez-vous forcer son statut et rendre ce jeu disponible pour l'emprunt ?","Rendre disponible","Laisser indisponible") )
            {
                // Rendre ce jeu disponible et donc empruntable
                IdStatut = 1 ;
                //prépare le requête de mise à jour du statut
                Requete.prepare("UPDATE jeux SET StatutJeux_IdStatutJeux=1 WHERE CodeJeu=:CodeDuJeu");
                //Entre les valeurs de la requête
                Requete.bindValue(":CodeDuJeu",JeuActif);
                //Exécute la requête
                if (!Requete.exec())
                {
                    qDebug()<< "F_Emprunt::on_Bt_OK_clicked() =>" << Requete.lastQuery() ;
                }
                //Récupère le statut sous forme de nom et l'affiche
                QSqlQuery RequeteStatut;
                RequeteStatut.prepare("SELECT StatutJeu FROM statutjeux WHERE IdStatutJeux=1");
                if (!RequeteStatut.exec())
                {
                    qDebug()<<"RequeteStatut : "<< RequeteStatut.lastQuery() ;

                }
                RequeteStatut.next();
                NomStatut=ObtenirValeurParNom(RequeteStatut,"StatutJeu").toString();
            }
            // SI l'utilisateur ne veut pas modifier le statut du jeu, on ne va pas dans le cas "1"
            else
            {
                break;
            }
        }
        case 1:
        {
            //Le mettre en vert
            ui->Le_StatutJeuARemplir->setStyleSheet("QLineEdit {color:green;}");
            ui->Lb_StatutJeu->setStyleSheet("QLabel {color:green;}");
            //Met le bouton "Ajouté" en cliquable
            ui->Bt_AjouterJeu->setEnabled(true);
            break;
        }
    // pour tous les autres statuts autre que disponible
        default:
        {
            //Le mettre en rouge
            ui->Le_StatutJeuARemplir->setStyleSheet("QLineEdit {color:red;}");
            ui->Lb_StatutJeu->setStyleSheet("QLabel {color:red;}");
            //Met le bouton "Ajouté" en non-cliquable
            ui->Bt_AjouterJeu->setEnabled(false);
            break;
        }
    }

    ui->Le_StatutJeuARemplir->setText(NomStatut);

    // SI mode réservation, on active le bouton de réservation
    if(ui->rB_Mode_Resa->isChecked())
    {
        ui->Bt_Reserver->setEnabled(true);
        if(iMode==MODE_MALLES)
        {
            ui->Bt_AjouterJeu->setEnabled(true);
        }
    }

    //met le focus sur le bouton "Ajouter"
    //ui->Bt_AjouterJeu->setFocus(Qt::TabFocusReason);
    //ui->Bt_AjouterJeu->setFocusPolicy(Qt::StrongFocus);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Enregistrer la remarque du jeu/////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *  @pre    Connexion avec la base de données, un jeu est actif dans la classe, la remarque a été modifiée
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

    //prépare le requête de mise à jour
    Requete.prepare("UPDATE jeux SET Remarque=:NouvelRemarque WHERE CodeJeu=:CodeDuJeu");

    //Entre les valeurs de la requête
    Requete.bindValue(":CodeDuJeu",JeuActif);
    Requete.bindValue(":NouvelRemarque",ui->TxE_RemarquesJeu->toPlainText());
    if (!Requete.exec())
    {
        qDebug()<<"F_Emprunt::on_Bt_ValiderRemarquesJeu_clicked => Requete : "<<Requete.lastQuery() ;
    }

    //Grise les boutons de modification de le remarque
    ui->Bt_ValiderRemarquesJeu->setEnabled(false);
    ui->Bt_AnnulerRemarquesJeu->setEnabled(false);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////Anuler le changement de la remarque du jeu///////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *  @pre    Connexion avec la base de données, un jeu est actif dans la classe, la remarque a été modifiée
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

    //prépare le requête d'annulation
    Requete.prepare("SELECT Remarque FROM jeux WHERE CodeJeu=:CodeDuJeu");
    Requete.bindValue(":CodeDuJeu",JeuActif);
    if (!Requete.exec())
    {
        qDebug()<<"F_Emprunt::on_Bt_AnnulerRemarquesJeu_clicked => Requete : "<<Requete.lastQuery() ;
    }

    Requete.next();

    //Récupère les remarques dans la base de données
    QString TextTemporaire = (Requete.value(0).toString());
    ui->TxE_RemarquesJeu->setText(TextTemporaire);

    //grise les boutons de modification des remarques du jeu
    ui->Bt_ValiderRemarquesJeu->setEnabled(false);
    ui->Bt_AnnulerRemarquesJeu->setEnabled(false);
}

void F_Emprunt::on_Bt_CalendrierMalles_clicked()
{
    pMalles->setWindowModality(Qt::NonModal);
    pMalles->AfficherCalendrier(this->geometry());
    int nResultat (0);

    nResultat = pMalles->exec();
    // Si paiement NOK, on sort de la fonction
    if(nResultat!=1)
    {
        return;
    }
}

/**
 * Quand clic sur le bouton Emprunter dans la fenetre de calendrier des malles, affiche le contenu de lam alle
 * @param iIdMalle
 */
void F_Emprunt::slot_Clic_Emprunter(int iIdMalle)
{
    this->AfficherMalle(iIdMalle);
}

void F_Emprunt::on_DtE_Retour_editingFinished()
{
    this->ActualiserListeJeux();
}

//! Renvoie l'ID de la malle réservée sélectionnée
int F_Emprunt::get_MalleReserveeSelectionnee()
{
    return this->ModeleJeuxReserves->data(ui->Tv_JeuxReserves->currentIndex(),Qt::UserRole+1).toInt();
}

//! Renvoie l'ID de la malle empruntée sélectionnée
int F_Emprunt::get_MalleEmprunteeSelectionnee()
{
    QHash<QString, QVariant> hInfosMalle=ModeleJeuxEmpruntes->data(ui->Tv_JeuxMembres->currentIndex(),Qt::UserRole+1).value<QHash<QString, QVariant> >();
    return hInfosMalle["IdMalle"].toInt();
}

void F_Emprunt::on_Bt_AjoutNonAdherent_clicked()
{
    F_Membres* pMembres;
    pMembres = new F_Membres(MODE_NON_ADHERENT,0);
    connect(pMembres,SIGNAL(Signal_Non_Adherent_Cree(int)),this,SLOT(slot_Non_Adherent_Cree(int)));
    pMembres->on_Bt_AjouterMembre_clicked();
    pMembres->setWindowModality(Qt::ApplicationModal);
    pMembres->showMaximized();
}

void F_Emprunt::slot_Non_Adherent_Cree(int iCodeMembre)
{
    MaJListeMembres();
    SearchMembre->MAJResults(this->VecteurMembres,3);
    SearchMembre->setCurrentText(QString::number(iCodeMembre));
    AfficherMembre(QString::number(iCodeMembre));
}

void F_Emprunt::on_rB_Mode_Emprunt_toggled(bool checked)
{
    this->SearchJeux->clear();
    this->ViderJeu();
    // Si mode emprunt
    if(checked)
    {
        ui->DtE_Depart->setVisible(false);
        ui->Lb_Depart->setVisible(false);
        ui->TbV_EmpruntAValider->setEnabled(true);
        ui->Bt_AjouterJeu->setVisible(true);
        ui->Bt_Regle->setVisible(true);
        ui->Bt_Emprunter->setVisible(true);
        ui->Lb_LieuRetrait->setVisible(false);
        ui->CBx_Retrait->setVisible(false);
        ui->Bt_Reserver->setVisible(false);
    }
    // Si mode réservation
    else
    {
        ui->DtE_Depart->setVisible(true);
        ui->Lb_Depart->setVisible(true);
        if(iMode!=MODE_MALLES)
        {
            ui->TbV_EmpruntAValider->setEnabled(false);
            ui->Bt_AjouterJeu->setVisible(false);
        }
        ui->Bt_Regle->setVisible(false);
        ui->Bt_Emprunter->setVisible(false);
        ui->Lb_LieuRetrait->setVisible(true);
        ui->CBx_Retrait->setVisible(true);
        ui->Bt_Reserver->setVisible(true);
    }
}

void F_Emprunt::on_DtE_Depart_dateChanged(const QDate &date)
{
    ui->Bt_Emprunter->setEnabled(false);
    on_CBx_TypeEmprunt_currentIndexChanged(ui->CBx_TypeEmprunt->currentIndex());
}

bool F_Emprunt::VerifJeuReserve()
{
    //Vérifier si ce jeu est réservé
    //Recherche de l'id du membre qui a réservé le jeu
    QSqlQuery RequeteJeuReserve;
    RequeteJeuReserve.prepare("SELECT Membres_IdMembre,ConfirmationReservation,DatePrevuEmprunt,DatePrevuRetour FROM reservation as r "
                              "LEFT JOIN jeux as j ON j.IdJeux=Jeux_IdJeux "
                              "WHERE DATE(DatePrevuEmprunt)<'"+
                              ui->DtE_Retour->date().toString("yyyy-MM-dd")+"' AND DATE(DatePrevuRetour)>'"+
                              ui->DtE_Depart->date().toString("yyyy-MM-dd")+"' AND CodeJeu=:CodeJeu");
    RequeteJeuReserve.bindValue(":CodeJeu",this->JeuActif);
    if (!RequeteJeuReserve.exec())
    {
        qDebug()<<"RequeteJeuReserve : "<< getLastExecutedQuery(RequeteJeuReserve) << RequeteJeuReserve.lastError();
    }
    RequeteJeuReserve.next();
    // Si ce jeu est réservé
    if ( RequeteJeuReserve.size() !=0 )
    {
        //si l'id du membre actuellement sélectionné n'est le même que celui du réserveur,
        if ( ObtenirValeurParNom(RequeteJeuReserve,"Membres_IdMembre").toString() != this->MembreActif )
        {
            QString sMessage;
            if(ObtenirValeurParNom(RequeteJeuReserve,"ConfirmationReservation").toInt()==1)
            {
                sMessage="Le jeu "+ui->Le_NomJeuARemplir->text()
                        +" est réservé à aux dates "+ObtenirValeurParNom(RequeteJeuReserve,"DatePrevuEmprunt").toDate().toString("dd-MM-yy")+" au "+
                        ObtenirValeurParNom(RequeteJeuReserve,"DatePrevuRetour").toDate().toString("dd-MM-yy")+", êtes-vous sûr de vouloir l'emprunter ou le réserver?";
                if(QMessageBox::question(this, "Jeu réservé", sMessage, "Oui", "Non") == 0)
                {
                    return false;
                }
            }
            else
            {
                sMessage="Le jeu "+ui->Le_NomJeuARemplir->text()+
                        " est en attente de confirmation de réservation.\n"+
                        "Soit la personne validera la réservation, "+
                        "soit celle-ci sera automatiquement supprimée.";
                // Le jeu est réservé mais pas par l'adhérent sélectionné actuellement
                QMessageBox::warning(this,"Jeu réservé !",sMessage,"Ok");
                return false;  // Mettre fin à cette fonction pour empêcher l'emprunt pour ce jeu
            }
        }
        else
        {
            //Modifier la réservation de ce jeu pour qu'il ne soit plus marqué "Emprunté"
            //et n'apparaisse plus dans les jeux réservés pour cet adhérent
            QSqlQuery RequeteJeuEmprunte;
            RequeteJeuEmprunte.prepare("UPDATE reservation SET JeuEmprunte=0 LEFT JOIN jeux as j ON j.IdJeux=Jeux_IdJeux WHERE CodeJeu=:CodeJeu");
            RequeteJeuEmprunte.bindValue(":CodeJeu",this->JeuActif);
            if (!RequeteJeuEmprunte.exec())
            {
                qDebug()<<"RequeteJeuEmprunte : "<< RequeteJeuEmprunte.lastQuery() ;

            }
            AfficherJeuxReserve(this->ModeleJeuxReserves,this->MembreActif,false);
            ui->Bt_SupprimerReservation->setEnabled(false);
        }
    }
    return true;
}
