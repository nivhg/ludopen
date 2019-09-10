//---------------------------------------------------
/** @file f_jeux.cpp
  * @brief Classe graphique permettant de gérer les jeux
  *
  * @author Florian MARY
  * @author STS IRIS, Lycée Nicolas APPERT, ORVAULT (FRANCE)
  * @since 01/01/2012
  * @version 0.1
  * @date 03/02/2013 Nicolas
  *
  * Cette classe permet de gérer les jeux : consulation, déclaration intervention ...
  *
  * Fabrication QtCreator
  *
  * @todo Afficher les photos des jeux
  *
*/

// En-tête propre à  l'application ----------------------------------------------
#include "f_jeux.h"
#include "ui_f_jeux.h"
#include "f_reservation.h"
#include "d_image.h"
#include "lb_image.h"
#include "acces_fichier_http.h"
#include "fonctions_globale.h"
#include "f_preferences.h"

// En-têtes standards -----------------------------------------------------------
#include <QtSql>
#include <QDesktopServices>
#include <QStandardItemModel>
#include <QProcess>
#include <QPixmap>
#include <QDir>
#include <QMessageBox>

/**
 * @brief Constructeur de la classe f_jeux
 *
 * @param parent
 */
F_Jeux::F_Jeux(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::F_Jeux)
{
    ui->setupUi(this);

    acces=new AccesFichierParHTTP(this);

    pReservation= new F_Reservation;
    pDetailsJeux=new F_DetailsJeux;
    pDeclarerIntervention=new F_DeclarerIntervention;
    // pas de jeu actuellement choisi
    JeuEnConsultation = "" ;
    ui->Bt_Reserver->setVisible(false);

    /////////////////////////////////////////////////////////
    //////////////////Création table view //////////////////
    ///////////////////////////////////////////////////////

    //Création d'un modèle pour le TableView des jeux
    this->ModelJeu = new QStandardItemModel() ;
    //Associe le modèle au TableView
    ui->TbV_NomJeux->setModel(this->ModelJeu);
    // Mise en lecture seule
    ui->TbV_NomJeux->setEditTriggers(0);
    // Autorise le tri pour ce tableau
    ui->TbV_NomJeux->setSortingEnabled(true);
    //Supprime le numéro des lignes
    ui->TbV_NomJeux->verticalHeader()->setVisible(false);
    //Création de l'objet QLabel pour l'affichage des images
    lb_image = new Lb_Image(this,ui->Lb_ImageName);
    //Gestion de l'évenement MousePress
    connect( lb_image, SIGNAL( SignalClic() ), this, SLOT( on_Lb_Image_clicked() ) );
    lb_image->setAlignment(Qt::AlignCenter);
    //Crée un curseur loupe et l'assigne à l'image
    //Initialisation des variables liées à l'affichage des images
    //QCursor Souris(QPixmap(":Loupe.png"));
    lb_image->setCursor(Qt::PointingHandCursor);
    lb_image->setMaximumSize(330,200);
    ui->gridLayout_11->addWidget(lb_image,0,3);
    // Faire défiler le tableau des jeux avec les flèches du clavier
    connect(ui->TbV_NomJeux->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(on_TbV_NomJeux_clicked(QModelIndex)));
    // Initialise la table view avec tous les jeux
    AfficherJeux() ;
    ui->TbW_LiensJeux->setColumnCount(2);
    ui->TbW_LiensJeux->setHorizontalHeaderItem(0,new QTableWidgetItem("Nom du jeu"));
    ui->TbW_LiensJeux->setHorizontalHeaderItem(1,new QTableWidgetItem("Descriptif du jeu"));

    F_MainWindow *main=dynamic_cast <F_MainWindow *>(parent);
    ui->W_Contenu->Initialisation(MODE_LECTURE_SEULE,main,"Hors départ ou retour du jeu");
    connect(ui->W_Historique,SIGNAL(Signal_ActualiserContenu()),ui->W_Contenu,SLOT(ActualiserContenu()));
    connect(ui->W_Contenu,SIGNAL(Signal_ActualiserHistoriqueMaintenance()),ui->W_Historique,SLOT(ActualiserHistoriqueMaintenance()));
}
////////////////////////////////////////////////////////////
///////////////// Destructeur /////////////////////////////
//////////////////////////////////////////////////////////
/**
 * @brief Destructeur de la classe f_jeux
 *
 */
