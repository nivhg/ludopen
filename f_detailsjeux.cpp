//---------------------------------------------------
/** @file f_detailsjeux.cpp
  * @brief Classe graphique permettant de visualiser les détails sur un jeu
  *
  * @author Florian MARY
  * @author STS IRIS, Lycée Nicolas APPERT, ORVAULT (FRANCE)
  * @since 01/01/2012
  * @version 0.1
  * @date 01/01/2012
  *
  * Cette classe permet de visualiser les différents détails sur un jeu, les prix, les historiques ....
  *
  * Fabrication QtCreator, LudOpen
  *
  *
*/
//-------------------------------------------------------
// EN-TETE STANDARD ----------------------------------
#include <QtSql>
#include <QStandardItemModel>

// En-tête propre à l'application ----------------------------------------------
#include "f_detailsjeux.h"
#include "ui_f_detailsjeux.h"



/**
 * @brief Constructeur de la classe f_detailsjeux
 *
 * @param parent
 */
F_DetailsJeux::F_DetailsJeux(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::F_DetailsJeux)
{
    ui->setupUi(this);
    

    
    
    //Création d'un model pour le TableView des details jeu
        this->ModelDetailHistoriqueJeu = new QStandardItemModel() ;
        this->ModelDetailEmpruntsJeu = new QStandardItemModel() ;
    //Associe le modèl au TableView
        ui->TbV_Historique->setModel(this->ModelDetailHistoriqueJeu);
        ui->TbV_HistoriqueEmprunts->setModel(this->ModelDetailEmpruntsJeu);
    // Mise en lecture seule
        ui->TbV_Historique->setEditTriggers(0);
        ui->TbV_HistoriqueEmprunts->setEditTriggers(0);
}

/**
 * @brief Destructeur de la classe f_detailsjeux
 *
 */
F_DetailsJeux::~F_DetailsJeux()
{
    delete ui;
}

/**
 * @brief Méthode qui renseigne du jeu en consultation l'attribut JeuEnConsultation
 *
 * @param CodeJeu
 */
void F_DetailsJeux::set_JeuEnConsultation(QString CodeJeu)
{
    this->JeuEnConsultation = CodeJeu ;
    
    // Requete pour avoir le nom du jeu en consultation et donc le mettre en titre de fenêtre
    this->JeuEnConsultation = CodeJeu ;
    QSqlQuery RequeteNomFenetre ;
    RequeteNomFenetre.prepare("SELECT NomJeu, CodeJeu FROM jeux WHERE CodeJeu = :CodeDuJeu") ;
    RequeteNomFenetre.bindValue(":CodeDuJeu", JeuEnConsultation);
    RequeteNomFenetre.exec() ;
    RequeteNomFenetre.next() ;
    QString NomFenetre = RequeteNomFenetre.value(0).toString() ;
    setWindowTitle(NomFenetre);
    
}

/**
 * @brief Méthode qui affiche les détails du jeu dans les champs correspondants
 *
 */
