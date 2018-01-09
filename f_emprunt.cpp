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

    pPaiement=new F_Paiement;
    MembreActif="";
    JeuActif="";
    //ui->TbV_Recherche->setVisible(false);
    ui->Lb_Cotisation->setText("");
    ui->Lb_CotisationARemplir->setText("");
    ui->Lb_MembreEcarte->setText("");

    //Met une date minimum pour le DateEdit du retour (la date du jour)
    QDateTime DateActuelle;
    DateActuelle=DateActuelle.currentDateTime();
    ui->DtE_Retour->setMinimumDate(DateActuelle.date());

    //Met une date minimum pour le DateEdit du départ (la date du jour)
    ui->DtE_Depart->setMinimumDate(DateActuelle.date());
    ui->DtE_Depart->setDate(DateActuelle.date());

    MaJListeMembres();

    SearchMembre = new SearchBox(this);
    SearchMembre->MAJResults(this->VecteurMembres,3);
    SearchMembre->show();
    ui->horizontalLayout->addWidget(SearchMembre);

    connect(SearchMembre,SIGNAL(SignalSuggestionFini()),this,SLOT(on_LE_SearchMembre_MembreTrouve()));

    SearchJeux = new SearchBox(this);
    this->iMode=iMode;
    if(iMode==MODE_MALLES)
    {
        SearchJeux->MAJResults(MaJListeJeux(),2);
        ui->Bt_Reservation->setText("Réserver Malle");
        ui->Bt_ValiderEmprunt->setVisible(false);
    }
    else
    {
        ui->Bt_Reservation->setText("Réserver Jeux");
    }
    SearchJeux->show();
    ui->Hlay16->addWidget(SearchJeux);

    connect(SearchJeux,SIGNAL(SignalSuggestionFini()),this,SLOT(on_LE_SearchJeux_jeuTrouve()));
    connect(SearchJeux,SIGNAL(returnPressed()),this,SLOT(on_LE_SearchJeux_returnPressed()));

    // On rend visible les composants de réservation de malle si on est dans l'onglet correspondant
    if(iMode==MODE_MALLES)
    {
        RendreVisibleMalle(true);
        SearchJeux->setEnabled(false);
    }
    else
    {
        QSqlQuery Requete;
        QString TypeEmprunt;

        if ( !  Requete.exec("SELECT TypeEmprunt,DureeEmprunt FROM typeemprunt") )
        {
            qDebug()<<"F_Emprunt::F_Emprunt => Requete "<<Requete.lastQuery();
        }
        //Tant qu'il y a des types d'emprunt dans la table TypesEmprunt,
        while(Requete.next())
        {
            //on entre un nouveau Item au ComboBox avec le nom du type d'emprunt
            TypeEmprunt=(Requete.value(0).toString())+" ("+(Requete.value(1).toString())+" jours)";
            ui->CBx_TypeEmprunt->addItem(TypeEmprunt);
        }
    }

    //Création d'un modèle pour le TableView des nouveaux emprunts
    this->ModeleEmpruntsAValider = new QStandardItemModel() ;
    //Associe le modèle au TableView
    ui->TbV_EmpruntAValider->setModel(this->ModeleEmpruntsAValider);
    //Met le TableView en lecture seule
    ui->TbV_EmpruntAValider->setEditTriggers(0);
    // Autorise le tri pour ce tableau
    ui->TbV_EmpruntAValider->setSortingEnabled(true);

    //Initialise les colones du TableView des nouveaux emprunts
    this->ModeleEmpruntsAValider->setColumnCount(4);
    this->ModeleEmpruntsAValider->setHorizontalHeaderItem(0, new QStandardItem("Code"));
    this->ModeleEmpruntsAValider->setHorizontalHeaderItem(1, new QStandardItem("Nom du jeu ou de la malle"));
    this->ModeleEmpruntsAValider->setHorizontalHeaderItem(2, new QStandardItem("Date Retour"));
    this->ModeleEmpruntsAValider->setHorizontalHeaderItem(3, new QStandardItem("Prix"));
    ui->TbV_EmpruntAValider->setColumnWidth(0,40);
    ui->TbV_EmpruntAValider->setColumnWidth(1,200);
    ui->TbV_EmpruntAValider->setColumnWidth(3,50);

    this->NbLignesEmpruntsAValider=0;

    //Création d'un modèle pour le TableView des jeux empruntés
    this->ModeleJeuxEmpruntes = new QStandardItemModel() ;
    //Associe le mod7l au TableView
    ui->TbV_JeuxMembres->setModel(this->ModeleJeuxEmpruntes);
    //Met le TableView en lecture seule
    ui->TbV_JeuxMembres->setEditTriggers(0);
    // Autorise le tri pour ce tableau
    ui->TbV_JeuxMembres->setSortingEnabled(true);
    //Initialise les colones du TableView des nouveaux emprunts
    this->ModeleJeuxEmpruntes->setColumnCount(4);
    this->ModeleJeuxEmpruntes->setHorizontalHeaderItem(0, new QStandardItem("Code"));
    this->ModeleJeuxEmpruntes->setHorizontalHeaderItem(1, new QStandardItem("Nom du jeu"));
    this->ModeleJeuxEmpruntes->setHorizontalHeaderItem(2, new QStandardItem("Date emprunt"));
    this->ModeleJeuxEmpruntes->setHorizontalHeaderItem(3, new QStandardItem("Date retour"));
    ui->TbV_JeuxMembres->setColumnWidth(0,40);
    ui->TbV_JeuxMembres->setColumnWidth(1,125);

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
    this->ModeleJeuxReserves->setHorizontalHeaderItem(0, new QStandardItem("Code"));
    this->ModeleJeuxReserves->setHorizontalHeaderItem(1, new QStandardItem("Nom du jeu"));
    this->ModeleJeuxReserves->setHorizontalHeaderItem(2, new QStandardItem("Date réservation"));
    this->ModeleJeuxReserves->setHorizontalHeaderItem(3, new QStandardItem("Date emprunt"));
    this->ModeleJeuxReserves->setHorizontalHeaderItem(4, new QStandardItem("Date retour"));
    this->ModeleJeuxReserves->setHorizontalHeaderItem(5, new QStandardItem("lieu"));
    ui->Tv_JeuxReserves->setColumnWidth(0,40);
    ui->Tv_JeuxReserves->setColumnWidth(1,125);

    // Bloque la saisie de code jeu à emprunter et du bouton OK tant que pas d'adhérent sélectionné
    ui->DtE_Retour->setDisplayFormat("dd/MM/yyyy");
    ui->DtE_Depart->setDisplayFormat("dd/MM/yyyy");
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