F_Jeux::~F_Jeux()
{
    delete ui;
}

////////////////////////////////////////////////////////////
///////////////// Clique sur le bouton Details/////////////
//////////////////////////////////////////////////////////

/**
 * @brief Méthode qui lance la fenêtre de détails
 *
 */
void F_Jeux::on_Bt_details_clicked()
{
    // Bloque le clic sur une autre fenêtre tant que celle-là est ouverte
    pDetailsJeux->setWindowModality(Qt::ApplicationModal);
    pDetailsJeux->show();
    pDetailsJeux->set_JeuEnConsultation(this->JeuEnConsultation);
    pDetailsJeux->AfficherDetailJeu();
}

////////////////////////////////////////////////////////////
////// Clique sur le bouton Declarer Intervention /////////
//////////////////////////////////////////////////////////

/**
 * @brief Méthode qui lance la fenêtre de déclaration d'intervention
 *
 */
void F_Jeux::on_Bt_DeclarerIntervention_clicked()
{
    this->pDeclarerIntervention->show();
    pDeclarerIntervention->set_JeuEnConsultation(this->JeuEnConsultation);
}

////////////////////////////////////////////////////////////
///////// Clique sur le bouton Règles du jeu //////////////
//////////////////////////////////////////////////////////
/**
 * @brief Méthode qui ouvre la règle du jeu
 *
 */
void F_Jeux::on_Bt_regle_clicked()
{
    // Ouvre la règle sauvée dans sCheminFichier
    QDesktopServices::openUrl(QUrl::fromLocalFile(sCheminFichier));
}

////////////////////////////////////////////////////////////
///////// Clique sur le bouton Valider description ////////
//////////////////////////////////////////////////////////
/**
 * @brief Méthode qui valider la nouvelle description
 *
 */
void F_Jeux::on_Bt_ValiderDescription_clicked()
{
    QSqlQuery RequeteValiderDescription;

    //prépare le requête de mise à jour
    RequeteValiderDescription.prepare("UPDATE jeux SET DescriptionJeu=:NouvelleDescription WHERE CodeJeu=:CodeDuJeu");

    //Entre les valeurs de la requête
    RequeteValiderDescription.bindValue(":CodeDuJeu",JeuEnConsultation);
    RequeteValiderDescription.bindValue(":NouvelleDescription",ui->TxE_description->toPlainText());

    //Exécute la requête
    if (!RequeteValiderDescription.exec())
    {
        qDebug() << "F_Jeux::on_Bt_ValiderDescription_clicked() : RequeteValiderDescription :" << RequeteValiderDescription.lastQuery()  ;
    }
    //Grise les boutons de modification de la description
    ui->Bt_ValiderDescription->setEnabled(false);
    ui->Bt_AnnulerDescription->setEnabled(false);
}
////////////////////////////////////////////////////////////
///////// Clique sur le bouton Annuler description ////////
//////////////////////////////////////////////////////////
/**
 * @brief Méthode qui annuler la description ajoutée
 *
 */
void F_Jeux::on_Bt_AnnulerDescription_clicked()
{
    QSqlQuery RequeteAnnulerDescription;

    RequeteAnnulerDescription.prepare("SELECT DescriptionJeu FROM jeux WHERE CodeJeu=:CodeDuJeu");
    RequeteAnnulerDescription.bindValue(":CodeDuJeu",JeuEnConsultation);
    //Execut la requête
    if (!RequeteAnnulerDescription.exec())
    {
        qDebug() << "F_Jeux::on_Bt_AnnulerDescription_clicked() : RequeteAnnulerDescription :" << RequeteAnnulerDescription.lastQuery()  ;
    }

    RequeteAnnulerDescription.next();

    //Récupère les remarques dans la base de données et les affiche
    QString TexteDescription = (RequeteAnnulerDescription.value(0).toString());
    ui->TxE_description->setText(TexteDescription);

    //Grise les boutons de modification des remarques du membre
    ui->Bt_ValiderDescription->setEnabled(false);
    ui->Bt_AnnulerDescription->setEnabled(false);
}

