//------------------------------------------------------------------------------
/** @file         f_emprunt.cpp
 *  @brief        Classe graphique permettant de gérer les emprunts
 *
 *  @author       Philippe BOTHEREL
 *  @author       STS IRIS, Lycée Nicolas APPERT, ORVAULT (FRANCE)
 *  @since        01/01/1012
 *  @version      0.1
<<<<<<< local
 *  @date         29/01/1013
 *  @author       William SOREL
=======
 *  @date         01/02/1012
 *
>>>>>>> other
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Constructeur///////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/** Description détaillée du CONSTRUCTEUR
 *  @pre    Connexion  à la base de données, existance de la table Emprunt et de ses champs
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

    if ( !  Requete.exec("SELECT TypeEmprunt,DureeEmprunt FROM typeemprunt") )
    {
        qDebug()<<"F_Emprunt::F_Emprunt => Requete "<<Requete.lastError();
    }
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
    this->ModeleListeDesMembres = new QStandardItemModel() ;
    //Associe le modèle au TableView
    ui->TbV_Recherche->setModel(this->ModeleListeDesMembres);
    //Met le TableView en lecture seule*/
    ui->TbV_Recherche->setEditTriggers(0);
    // Autorise le tri pour ce tableau
    ui->TbV_Recherche->setSortingEnabled(true);

    //Initialise le tableau avec tous les membres
    on_LE_RechercheMembre_textChanged("");

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
    this->ModeleEmpruntsAValider->setHorizontalHeaderItem(1, new QStandardItem("Nom du jeu"));
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
    ui->TbV_JeuxReserves->setModel(this->ModeleJeuxReserves);
    //Met le TableView en lecture seule
    ui->TbV_JeuxReserves->setEditTriggers(0);
    // Autorise le tri pour ce tableau
    ui->TbV_JeuxReserves->setSortingEnabled(true);

    //Initialise les colones du TableView des nouveaux emprunts
    this->ModeleJeuxReserves->setColumnCount(5);
    this->ModeleJeuxReserves->setHorizontalHeaderItem(0, new QStandardItem("Code"));
    this->ModeleJeuxReserves->setHorizontalHeaderItem(1, new QStandardItem("Nom du jeu"));
    this->ModeleJeuxReserves->setHorizontalHeaderItem(2, new QStandardItem("Date réservation"));
    this->ModeleJeuxReserves->setHorizontalHeaderItem(3, new QStandardItem("Date emprunt"));
    this->ModeleJeuxReserves->setHorizontalHeaderItem(4, new QStandardItem("Date retour"));
    this->ModeleJeuxReserves->setHorizontalHeaderItem(5, new QStandardItem("lieu"));
    ui->TbV_JeuxReserves->setColumnWidth(0,40);
    ui->TbV_JeuxReserves->setColumnWidth(1,125);

    // Bloque la saisie de code jeu à emprunter et du bouton OK tant que pas d'adhérent sélectionné
    ui->LE_CodeJeu->setEnabled(false);
    ui->Bt_OK->setDisabled(true);
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
    delete this->ModeleListeDesMembres;
    this->ModeleListeDesMembres=NULL;
    delete this->ModeleEmpruntsAValider;
    this->ModeleEmpruntsAValider=NULL;
    delete this->ModeleJeuxEmpruntes;
    this->ModeleJeuxEmpruntes=NULL;
    delete this->ModeleJeuxReserves;
    this->ModeleJeuxReserves=NULL;
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
        qDebug()<<"F_Emprunt::CalculerDateRetour => Requete "<<Requete.lastError();
    }
    Requete.next();

    DateRetour=DateRetour.addDays(Requete.value(0).toInt());

    ui->DtE_Retour->setDate(DateRetour);
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
        ui->LE_CodeJeu->setText(this->JeuActif);
        on_Bt_OK_clicked();
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
        qDebug()<<"F_Emprunt::JeuxReserve => Requete IdDuMembre "<<RequeteIdMembre.lastError();
    }

    RequeteIdMembre.next();

    //Récupère les remarques'id dans la base de données
    IdDuMembre=RequeteIdMembre.value(0).toInt();

    QSqlQuery RequeteJeuReserve;
    unsigned int  NbrTotalDeJeuxDejaSurCeCompteAdherent =0;
    RequeteJeuReserve.prepare("SELECT DateReservation,DatePrevuEmprunt,DatePrevuRetour,NomLieux,NomJeu,CodeJeu "
                              "FROM reservation,lieux,jeux "
                              "WHERE JeuEmprunte=1 AND Membres_IdMembre=:IdMembre AND IdLieux=Lieux_IdLieux AND IdJeux=Jeux_IdJeux" );

    RequeteJeuReserve.bindValue(":IdMembre",IdDuMembre);

    if (!RequeteJeuReserve.exec())
    {
        qDebug()<<"F_Emprunt::JeuxReserve => Requete Jeux Reservés "<<RequeteJeuReserve.lastError();
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
    ui->TbV_JeuxReserves->setColumnWidth(0,40);
    ui->TbV_JeuxReserves->setColumnWidth(1,125);
    ui->TbV_JeuxReserves->verticalHeader()->setVisible(false);

    //Tant qu'il y a des membres dans la table membres,
    while(RequeteJeuReserve.next())
    {
        //on ajoute une nouvelle ligne du table view
        this->ModeleJeuxReserves->setItem( NbrTotalDeJeuxDejaSurCeCompteAdherent, 0, new QStandardItem(RequeteJeuReserve.value(5).toString() ));
        this->ModeleJeuxReserves->setItem( NbrTotalDeJeuxDejaSurCeCompteAdherent, 1, new QStandardItem(RequeteJeuReserve.value(4).toString()));
        this->ModeleJeuxReserves->setItem( NbrTotalDeJeuxDejaSurCeCompteAdherent, 2, new QStandardItem(RequeteJeuReserve.value(0).toDate().toString("ddd d MMMM yyyy") ));
        this->ModeleJeuxReserves->setItem( NbrTotalDeJeuxDejaSurCeCompteAdherent, 3, new QStandardItem(RequeteJeuReserve.value(1).toDate().toString("ddd d MMMM yyyy") ));
        this->ModeleJeuxReserves->setItem( NbrTotalDeJeuxDejaSurCeCompteAdherent, 4, new QStandardItem(RequeteJeuReserve.value(2).toDate().toString("ddd d MMMM yyyy") ));
        this->ModeleJeuxReserves->setItem( NbrTotalDeJeuxDejaSurCeCompteAdherent, 5, new QStandardItem(RequeteJeuReserve.value(3).toString() ));

        //Savoir si le jeu est disponible ou non
        QSqlQuery Requetejeu;
        Requetejeu.prepare("SELECT IdEmprunts FROM emprunts,jeux WHERE CodeJeu=:CodeDuJeu AND Jeux_IdJeux=IdJeux AND DateRetour IS NULL");
        Requetejeu.bindValue(":CodeDuJeu",RequeteJeuReserve.value(5).toString());

        if ( ! Requetejeu.exec() )
        {
            qDebug()<<"F_Emprunt::AfficherJeuxReserve => Requetejeu "<<Requetejeu.lastError();
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
        // Autorise la saisie de code jeu et du bouton ok
        ui->LE_CodeJeu->setEnabled(true);
        ui->Bt_OK->setEnabled(true);

        //Grise les boutons de modification des remarques du membre
        ui->Bt_ValiderRemarques->setEnabled(false);
        ui->Bt_AnnulerRemarques->setEnabled(false);

        //Prépare la requête
        QSqlQuery Requete;
        Requete.prepare("SELECT Nom,Prenom,NbreRetard,Ecarte,Remarque,NbreJeuxAutorises,IdMembre FROM membres WHERE CodeMembre=:CodeDuMembre");
        Requete.bindValue(":CodeDuMembre",CodeMembre);
        if ( ! Requete.exec() )
        {
            qDebug()<<"F_Emprunt::AfficherMembre => Requete " << Requete.lastError();
        }
        Requete.next();

        //Récupère le Nom dans la base de données puis l'affiche
        ui->Le_NomARemplir->setText(Requete.value(0).toString());

        //Récupère le Prénom dans la base de données puis l'affiche
        ui->Le_PrenomARemplir->setText(Requete.value(1).toString());

        //Récupère le nombre de retards dans la base de données  puis l'affiche
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

        //Récupère les remarques dans la base de données puis les affiches
        ui->TxE_Remarques->setText(Requete.value(4).toString());

        //Grise les boutons de modification des remarques du membre
        ui->Bt_ValiderRemarques->setEnabled(false);
        ui->Bt_AnnulerRemarques->setEnabled(false);

        //Grise les bouons de suppression des tableau des jeux
        ui->Bt_SupprimerEmpruntAValider->setEnabled(false);
        ui->Bt_SupprimerReservation->setEnabled(false);

        //Récupère le nombre de jeux empruntables dans la base de données puis l'affiche
        ui->Le_NbreJeuxEmpr->setText(Requete.value(5).toString());

        //Affiche l'état de la cotisation
        //Savoir si le membre  a un membre associer
        QSqlQuery RequeteMembreAssocier ;
        RequeteMembreAssocier.prepare("SELECT MembreAssocie FROM membres WHERE CodeMembre=:CodeDuMembre AND MembreAssocie !=0");
        RequeteMembreAssocier.bindValue(":codeDuMembre",this->MembreActif);
        if ( ! RequeteMembreAssocier.exec() )
        {
            qDebug()<<"F_Emprunt::AfficherMembre => RequeteMembreAssocier " << RequeteMembreAssocier.lastError();
        }
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
////////////////////////////////////////////Emprunte les nouveau Jeux:://////////////////////////////////
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
            qDebug()<<"F_Emprunt::EmprunterJeux "<<RequeteEmprunt.lastError().text();
        }

        //Mettre le statut du jeux à "Emprunté"
        QSqlQuery RequeteStatut;
        RequeteStatut.prepare("UPDATE jeux SET StatutJeux_IdStatutJeux=3 WHERE IdJeux=:IdDuJeu");
        RequeteStatut.bindValue(":IdDuJeu",NouveauEmprunts[i].idJeu);
        if (!RequeteStatut.exec())
        {
            qDebug()<<"F_Emprunt::EmprunterJeux => requête statut  " << RequeteStatut.lastError() ;
        }

        //Virer la résa une fois que le jeu réservé a été emprunté
        //savoir si le jeu en cours de validation d'emprunt était réservé
        QSqlQuery RequeteResa;
        RequeteResa.prepare("SELECT idReservation FROM reservation WHERE Jeux_IdJeux=:IdDuJeu");
        RequeteResa.bindValue(":IdDuJeu",NouveauEmprunts[i].idJeu);
        if ( ! RequeteResa.exec() )
        {
            qDebug()<<"F_Emprunt::EmprunterJeux => RequeteResa=" << RequeteResa.lastError().text();
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
                qDebug()<<"F_Emprunt::EmprunterJeux => RequeteSuppressionResa=" << RequeteSuppressionResa.lastError().text();
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
                             "WHERE Prestations_IdPrestation IS NOT NULL AND Membres_IdMembre=IdMembre AND CodeMembre=:CodeDuMembre ORDER BY DateExpiration DESC " );
    RequeteCotisation.bindValue(":CodeDuMembre",CodeMembre );
    if ( ! RequeteCotisation.exec())
    {
        qDebug()<<"F_Emprunt::AfficherEtatCotisation => RequeteCotisation : "<<RequeteCotisation.lastError();
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
            //On précise le code du membre qui possède la cotisation
            Cotisation="Cotisation à jours (membre "+ CodeMembre +")";
        }
        //Sinon,
        else
        {
            //On affiche juste l'état de la cotisation
            Cotisation="Cotisation à jours";
        }
        EtatDeLaCotisation=true;
        QString JourRestant ;
        JourRestant.setNum(NombreJoursRestant);
        ui->Lb_Cotisation->setText(Cotisation);
        ui->Lb_CotisationARemplir->setText("("+ JourRestant +" jours restant)");
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
    RequeteCartes.bindValue("CodeDuMembre",this->MembreActif);
    if ( ! RequeteCartes.exec())
    {
        qDebug()<<"F_Emprunt::CalculerCreditsRestants => RequeteCartes : " << RequeteCartes.lastError();
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
        qDebug()<< "F_Emprunt::AfficherJeuxEnEmprunt => RequeteIdMembre  "<< RequeteIdMembre.lastError();
    }

    RequeteIdMembre.next();

    //Récupère l'id dans la base de données
    IdDuMembre=RequeteIdMembre.value(0).toInt();

    QSqlQuery RequeteJeuEmprunte;
    unsigned int  NbrTotalDeJeuxDejaSurCeCompteAdherent =0;
    RequeteJeuEmprunte.prepare("SELECT DateEmprunt,DateRetourPrevu,NomJeu,CodeJeu FROM emprunts,jeux WHERE Membres_IdMembre=:IdDuMembre AND IdJeux=Jeux_IdJeux AND DateRetour IS NULL");
    RequeteJeuEmprunte.bindValue(":IdDuMembre",IdDuMembre);
    if( ! RequeteJeuEmprunte.exec())
    {
        qDebug()<<"F_Emprunt::AfficherJeuxEnEmprunt => RequeteJeuEmprunte : "<<RequeteJeuEmprunte.lastError();
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
        this->ModeleJeuxEmpruntes->setItem( NbrTotalDeJeuxDejaSurCeCompteAdherent, 2, new QStandardItem(RequeteJeuEmprunte.value(0).toDate().toString("ddd d MMMM yyyy") ));
        this->ModeleJeuxEmpruntes->setItem( NbrTotalDeJeuxDejaSurCeCompteAdherent, 3, new QStandardItem(RequeteJeuEmprunte.value(1).toDate().toString("ddd d MMMM yyyy") ));

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
    on_Bt_OK_clicked();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Rechercher un membre avec son nom//////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *  @pre    Connexion avec la base de données
 *  @post   Table membres remplie
 *  @see    ModeleListeDesMembres
 */
void F_Emprunt::on_LE_RechercheMembre_textChanged(const QString &arg1)
{
    unsigned int  NbrTotalDeJeuxDejaSurCeCompteAdherent (0);
    QString Nom=arg1;

    //Si il y plus de 2 caractères dans le nom du membre recherché, alors
    if(Nom.size()>=2)
    {
        Nom="%"+Nom+"%";

        QSqlQuery RequeteMembre;
        NbrTotalDeJeuxDejaSurCeCompteAdherent=0;

        //Rechercher dans la table membres, les membres qui contiennent dans leur nom le suite de caractères présente dans le line édit du nom
        RequeteMembre.prepare("SELECT CodeMembre,Nom,Prenom,DateNaissance FROM membres WHERE Nom LIKE (:Nom) ORDER BY Nom ASC");
        RequeteMembre.bindValue(":Nom",Nom);
        if( ! RequeteMembre.exec())
        {
            qDebug()<<"F_Emprunt::AfficherJeuxEnEmprunt => RequeteMembre : "<<RequeteMembre.lastError();
        }
        //On vide le modèle
        this->ModeleListeDesMembres->clear();
        //Indique le nombes de colones puis leurs noms
        this->ModeleListeDesMembres->setColumnCount(4);
        this->ModeleListeDesMembres->setHorizontalHeaderItem(0, new QStandardItem("Code"));
        this->ModeleListeDesMembres->setHorizontalHeaderItem(1, new QStandardItem("Nom"));
        this->ModeleListeDesMembres->setHorizontalHeaderItem(2, new QStandardItem("Prénom"));
        this->ModeleListeDesMembres->setHorizontalHeaderItem(3, new QStandardItem("Date de naissance"));
        //Impose une taille aux colones
        ui->TbV_Recherche->setColumnWidth(0,40);
        ui->TbV_Recherche->setColumnWidth(3,40);

        //Tant qu'il y a des membres dans la table membres,
        while(RequeteMembre.next())
        {
            //on ajoute une nouvelle ligne du table view et la remplie avec les informations du membre
            this->ModeleListeDesMembres->setItem( NbrTotalDeJeuxDejaSurCeCompteAdherent, 0, new QStandardItem(RequeteMembre.value(0).toString() ));
            this->ModeleListeDesMembres->setItem( NbrTotalDeJeuxDejaSurCeCompteAdherent, 1, new QStandardItem(RequeteMembre.value(1).toString() ));
            this->ModeleListeDesMembres->setItem( NbrTotalDeJeuxDejaSurCeCompteAdherent, 2, new QStandardItem(RequeteMembre.value(2).toString() ));
            this->ModeleListeDesMembres->setItem( NbrTotalDeJeuxDejaSurCeCompteAdherent, 3, new QStandardItem(RequeteMembre.value(3).toString() ));
             NbrTotalDeJeuxDejaSurCeCompteAdherent++;
        }
    }
    else
    {
        //On Affiche tous les membres :
        QSqlQuery RequeteMembre;
        NbrTotalDeJeuxDejaSurCeCompteAdherent =0;
        if ( ! RequeteMembre.exec("SELECT CodeMembre,Nom,Prenom,DateNaissance FROM membres ORDER BY Nom ASC") )
        {
            qDebug()<<"F_Emprunt::F_Emprunt => RequeteMembre "<<RequeteMembre.lastError();
        }
        //On vide le modèle
        this->ModeleListeDesMembres->clear();
        //Indique le nombes de colones puis leurs noms
        this->ModeleListeDesMembres->setColumnCount(4);
        this->ModeleListeDesMembres->setHorizontalHeaderItem(0, new QStandardItem("Code"));
        this->ModeleListeDesMembres->setHorizontalHeaderItem(1, new QStandardItem("Nom"));
        this->ModeleListeDesMembres->setHorizontalHeaderItem(2, new QStandardItem("Prénom"));
        this->ModeleListeDesMembres->setHorizontalHeaderItem(3, new QStandardItem("Date de naissance"));
        //impose une taille aux colones
        ui->TbV_Recherche->setColumnWidth(0,40);
        ui->TbV_Recherche->setColumnWidth(3,40);
        ui->TbV_Recherche->verticalHeader()->setVisible(false);

        //Tant qu'il y a des membres dans la table membres,
        while(RequeteMembre.next())
        {
            //on ajoute une nouvelle ligne du table view et la emplie avec le membre
            this->ModeleListeDesMembres->setItem( NbrTotalDeJeuxDejaSurCeCompteAdherent, 0, new QStandardItem(RequeteMembre.value(0).toString() ));
            this->ModeleListeDesMembres->setItem( NbrTotalDeJeuxDejaSurCeCompteAdherent, 1, new QStandardItem(RequeteMembre.value(1).toString() ));
            this->ModeleListeDesMembres->setItem( NbrTotalDeJeuxDejaSurCeCompteAdherent, 2, new QStandardItem(RequeteMembre.value(2).toString() ));
            this->ModeleListeDesMembres->setItem( NbrTotalDeJeuxDejaSurCeCompteAdherent, 3, new QStandardItem(RequeteMembre.value(3).toString() ));
            NbrTotalDeJeuxDejaSurCeCompteAdherent++;
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Pression sur entrée pour le code membre////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
*  @pre    Connexion avec la base de données, line édit du code membre rempli
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
/////////////////////////////////////////Clic sur un membre dans le tableau//////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *  @pre    Connexion avec la base de données, tableau des membre remplie
 *  @post
 *  @param
 *  @retval
 *  @return
 *  @test
 *  @see    ModeleListeDesMembres
 */
void F_Emprunt::on_TbV_Recherche_clicked(const QModelIndex &index)
{
    //Met le code du membre sellectionné dans le line édit du code du membre
    ui->LE_CodeMembre->setText(this->ModeleListeDesMembres->index(index.row(),0).data().toString());
    //clic sur la recherche du membre
    on_Bt_RechercheOK_clicked();
    ui->TbV_Recherche->selectRow(index.row());
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
void F_Emprunt::on_TbV_JeuxReserves_clicked(const QModelIndex &index)
{
    ui->TbV_JeuxReserves->selectRow(index.row());
    ui->Bt_SupprimerReservation->setEnabled(true);
    //Met le code du jeu sellectionné dans le line édit du code du membre
    ui->LE_CodeJeu->setText(this->ModeleJeuxReserves->index(index.row(),0).data().toString());
    //clic sur la recherche du jeu
    on_Bt_OK_clicked();
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

            qDebug()<< req.lastError();
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
            qDebug()<< req.lastError();
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
            qDebug()<< req.lastError();
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
            qDebug()<< req.lastError();
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
            qDebug()<< req.lastError();
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
            qDebug()<< req.lastError();
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
            qDebug()<< req.lastError();
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
            qDebug()<< req.lastError();
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
            qDebug()<< req.lastError();
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
            qDebug()<< req.lastError();
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
    RequeteIdJeu.bindValue(":CodeDuJeu",(this->ModeleJeuxReserves->index(ui->TbV_JeuxReserves->currentIndex().row(),0).data().toString()));

    if ( ! RequeteIdJeu.exec() )
    {
        qDebug()<< "F_Emprunt::on_Bt_SupprimerReservation_clicked() =>" << RequeteIdJeu.lastQuery() ;
    }
    RequeteIdJeu.next();

    // Demander confirmation pour la suppression de cette réservation
    if( ! QMessageBox::warning(this,"Suppression d'une réservation !","Voulez-vous supprimer cette réservation ?\nNom du jeu :"+this->ModeleJeuxReserves->index(ui->TbV_JeuxReserves->currentIndex().row(),1).data().toString(),"Supprimer cette réservation","Laisser") )
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

    //Savoir combien de jeux sont en cours d'emprunt :
    QSqlQuery RequeteNbJeuEmprunte;
    int  NbrTotalDeJeuxDejaSurCeCompteAdherent = 0;
    RequeteNbJeuEmprunte.prepare("SELECT Jeux_IdJeux FROM emprunts,membres "
                                 "WHERE DateRetour IS NULL AND Membres_IdMembre=IdMembre AND CodeMembre=:CodeDuMembre" );
    RequeteNbJeuEmprunte.bindValue(":CodeDuMembre",this->MembreActif);
    if(!RequeteNbJeuEmprunte.exec())
    {
        qDebug()<<"F_Emprunt::on_Bt_Ajouter_clicked() => RequeteNbJeuEmprunte "<<RequeteNbJeuEmprunte.lastError();
    }

    //Tant qu'il y a des jeux
    while(RequeteNbJeuEmprunte.next())
    {
         NbrTotalDeJeuxDejaSurCeCompteAdherent++;
    }

    //on ajoute le nombre de nouveaux emprunts
    NbrTotalDeJeuxDejaSurCeCompteAdherent =  NbrTotalDeJeuxDejaSurCeCompteAdherent+NouveauEmprunts.size();

    //Met le bouton "Valider les emprunts" en cliquable
    ui->Bt_ValiderEmprunt->setEnabled(true);

    QSqlQuery RequeteIdJeu;
    unsigned int IdDuJeu (0);

    //Prépare le requête pour récupérer l'id du jeu
    RequeteIdJeu.prepare("SELECT IdJeux FROM jeux WHERE CodeJeu=:CodeDuJeu");
    RequeteIdJeu.bindValue(":CodeDuJeu",this->JeuActif);

    //Exécute la requête
    if (!RequeteIdJeu.exec())
    {
        qDebug()<<"F_Emprunt::on_Bt_Ajouter_clicked => RequeteIdJeu : "<< RequeteIdJeu.lastError() ;
    }

    RequeteIdJeu.next();

    //Récupère l'es remarques'id dans la base de données
    IdDuJeu=RequeteIdJeu.value(0).toInt();

    QSqlQuery RequeteIdMembre;
    unsigned int IdDuMembre (0);

    //Prépare le requête pour récupérer l'id du membre
    RequeteIdMembre.prepare("SELECT IdMembre FROM membres WHERE CodeMembre=:CodeDuMembre");
    RequeteIdMembre.bindValue(":CodeDuMembre",this->MembreActif);

    //Exécute la requête
    if (!RequeteIdMembre.exec())
    {
        qDebug()<<"F_Emprunt::on_Bt_Ajouter_clicked => RequeteIdMembre : "<< RequeteIdMembre.lastError() ;
    }

    RequeteIdMembre.next();

    //Récupère l'id dans la base de données
    IdDuMembre=RequeteIdMembre.value(0).toInt();

    //Si le jeux est disponible
    if (ui->Le_StatutJeuARemplir->text()=="Disponible")
    {
        //Si le nombre de jeux que possède ce membre dépasse le nombre de jeux autorisé,
        if(ui->Le_NbreJeuxEmpr->text().toInt()< NbrTotalDeJeuxDejaSurCeCompteAdherent+1)
        {
            QString Message ;
            Message ="Déjà "+Message.setNum( NbrTotalDeJeuxDejaSurCeCompteAdherent )+" jeux empruntés !\nVoulez-vous quand même autoriser l'emprunt de ce jeu ?";
            if (QMessageBox::question(this,"Nombre maximum de jeux dépassé !",Message,"Autoriser","Refuser")==1)
            {
                return;  // Mettre fin à cette fonction pour empêcher l'emprunt
            }
        }

        //Vérifier si ce jeu est réservé
        //Recherche de l'id du membre qui a réservé le jeu
        QSqlQuery RequeteJeuReserve;
        RequeteJeuReserve.prepare("SELECT Membres_IdMembre FROM reservation WHERE Jeux_IdJeux=:IdDuJeu");
        RequeteJeuReserve.bindValue(":IdDuJeu",IdDuJeu);
        if (!RequeteJeuReserve.exec())
        {
            qDebug()<<"F_Emprunt::on_Bt_Ajouter_clicked => RequeteJeuReserve : "<< RequeteJeuReserve.lastError() ;
        }
        RequeteJeuReserve.next();
        // Si ce jeu est réservé
        if ( RequeteJeuReserve.size() !=0 )
        {
            //si l'id du membre actuellement sélectionné n'est le même que celui du réserveur,
            if ( RequeteJeuReserve.value(0) != IdDuMembre )
            {
                // Le jeu est réservé mais pas par l'adhérent sélectionné actuellement
                QMessageBox::warning(this,"Jeu réservé !","Ce jeu "+ui->Lb_NomJeu->text()+"est réservé et ne peut sortir de la ludothèque.","Ok");
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
                    qDebug()<<"F_Emprunt::on_Bt_Ajouter_clicked => RequeteJeuEmprunte : "<< RequeteJeuEmprunte.lastError() ;

                }
                AfficherJeuxReserve();
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

        ModeleEmpruntsAValider->setItem(this->NbLignesEmpruntsAValider,0, new QStandardItem(ui->LE_CodeJeu->text()));
        ModeleEmpruntsAValider->setItem(this->NbLignesEmpruntsAValider,1, new QStandardItem(ui->Le_NomJeuARemplir->text()));
        ModeleEmpruntsAValider->setItem(this->NbLignesEmpruntsAValider,2, new QStandardItem( this->NouveauEmprunts[this->NbLignesEmpruntsAValider].DateRetourPrevu.toString("ddd d MMMM yyyy") ));
        ModeleEmpruntsAValider->setItem(this->NbLignesEmpruntsAValider,3, new QStandardItem(ui->Le_PrixEmpruntARemplir->text()));

        this->NbLignesEmpruntsAValider++;

        //Mettre le statut du jeux à "Emprunt à valider"
        QSqlQuery RequeteStatut;
        RequeteStatut.prepare("UPDATE jeux SET StatutJeux_IdStatutJeux=2 WHERE IdJeux=:IdDuJeu");
        RequeteStatut.bindValue(":IdDuJeu",IdDuJeu);

        if ( ! RequeteStatut.exec())
        {
            qDebug()<<"F_Emprunt::on_Bt_Ajouter_clicked => RequeteStatut "<< RequeteStatut.lastError() ;
        }
    }

    //Calcule du nombre de crédits à demander
    int NbCredits (0);
    for(register int i=0 ; i <= this->NbLignesEmpruntsAValider ; i++)
    {
        // certains jeux coutent plus de crédit que d'autres pour être empruntés
        NbCredits=NbCredits+ this->ModeleEmpruntsAValider->index(i,3).data().toInt();
    }

    //qDebug()<<" F_Emprunt::on_Bt_Ajouter_clicked => NbLignesEmpruntsAValider=" << NbLignesEmpruntsAValider << "NbCredits=" << NbCredits ;
    //Si le le prix des nouveaux emprunts est plus cher que les crédits restants, alors
    if (NbCredits>ui->Le_CreditRestantARemplir->text().toInt())
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
    ui->LE_CodeJeu->setFocus();
    this->ViderJeu();
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
        qDebug()<<"F_Emprunt::on_Bt_SupprimerEmpruntAValider_clicked => RequeteStatut : "<< RequeteStatut.lastError() ;
    }

    // Si ce jeu était un jeu réservé par cet adhérent
    //Recherche de l'id du jeu à modifier
    int IdDuJeu = this->NouveauEmprunts[ui->TbV_EmpruntAValider->currentIndex().row()].idJeu ;
    QSqlQuery RequeteJeuReserve;
    RequeteJeuReserve.prepare("SELECT Membres_IdMembre FROM reservation WHERE JeuEmprunte=0 AND Jeux_IdJeux=:IdDuJeu");
    RequeteJeuReserve.bindValue(":IdDuJeu",IdDuJeu);
    if (!RequeteJeuReserve.exec())
    {
        qDebug()<<"F_Emprunt::on_Bt_SupprimerEmpruntAValider_clicked => RequeteJeuReserve : "<< RequeteJeuReserve.lastError() ;
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
            qDebug()<<"F_Emprunt::on_Bt_SupprimerEmpruntAValider_clicked => RequeteJeuEmprunte : "<< RequeteJeuEmprunte.lastError() ;

        }
        AfficherJeuxReserve();
    }

    //Supprimer la ligne du vecteur
    this->NouveauEmprunts.remove(ui->TbV_EmpruntAValider->currentIndex().row());
    this->NbLignesEmpruntsAValider--;

    //Supprime la ligne du tableau
    this->ModeleEmpruntsAValider->removeRow(ui->TbV_EmpruntAValider->currentIndex().row());

    //Grise le bouton de suppression des nouveau emprunts
    ui->Bt_SupprimerEmpruntAValider->setEnabled(false);
    // Si plus de nouvel emprunt car tous supprimés,
    if ( this->NouveauEmprunts.count() == 0)
    {
        //Grise le bouton de validation des nouveaux emprunts
        ui->Bt_ValiderEmprunt->setEnabled(false);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Clic sur le bouton OK du code membre///////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *  @pre    Connexion avec la base de données, line édit du code membre rempli
 *  @post
 *  @param
 *  @retval
 *  @return
 *  @test
 *  @see    ModeleListeDesMembres
 */
void F_Emprunt::on_Bt_RechercheOK_clicked()
{
    //S'il y a encore des emprunts à valider
    if (this->NbLignesEmpruntsAValider != 0)
    {
        // affichage d'un message d'erreur
        QMessageBox::warning(this,"Emprunt non validé !","Vous ne pouvez changer d'adhérent car \nvous n'avez pas encore valider les emprunts.","Ok");
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
        qDebug()<<"F_Emprunt::on_Bt_ValiderRemarques_clicked => Requete : "<< Requete.lastError() ;

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
        qDebug()<<"F_Emprunt::on_Bt_AnnulerRemarques_clicked => Requete : "<< Requete.lastError() ;

    }

    Requete.next();

    //Récupère les remarques dans la base de données et les affiche
    QString TextTemporaire = (Requete.value(0).toString());
    ui->TxE_Remarques->setText(TextTemporaire);

    //Grise les boutons de modification des remarques du membre
    ui->Bt_ValiderRemarques->setEnabled(false);
    ui->Bt_AnnulerRemarques->setEnabled(false);
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
void F_Emprunt::on_Bt_OK_clicked()
{
    //Vérification qu'il y a un membre sélectionné.
    QSqlQuery RequeteMembre;
    RequeteMembre.prepare("SELECT CodeMembre FROM membres WHERE CodeMembre=:CodeDuMembre");
    RequeteMembre.bindValue(":CodeDuMembre",this->MembreActif);
    if (!RequeteMembre.exec())
    {
        qDebug()<<"F_Emprunt::on_Bt_OK_clicked => RequeteMembre : "<< RequeteMembre.lastError() ;

    }
    RequeteMembre.next();
    if(RequeteMembre.size()==0)
    {
        //On envoie un message d'erreur
        QMessageBox::warning(this,"Pas de membre choisi...","Attention !\nVous n'avez sélectionné aucun membre.","Ok");
        return;
    }

    //Récupère le code du jeu qui a été saisi
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
    Requete.prepare("SELECT NomJeu,ContenuJeu,PrixLoc,Caution,Remarque,StatutJeux_IdStatutJeux,EtatsJeu_idEtatsJeu,IdJeux FROM jeux WHERE CodeJeu=:CodeDuJeu");
    Requete.bindValue(":CodeDuJeux",this->JeuActif);
    if (!Requete.exec())
    {
        qDebug()<<"F_Emprunt::on_Bt_OK_clicked => Requete : "<< Requete.lastError() ;

    }
    Requete.next();

    //Récupère le nom du jeu et l'affiche
    ui->Le_NomJeuARemplir->setText( Requete.value(0).toString() ) ;

    //Récupère le contenu de le boîte et l'affiche
    ui->TxE_ContenuBoite->setText(Requete.value(1).toString() );

    //Récupère le prix de l'emprunt et l'affiche
    ui->Le_PrixEmpruntARemplir->setText(Requete.value(2).toString());

    //Récupère le prix de la caution et l'affiche
    ui->Le_PrixCautionARemplir->setText(Requete.value(3).toString());

    //Récupère la remarque et l'affiche
    ui->TxE_RemarquesJeu->setText(Requete.value(4).toString());

    //Grise les boutons de modification de la remarque du jeu
    ui->Bt_ValiderRemarquesJeu->setEnabled(false);
    ui->Bt_AnnulerRemarquesJeu->setEnabled(false);

    //Récupère l'id de l'état
    unsigned int IdEtat =(Requete.value(6).toInt());

    //Récupère l'id du statut
    unsigned int IdStatut =(Requete.value(5).toInt());

    //si le statut est marqué comme "Emprunt à valider", demander s'il faut changer ce statut pour le rendre disponible.
    if ( IdStatut == 2 )
    {
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
        qDebug()<<"F_Emprunt::on_Bt_OK_clicked => RequeteStatut : "<< RequeteStatut.lastError() ;

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
                qDebug()<<"F_Emprunt::on_Bt_Ajouter_clicked => RequeteIdMembre : "<<RequeteIdMembre.lastError() ;
            }

            RequeteIdMembre.next();

            //Recherche de l'id du membre qui a réservé le jeu
            QSqlQuery RequeteJeuReserve;
            RequeteJeuReserve.prepare("SELECT Membres_IdMembre FROM reservation WHERE Jeux_IdJeux=:IdDuJeu");
            RequeteJeuReserve.bindValue(":IdDuJeu",Requete.value(7));
            if (!RequeteJeuReserve.exec())
            {
                qDebug()<<"F_Emprunt::on_Bt_OK_clicked => RequeteJeuReserve : "<<RequeteJeuReserve.lastError() ;
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
        else // pour tout les autres statuts autre que disponible
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
        qDebug()<<"F_Emprunt::on_Bt_OK_clicked => RequeteEtat : "<<RequeteEtat.lastError() ;
    }
    RequeteEtat.next();
    //Récupère l'état et l'affiche
    ui->Le_EtatJeuARemplir->setText(RequeteEtat.value(0).toString());

    //met le focus sur le bouton "Ajouter"
    ui->Bt_Ajouter->setFocus();
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
        qDebug()<<"F_Emprunt::on_Bt_ValiderRemarquesJeu_clicked => Requete : "<<Requete.lastError() ;
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
        qDebug()<<"F_Emprunt::on_Bt_AnnulerRemarquesJeu_clicked => Requete : "<<Requete.lastError() ;
    }

    Requete.next();

    //Récupère les remarques dans la base de données
    QString TextTemporaire = (Requete.value(0).toString());
    ui->TxE_RemarquesJeu->setText(TextTemporaire);

    //grise les boutons de modification des remarques du jeu
    ui->Bt_ValiderRemarquesJeu->setEnabled(false);
    ui->Bt_AnnulerRemarquesJeu->setEnabled(false);
}