/**   Mise  jour de la liste des membres
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
    //Prépare la requête
    if(filtre!="")
    {
        filtre=" WHERE "+filtre;
    }
    Requete.prepare("SELECT NomJeu,CodeJeu FROM jeux"+filtre);

    //Execute une requète sql qui retourne la liste des jeux
    //Si la requète est correcte -> Remplissage du veteur VecteurJeux
    // avec le résultat de la requête et on retourne vrai.
    if(Requete.exec())
    {
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
    else //Sinon on affiche un message d'erreur et on retourne Faux
    {
        qDebug()<< "F_Emprunt::MaJListeJeux() : Erreur de connexion avec la base de donnée !" << endl ;
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
    ui->Bt_Ajouter->setEnabled(false);
    // Plus de jeu actif sélectionné
    this->JeuActif = "" ;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////Calcule de la Date de retout /////////////////////////////
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
    if ( ! Requete.exec() )
    {
        qDebug()<<"F_Emprunt::CalculerDateRetour => Requete "<<Requete.lastQuery();
        return;
    }
    Requete.next();

    DateRetour=DateRetour.addDays(Requete.value(0).toInt());

    ui->DtE_Retour->setDate(DateRetour);

}

//! Actualise le combobox des types de malles
void F_Emprunt::ActualiserTypeMalles(int iTitreMembre)
{
    //On prépare et exécute la requête qui permet de récupérer la durée dy type d'emprunt
    QSqlQuery Requete;

    Requete.prepare("SELECT * FROM typemalles WHERE TitreMembre_IdTitreMembre=:titremembre");
    Requete.bindValue(":titremembre",iTitreMembre);
    Requete.exec();
    if ( ! Requete.exec() )
    {
        qDebug()<<"F_Emprunt::ActualiserTypeMalles => Requete "<<Requete.lastQuery();
        return;
    }
    ui->CBx_TypeMalle->clear();
    ui->CBx_TypeMalle->addItem("");
    QHash<QString, QVariant> HashRecord;
    while(Requete.next())
    {
        for(int i=0;i<Requete.record().count();i++)
        {
            QString tmp=Requete.record().fieldName(i);
            int valeur=Requete.value(i).toInt();
            HashRecord[Requete.record().fieldName(i)]=Requete.value(i).toInt();
            HashTypeMalle[ObtenirValeurParNom(Requete,"IdTypeMalles").toInt()]=HashRecord;
        }
        ui->CBx_TypeMalle->addItem(ObtenirValeurParNom(Requete,"NomTypeMalles").toString(),
                                   ObtenirValeurParNom(Requete,"IdTypeMalles").toInt());
    }
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
        SearchJeux->setText(this->JeuActif);
        on_LE_SearchJeux_jeuTrouve();
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
void F_Emprunt::AfficherJeuxReserve()
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
    unsigned int  NbrTotalDeJeuxDejaSurCeCompteAdherent =0;
    RequeteJeuReserve.prepare("SELECT DateReservation,DatePrevuEmprunt,DatePrevuRetour,NomLieux,NomJeu,"
                              "CodeJeu FROM reservation,lieux,jeux WHERE JeuEmprunte=1 AND "
                              "Membres_IdMembre=:IdMembre AND IdLieux=Lieux_IdLieuxReservation AND "
                              "IdJeux=Jeux_IdJeux" );

    RequeteJeuReserve.bindValue(":IdMembre",IdDuMembre);

    if (!RequeteJeuReserve.exec())
    {
        qDebug()<<"F_Emprunt::JeuxReserve => Requete Jeux Reservés "<<RequeteJeuReserve.lastQuery();
    }

    //On vide le modèle
    this->ModeleJeuxReserves->clear();
    //Initialise les colones du TableView des nouveaux emprunts
    this->ModeleJeuxReserves->setColumnCount(5);
    this->ModeleJeuxReserves->setHorizontalHeaderItem(0, new QStandardItem("Code"));
    this->ModeleJeuxReserves->setHorizontalHeaderItem(1, new QStandardItem("Nom du jeu"));
    this->ModeleJeuxReserves->setHorizontalHeaderItem(2, new QStandardItem("Date réservation"));
    this->ModeleJeuxReserves->setHorizontalHeaderItem(3, new QStandardItem("Date emprunt"));
    this->ModeleJeuxReserves->setHorizontalHeaderItem(4, new QStandardItem("Date retour"));
    this->ModeleJeuxReserves->setHorizontalHeaderItem(5, new QStandardItem("lieu"));
    ui->Tv_JeuxReserves->setColumnWidth(0,40);
    ui->Tv_JeuxReserves->setColumnWidth(1,125);

    //Tant qu'il y a des membres dans la table membres,
    while(RequeteJeuReserve.next())
    {
        //on ajoute une nouvelle ligne du table view
        this->ModeleJeuxReserves->setItem( NbrTotalDeJeuxDejaSurCeCompteAdherent, 0, new QStandardItem(RequeteJeuReserve.value(5).toString() ));
        this->ModeleJeuxReserves->setItem( NbrTotalDeJeuxDejaSurCeCompteAdherent, 1, new QStandardItem(RequeteJeuReserve.value(4).toString()));
        this->ModeleJeuxReserves->setItem( NbrTotalDeJeuxDejaSurCeCompteAdherent, 2, new QStandardItem(RequeteJeuReserve.value(0).toDate().toString("dd-MM-yyyy") ));
        this->ModeleJeuxReserves->setItem( NbrTotalDeJeuxDejaSurCeCompteAdherent, 3, new QStandardItem(RequeteJeuReserve.value(1).toDate().toString("dd-MM-yyyy") ));
        this->ModeleJeuxReserves->setItem( NbrTotalDeJeuxDejaSurCeCompteAdherent, 4, new QStandardItem(RequeteJeuReserve.value(2).toDate().toString("dd-MM-yyyy") ));
        this->ModeleJeuxReserves->setItem( NbrTotalDeJeuxDejaSurCeCompteAdherent, 5, new QStandardItem(RequeteJeuReserve.value(3).toString() ));

        //Savoir si le jeu est disponible ou non
        QSqlQuery Requetejeu;
        Requetejeu.prepare("SELECT IdEmprunts FROM emprunts,jeux WHERE CodeJeu=:CodeDuJeu AND Jeux_IdJeux=IdJeux AND DateRetour IS NULL");
        Requetejeu.bindValue(":CodeDuJeu",RequeteJeuReserve.value(5).toString());

        if ( ! Requetejeu.exec() )
        {
            qDebug()<<"F_Emprunt::AfficherJeuxReserve => Requetejeu "<<Requetejeu.lastQuery();
        }

        if(Requetejeu.size()==0)
        {
            this->ModeleJeuxReserves->setData(ModeleJeuxReserves->index( NbrTotalDeJeuxDejaSurCeCompteAdherent,3),QColor(Qt::green), Qt::BackgroundColorRole);
        }
        else
        {
            this->ModeleJeuxReserves->setData(ModeleJeuxReserves->index( NbrTotalDeJeuxDejaSurCeCompteAdherent,3),QColor(Qt::red), Qt::BackgroundColorRole);
        }

        NbrTotalDeJeuxDejaSurCeCompteAdherent++;
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

    if(CodeMembre != "")
    {
        //Grise les boutons de modification des remarques du membre
        ui->Bt_ValiderRemarques->setEnabled(false);
        ui->Bt_AnnulerRemarques->setEnabled(false);

        //Prépare la requête
        QSqlQuery Requete;
        Requete.prepare("SELECT Nom,Prenom,NbreRetard,Ecarte,Remarque,NbreJeuxAutorises,IdMembre,TitreMembre_IdTitreMembre FROM membres WHERE CodeMembre=:CodeDuMembre");
        Requete.bindValue(":CodeDuMembre",CodeMembre);
        if ( ! Requete.exec() )
        {
            qDebug()<<"F_Emprunt::AfficherMembre => Requete " << Requete.lastQuery();
        }
        Requete.next();

        //Récupère le Nom dans la base de données puis l'affiche
        ui->Le_NomARemplir->setText(Requete.value(0).toString());

        //Récupère le Prénom dans la base de données puis l'affiche
        ui->Le_PrenomARemplir->setText(Requete.value(1).toString());

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

        //Récupère les remarques dans la base de données puis les affiches
        ui->TxE_Remarques->setText(Requete.value(4).toString());

        //Grise les boutons de modification des remarques du membre
        ui->Bt_ValiderRemarques->setEnabled(false);
        ui->Bt_AnnulerRemarques->setEnabled(false);

        //Grise les bouons de suppression des tableau des jeux
        ui->Bt_SupprimerEmpruntAValider->setEnabled(false);
        ui->Bt_SupprimerReservation->setEnabled(false);

        //Récupère le nombre de jeux empruntables dans la base de données puis l'affiche
        ui->Le_NbJeuxEmpruntables->setText(Requete.value(5).toString());

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
        AfficherJeuxEnEmprunt();
        // Affiche les jeux réservés
        AfficherJeuxReserve();
        // Affiche le nombre de crédits restants
        CalculerCreditsRestants();
        // Affiche en bas à droite le nombre d'emprunt
        AfficherNbEmpruntsEnCours();
        ui->CBx_TypeMalle->setEnabled(true);
        ActualiserTypeMalles(ObtenirValeurParNom(Requete,"TitreMembre_IdTitreMembre").toInt());
    }
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
    //Met le bouton "Valider les emprunts" en non-cliquable
    ui->Bt_ValiderEmprunt->setEnabled(false);
    //Met le bouton  de suppression des nouveaux emprunts en non-cliquable
    ui->Bt_SupprimerEmpruntAValider->setEnabled(false);

    // Traiter chaque jeu en cours d'emprunt
    for( register int i=0 ; i < NouveauEmprunts.size() ; i++ )
    {
        //Création de l'emprunt dans la table des emprunts
        QSqlQuery RequeteEmprunt;

        RequeteEmprunt.prepare("INSERT INTO emprunts "
                               "(Jeux_IdJeux,Membres_IdMembre,TypeEmprunt_IdTypeEmprunt,"
                                "ModePaiement_IdModePaiement,DateEmprunt,DateRetourPrevu,NbrPrologation,Cautionnement,MontantCaution) "
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
        QSqlQuery RequeteResa;
        RequeteResa.prepare("SELECT idReservation FROM reservation WHERE Jeux_IdJeux=:IdDuJeu");
        RequeteResa.bindValue(":IdDuJeu",NouveauEmprunts[i].idJeu);
        if ( ! RequeteResa.exec() )
        {
            qDebug()<<"F_Emprunt::EmprunterJeux => RequeteResa=" << RequeteResa.lastQuery();
        }
        RequeteResa.next();
        if ( RequeteResa.size() != 0 )
        {
            QSqlQuery RequeteSuppressionResa;
            RequeteSuppressionResa.prepare("DELETE FROM reservation WHERE idReservation=:IdResa");
            RequeteSuppressionResa.bindValue(":IdResa",RequeteResa.value(0));
            RequeteSuppressionResa.exec();
            if ( ! RequeteSuppressionResa.exec() )
            {
                qDebug()<<"F_Emprunt::EmprunterJeux => RequeteSuppressionResa=" << RequeteSuppressionResa.lastQuery();
            }
        }
    }

    //Vider le tableau des nouveaux emprunts
    NouveauEmprunts.clear();
    ModeleEmpruntsAValider->clear();
    //Initialise les colones du TableView des nouveaux emprunts
    this->ModeleEmpruntsAValider->setColumnCount(4);
    this->ModeleEmpruntsAValider->setHorizontalHeaderItem(0, new QStandardItem("Code"));
    this->ModeleEmpruntsAValider->setHorizontalHeaderItem(1, new QStandardItem("Nom du jeu"));
    this->ModeleEmpruntsAValider->setHorizontalHeaderItem(2, new QStandardItem("Date Retour"));
    this->ModeleEmpruntsAValider->setHorizontalHeaderItem(3, new QStandardItem("Prix Emprunt"));
    ui->TbV_EmpruntAValider->setColumnWidth(0,40);
    ui->TbV_EmpruntAValider->setColumnWidth(1,100);

    this->NbLignesEmpruntsAValider=0;

    AfficherJeuxEnEmprunt();
    AfficherJeuxReserve();
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
                          "AND IdCarte=CartesPrepayees_IdCarte ");
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
void F_Emprunt::AfficherJeuxEnEmprunt()
{
    QDate DateActuelle;
    QSqlQuery RequeteIdMembre;
    unsigned int IdDuMembre (0);

    DateActuelle=DateActuelle.currentDate();

    //Prépare le requête pour récupérer l'id du membre
    RequeteIdMembre.prepare("SELECT IdMembre FROM membres WHERE CodeMembre=:CodeDuMembre");
    RequeteIdMembre.bindValue(":CodeDuMembre",MembreActif);

    //Exécute la requête
    if (!RequeteIdMembre.exec())
    {
        qDebug()<< "F_Emprunt::AfficherJeuxEnEmprunt => RequeteIdMembre  "<< RequeteIdMembre.lastQuery();
    }

    RequeteIdMembre.next();

    //Récupère l'id dans la base de données
    IdDuMembre=RequeteIdMembre.value(0).toInt();

    QSqlQuery RequeteJeuEmprunte;
    unsigned int  NbrTotalDeJeuxDejaSurCeCompteAdherent =0;
    RequeteJeuEmprunte.prepare("SELECT DateEmprunt,DateRetourPrevu,NomJeu,CodeJeu FROM emprunts,jeux "
                               "WHERE Membres_IdMembre=:IdDuMembre AND IdJeux=Jeux_IdJeux AND "
                               "DateRetour IS NULL");
    RequeteJeuEmprunte.bindValue(":IdDuMembre",IdDuMembre);
    if( ! RequeteJeuEmprunte.exec())
    {
        qDebug()<<"F_Emprunt::AfficherJeuxEnEmprunt => RequeteJeuEmprunte : "<<RequeteJeuEmprunte.lastQuery();
    }

    //On vide le modèle
    this->ModeleJeuxEmpruntes->clear();
    //Initialise les colones du TableView des nouveaux emprunts
    this->ModeleJeuxEmpruntes->setColumnCount(4);
    this->ModeleJeuxEmpruntes->setHorizontalHeaderItem(0, new QStandardItem("Code"));
    this->ModeleJeuxEmpruntes->setHorizontalHeaderItem(1, new QStandardItem("Nom du jeu"));
    this->ModeleJeuxEmpruntes->setHorizontalHeaderItem(2, new QStandardItem("Date emprunt"));
    this->ModeleJeuxEmpruntes->setHorizontalHeaderItem(3, new QStandardItem("Date retour"));
    ui->TbV_JeuxMembres->setColumnWidth(0,40);
    ui->TbV_JeuxMembres->setColumnWidth(1,125);
    ui->TbV_JeuxMembres->verticalHeader()->setVisible(false);

    //Tant qu'il y a des jeux,
    while(RequeteJeuEmprunte.next())
    {
        //on ajoute une nouvelle ligne du table view
        this->ModeleJeuxEmpruntes->setItem( NbrTotalDeJeuxDejaSurCeCompteAdherent, 0, new QStandardItem(RequeteJeuEmprunte.value(3).toString() ));
        this->ModeleJeuxEmpruntes->setItem( NbrTotalDeJeuxDejaSurCeCompteAdherent, 1, new QStandardItem(RequeteJeuEmprunte.value(2).toString()));
        this->ModeleJeuxEmpruntes->setItem( NbrTotalDeJeuxDejaSurCeCompteAdherent, 2, new QStandardItem(RequeteJeuEmprunte.value(0).toDate().toString("dd-MM-yyyy") ));
        this->ModeleJeuxEmpruntes->setItem( NbrTotalDeJeuxDejaSurCeCompteAdherent, 3, new QStandardItem(RequeteJeuEmprunte.value(1).toDate().toString("dd-MM-yyyy") ));

        if (RequeteJeuEmprunte.value(1).toDate()>DateActuelle)
        {
            this->ModeleJeuxEmpruntes->setData(ModeleJeuxEmpruntes->index( NbrTotalDeJeuxDejaSurCeCompteAdherent,3),QColor(Qt::green), Qt::BackgroundColorRole);
        }
        else
        {
            this->ModeleJeuxEmpruntes->setData(ModeleJeuxEmpruntes->index( NbrTotalDeJeuxDejaSurCeCompteAdherent,3),QColor(Qt::red), Qt::BackgroundColorRole);
        }
         NbrTotalDeJeuxDejaSurCeCompteAdherent++;
    }
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
    QString NbEmpruntPossibleAujourdhui ;
    NbEmpruntPossibleAujourdhui.setNum(ui->Le_NbJeuxEmpruntables->text().toInt() -
                                       this->ModeleJeuxEmpruntes->rowCount());
    QString NbNouveauxEmprunts;
    NbNouveauxEmprunts.setNum( this->ModeleEmpruntsAValider->rowCount() );
    ui->Lb_NbNouveauEmprunt->setText( NbNouveauxEmprunts );
    ui->Lb_NbEmpruntPossibleAujourdhui->setText( NbEmpruntPossibleAujourdhui );
}

/** Affiche ou cache les composants liés aux malles
 *  @pre
 *  @post
 */