////////////////////////////////////////////////////////////
///////////////// texte description qui change/////////////
//////////////////////////////////////////////////////////
/**
 * @brief Méthode qui active le bouton valider et annuler lorsque le contenu de la case description change
 *
 */
void F_Jeux::on_TxE_description_textChanged()
{
    ui->Bt_AnnulerDescription->setEnabled(true);
    ui->Bt_ValiderDescription->setEnabled(true);
}

////////////////////////////////////////////////////////////
///////// Remplissage du table view avec recherche ////////
//////////////////////////////////////////////////////////
/**
 * @brief Méthode qui modifier le contenu du tableau en fonction de ce que l'on rentre dans la case de recherche par nom
 *
 * @param arg1
 */
void F_Jeux::on_Le_recherchenom_textChanged(const QString &arg1)
{
    qDebug() << "F_Jeux::on_Le_recherchenom_textChanged" << "Texte : " << arg1;
    unsigned int NumeroLigne (0);
    const int CodeColonne=0;
    const int CodeNomJeu=1;
    int ColonneRecherche;
    QString NomJeu=arg1;
    QString TexteCellule;
    bool numeric;
    NomJeu.toInt(&numeric);
    int firstrow=-1;
    // Déconnexion du défilement du tableau des jeux avec les flêches du clavier
    disconnect(ui->TbV_NomJeux->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(on_TbV_NomJeux_clicked(QModelIndex)));
    bool test;
    //Tant qu'il y a des jeux dans la table jeux,
    for(int i=0;i<this->ModelJeu->rowCount();i++)
    {
        // Si le texte saisie est un nombre
        if( numeric )
        {
            test=(this->ModelJeu->item(i,CodeColonne)->text()==NomJeu);
        }
        else
        {
            TexteCellule = this->ModelJeu->item(i,CodeNomJeu)->text();
            test=(TexteCellule.indexOf(NomJeu,0,Qt::CaseInsensitive ) != string::npos);
        }

        if(test)
        {
            ui->TbV_NomJeux->showRow(i);
            if(firstrow==-1)
            {
                firstrow=i;
                ui->TbV_NomJeux->selectRow(firstrow);
                qDebug() << "F_Jeux::on_Le_recherchenom_textChanged" << "Selecting " << firstrow;
            }
        }
        else
        {
            ui->TbV_NomJeux->hideRow(i);
        }
    }
    // Reconnexion du défilement du tableau des jeux avec les flèches du clavier
    connect(ui->TbV_NomJeux->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(on_TbV_NomJeux_clicked(QModelIndex)));
    AfficherJeu(this->ModelJeu->index(firstrow, 0).data().toString());
}

/**
 * @brief Méthode qui récupère le code du jeu sur lequel on a cliqué dans le tableau
 *
 * @param index
 */
void F_Jeux::on_TbV_NomJeux_clicked(const QModelIndex &index)
{
    AfficherJeu(this->ModelJeu->index(index.row(), 0).data().toString());
    ActualiserLienJeux();
}

/**
 * @brief Grise les boutons de réservation et d'intervention si aucun jeu choisi
 *
 */
void F_Jeux::on_Le_nom_textChanged(const QString &arg1)
{
   // Si pas de nom de jeu
   if ( ui->Le_nom->text().isEmpty() )
   {
      ui->Bt_DeclarerIntervention->setDisabled(true);
   }
   else
   {
      // Rendre possible la réservation et les interventions pour ce jeu affiché
      // et seulement si un jeu a été choisi
      ui->Bt_DeclarerIntervention->setEnabled(true);
   }
}

void F_Jeux::on_Bt_Gauche_clicked()
{
    lb_image->AfficherImagePrecedente();
}

void F_Jeux::on_Bt_Droite_clicked()
{
    lb_image->AfficherImageSuivante();
}

