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
  * @todo Vérification du bon fonctionnement et de la cohérence des résultats
  *
*/
#include <QtSql>
#include <QDesktopServices>

// EN-TETES UNIQUEMENT UTILISÉS DANS CE FICHIER
#include "f_jeux.h"
#include "ui_f_jeux.h"
#include "f_reservation.h"
#include <QStandardItemModel>
#include <QProcess>
#include <QPixmap>
#include <QDir>
#include<QMessageBox>

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
    // Initialise la table view avec tous les jeux
    on_Le_recherchenom_textChanged("") ;
    //Supprime le numéro des lignes
    ui->TbV_NomJeux->verticalHeader()->setVisible(false);
    // Faire défiler le tableau des jeux avec les flêches du clavier
    connect(ui->TbV_NomJeux->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(on_TbV_NomJeux_clicked(QModelIndex)));
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
void F_Jeux::on_Bt_reserver_clicked()
{
    //Savoir si le jeux existe
    QSqlQuery RequeteJeu;
    RequeteJeu.prepare("SELECT IdJeux FROM jeux WHERE CodeJeu=:CodeDuJeu");
    RequeteJeu.bindValue(":CodeDuJeu",this->JeuEnConsultation);
    RequeteJeu.exec();
    if (!RequeteJeu.exec())
    {
        qDebug() << "F_Jeux::on_Bt_reserver_clicked() : RequeteJeu :" << RequeteJeu.lastError().text()  ;
    }
    RequeteJeu.next();

    if(RequeteJeu.size()==0)
    {
        QMessageBox::information(this,"Réservation impossible","Ce jeu n'existe pas\nSélectionnez un jeu valide","Ok");
        return;
    }

    //Savoir si le jeux est déja réservé :
    QSqlQuery RequeteResa;
    RequeteResa.prepare("SELECT idReservation FROM reservation WHERE Jeux_IdJeux=:IdDuJeu AND JeuEmprunte=1");
    RequeteResa.bindValue(":IdDuJeu",RequeteJeu.value(0));

    if (!RequeteResa.exec())
    {
        qDebug() << "F_Jeux::on_Bt_reserver_clicked() : RequeteResa :" << RequeteResa.lastError().text()  ;
    }
    if (RequeteResa.size()!=0)
    {
        QMessageBox::information(this,"Réservation impossible !","Ce jeu est déjà réservé.","Ok");
        return;
    }

    pReservation->setWindowModality(Qt::ApplicationModal);
    pReservation->show();
    pReservation->set_JeuActif(this->JeuEnConsultation);
    pReservation->AfficherJeu();
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
void F_Jeux::on_Bt_declarer_clicked()
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
void F_Jeux::on_Bt_ok_clicked()
{
    ui->TxE_contenu->setReadOnly(false);
    ui->TxE_description->setReadOnly(false);
    ui->TxE_emplacement->setReadOnly(false);
    ui->TxE_remarques->setReadOnly(false);
    ///////////////////////////////////////////////////
    ////////////// Recherche par code ////////////////
    /////////////////////////////////////////////////
    
    QSqlQuery RequeteRechercheCode ;
    QString Code = ui->Le_recherchecode->text() ;

    JeuEnConsultation = Code ;

    RequeteRechercheCode.prepare("SELECT NomJeu, CodeJeu, NomCreateurJeu, ContenuJeu, Remarque, StatutJeux_IdStatutJeux, EtatsJeu_IdEtatsJeu, Emplacement_IdEmplacement, AgeMin, AgeMax, NbrJoueurMin, NbrJoueurMax, TypeJeux_Classification, DescriptionJeu,Editeur_IdEditeur FROM jeux WHERE CodeJeu=:CodeDuJeu") ;
    RequeteRechercheCode.bindValue(":CodeDuJeu", Code);
    if (!RequeteRechercheCode.exec())
    {
        qDebug() << "F_Jeux::on_Bt_ok_clicked() : RequeteRechercheCode :" << RequeteRechercheCode.lastError().text()  ;
    }
    RequeteRechercheCode.next();

    QString Le_Nom =  RequeteRechercheCode.value(0).toString() ;
    QString Le_Code =  RequeteRechercheCode.value(1).toString() ;
    QString Le_Createur =  RequeteRechercheCode.value(2).toString() ;
    QString TxE_Contenu = RequeteRechercheCode.value(3).toString() ;
    QString TxE_Remarques = RequeteRechercheCode.value(4).toString() ;
    QString TxE_Description = RequeteRechercheCode.value(13).toString() ;
    QString Le_AgeMin = RequeteRechercheCode.value(8).toString() ;
    QString Le_AgeMax = RequeteRechercheCode.value(9).toString() ;
    QString Le_NbrJoueurMin = RequeteRechercheCode.value(10).toString() ;
    QString Le_NbrJoueurMax = RequeteRechercheCode.value(11).toString() ;

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
    int IdStatut =(RequeteRechercheCode.value(5).toInt());
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
    int IdEditeur =(RequeteRechercheCode.value(14).toInt());
    QSqlQuery RequeteEditeur;
    QString Le_Editeur ;

    RequeteEditeur.prepare("SELECT NomFournisseur FROM fournisseursediteur WHERE IdFournisseur=:IdFournisseur");
    RequeteEditeur.bindValue(":IdFournisseur",IdEditeur);
    RequeteEditeur.exec() ;
    RequeteEditeur.next();
    Le_Editeur = (RequeteEditeur.value(0).toString());
    ui->Le_editeur->setText(Le_Editeur);
    // aligne le curseur à gauche
    ui->Le_statut->setCursorPosition(0) ;

    /////////////////////////////////////////////////////////
    //////////// Remplissage label Etat Jeu /////////////////
    /////////////////////////////////////////////////////////
    int IdEtat =(RequeteRechercheCode.value(6).toInt());

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
    int IdEmplacement =(RequeteRechercheCode.value(7).toInt());

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
    QString Classification =(RequeteRechercheCode.value(12).toString());

    QSqlQuery RequeteClassification;
    QString Le_Classification ;
    QString LE_NumClassification ;

    RequeteClassification.prepare("SELECT TypeJeux, Classification FROM typejeux WHERE Classification=:NumDeLaClassifciation");
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
    ////////////// Photo //////////////////////////////////
    //////////////////////////////////////////////////////
    QString sCheminImage=QApplication::applicationDirPath() + "/photos/" + ui->Le_code->text() ;

    if( QFile::exists( sCheminImage + ".jpg" ) )
    {
        sCheminImage = sCheminImage + ".jpg" ;
    }
    else
    if( QFile::exists( sCheminImage + ".jpeg" ) )
    {
        sCheminImage = sCheminImage + ".jpeg" ;
    }
    else
    if( QFile::exists( sCheminImage + ".png" ) )
    {
        sCheminImage = sCheminImage + ".png" ;
    }
    else
    if( QFile::exists( sCheminImage + ".bmp" ) )
    {
        sCheminImage = sCheminImage + ".bmp" ;
    }
    else
    if( QFile::exists( sCheminImage + ".gif" ) )
    {
        sCheminImage = sCheminImage + ".gif" ;
    }

    QDir CheminFichierImage( sCheminImage );
    // si le chemin est faux ou l'image n'existe pas, efface l'image d'avant automatiquement
    QPixmap Image( CheminFichierImage.absolutePath() ) ;
    ui->Lb_Image->setPixmap( Image ) ;
    //Met l'image à l'échelle du cadre
    ui->Lb_Image->setScaledContents( true ) ;
    //qDebug()<< "F_Jeux::on_Bt_ok_clicked() =>  sCheminImage=" <<  CheminFichierImage.absolutePath() ;

    /////////////////////////////////////////////
    // action particulière pour certains champs
    /////////////////////////////////////////////
    /*
    if(ui->Le_agemax->text() == "" || ui->Le_agemax->text() == "0")
    {
        ui->Le_agemax->setText("Non renseigné");
        ui->Le_agemax->setStyleSheet("color: red");
    }
    else
    {
        ui->Le_agemax->setStyleSheet("color: black");
    }
    //-------------------------------------------
    if(ui->Le_agemin->text() == "")
    {
        ui->Le_agemin->setText("Non renseigné");
        ui->Le_agemin->setStyleSheet("color: red");
    }
    else
    {
        ui->Le_agemin->setStyleSheet("color: black");
    }
    //-------------------------------------------
    if(ui->Le_classification->text() == "")
    {
        ui->Le_classification->setText("Non renseigné");
        ui->Le_classification->setStyleSheet("color: red");
    }
    else
    {
        ui->Le_classification->setStyleSheet("color: black");
    }
    //-------------------------------------------
    if(ui->Le_createur->text() == "")
    {
        ui->Le_createur->setText("Non renseigné");
        ui->Le_createur->setStyleSheet("color: red");
    }
    else
    {
        ui->Le_createur->setStyleSheet("color: black");
    }
    //-------------------------------------------
    if(ui->Le_editeur->text() == "")
    {
        ui->Le_editeur->setText("Non renseigné");
        ui->Le_editeur->setStyleSheet("color: red");
    }
    else
    {
        ui->Le_editeur->setStyleSheet("color: black");
    }
    //------------------------------------------
    if(ui->Le_etat->text() == "")
    {
        ui->Le_etat->setText("Non renseigné");
        ui->Le_etat->setStyleSheet("color: red");
    }
    else
    {
        ui->Le_etat->setStyleSheet("color: black");
    }
    //-------------------------------------------
    if(ui->Le_nbrejoueursmax->text() == "" || ui->Le_nbrejoueursmax->text() == "0")
    {
        ui->Le_nbrejoueursmax->setText("Non renseigné");
        ui->Le_nbrejoueursmax->setStyleSheet("color: red");
    }
    else
    {
        ui->Le_nbrejoueursmax->setStyleSheet("color: black");
    }
    //-------------------------------------------
    if(ui->Le_nbrejoueursmin->text() == "")
    {
        ui->Le_nbrejoueursmin->setText("Non renseigné");
        ui->Le_nbrejoueursmin->setStyleSheet("color: red");
    }
    else
    {
        ui->Le_nbrejoueursmin->setStyleSheet("color: black");
    }
    //-------------------------------------------
    if(ui->LE_NumClassification->text() == "")
    {
        ui->Le_nbrejoueursmin->setStyleSheet("background-color: red");
    }
    else
    {
        ui->Le_nbrejoueursmin->setStyleSheet("background-color: white");
    }
    //-------------------------------------------
    if(ui->TxE_contenu->toPlainText() == "")
    {
        ui->TxE_contenu->setText("Non renseigné");
        ui->TxE_contenu->setStyleSheet("color: red");
    }
    else
    {
        ui->TxE_contenu->setStyleSheet("color: black");
    }
    //-------------------------------------------
    if(ui->TxE_description->toPlainText() == "")
    {
        ui->TxE_description->setText("Non renseigné");
        ui->TxE_description->setStyleSheet("color: red");
    }
    else
    {
        ui->TxE_description->setStyleSheet("color: black");
    }
    //-------------------------------------------
    if(ui->TxE_emplacement->toPlainText() == "")
    {
        ui->TxE_emplacement->setText("Non renseigné");
        ui->TxE_emplacement->setStyleSheet("color: red");
    }
    else
    {
        ui->TxE_emplacement->setStyleSheet("color: black");
    }
    //-------------------------------------------
    if(ui->TxE_remarques->toPlainText() == "")
    {
        ui->TxE_remarques->setText("Non renseigné");
        ui->TxE_remarques->setStyleSheet("color: red");
    }
    else
    {
        ui->TxE_remarques->setStyleSheet("color: black");
    }
    */
}

////////////////////////////////////////////////////////////
///////// Clique sur le bouton Règles du jeu //////////////
//////////////////////////////////////////////////////////
/**
 * @brief Méthode qui ouvre la règle du jeu (non fonctionnel)
 *
 */
void F_Jeux::on_Bt_regle_clicked()
{
    QSqlQuery RequeteRegleDuJeu ;
    QString DebutCheminRegleDuJeu ;
    QString FinCheminRegleJeu ;
    QString CheminComplet ;

    RequeteRegleDuJeu.prepare("SELECT CodeJeu,CheminRegle, IdPreferences FROM jeux, preferences WHERE CodeJeu = :CodeDuJeu AND IdPreferences = 1") ;
    RequeteRegleDuJeu.bindValue(":CodeDuJeu", ui->Le_code->text().toInt());
    RequeteRegleDuJeu.exec() ;

    RequeteRegleDuJeu.next() ;

    DebutCheminRegleDuJeu = RequeteRegleDuJeu.value(1).toString() ;
    FinCheminRegleJeu = ui->Le_code->text() ;

    CheminComplet = DebutCheminRegleDuJeu + "/" + FinCheminRegleJeu /* + ".pdf" */;

    QDesktopServices::openUrl(QUrl::fromLocalFile(CheminComplet));

    if(!QDesktopServices::openUrl(QUrl::fromLocalFile(CheminComplet)))
    {
        QMessageBox::information(this, "Règle du jeu", "Ce jeu n'a pas de règle correspondant", "OK") ;
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
        qDebug() << "F_Jeux::on_Bt_AnnulerRemarques_clicked() : RequeteAnnulerContenu :" << RequeteAnnulerRemarques.lastError().text()  ;
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
        qDebug() << "F_Jeux::on_Bt_ValiderDescription_clicked() : RequeteValiderDescription :" << RequeteValiderDescription.lastError().text()  ;
    }
    //Grise les boutons de modification de la description
    ui->Bt_ValiderDescription->setEnabled(false);
    ui->Bt_AnnulerDescription->setEnabled(false);
    ui->TxE_description->setReadOnly(true);
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
        qDebug() << "F_Jeux::on_Bt_AnnulerDescription_clicked() : RequeteAnnulerDescription :" << RequeteAnnulerDescription.lastError().text()  ;
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
        qDebug() << "F_Jeux::on_Bt_ValiderContenu_clicked() : RequeteValiderContenu :" << RequeteValiderContenu.lastError().text()  ;
    }

    //Grise les boutons de modification du contenu
    ui->Bt_ValiderContenu->setEnabled(false);
    ui->Bt_AnnulerContenu->setEnabled(false);
    ui->TxE_contenu->setReadOnly(true);
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
        qDebug() << "F_Jeux::on_Bt_AnnulerContenu_clicked() : RequeteAnnulerContenu :" << RequeteAnnulerContenu.lastError().text()  ;
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
    ui->Le_recherchecode->setText("");
    unsigned int NumeroLigne (0);
    QString NomJeu=arg1;

    if(NomJeu.size()>=2)
    {
        NomJeu="%"+NomJeu+"%";

        QSqlQuery RequeteRechercheJeu;
        NumeroLigne=0;

        RequeteRechercheJeu.prepare("SELECT CodeJeu,NomJeu FROM jeux WHERE NomJeu LIKE (:NomJeu)");
        RequeteRechercheJeu.bindValue(":NomJeu",NomJeu);

        if (!RequeteRechercheJeu.exec())
        {
            qDebug() << "F_Jeux::on_Le_recherchenom_textChanged() : RequeteRechercheJeu :" << RequeteRechercheJeu.lastError().text()  ;
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
    else
    {
        QSqlQuery RequeteRechercheJeu;
        NumeroLigne =0;
        RequeteRechercheJeu.exec("SELECT  CodeJeu,NomJeu FROM jeux ORDER BY NomJeu ASC");

        //On vide le model
        this->ModelJeu->clear();
        //Indique le nombes de colones puis leurs noms
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
}
////////////////////////////////////////////////////////////
///////// Lien entre le bouton OK et la touche Entree//////
//////////////////////////////////////////////////////////
/**
 * @brief Méthode qui active la méthode Bt_Ok_Clickek lorsque l'on appui sur la touche entrée du clavier
 *
 */
void F_Jeux::on_Le_recherchecode_returnPressed()
{
    on_Bt_ok_clicked() ;
}


/**
 * @brief Méthode qui récupère le code du jeu sur lequel on a cliqué dans le tableau
 *
 * @param index
 */
void F_Jeux::on_TbV_NomJeux_clicked(const QModelIndex &index)
{
    // Récupère le code du jeu, et l'inscrit dans le LineEdit
    ui->Le_recherchecode->setText(this->ModelJeu->index(index.row(), 0).data().toString());
    //Valide la code du jeu récupéré
    on_Bt_ok_clicked() ;
    //Séléctionne la ligne sur laquelle on a cliqué
    ui->TbV_NomJeux->selectRow(index.row());
}

/**
 * @brief Méthode qui vide le contenu de la case de recherche par nom lorsque l'on rentre un code
 *
 * @param arg1
 */
void F_Jeux::on_Le_recherchecode_textChanged(const QString &arg1)
{
    //ui->Le_recherchenom->setText("");
    
    if(ui->Le_recherchecode->text() == "" && ui->Le_recherchenom->text() == "")
    {
        on_Le_recherchenom_textChanged("") ;
    }

}
/**
 * @brief Méthode qui actualise la case de recherche par code avec le code du jeu actif
 *
 */
void F_Jeux::ActualiserJeux()
{
    ui->Le_recherchecode->setText(this->JeuEnConsultation);
    on_Bt_ok_clicked() ;
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