void F_Emprunt::RendreVisibleMalle(bool booleen)
{
    ui->Lb_TypeMalle->setVisible(booleen);
    ui->CBx_TypeMalle->setVisible(booleen);
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
    QRect geo=ui->gridLayout_3->geometry();
    this->AfficherMembre(SearchMembre->text());
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
    //Calcule la nouvelle date du retour
    this->CalculerDateRetour();
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
void F_Emprunt::on_TbV_JeuxMembres_clicked(const QModelIndex &index)
{
    ui->TbV_JeuxMembres->selectRow(index.row());
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
    //ui->Tv_JeuxReserves->selectRow(index.row());
    ui->Bt_SupprimerReservation->setEnabled(true);
    //Met le code du jeu sellectionné dans le line édit du code du membre
    SearchJeux->setText(this->ModeleJeuxReserves->index(index.row(),0).data().toString());
    //clic sur la recherche du jeu
    on_LE_SearchJeux_jeuTrouve();
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
    qDebug()<< "F_Emprunt::on_TbV_EmpruntAValider_clicked";
/*
    QDate dates;

    dates=dates.currentDate() ;
    dates=dates.addDays(-30);
    for (register int i=1; i<100 ;i++)
    {
        dates=dates.addDays(1);
        QSqlQuery req ;
        req.prepare("INSERT INTO abonnements (Prestations_IdPrestation,ModePaiement_IdModePaiement,CartesPrepayees_IdCarte,Membres_IdMembre,DateSouscription,DateExpiration,CreditRestant) "
                    " VALUES ( 3,1,NULL,:Membre,NULL,:Date,NULL)");
        req.bindValue(":Membre",i);
        req.bindValue(":Date",dates);
        if(!req.exec())
        {

            qDebug()<< req.lastQuery();
        }
    }
    dates=dates.currentDate() ;
    dates=dates.addDays(-30);
    for (register int i=100; i<200 ;i++)
    {
        dates=dates.addDays(1);
        QSqlQuery req ;
        req.prepare("INSERT INTO abonnements(Prestations_IdPrestation,ModePaiement_IdModePaiement,CartesPrepayees_IdCarte,Membres_IdMembre,DateSouscription,DateExpiration,CreditRestant)"
                    " VALUES ( 3,1,NULL,:Membre,NULL,:Date,NULL)");
        req.bindValue(":Membre",i);
        req.bindValue(":Date",dates);
        if(!req.exec())
        {
            qDebug()<< req.lastQuery();
        }
    }

    dates=dates.currentDate() ;
    dates=dates.addDays(-30);
    for (register int i=200; i<300 ;i++)
    {
        dates=dates.addDays(1);
        QSqlQuery req ;
        req.prepare("INSERT INTO abonnements(Prestations_IdPrestation,ModePaiement_IdModePaiement,CartesPrepayees_IdCarte,Membres_IdMembre,DateSouscription,DateExpiration,CreditRestant)"
                    " VALUES ( 3,1,NULL,:Membre,NULL,:Date,NULL)");
        req.bindValue(":Membre",i);
        req.bindValue(":Date",dates);
        if(!req.exec())
        {
            qDebug()<< req.lastQuery();
        }
    }

    dates=dates.currentDate() ;
    dates=dates.addDays(-30);
    for (register int i=300; i<400 ;i++)
    {
        dates=dates.addDays(1);
        QSqlQuery req ;
        req.prepare("INSERT INTO abonnements(Prestations_IdPrestation,ModePaiement_IdModePaiement,CartesPrepayees_IdCarte,Membres_IdMembre,DateSouscription,DateExpiration,CreditRestant)"
                    " VALUES ( 3,1,NULL,:Membre,NULL,:Date,NULL)");
        req.bindValue(":Membre",i);
        req.bindValue(":Date",dates);
        if(!req.exec())
        {
            qDebug()<< req.lastQuery();
        }
    }

    dates=dates.currentDate() ;
    dates=dates.addDays(-30);
    for (register int i=400; i<500 ;i++)
    {
        dates=dates.addDays(1);
        QSqlQuery req ;
        req.prepare("INSERT INTO abonnements(Prestations_IdPrestation,ModePaiement_IdModePaiement,CartesPrepayees_IdCarte,Membres_IdMembre,DateSouscription,DateExpiration,CreditRestant)"
                    " VALUES ( 3,1,NULL,:Membre,NULL,:Date,NULL)");
        req.bindValue(":Membre",i);
        req.bindValue(":Date",dates);
        if(!req.exec())
        {
            qDebug()<< req.lastQuery();
        }
    }

    dates=dates.currentDate() ;
    dates=dates.addDays(-30);
    for (register int i=500; i<600 ;i++)
    {
        dates=dates.addDays(1);
        QSqlQuery req ;
        req.prepare("INSERT INTO abonnements(Prestations_IdPrestation,ModePaiement_IdModePaiement,CartesPrepayees_IdCarte,Membres_IdMembre,DateSouscription,DateExpiration,CreditRestant)"
                    " VALUES ( 3,1,NULL,:Membre,NULL,:Date,NULL)");
        req.bindValue(":Membre",i);
        req.bindValue(":Date",dates);
        if(!req.exec())
        {
            qDebug()<< req.lastQuery();
        }
    }

    dates=dates.currentDate() ;
    dates=dates.addDays(-30);
    for (register int i=600; i<700 ;i++)
    {
        dates=dates.addDays(1);
        QSqlQuery req ;
        req.prepare("INSERT INTO abonnements(Prestations_IdPrestation,ModePaiement_IdModePaiement,CartesPrepayees_IdCarte,Membres_IdMembre,DateSouscription,DateExpiration,CreditRestant)"
                    " VALUES ( 3,1,NULL,:Membre,NULL,:Date,NULL)");
        req.bindValue(":Membre",i);
        req.bindValue(":Date",dates);
        if(!req.exec())
        {
            qDebug()<< req.lastQuery();
        }
    }

    dates=dates.currentDate() ;
    dates=dates.addDays(-30);
    for (register int i=700; i<800 ;i++)
    {
        dates=dates.addDays(1);
        QSqlQuery req ;
        req.prepare("INSERT INTO abonnements(Prestations_IdPrestation,ModePaiement_IdModePaiement,CartesPrepayees_IdCarte,Membres_IdMembre,DateSouscription,DateExpiration,CreditRestant)"
                    " VALUES ( 3,1,NULL,:Membre,NULL,:Date,NULL)");
        req.bindValue(":Membre",i);
        req.bindValue(":Date",dates);
        if(!req.exec())
        {
            qDebug()<< req.lastQuery();
        }
    }

    dates=dates.currentDate() ;
    dates=dates.addDays(-30);
    for (register int i=800; i<900 ;i++)
    {
        dates=dates.addDays(1);
        QSqlQuery req ;
        req.prepare("INSERT INTO abonnements(Prestations_IdPrestation,ModePaiement_IdModePaiement,CartesPrepayees_IdCarte,Membres_IdMembre,DateSouscription,DateExpiration,CreditRestant)"
                    " VALUES ( 3,1,NULL,:Membre,NULL,:Date,NULL)");
        req.bindValue(":Membre",i);
        req.bindValue(":Date",dates);
        if(!req.exec())
        {
            qDebug()<< req.lastQuery();
        }
    }

    dates=dates.currentDate() ;
    dates=dates.addDays(-30);
    for (register int i=900; i<1000 ;i++)
    {
        dates=dates.addDays(1);
        QSqlQuery req ;
        req.prepare("INSERT INTO abonnements(Prestations_IdPrestation,ModePaiement_IdModePaiement,CartesPrepayees_IdCarte,Membres_IdMembre,DateSouscription,DateExpiration,CreditRestant)"
                    " VALUES ( 3,1,NULL,:Membre,NULL,:Date,NULL)");
        req.bindValue(":Membre",i);
        req.bindValue(":Date",dates);
        if(!req.exec())
        {
            qDebug()<< req.lastQuery();
        }
    }
*/
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
    //Récupération de l'id du jeu avec son code
    QSqlQuery RequeteIdJeu;
    RequeteIdJeu.prepare("SELECT IdJeux FROM jeux WHERE CodeJeu=:CodeDuJeu");
    RequeteIdJeu.bindValue(":CodeDuJeu",(this->ModeleJeuxReserves->index(ui->Tv_JeuxReserves->currentIndex().row(),0).data().toString()));

    if ( ! RequeteIdJeu.exec() )
    {
        qDebug()<< "F_Emprunt::on_Bt_SupprimerReservation_clicked() =>" << RequeteIdJeu.lastQuery() ;
    }
    RequeteIdJeu.next();

    // Demander confirmation pour la suppression de cette réservation
    if( ! QMessageBox::warning(this,"Suppression d'une réservation !","Voulez-vous supprimer cette réservation ?\nNom du jeu :"+this->ModeleJeuxReserves->index(ui->Tv_JeuxReserves->currentIndex().row(),1).data().toString(),"Supprimer cette réservation","Laisser") )
    {
        //suppression de la réservation
        QSqlQuery RequeteSupp;
        RequeteSupp.prepare("DELETE FROM reservation WHERE Jeux_IdJeux=:IdDuJeu");
        RequeteSupp.bindValue(":IdDuJeu",RequeteIdJeu.value(0));

        if (!RequeteSupp.exec())
        {
            qDebug()<< "F_Emprunt::on_Bt_SupprimerReservation_clicked() =>" << RequeteSupp.lastQuery() ;
        }

        RequeteSupp.next();

        AfficherJeuxReserve();
    }
    //Grise le bouton de suppression du tableau des réservations
    ui->Bt_SupprimerReservation->setEnabled(false);
    // Efface les info sur le jeu actuellement affiché
    this->ViderJeu();
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
void F_Emprunt::on_Bt_Ajouter_clicked()
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

    int  NbrTotalDeJeuxDejaSurCeCompteAdherent = 0;

    if(iMode!=MODE_MALLES)
    {
        //Savoir combien de jeux sont en cours d'emprunt :
        QSqlQuery RequeteNbJeuEmprunte;
        RequeteNbJeuEmprunte.prepare("SELECT Jeux_IdJeux FROM emprunts,membres "
                                     "WHERE DateRetour IS NULL AND Membres_IdMembre=IdMembre AND "
                                     "CodeMembre=:CodeDuMembre" );
        RequeteNbJeuEmprunte.bindValue(":CodeDuMembre",this->MembreActif);
        if(!RequeteNbJeuEmprunte.exec())
        {
            qDebug()<<"F_Emprunt::on_Bt_Ajouter_clicked() => RequeteNbJeuEmprunte "<<RequeteNbJeuEmprunte.lastQuery();
        }

        //Tant qu'il y a des jeux
        while(RequeteNbJeuEmprunte.next())
        {
             NbrTotalDeJeuxDejaSurCeCompteAdherent++;
        }
        //on ajoute le nombre de nouveaux emprunts
        NbrTotalDeJeuxDejaSurCeCompteAdherent =  NbrTotalDeJeuxDejaSurCeCompteAdherent+NouveauEmprunts.size();
    }

    //Met le bouton "Valider les emprunts" en cliquable
    ui->Bt_ValiderEmprunt->setEnabled(true);

    QSqlQuery RequeteIdJeu;
    unsigned int IdDuJeu (0);

    //Prépare le requête pour récupérer l'id du jeu et son prix
    RequeteIdJeu.prepare("SELECT * FROM jeux WHERE CodeJeu=:CodeDuJeu");
    RequeteIdJeu.bindValue(":CodeDuJeu",this->JeuActif);

    //Exécute la requête
    if (!RequeteIdJeu.exec())
    {
        qDebug()<<"F_Emprunt::on_Bt_Ajouter_clicked => RequeteIdJeu : "<< RequeteIdJeu.lastQuery() ;
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
        qDebug()<<"F_Emprunt::on_Bt_Ajouter_clicked => RequeteIdMembre : "<< RequeteIdMembre.lastQuery() ;
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

    //Si le jeu est disponible
    if (ui->Le_StatutJeuARemplir->text()=="Disponible")
    {
        bool Verification;
        QString Message ;
        if(iMode==MODE_MALLES)
        {
            int row=ModeleEmpruntsAValider->rowCount();

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
                NbCredits+=ui->Le_PrixEmpruntARemplir->text().toInt();
            }
            if(NbJeuxSpeciauxAValider!=0 && NbJeuxSpeciauxAValider>=
                    HashTypeMalle[ui->CBx_TypeMalle->currentData().toInt()]["NbJeuxSpeciauxEmpruntable"].toInt())
            {
                Verification=true;
                Message ="Déjà "+Message.setNum( NbJeuxSpeciauxAValider )+" "+
                        F_Preferences::ObtenirValeur("LibelleJeuxSpeciaux")+
                        " dans le panier!\nVoulez-vous quand même autoriser l'emprunt de ce jeu ?";
            }
            else if(NbAutresJeuxAValider!=0 && NbAutresJeuxAValider>=
                    HashTypeMalle[ui->CBx_TypeMalle->currentData().toInt()]["NbAutresJeuxEmpruntable"].toInt())
            {
                Verification=true;
                Message ="Déjà "+Message.setNum( NbAutresJeuxAValider )+" "+
                        F_Preferences::ObtenirValeur("LibelleAutresJeux")+
                        " dans le panier!\nVoulez-vous quand même autoriser l'emprunt de ce jeu ?";
            }
            else if(NbCredits>=HashTypeMalle[ui->CBx_TypeMalle->currentData().toInt()]["TotalCreditsMalle"].toInt())
            {
                Verification=true;
                Message ="Déjà "+Message.setNum( NbCredits )+" crédits dans le panier!\nVoulez-vous quand même autoriser l'emprunt de ce jeu ?";
            }

        }
        else
        {
            Verification=ui->Le_NbJeuxEmpruntables->text().toInt()< NbrTotalDeJeuxDejaSurCeCompteAdherent+1;
            Message ="Déjà "+Message.setNum( NbrTotalDeJeuxDejaSurCeCompteAdherent )+" jeux empruntés !\nVoulez-vous quand même autoriser l'emprunt de ce jeu ?";
        }
        //Si le nombre de jeux que possède ce membre dépasse le nombre de jeux autorisé,
        if(Verification)
        {
            if (QMessageBox::question(this,"Nombre maximum de jeux/crédits dépassé !",Message,
                                      "Autoriser","Refuser")==1)
            {
                return;  // Mettre fin à cette fonction pour empêcher l'emprunt
            }
        }

        //Vérifier si ce jeu est réservé
        //Recherche de l'id du membre qui a réservé le jeu
        QSqlQuery RequeteJeuReserve;
        RequeteJeuReserve.prepare("SELECT Membres_IdMembre,ConfirmationReservation FROM reservation WHERE Jeux_IdJeux=:IdDuJeu");
        RequeteJeuReserve.bindValue(":IdDuJeu",IdDuJeu);
        if (!RequeteJeuReserve.exec())
        {
            qDebug()<<"F_Emprunt::on_Bt_Ajouter_clicked => RequeteJeuReserve : "<< RequeteJeuReserve.lastQuery() ;
        }
        RequeteJeuReserve.next();
        // Si ce jeu est réservé
        if ( RequeteJeuReserve.size() !=0 )
        {
            //si l'id du membre actuellement sélectionné n'est le même que celui du réserveur,
            if ( RequeteJeuReserve.value(0) != IdDuMembre )
            {
                QString sMessage;
                if(RequeteJeuReserve.value(1).toInt()==1)
                {
                    sMessage="Ce jeu "+ui->Le_NomJeuARemplir->text()
                            +" est réservé et ne peut sortir de la ludothèque.";
                }
                else
                {
                    sMessage="Ce jeu "+ui->Le_NomJeuARemplir->text()+
                            " est en attente de confirmation de réservation.\n"+
                            "Soit la personne validera la réservation, "+
                            "soit celle-ci sera automatiquement supprimée.";
                }
                // Le jeu est réservé mais pas par l'adhérent sélectionné actuellement
                QMessageBox::warning(this,"Jeu réservé !",sMessage,"Ok");
                return ;  // Mettre fin à cette fonction pour empêcher l'emprunt pour ce jeu
            }
            else
            {
                //Modifier la réservation de ce jeu pour qu'il soit marqué "Emprunté"
                //et n'apparaisse plus dans les jeux réservés pour cet adhérent
                QSqlQuery RequeteJeuEmprunte;
                RequeteJeuEmprunte.prepare("UPDATE reservation SET JeuEmprunte=0 WHERE Jeux_IdJeux=:IdDuJeu");
                RequeteJeuEmprunte.bindValue(":IdDuJeu",IdDuJeu);
                if (!RequeteJeuEmprunte.exec())
                {
                    qDebug()<<"F_Emprunt::on_Bt_Ajouter_clicked => RequeteJeuEmprunte : "<< RequeteJeuEmprunte.lastQuery() ;

                }
                AfficherJeuxReserve();
            }
        }

        Emprunts Emprunt;
        //Récupération de la date du jour
        QDateTime DateActuelle;
        DateActuelle=DateActuelle.currentDateTime();

        //Remplissage de l'emprunt dans le Vecteur Emprunt
        Emprunt.idJeu= IdDuJeu;
        Emprunt.idMembre= IdDuMembre;
        Emprunt.idTypeEmprunt= ui->CBx_TypeEmprunt->currentIndex()+1;
        Emprunt.DateRetourPrevu= ui->DtE_Retour->date();
        Emprunt.DateEmprunt= DateActuelle;
        Emprunt.PrixEmprunt= ui->Le_PrixEmpruntARemplir->text().toInt();
        Emprunt.PrixCaution= PrixCaution;
        this->NouveauEmprunts.push_back(Emprunt);
        ModeleEmpruntsAValider->setItem(this->NbLignesEmpruntsAValider,0, new QStandardItem(SearchJeux->text()));
        ModeleEmpruntsAValider->setItem(this->NbLignesEmpruntsAValider,1, new QStandardItem(ui->Le_NomJeuARemplir->text()));
        ModeleEmpruntsAValider->setItem(this->NbLignesEmpruntsAValider,2, new QStandardItem( this->NouveauEmprunts[this->NbLignesEmpruntsAValider].DateRetourPrevu.toString("dd-MM-yyyy") ));
        ModeleEmpruntsAValider->setItem(this->NbLignesEmpruntsAValider,3, new QStandardItem(ui->Le_PrixEmpruntARemplir->text()));
        // Stocke l'info de savoir si le jeu est un jeu spécial ou pas
        ModeleEmpruntsAValider->setData(ModeleEmpruntsAValider->index(this->NbLignesEmpruntsAValider,0),
                                  QVariant(ObtenirValeurParNom(RequeteIdJeu,
                                F_Preferences::ObtenirValeur("FiltreJeuxSpeciauxNomChamps")).toString()==
                                F_Preferences::ObtenirValeur("FiltreJeuxSpeciauxValeur")),Qt::UserRole);
        this->NbLignesEmpruntsAValider++;

        //Mettre le statut du jeux à "Emprunt à valider"
        QSqlQuery RequeteStatut;
        RequeteStatut.prepare("UPDATE jeux SET StatutJeux_IdStatutJeux=2 WHERE IdJeux=:IdDuJeu");
        RequeteStatut.bindValue(":IdDuJeu",IdDuJeu);

        if ( ! RequeteStatut.exec())
        {
            qDebug()<<"F_Emprunt::on_Bt_Ajouter_clicked => RequeteStatut "<< RequeteStatut.lastQuery() ;
        }
    }

    //Calcule du nombre de crédits à demander
    for(register int i=0 ; i <= this->NbLignesEmpruntsAValider ; i++)
    {
        // certains jeux coutent plus de crédit que d'autres pour être empruntés
        NbCredits=NbCredits+ this->ModeleEmpruntsAValider->index(i,3).data().toInt();
    }

    //qDebug()<<" F_Emprunt::on_Bt_Ajouter_clicked => NbLignesEmpruntsAValider=" << NbLignesEmpruntsAValider << "NbCredits=" << NbCredits ;

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
    ui->CBx_TypeMalle->setEnabled(false);
    this->ViderJeu();
    // Affiche en bas à droite le nombre d'emprunt
    AfficherNbEmpruntsEnCours();
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
void F_Emprunt::on_Bt_ValiderEmprunt_clicked()
{
    //Calcule du nombre de crédits à demander
    int NbCredits (0);
    for(register int i=0 ; i < this->NbLignesEmpruntsAValider ; i++)
    {
        NbCredits = NbCredits + this->ModeleEmpruntsAValider->index(i,3).data().toInt();
    }

    pPaiement->setWindowModality(Qt::ApplicationModal);
    pPaiement->AfficherPaiement(NbCredits,this->MembreActif);
    int nResultat (0);

    nResultat = pPaiement->exec();

    // Si paiement ok, valider les emprunts
    if (nResultat == 1)
    {
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
        ui->CBx_TypeMalle->setEnabled(true);
    }
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
    RequeteStatut.bindValue( ":CodeDuJeu" , this->ModeleEmpruntsAValider->index(ui->TbV_EmpruntAValider->currentIndex().row(),0).data().toString() );
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
        AfficherJeuxReserve();
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
        ui->Bt_ValiderEmprunt->setEnabled(false);
        ui->CBx_TypeMalle->setEnabled(true);
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
    if(SearchJeux->text()!="")
    {
        on_Bt_Ajouter_clicked();
        SearchJeux->setText("");
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
    this->JeuActif= SearchJeux->text();

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
                    "EtatsJeu_idEtatsJeu,IdJeux FROM jeux WHERE CodeJeu=:CodeDuJeu");
    Requete.bindValue(":CodeDuJeu",this->JeuActif);
    if (!Requete.exec())
    {
        qDebug()<<"F_Emprunt::on_Bt_OK_clicked => Requete : "<< Requete.lastQuery();

    }
    Requete.next();

    //Récupère le nom du jeu et l'affiche
    ui->Le_NomJeuARemplir->setText( Requete.value(0).toString() ) ;

    //Récupère le prix de la caution et l'enregistre dans PrixCaution
    PrixCaution=Requete.value(1).toInt();

    //Récupère le prix de l'emprunt et l'affiche
    ui->Le_PrixEmpruntARemplir->setText(Requete.value(2).toString());

    //Récupère la remarque et l'affiche
    ui->TxE_RemarquesJeu->setText(Requete.value(4).toString());

    //Grise les boutons de modification de la remarque du jeu
    ui->Bt_ValiderRemarquesJeu->setEnabled(false);
    ui->Bt_AnnulerRemarquesJeu->setEnabled(false);

    QDateTime DateActuelle;
    DateActuelle= DateActuelle.currentDateTime();
    ui->DtE_Depart->setDate(DateActuelle.date());

    //Savoir si le jeu est déja réservé
    QCalendarWidget * Calendrier=new QCalendarWidget();
    QCalendarWidget * Calendrier2=new QCalendarWidget();
    QSqlQuery RequeteResa;
    RequeteResa.prepare("SELECT idReservation,ConfirmationReservation,DatePrevuEmprunt,DatePrevuRetour "
                        "FROM reservation LEFT JOIN jeux ON Jeux_IdJeux=IdJeux "
                        "WHERE CodeJeu=:CodeJeu AND JeuEmprunte=1");
    RequeteResa.bindValue(":CodeJeu",this->JeuActif);

    if (!RequeteResa.exec())
    {
        qDebug() << "F_Emprunt::AfficherJeu : RequeteResa :" << RequeteResa.lastQuery()  ;
    }
    else
    {
       if(RequeteResa.size()!=0)
       {
           QMessageBox::warning(this, "Attention !",
               "Une ou plusieurs réservations ont déjà été faite sur ce jeu.\n"
               "Merci de prévenir l'adhérent que le jeu risque de ne pas être disponible "
               "à la date souhaitée.");
       }
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
    }

    ui->DtE_Depart->setCalendarWidget(Calendrier);
    ui->DtE_Retour->setCalendarWidget(Calendrier2);

    //Récupère l'id de l'état
    unsigned int IdEtat =(Requete.value(6).toInt());

    //Récupère l'id du statut
    unsigned int IdStatut =(Requete.value(5).toInt());

    //si le statut est marqué comme "Emprunt à valider", demander s'il faut changer ce statut pour le rendre disponible.
    if ( IdStatut == 2 )
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
        }
    }

    //Récupère le statut sous forme de nom et l'affiche
    QSqlQuery RequeteStatut;
    RequeteStatut.prepare("SELECT StatutJeu FROM statutjeux WHERE IdStatutJeux=:IdStatutDuJeu");
    RequeteStatut.bindValue(":IdStatutDuJeu",IdStatut);
    if (!RequeteStatut.exec())
    {
        qDebug()<<"F_Emprunt::on_Bt_OK_clicked => RequeteStatut : "<< RequeteStatut.lastQuery() ;

    }
    RequeteStatut.next();
    ui->Le_StatutJeuARemplir->setText(RequeteStatut.value(0).toString());

    //si le statut est "Disponible", alors
    if ( IdStatut == 1 )
    {
        //Le mettre en vert
        ui->Le_StatutJeuARemplir->setStyleSheet("QLineEdit {color:green;}");
        ui->Lb_StatutJeu->setStyleSheet("QLabel {color:green;}");
        //Met le bouton "Ajouté" en cliquable
        ui->Bt_Ajouter->setEnabled(true);
    }
    else
    {
        //====================================================================
        //    TO DO chercher si ce jeu est réservé
        //    et prévenir que ce jeu est réservé
        //====================================================================
        //si le statut est "Résevé", alors
        if (IdStatut==4)
        {
            //Rechercher si la personne qui a emprunté est ce membre
            //Prépare le requête pour récupérer l'id du membre
            QSqlQuery RequeteIdMembre;
            RequeteIdMembre.prepare("SELECT IdMembre FROM membres WHERE CodeMembre=:CodeDuMembre");
            RequeteIdMembre.bindValue(":CodeDuMembre",this->MembreActif);
            //Exécute la requête
            if (!RequeteIdMembre.exec())
            {
                qDebug()<<"F_Emprunt::on_Bt_Ajouter_clicked => RequeteIdMembre : "<<RequeteIdMembre.lastQuery() ;
            }

            RequeteIdMembre.next();

            //Recherche de l'id du membre qui a réservé le jeu
            QSqlQuery RequeteJeuReserve;
            RequeteJeuReserve.prepare("SELECT Membres_IdMembre FROM reservation WHERE Jeux_IdJeux=:IdDuJeu");
            RequeteJeuReserve.bindValue(":IdDuJeu",Requete.value(7));
            if (!RequeteJeuReserve.exec())
            {
                qDebug()<<"F_Emprunt::on_Bt_OK_clicked => RequeteJeuReserve : "<<RequeteJeuReserve.lastQuery() ;
            }

            RequeteJeuReserve.next();

            //si l'id du membre est le même que celui du réserveur,
            if(RequeteJeuReserve.value(0)==RequeteIdMembre.value(0))
            {
                //Le mettre en vert
                ui->Le_StatutJeuARemplir->setStyleSheet("QLineEdit {color:green;}");
                ui->Lb_StatutJeu->setStyleSheet("QLabel {color:green;}");
                //Met le bouton "Ajouté" en cliquable
                ui->Bt_Ajouter->setEnabled(true);
            }
            else
            {
                //Le mettre en rouge
                ui->Le_StatutJeuARemplir->setStyleSheet("QLineEdit {color:red;}");
                ui->Lb_StatutJeu->setStyleSheet("QLabel {color:red;}");
                //Met le bouton "Ajouté" en non-cliquable
                ui->Bt_Ajouter->setEnabled(false);
            }
        }
        else // pour tous les autres statuts autre que disponible
        {
            //Le mettre en rouge
            ui->Le_StatutJeuARemplir->setStyleSheet("QLineEdit {color:red;}");
            ui->Lb_StatutJeu->setStyleSheet("QLabel {color:red;}");
            //Met le bouton "Ajouté" en non-cliquable
            ui->Bt_Ajouter->setEnabled(false);
        }
    }

    //Prépare la requête pour cherche l'état du jeu et l'Exécute
    QSqlQuery RequeteEtat;
    RequeteEtat.prepare("SELECT Etat FROM etatsjeu WHERE idEtatsJeu=:IdEtatDuJeu");
    RequeteEtat.bindValue(":IdEtatDuJeu",IdEtat);
    if (!RequeteEtat.exec())
    {
        qDebug()<<"F_Emprunt::on_Bt_OK_clicked => RequeteEtat : "<<RequeteEtat.lastQuery() ;
    }
    RequeteEtat.next();

    //met le focus sur le bouton "Ajouter"
    //ui->Bt_Ajouter->setFocus(Qt::TabFocusReason);
    //ui->Bt_Ajouter->setFocusPolicy(Qt::StrongFocus);
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

void F_Emprunt::on_CBx_TypeMalle_currentIndexChanged(int index)
{
    SearchJeux->setEnabled(index!=0);
    QString ArgMAJListeJeux;

    // Si il n'y a pas de jeux spéciaux empruntable, on les exclue de la requête
    if(HashTypeMalle[ui->CBx_TypeMalle->itemData(index).toInt()]["NbJeuxSpeciauxEmpruntable"]==0)
    {
        ArgMAJListeJeux=F_Preferences::ObtenirValeur("FiltreJeuxSpeciauxNomChamps")+"!="+
                F_Preferences::ObtenirValeur("FiltreJeuxSpeciauxValeur");
    }
    // Si il n'y a pas des jeux autres que les jeux spéciaux empruntable, on les exclue de la requête
    else if(HashTypeMalle[ui->CBx_TypeMalle->itemData(index).toInt()]["NbAutresJeuxEmpruntable"]==0)
    {
        ArgMAJListeJeux=F_Preferences::ObtenirValeur("FiltreJeuxSpeciauxNomChamps")+"="+
                F_Preferences::ObtenirValeur("FiltreJeuxSpeciauxValeur");
    }
    SearchJeux->MAJResults(MaJListeJeux(ArgMAJListeJeux),2);
    //Récupère le nombre de jeux empruntables dans la base de données puis l'affiche
    QString TotalJeuxEmpruntable=HashTypeMalle[ui->CBx_TypeMalle->itemData(index).toInt()]["TotalJeuxEmpruntable"].toString();
    ui->Lb_NbEmpruntPossibleAujourdhui->setText(TotalJeuxEmpruntable );
    ui->Le_NbJeuxEmpruntables->setText(TotalJeuxEmpruntable);

    int duree=HashTypeMalle[ui->CBx_TypeMalle->itemData(index).toInt()]["DureePret"].toInt();

    ui->CBx_TypeEmprunt->clear();
    ui->CBx_TypeEmprunt->addItem(QString::number(duree)+" jours");
    //ui->CBx_TypeEmprunt->setDisabled(true);
    QDate DateRetour;
    //DateRetour prend la date actuelle
    DateRetour=DateRetour.currentDate();
    DateRetour=DateRetour.addDays(duree);

    ui->DtE_Retour->setDate(DateRetour);
}