void F_Jeux::on_Lb_Image_clicked()
{
    disconnect( lb_image, SIGNAL( SignalClic() ), this, SLOT( on_Lb_Image_clicked() ) );
    lb_image->setMaximumSize(16777215,16777215);
    QDialog *d_image = new D_Image(this,lb_image);
    d_image->showMaximized();
    d_image->exec();
    lb_image->setMaximumSize(330,200);
    ui->gridLayout_11->addWidget(lb_image,0,3);
    connect( lb_image, SIGNAL( SignalClic() ), this, SLOT( on_Lb_Image_clicked() ) );
}


/**
 * @brief Méthode qui récupère le code du jeu en cours de consultation
 *
 * @return QString
 */
QString F_Jeux::get_JeuEnConsultation()
{
    return this->JeuEnConsultation ;
}

/**
 * @brief Fixe le code du jeu en cours de consultation
 *
 * @param CodeJeuChoisi Code du jeu
 */
void F_Jeux::set_JeuEnConsultation(QString CodeJeuChoisi)
{
    this->JeuEnConsultation = CodeJeuChoisi ;
    ui->W_Contenu->Definir_CodeJeu(JeuEnConsultation);
    ui->W_Historique->Definir_CodeJeu(JeuEnConsultation);
}

void F_Jeux::SlotRegleTelecharger(QString CheminFichier)
{
    sCheminFichier=CheminFichier;
    ui->Bt_regle->setEnabled(true);
}

/**
 * @brief Méthode qui permet d'affiche les informations sur le jeu correspondant à la recherche
 *
 */