void F_DetailsJeux::AfficherDetailJeu()
{
    QSqlQuery RequeteDetailJeu ;
    QString Code = this->JeuEnConsultation ;
    JeuEnConsultation = Code ;

    RequeteDetailJeu.prepare("SELECT DateAchat, EtatInitial, PrixAchat, PrixLoc, Caution FROM jeux WHERE CodeJeu=:CodeDuJeu") ;
    RequeteDetailJeu.bindValue(":CodeDuJeu", Code) ;

    RequeteDetailJeu.exec() ;
    RequeteDetailJeu.next() ;

    QString LE_DateAchat = RequeteDetailJeu.value(0).toDate().toString("ddd d MMMM yyyy") ;
    QString LE_EtatInitial = RequeteDetailJeu.value(1).toString() ;
    QString LE_PrixAchat = RequeteDetailJeu.value(2).toString() ;
    QString LE_PrixLoc = RequeteDetailJeu.value(3).toString() ;
    QString LE_Caution = RequeteDetailJeu.value(4).toString() ;
        
    ui->LE_DateAchat->setText(LE_DateAchat);
    ui->LE_EtatInitial->setText(LE_EtatInitial);
    ui->LE_PrixAchat->setText(LE_PrixAchat);
    ui->LE_PrixLocation->setText(LE_PrixLoc);
    ui->LE_Caution->setText(LE_Caution);
    
    //////////////////////////////////////////////////////
    ///////Affichage correspondant dans le table view////
    //////////////// Historique intervention ///////////
    ////////////////////////////////////////////////////
    unsigned int NumeroLigne (0);
    QSqlQuery RequeteHistoriqueInterventionJeu;
    JeuEnConsultation = Code ;

    

    RequeteHistoriqueInterventionJeu.prepare("SELECT `DateIntervention`,`TypeIntervention`, CodeJeu FROM`intervientionsjeu`, jeux WHERE `Jeux_IdJeux` = IdJeux AND CodeJeu = :CodeDuJeu ORDER BY DateIntervention DESC");
    RequeteHistoriqueInterventionJeu.bindValue(":CodeDuJeu",Code);
    RequeteHistoriqueInterventionJeu.exec();
    
    //On vide le model
    this->ModelDetailHistoriqueJeu->clear();
    //Indique le nombres de colonnes puis leurs noms
    this->ModelDetailHistoriqueJeu->setColumnCount(2);
    this->ModelDetailHistoriqueJeu->setHorizontalHeaderItem(0, new QStandardItem("Date intervention"));
    this->ModelDetailHistoriqueJeu->setHorizontalHeaderItem(1, new QStandardItem("Type intervention"));
    //Impose une taille aux colonnes
    ui->TbV_Historique->setColumnWidth(0,120);
    ui->TbV_Historique->setColumnWidth(1,120);
    while(RequeteHistoriqueInterventionJeu.next())
    {
        //On ajoute une nouvelle ligne du table view avec les données
        this->ModelDetailHistoriqueJeu->setItem(NumeroLigne, 0, new QStandardItem(RequeteHistoriqueInterventionJeu.value(0).toDate().toString("ddd d MMMM yyyy") ));
        this->ModelDetailHistoriqueJeu->setItem(NumeroLigne, 1, new QStandardItem(RequeteHistoriqueInterventionJeu.value(1).toString() ));
        // Adapte la taille de la ligne en fonction de son contenu
        this->ui->TbV_Historique->resizeRowToContents(NumeroLigne) ;
        NumeroLigne ++ ;
    }
    
    //////////////////////////////////////////////////////
    ///////Affichage correspondant dans le table view////
    //////////////// Historique emprunts  ///////////////
    ////////////////////////////////////////////////////
    
    NumeroLigne = 0;
    QSqlQuery RequeteHistoriqueEmprunt;
    JeuEnConsultation = Code ;


    RequeteHistoriqueEmprunt.prepare("SELECT Prenom, Nom, DateEmprunt, DateRetour, Membres_IdMembre FROM emprunts, membres, jeux WHERE Membres_IdMembre = IdMembre AND CodeJeu = :CodeDuJeu AND Jeux_IdJeux = IdJeux ORDER BY DateEmprunt DESC");
    RequeteHistoriqueEmprunt.bindValue(":CodeDuJeu",Code);
    RequeteHistoriqueEmprunt.exec();
    
    //On vide le model
    this->ModelDetailEmpruntsJeu->clear();
    //Indique le nombres de colonnes puis leurs noms
    this->ModelDetailEmpruntsJeu->setColumnCount(4);
    this->ModelDetailEmpruntsJeu->setHorizontalHeaderItem(0, new QStandardItem("Nom"));
    this->ModelDetailEmpruntsJeu->setHorizontalHeaderItem(1, new QStandardItem("Prenom"));
    this->ModelDetailEmpruntsJeu->setHorizontalHeaderItem(2, new QStandardItem("Date emprunt"));
    this->ModelDetailEmpruntsJeu->setHorizontalHeaderItem(3, new QStandardItem("Date retour"));
    
    //Impose une taille aux colonnes
    ui->TbV_HistoriqueEmprunts->setColumnWidth(0,200);
    ui->TbV_HistoriqueEmprunts->setColumnWidth(1,200);
    ui->TbV_HistoriqueEmprunts->setColumnWidth(2, 150);
    ui->TbV_HistoriqueEmprunts->setColumnWidth(3, 150);
    while(RequeteHistoriqueEmprunt.next())
    {
        //On ajoute une nouvelle ligne du table view avec les données
        this->ModelDetailEmpruntsJeu->setItem(NumeroLigne, 0, new QStandardItem(RequeteHistoriqueEmprunt.value(1).toString() ));
        this->ModelDetailEmpruntsJeu->setItem(NumeroLigne, 1, new QStandardItem(RequeteHistoriqueEmprunt.value(0).toString() ));
        this->ModelDetailEmpruntsJeu->setItem(NumeroLigne, 2, new QStandardItem(RequeteHistoriqueEmprunt.value(2).toDate().toString("ddd d MMMM yyyy") ));
        this->ModelDetailEmpruntsJeu->setItem(NumeroLigne, 3, new QStandardItem(RequeteHistoriqueEmprunt.value(3).toDate().toString("ddd d MMMM yyyy") ));
        NumeroLigne ++ ;
    }



}

/**
 * @brief Méthode qui quitte la fenêtre de déclaration d'intervention
 *
 */
void F_DetailsJeux::on_Bt_Quitter_clicked()
{
    this->hide();
}
