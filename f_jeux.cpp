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

    pReservation= new F_Reservation;
    pDetailsJeux=new F_DetailsJeux;
    pDeclarerIntervention=new F_DeclarerIntervention;
    // pas de jeu actuellement choisi
    JeuEnConsultation = "" ;

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
    lb_image = new Lb_Image(this);
    //Gestion de l'évenement MousePress
    connect( lb_image, SIGNAL( clicked() ), this, SLOT( on_Lb_Image_clicked() ) );
    lb_image->setAlignment(Qt::AlignCenter);
    //Crée un curseur loupe et l'assigne à l'image
    //Initialisation des variables liées à l'affichage des images
    QCursor Souris(QPixmap(QApplication::applicationDirPath() + "/Loupe.png"));
    lb_image->setCursor(Souris);
    ui->gridLayout_11->addWidget(lb_image,0,3);
    // Faire défiler le tableau des jeux avec les flèches du clavier
    connect(ui->TbV_NomJeux->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(on_TbV_NomJeux_clicked(QModelIndex)));
    // Initialise la table view avec tous les jeux
    AfficherJeux() ;
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
///////////////// Clique sur le bouton reserver////////////
//////////////////////////////////////////////////////////
/**
 * @brief Méthode qui lance la fenêtre de réservation d'un jeu
 *
 */