void F_Jeux::AfficherJeu(QString Jeu)
{
    if(Jeu=="")
    {
        return;
    }
    this->JeuEnConsultation=Jeu;
    ui->W_Contenu->Definir_CodeJeu(JeuEnConsultation);
    ui->W_Historique->Definir_CodeJeu(JeuEnConsultation);
    ui->TxE_description->setReadOnly(false);
    ui->TxE_emplacement->setReadOnly(false);

    ///////////////////////////////////////////////////
    ////////////// Recherche par code ////////////////
    /////////////////////////////////////////////////

    QSqlQuery RequeteRechercheCode ;

    RequeteRechercheCode.prepare("SELECT IdJeux,NomJeu,CodeJeu,NomCreateurJeu,Remarque,StatutJeux_IdStatutJeux,"
                                 "EtatsJeu_IdEtatsJeu,Emplacement_IdEmplacement,AgeMin,AgeMax,NbrJoueurMin,NbrJoueurMax,"
                                 "TypeJeux_Classification,DescriptionJeu,Editeur_IdEditeur FROM jeux WHERE CodeJeu=:CodeDuJeu") ;
    RequeteRechercheCode.bindValue(":CodeDuJeu", this->JeuEnConsultation);
    if (!RequeteRechercheCode.exec())
    {
        qDebug() << "F_Jeux::AfficherJeu() : RequeteRechercheCode :" << RequeteRechercheCode.lastQuery()  ;
    }
    RequeteRechercheCode.next();

    QString Le_Nom =  ObtenirValeurParNom(RequeteRechercheCode,"NomJeu").toString() ;
    QString Le_Code =  ObtenirValeurParNom(RequeteRechercheCode,"CodeJeu").toString() ;
    QString Le_Createur =  ObtenirValeurParNom(RequeteRechercheCode,"NomCreateurJeu").toString() ;
    QString TxE_Remarques = ObtenirValeurParNom(RequeteRechercheCode,"Remarque").toString() ;
    QString TxE_Description = ObtenirValeurParNom(RequeteRechercheCode,"DescriptionJeu").toString() ;
    QString Le_AgeMin = ObtenirValeurParNom(RequeteRechercheCode,"AgeMin").toString() ;
    QString Le_AgeMax = ObtenirValeurParNom(RequeteRechercheCode,"AgeMax").toString() ;
    QString Le_NbrJoueurMin = ObtenirValeurParNom(RequeteRechercheCode,"NbrJoueurMin").toString() ;
    QString Le_NbrJoueurMax = ObtenirValeurParNom(RequeteRechercheCode,"NbrJoueurMax").toString() ;
    int IdStatut =(ObtenirValeurParNom(RequeteRechercheCode,"StatutJeux_IdStatutJeux").toInt());
    int IdEditeur =(ObtenirValeurParNom(RequeteRechercheCode,"Editeur_IdEditeur").toInt());
    int IdEtat =(ObtenirValeurParNom(RequeteRechercheCode,"EtatsJeu_IdEtatsJeu").toInt());
    int IdEmplacement =(ObtenirValeurParNom(RequeteRechercheCode,"Emplacement_IdEmplacement").toInt());
    QString Classification =(ObtenirValeurParNom(RequeteRechercheCode,"TypeJeux_Classification").toString());

    // Remplir les champs en fonction du jeu choisi.
    ui->Le_nom->setText(Le_Nom) ;

    // aligne le curseur à gauche
    ui->Le_nom->setCursorPosition(0) ;
    ui->Le_code->setText(Le_Code);
    ui->Le_createur->setText(Le_Createur);

    // aligne le curseur à gauche
    ui->Le_createur->setCursorPosition(0) ;
    ui->Le_agemin->setText(Le_AgeMin);
    ui->Le_agemax->setText(Le_AgeMax);
    ui->Le_nbrejoueursmin->setText(Le_NbrJoueurMin);
    ui->Le_nbrejoueursmax->setText(Le_NbrJoueurMax);
    ui->TxE_description->setText(TxE_Description);

    // Bloquer certains boutons
    ui->Bt_ValiderDescription->setEnabled(false);
    ui->Bt_AnnulerDescription->setEnabled(false);

    ui->W_Contenu->ActualiserContenu();

    //////////////////////////////////////////////////////////
    /////////// Remplissage label statut /////////////////////
    ///////////////////////////////////////////////////////////
    QSqlQuery RequeteStatut,RequeteResa;
    QString Le_Statut ;

    RequeteStatut.prepare("SELECT StatutJeu FROM statutjeux WHERE IdStatutJeux=:IdStatutDuJeu");
    RequeteStatut.bindValue(":IdStatutDuJeu",IdStatut);
    RequeteStatut.exec() ;
    RequeteStatut.next();

    Le_Statut = (RequeteStatut.value(0).toString());
    ui->Le_statut->setText(Le_Statut);
    // aligne le curseur à gauche
    ui->Le_statut->setCursorPosition(0) ;
    //---------------- MISE EN COULEUR DU STATUT ----------------------------
    switch (IdStatut)
    {
    case 1:  // Disponible en vert
        ui->Le_statut->setStyleSheet("color: green");
    break ;
    case 2: // Emprunté ou emprunt à valider en noir
    case 3:
        ui->Le_statut->setStyleSheet("color: blue");
    break ;
    case 4: // En réparation en orange
        ui->Le_statut->setStyleSheet("color: orange");
    break ;
    case 5: // Indisponible en rouge
        ui->Le_statut->setStyleSheet("color: red");
    break ;
    default :
        ui->Le_statut->setStyleSheet("color: black");
    }

    ///////////////////////////////////////////////////////////
    /////////// Affichage du bouton réservé ///////////////////
    ///////////////////////////////////////////////////////////
    RequeteResa.prepare("SELECT idReservation FROM reservation WHERE Jeux_IdJeux=:IdDuJeu");
    RequeteResa.bindValue(":IdDuJeu",ObtenirValeurParNom(RequeteRechercheCode,"IdJeux").toString());
    if (!RequeteResa.exec())
    {
        qDebug() << "F_Jeux::AfficherJeu() : RequeteResa :" << RequeteResa.lastQuery()  ;
    }
    else
    {
        RequeteResa.next();
        if(RequeteResa.size()!=0)
        {
            this->IdReservation=ObtenirValeurParNom(RequeteResa,"idReservation").toInt();
        }
    }
    //////////////////////////////////////////////////////////
    /////////// Remplissage label Editeur ////////////////////
    ///////////////////////////////////////////////////////////
    QSqlQuery RequeteEditeur;
    QString Le_Editeur ;

    RequeteEditeur.prepare("SELECT NomFournisseur FROM fournisseursediteur WHERE IdFournisseur=:IdFournisseur");
    RequeteEditeur.bindValue(":IdFournisseur",IdEditeur);
    RequeteEditeur.exec();
    if( RequeteEditeur.isValid() )
    {
        RequeteEditeur.next();
        Le_Editeur = (RequeteEditeur.value(0).toString());
    }
    else
    {
        Le_Editeur = "";
    }

    ui->Le_editeur->setText(Le_Editeur);
    // aligne le curseur à gauche
    ui->Le_statut->setCursorPosition(0) ;

    /////////////////////////////////////////////////////////
    //////////// Remplissage label Etat Jeu /////////////////
    /////////////////////////////////////////////////////////

    QSqlQuery RequeteEtat;
    QString Le_Etat ;

    RequeteEtat.prepare("SELECT Etat FROM etatsjeu WHERE IdEtatsJeu=:IdEtatDuJeu");
    RequeteEtat.bindValue(":IdEtatDuJeu",IdEtat);
    RequeteEtat.exec() ;
    RequeteEtat.next();


    Le_Etat = (RequeteEtat.value(0).toString());
    ui->Le_etat->setText(Le_Etat);
    // aligne le curseur à gauche
    ui->Le_etat->setCursorPosition(0) ;

    ////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////
    ///////// Remplissage TexteEdit Emplacement ////////////
    ///////////////////////////////////////////////////////

    QSqlQuery RequeteEmplacement;
    QString TxE_Emplacement ;

    RequeteEmplacement.prepare("SELECT NomEmplacement FROM emplacement WHERE IdEmplacement=:IdEmplacementDuJeu");
    RequeteEmplacement.bindValue(":IdEmplacementDuJeu",IdEmplacement);
    RequeteEmplacement.exec() ;
    RequeteEmplacement.next();


    TxE_Emplacement = (RequeteEmplacement.value(0).toString());
    ui->TxE_emplacement->setText(TxE_Emplacement);

    //////////////////////////////////////////////////////
    ///////// Remplissage Label classification //////////
    ////////////////////////////////////////////////////

    QSqlQuery RequeteClassification;
    QString Le_Classification ;
    QString LE_NumClassification ;

    RequeteClassification.prepare("SELECT TypeJeux,Classification FROM typejeux WHERE Classification=:NumDeLaClassifciation");
    RequeteClassification.bindValue(":NumDeLaClassifciation",Classification);
    RequeteClassification.exec() ;
    RequeteClassification.next();


    Le_Classification = (RequeteClassification.value(0).toString());
    ui->Le_classification->setText(Le_Classification);
    // aligne le curseur à gauche
    ui->Le_classification->setCursorPosition(0) ;

    LE_NumClassification = (RequeteClassification.value(1).toString()) ;
    ui->LE_NumClassification->setText(LE_NumClassification);
    ui->LE_NumClassification->setCursorPosition(0);

    ////////////////////////////////////////////////////////
    ////////////// Chargement Photo //////////////////////////////////
    //////////////////////////////////////////////////////

    QString code=ui->Le_code->text();
    lb_image->ChargerImage(QSize(200,200),code,Qt::PointingHandCursor);
    ui->Lb_ImageName->setText("");

    ////////////////////////////////////////////////////////
    ////////////// Règles //////////////////////////////////
    //////////////////////////////////////////////////////

    delete acces;
    acces=new AccesFichierParHTTP(this);
    connect( acces, SIGNAL( SignalFichierTelecharger(QString) ), this, SLOT( SlotRegleTelecharger(QString) ) );

    ui->Bt_regle->setEnabled(false);

    QStringList ListeExtension;
    ListeExtension<<"pdf"<<"docx"<<"doc"<<"html"<<"htm";
    acces->LancerTelechargements(F_Preferences::ObtenirValeur("CheminReglesJeux"),code,ListeExtension);
    ActualiserLienJeux();
}

void F_Jeux::AfficherJeux()
{
     unsigned int NumeroLigne (0);
     QSqlQuery RequeteRechercheJeu;
     NumeroLigne=0;

     RequeteRechercheJeu.prepare("SELECT CodeJeu,NomJeu FROM jeux ORDER BY NomJeu");

     if (!RequeteRechercheJeu.exec())
     {
         qDebug() << "F_Jeux::AfficherJeux() : RequeteRechercheJeu :" << RequeteRechercheJeu.lastQuery()  ;
     }
     //On vide le modèle
     this->ModelJeu->clear();
     //Indique le nombres de colones puis leurs noms
     this->ModelJeu->setColumnCount(2);
     this->ModelJeu->setHorizontalHeaderItem(0, new QStandardItem("Code"));
     this->ModelJeu->setHorizontalHeaderItem(1, new QStandardItem("Nom"));
     //impose une taille aux colones
     ui->TbV_NomJeux->setColumnWidth(0,45);
     ui->TbV_NomJeux->setColumnWidth(1,205);

     //Tant qu'il y a des jeux dans la table jeux,
     while(RequeteRechercheJeu.next())
     {
         //on ajoute une nouvelle ligne du table view
         this->ModelJeu->setItem(NumeroLigne, 0, new QStandardItem(RequeteRechercheJeu.value(0).toString() ));
         this->ModelJeu->setItem(NumeroLigne, 1, new QStandardItem(RequeteRechercheJeu.value(1).toString() ));
         NumeroLigne++;
     }
}