void F_Jeux::on_Bt_Reserver_clicked()
{
    //Savoir si le jeux existe
    QSqlQuery RequeteJeu;
    RequeteJeu.prepare("SELECT IdJeux FROM jeux WHERE CodeJeu=:CodeDuJeu");
    RequeteJeu.bindValue(":CodeDuJeu",this->JeuEnConsultation);
    RequeteJeu.exec();
    if (!RequeteJeu.exec())
    {
        qDebug() << "F_Jeux::on_Bt_reserver_clicked() : RequeteJeu :" << RequeteJeu.lastQuery()  ;
    }
    RequeteJeu.next();

    if(RequeteJeu.size()==0)
    {
        QMessageBox::information(this,"Réservation impossible !","Ce jeu n'existe pas\nSélectionnez un jeu valide","Ok");
        return;
    }
    else
    {
       //Savoir si le jeux est déja réservé
       QSqlQuery RequeteResa;
       RequeteResa.prepare("SELECT idReservation FROM reservation WHERE Jeux_IdJeux=:IdDuJeu AND JeuEmprunte=1");
       RequeteResa.bindValue(":IdDuJeu",RequeteJeu.value(0));

       if (!RequeteResa.exec())
       {
           qDebug() << "F_Jeux::on_Bt_reserver_clicked() : RequeteResa :" << RequeteResa.lastQuery()  ;
       }
       else
       {
          if (RequeteResa.size()!=0)
          {
             QMessageBox::information(this,"Réservation impossible !","Ce jeu est déjà réservé.","Ok");
          }
          else
          {
             pReservation->setWindowModality(Qt::ApplicationModal);
             pReservation->set_JeuActif(this->JeuEnConsultation);
             pReservation->AfficherJeu();
             pReservation->show();
          }
       }
    }
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
///////////////// Clique sur le bouton OK /////////////////
//////////////////////////////////////////////////////////

/**
 * @brief Méthode qui permet d'affiche les informations sur le jeu correspondant à la recherche
 *
 */
void F_Jeux::AfficherJeu()
{
    ui->TxE_contenu->setReadOnly(false);
    ui->TxE_description->setReadOnly(false);
    ui->TxE_emplacement->setReadOnly(false);
    ui->TxE_remarques->setReadOnly(false);

    ///////////////////////////////////////////////////
    ////////////// Recherche par code ////////////////
    /////////////////////////////////////////////////
    
    QSqlQuery RequeteRechercheCode ;

    JeuEnConsultation = this->nIdJeuSelectionne ;

    RequeteRechercheCode.prepare("SELECT NomJeu,CodeJeu,NomCreateurJeu,ContenuJeu,Remarque,StatutJeux_IdStatutJeux,"
                                 "EtatsJeu_IdEtatsJeu,Emplacement_IdEmplacement,AgeMin,AgeMax,NbrJoueurMin,NbrJoueurMax,"
                                 "TypeJeux_Classification,DescriptionJeu,Editeur_IdEditeur FROM jeux WHERE CodeJeu=:CodeDuJeu") ;
    RequeteRechercheCode.bindValue(":CodeDuJeu", this->nIdJeuSelectionne);
    if (!RequeteRechercheCode.exec())
    {
        qDebug() << "F_Jeux::AfficherJeu() : RequeteRechercheCode :" << RequeteRechercheCode.lastQuery()  ;
    }
    RequeteRechercheCode.next();

    QString Le_Nom =  RequeteRechercheCode.value(RequeteRechercheCode.record().indexOf("NomJeu")).toString() ;
    QString Le_Code =  RequeteRechercheCode.value(RequeteRechercheCode.record().indexOf("CodeJeu")).toString() ;
    QString Le_Createur =  RequeteRechercheCode.value(RequeteRechercheCode.record().indexOf("NomCreateurJeu")).toString() ;
    QString TxE_Contenu = RequeteRechercheCode.value(RequeteRechercheCode.record().indexOf("ContenuJeu")).toString() ;
    QString TxE_Remarques = RequeteRechercheCode.value(RequeteRechercheCode.record().indexOf("Remarque")).toString() ;
    QString TxE_Description = RequeteRechercheCode.value(RequeteRechercheCode.record().indexOf("DescriptionJeu")).toString() ;
    QString Le_AgeMin = RequeteRechercheCode.value(RequeteRechercheCode.record().indexOf("AgeMin")).toString() ;
    QString Le_AgeMax = RequeteRechercheCode.value(RequeteRechercheCode.record().indexOf("AgeMin")).toString() ;
    QString Le_NbrJoueurMin = RequeteRechercheCode.value(RequeteRechercheCode.record().indexOf("NbrJoueurMin")).toString() ;
    QString Le_NbrJoueurMax = RequeteRechercheCode.value(RequeteRechercheCode.record().indexOf("NbrJoueurMax")).toString() ;
    int IdStatut =(RequeteRechercheCode.value(RequeteRechercheCode.record().indexOf("StatutJeux_IdStatutJeux")).toInt());
    int IdEditeur =(RequeteRechercheCode.value(RequeteRechercheCode.record().indexOf("Editeur_IdEditeur")).toInt());
    int IdEtat =(RequeteRechercheCode.value(RequeteRechercheCode.record().indexOf("EtatsJeu_IdEtatsJeu")).toInt());
    int IdEmplacement =(RequeteRechercheCode.value(RequeteRechercheCode.record().indexOf("Emplacement_IdEmplacement")).toInt());
    QString Classification =(RequeteRechercheCode.value(RequeteRechercheCode.record().indexOf("TypeJeux_Classification")).toString());

    // Remplir les champs en fonction du jeu choisi.
    ui->Le_nom->setText(Le_Nom) ;

    // aligne le curseur à gauche
    ui->Le_nom->setCursorPosition(0) ;
    ui->Le_code->setText(Le_Code);
    ui->Le_createur->setText(Le_Createur);

    // aligne le curseur à gauche
    ui->Le_createur->setCursorPosition(0) ;
    ui->TxE_contenu->setText(TxE_Contenu);
    ui->Le_agemin->setText(Le_AgeMin);
    ui->Le_agemax->setText(Le_AgeMax);
    ui->Le_nbrejoueursmin->setText(Le_NbrJoueurMin);
    ui->Le_nbrejoueursmax->setText(Le_NbrJoueurMax);
    ui->TxE_remarques->setText(TxE_Remarques);
    ui->TxE_description->setText(TxE_Description);

    // Bloquer certains boutons
    ui->Bt_ValiderContenu->setEnabled(false);
    ui->Bt_AnnulerContenu->setEnabled(false);
    ui->Bt_ValiderRemarques->setEnabled(false);
    ui->Bt_AnnulerRemarques->setEnabled(false);
    ui->Bt_ValiderDescription->setEnabled(false);
    ui->Bt_AnnulerDescription->setEnabled(false);
    
    //////////////////////////////////////////////////////////
    /////////// Remplissage label statut /////////////////////
    ///////////////////////////////////////////////////////////
    QSqlQuery RequeteStatut;
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

    RequeteEmplacement.prepare("SELECT Nom FROM emplacement WHERE IdEmplacement=:IdEmplacementDuJeu");
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

    ui->Lb_ImageName->setText(lb_image->ChargerImage(QSize(200,200),ui->Le_code->text()));

    ////////////////////////////////////////////////////////
    ////////////// Règles //////////////////////////////////
    //////////////////////////////////////////////////////
    QString sCheminReglePref;
    QSqlQuery RequeteCheminReglesJeux;
    RequeteCheminReglesJeux.exec("SELECT CheminReglesJeux FROM preferences WHERE IdPreferences = 1");
    // si il y a des préférences
    RequeteCheminReglesJeux.next();
    sCheminReglePref = RequeteCheminReglesJeux.value(0).toString();

    // Suppression des fichiers temporaires du précédent jeu
    if( EstCeURL(sCheminReglePref) )
    {
        QDir CheminFichierImage;
        for(int i=0;i<sCheminFichier.count();i++)
        {
            CheminFichierImage.remove(sCheminFichier[i]);
        }
    }

    QString TypeRegle;
    AccesFichierParHTTP manager;
    QString NomFichier;
    QStringList ListeExtension;
    ListeExtension<<"pdf"<<"docx"<<"doc"<<"html"<<"htm"<<"jpg"<<"jpeg"<<"png"<<"bmp"<<"gif"<<"xcf";
    int i=2;
    //qDebug() << "F_Jeux::AfficherJeu()" << "Code jeu" << this->nIdJeuSelectionne<< "ReglePref" << sCheminReglePref;
    // Recherche la première règle
    if(manager.FichierEtExtensionsExiste( &sCheminFichier, sCheminReglePref, this->nIdJeuSelectionne , &TypeRegle,ListeExtension))
    {
        //qDebug() << "F_Jeux::AfficherJeu()" << "Regles trouvée";
        ui->Bt_regle->setEnabled(true);
        i++;
        NomFichier = this->nIdJeuSelectionne + "-" + QString::number(2);
        // Tant qu'il existe des règles avec le même code jeu
        while( manager.FichierEtExtensionsExiste( &sCheminFichier,sCheminReglePref,NomFichier,&TypeRegle,ListeExtension))
        {
           // Nom du prochain fichier à chercher
           NomFichier = this->nIdJeuSelectionne + "-" + QString::number(i++);
        }
    }
    else
    {
        //qDebug() << "F_Jeux::AfficherJeu()" << "Regles non trouvée, Code :"<<this->nIdJeuSelectionne;
        ui->Bt_regle->setEnabled(false);
    }
    //qDebug() << "F_Jeux::AfficherJeu()" << "iNbFichier:" << iNbFichier<<" Code : "<<this->nIdJeuSelectionne;
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
    // Ouvre tous les fichiers sauvés dans sCheminFichier
    for(int i=0;i<sCheminFichier.count();i++)
    {
        QDesktopServices::openUrl(QUrl::fromLocalFile(sCheminFichier[i]));
    }
}
////////////////////////////////////////////////////////////
///////////////// Clique sur le bouton Valider remarque////
//////////////////////////////////////////////////////////
/**
 * @brief Méthode qui valider les remarques ajoutées
 *
 */
void F_Jeux::on_Bt_ValiderRemarques_clicked()
{
    QSqlQuery RequeteValiderRemarque;

    //prépare le requête de mise à jour
    RequeteValiderRemarque.prepare("UPDATE jeux SET Remarque=:NouvelRemarque WHERE CodeJeu=:CodeDuJeu");

    //Entre les valeurs de la requête
    RequeteValiderRemarque.bindValue(":CodeDuJeu",JeuEnConsultation);
    RequeteValiderRemarque.bindValue(":NouvelRemarque",ui->TxE_remarques->toPlainText());

    //Exécute la requête
    if (!RequeteValiderRemarque.exec())
    {

    }

    //Grise les boutons de modification de le remarque
    ui->Bt_ValiderRemarques->setEnabled(false);
    ui->Bt_AnnulerRemarques->setEnabled(false);
    ui->TxE_remarques->setReadOnly(true);
}
////////////////////////////////////////////////////////////
///////////////// Clique sur le bouton AnnulerRemarques////
//////////////////////////////////////////////////////////
/**
 * @brief Méthode qui annuler les remarques ajoutées
 *
 */
void F_Jeux::on_Bt_AnnulerRemarques_clicked()
{
    QSqlQuery RequeteAnnulerRemarques;

    RequeteAnnulerRemarques.prepare("SELECT Remarque FROM jeux WHERE CodeJeu=:CodeDuJeu");
    RequeteAnnulerRemarques.bindValue(":CodeDuJeu",JeuEnConsultation);

    //Exécute la requête
    if (!RequeteAnnulerRemarques.exec())
    {
        qDebug() << "F_Jeux::on_Bt_AnnulerRemarques_clicked() : RequeteAnnulerContenu :" << RequeteAnnulerRemarques.lastQuery()  ;
    }

    RequeteAnnulerRemarques.next();

    //Récupère les remarques dans la base de données et les affiche
    QString TexteRemarque = (RequeteAnnulerRemarques.value(0).toString());
    ui->TxE_remarques->setText(TexteRemarque);

    //Grise les boutons de modification des remarques du membre
    ui->Bt_ValiderRemarques->setEnabled(false);
    ui->Bt_AnnulerRemarques->setEnabled(false);
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
///////// Clique sur le bouton Valider contenu ////////
//////////////////////////////////////////////////////////
/**
 * @brief Méthode qui valide le nouveau contenu
 *
 */
void F_Jeux::on_Bt_ValiderContenu_clicked()
{
    QSqlQuery RequeteValiderContenu;

    //prépare le requête de mise à jour
    RequeteValiderContenu.prepare("UPDATE jeux SET ContenuJeu=:NouveauContenu WHERE CodeJeu=:CodeDuJeu");

    //Entre les valeurs de la requête
    RequeteValiderContenu.bindValue(":CodeDuJeu",JeuEnConsultation);
    RequeteValiderContenu.bindValue(":NouveauContenu",ui->TxE_contenu->toPlainText());

    //Exécute la requête
    if (!RequeteValiderContenu.exec())
    {
        qDebug() << "F_Jeux::on_Bt_ValiderContenu_clicked() : RequeteValiderContenu :" << RequeteValiderContenu.lastQuery()  ;
    }

    //Grise les boutons de modification du contenu
    ui->Bt_ValiderContenu->setEnabled(false);
    ui->Bt_AnnulerContenu->setEnabled(false);
}
////////////////////////////////////////////////////////////
///////// Clique sur le bouton Annuler contenu ////////////
//////////////////////////////////////////////////////////
/**
 * @brief Méthode qui annuler le nouveau contenu ajoutée
 *
 */
void F_Jeux::on_Bt_AnnulerContenu_clicked()
{
    QSqlQuery RequeteAnnulerContenu;

    RequeteAnnulerContenu.prepare("SELECT ContenuJeu FROM jeux WHERE CodeJeu=:CodeDuJeu");
    RequeteAnnulerContenu.bindValue(":CodeDuJeu",JeuEnConsultation);
    //Exécute la requête
    if (!RequeteAnnulerContenu.exec())
    {
        qDebug() << "F_Jeux::on_Bt_AnnulerContenu_clicked() : RequeteAnnulerContenu :" << RequeteAnnulerContenu.lastQuery()  ;
    }

    RequeteAnnulerContenu.next();

    //Récupère les remarques dans la base de données et les affiche
    QString TexteContenu = (RequeteAnnulerContenu.value(0).toString());
    ui->TxE_contenu->setText(TexteContenu);

    //Grise les boutons de modification du contenu
    ui->Bt_ValiderContenu->setEnabled(false);
    ui->Bt_AnnulerContenu->setEnabled(false);
}
////////////////////////////////////////////////////////////
///////////////// Lorsque le texte remarques change ////////
//////////////////////////////////////////////////////////
/**
 * @brief Méthode qui active le bouton valider et annuler lorsque le contenu de la case remarque change
 *
 */
void F_Jeux::on_TxE_remarques_textChanged()
{
    ui->Bt_AnnulerRemarques->setEnabled(true);
    ui->Bt_ValiderRemarques->setEnabled(true);
    //qDebug()<<"F_Jeux::on_TxE_remarques_textChanged()";
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
///////// texte contenu qui change ////////////////////////
//////////////////////////////////////////////////////////
/**
 * @brief Méthode qui active le bouton valider et annuler lorsque le contenu de la case contenu change
 *
 */
void F_Jeux::on_TxE_contenu_textChanged()
{
    ui->Bt_ValiderContenu->setEnabled(true);
    ui->Bt_AnnulerContenu->setEnabled(true);
}

void F_Jeux::AfficherJeux()
{
     unsigned int NumeroLigne (0);
     QSqlQuery RequeteRechercheJeu;
     NumeroLigne=0;

     RequeteRechercheJeu.prepare("SELECT CodeJeu,NomJeu FROM jeux");

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
    //Tant qu'il y a des jeux dans la table jeux,
    for(int i=0;i<this->ModelJeu->rowCount();i++)
    {
        // Si le texte saisie est un nombre
        if( numeric )
        {
            ColonneRecherche=CodeColonne;
        }
        else
        {
            ColonneRecherche=CodeNomJeu;
        }

        TexteCellule = this->ModelJeu->item(i,ColonneRecherche)->text();
        if(TexteCellule.indexOf(
                    NomJeu,0,Qt::CaseInsensitive ) != string::npos )
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
    this->nIdJeuSelectionne=this->ModelJeu->index(firstrow, 0).data().toString();
    AfficherJeu();
}

/**
 * @brief Méthode qui récupère le code du jeu sur lequel on a cliqué dans le tableau
 *
 * @param index
 */
void F_Jeux::on_TbV_NomJeux_clicked(const QModelIndex &index)
{
    this->nIdJeuSelectionne=this->ModelJeu->index(index.row(), 0).data().toString();
    AfficherJeu() ;
}

/**
 * @brief Méthode qui actualise la case de recherche par code avec le code du jeu actif
 *
 */
void F_Jeux::ActualiserJeux()
{
    AfficherJeu() ;
    ui->Le_recherchenom->setFocus();
}

/**
 * @brief Méthode qui récupère le code du jeu en cour de consultation
 *
 * @return QString
 */
QString F_Jeux::get_JeuEnConsultation()
{
    return this->JeuEnConsultation ;
}

/**
 * @brief Fixe le code du jeu en cour de consultation
 *
 * @param CodeJeuChoisi Code du jeu
 */
void F_Jeux::set_JeuEnConsultation(QString CodeJeuChoisi)
{
    this->JeuEnConsultation = CodeJeuChoisi ;
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
      ui->Bt_Reserver->setDisabled(true);
   }
   else
   {
      // Rendre possible la réservation et les interventions pour ce jeu affiché
      // et seulement si un jeu a été choisi
      ui->Bt_DeclarerIntervention->setEnabled(true);
      ui->Bt_Reserver->setEnabled(true);
   }
}

void F_Jeux::on_Bt_Gauche_clicked()
{
    ui->Lb_ImageName->setText(lb_image->AfficherImagePrecedente());
}

void F_Jeux::on_Bt_Droite_clicked()
{
    ui->Lb_ImageName->setText(lb_image->AfficherImageSuivante());
}

void F_Jeux::on_Lb_Image_clicked()
{
    disconnect( lb_image, SIGNAL( clicked() ), this, SLOT( on_Lb_Image_clicked() ) );
    QDialog *d_image = new D_Image(this,lb_image);
    d_image->exec();
    ui->gridLayout_11->addWidget(lb_image,0,3);
    connect( lb_image, SIGNAL( clicked() ), this, SLOT( on_Lb_Image_clicked() ) );
}