/**
 * @brief Méthode qui actualise la case de recherche par code avec le code du jeu actif
 *
 */
void F_Jeux::ActualiserJeux()
{
    AfficherJeu(this->JeuEnConsultation);
    ui->Le_recherchenom->setFocus();
}

void F_Jeux::ActualiserLienJeux()
{
    ui->TbW_LiensJeux->clearContents();
    ui->TbW_LiensJeux->setRowCount(0);
    int i=0;
    QSqlQuery Requete;
    Requete.prepare("SELECT CodeJeu,IdLienJeuxJeux,NomJeu,IF(Jeux_IdJeuxSource=(SELECT IdJeux FROM jeux WHERE CodeJeu=:CodeJeu),"
                    "DescriptifDestination,DescriptifSource) "
                    "as Descriptif,NomLien FROM lienjeuxjeux LEFT JOIN jeux ON IdJeux="
                    "IF(Jeux_IdJeuxSource=(SELECT IdJeux FROM jeux WHERE CodeJeu=:CodeJeu),Jeux_IdJeuxDestination,Jeux_IdJeuxSource)  "
                    "LEFT JOIN typelien ON TypeLien_IdTypeLien=IdTypeLien WHERE Jeux_IdJeuxSource="
                    "(SELECT IdJeux FROM jeux WHERE CodeJeu=:CodeJeu) OR Jeux_IdJeuxDestination="
                    "(SELECT IdJeux FROM jeux WHERE CodeJeu=:CodeJeu)");
    Requete.bindValue(":CodeJeu",JeuEnConsultation);

    if(!Requete.exec())
    {
        qDebug()<< getLastExecutedQuery(Requete)<<Requete.lastError();
    }
    else
    {
        while(Requete.next())
        {
            ui->TbW_LiensJeux->setRowCount(i+1);
            QTableWidgetItem *item=new QTableWidgetItem(ObtenirValeurParNom(Requete,"NomJeu").toString());
            item->setData(Qt::UserRole,ObtenirValeurParNom(Requete,"CodeJeu").toInt());
            QFont font=item->font();
            font.setUnderline(true);
            item->setFont(font);
            item->setTextColor(Qt::blue);
            ui->TbW_LiensJeux->setItem(i,0,item);
            ui->TbW_LiensJeux->setItem(i,1,new QTableWidgetItem(ObtenirValeurParNom(Requete,"Descriptif").toString()));
            ui->TbW_LiensJeux->setItem(i++,2,new QTableWidgetItem(ObtenirValeurParNom(Requete,"NomLien").toString()));
        }
    }
    ui->TbW_LiensJeux->resizeColumnsToContents();
}

void F_Jeux::on_TbW_LiensJeux_clicked(const QModelIndex &index)
{
    AfficherJeu(ui->TbW_LiensJeux->currentItem()->data(Qt::UserRole).toString());
}
