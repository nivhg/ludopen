/** @file    f_detailsjeux.cpp
  * @brief   Classe graphique permettant de visualiser les détails sur un jeu
  *
  * @author  Florian MARY
  * @author  STS IRIS, Lycée Nicolas APPERT, ORVAULT (FRANCE)
  * @since   01/01/2012
  * @version 0.1
  * @date    05/03/2013
  * @author  William SOREL
  *
  * Cette classe permet de visualiser les différents détails sur un jeu, les prix, les historiques ....
  *
  *
*/


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
    //Création d'un modèle pour le TableView des détails de ce jeu
    this->ModelHistoriqueInterventions = new QStandardItemModel() ;
    this->ModelHistoriqueEmprunts = new QStandardItemModel() ;
    //Associe le modèle au TableView
    ui->TbV_HistoriqueInterventions->setModel(this->ModelHistoriqueInterventions);
    ui->TbV_HistoriqueEmprunts->setModel(this->ModelHistoriqueEmprunts);
    // Mise en lecture seule
    ui->TbV_HistoriqueInterventions->setEditTriggers(0);
    ui->TbV_HistoriqueEmprunts->setEditTriggers(0);
    // Permettre le tri des colonnes
    ui->TbV_HistoriqueInterventions->setSortingEnabled(true);
    ui->TbV_HistoriqueEmprunts->setSortingEnabled(true);
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
    RequeteNomFenetre.prepare("SELECT NomJeu,CodeJeu FROM jeux WHERE CodeJeu=:CodeDuJeu") ;
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

    RequeteDetailJeu.prepare("SELECT DateAchat,EtatInitial,PrixAchat,PrixLoc,Caution FROM jeux WHERE CodeJeu=:CodeDuJeu") ;
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

    RequeteHistoriqueInterventionJeu.prepare("SELECT DateIntervention,TypeIntervention,CodeJeu FROM intervientionsjeu,jeux WHERE Jeux_IdJeux=IdJeux AND CodeJeu=:CodeDuJeu ORDER BY DateIntervention DESC");
    RequeteHistoriqueInterventionJeu.bindValue(":CodeDuJeu",Code);
    RequeteHistoriqueInterventionJeu.exec();
    
    //On vide le model
    this->ModelHistoriqueInterventions->clear();
    //Indique le nombres de colonnes puis leurs noms
    this->ModelHistoriqueInterventions->setColumnCount(2);
    this->ModelHistoriqueInterventions->setHorizontalHeaderItem(0, new QStandardItem("Date d'intervention"));
    this->ModelHistoriqueInterventions->setHorizontalHeaderItem(1, new QStandardItem("Type d'intervention"));
    //Impose une taille aux colonnes
    ui->TbV_HistoriqueInterventions->setColumnWidth(0,120);
    ui->TbV_HistoriqueInterventions->setColumnWidth(1,120);
    while(RequeteHistoriqueInterventionJeu.next())
    {
        //On ajoute une nouvelle ligne du table view avec les données
        this->ModelHistoriqueInterventions->setItem(NumeroLigne, 0, new QStandardItem(RequeteHistoriqueInterventionJeu.value(0).toDate().toString("dd-MM-yyyy") ));
        this->ModelHistoriqueInterventions->setItem(NumeroLigne, 1, new QStandardItem(RequeteHistoriqueInterventionJeu.value(1).toString() ));
        // Adapte la taille de la ligne en fonction de son contenu
        this->ui->TbV_HistoriqueInterventions->resizeRowToContents(NumeroLigne) ;
        NumeroLigne ++ ;
    }
    
    /////////////////////////////////////////////////////
    ///////Affichage correspondant dans le table view////
    //////////////// Historique des emprunts  ///////////
    /////////////////////////////////////////////////////
    
    NumeroLigne = 0;
    QSqlQuery RequeteHistoriqueEmprunt;
    JeuEnConsultation = Code ;

    RequeteHistoriqueEmprunt.prepare("SELECT Prenom,Nom,DateEmprunt,DateRetour,Membres_IdMembre FROM emprunts,membres,jeux WHERE Membres_IdMembre=IdMembre AND CodeJeu=:CodeDuJeu AND Jeux_IdJeux=IdJeux ORDER BY DateEmprunt DESC");
    RequeteHistoriqueEmprunt.bindValue(":CodeDuJeu",Code);
    RequeteHistoriqueEmprunt.exec();
    
    //On vide le model
    this->ModelHistoriqueEmprunts->clear();
    //Indique le nombres de colonnes puis leurs noms
    this->ModelHistoriqueEmprunts->setColumnCount(4);
    this->ModelHistoriqueEmprunts->setHorizontalHeaderItem(0, new QStandardItem("Nom"));
    this->ModelHistoriqueEmprunts->setHorizontalHeaderItem(1, new QStandardItem("Prénom"));
    this->ModelHistoriqueEmprunts->setHorizontalHeaderItem(2, new QStandardItem("Date d'emprunt"));
    this->ModelHistoriqueEmprunts->setHorizontalHeaderItem(3, new QStandardItem("Date de retour"));
    
    //Impose une taille aux colonnes
    ui->TbV_HistoriqueEmprunts->setColumnWidth(0,200);  // Nom
    ui->TbV_HistoriqueEmprunts->setColumnWidth(1,150);  // Prénom
    ui->TbV_HistoriqueEmprunts->setColumnWidth(2,100);  // Date d'emprunt
    ui->TbV_HistoriqueEmprunts->setColumnWidth(3,100);  // Date de retour
    while(RequeteHistoriqueEmprunt.next())
    {
        //On ajoute une nouvelle ligne du table view avec les données
        this->ModelHistoriqueEmprunts->setItem(NumeroLigne, 0, new QStandardItem(RequeteHistoriqueEmprunt.value(1).toString() ));
        this->ModelHistoriqueEmprunts->setItem(NumeroLigne, 1, new QStandardItem(RequeteHistoriqueEmprunt.value(0).toString() ));
        this->ModelHistoriqueEmprunts->setItem(NumeroLigne, 2, new QStandardItem(RequeteHistoriqueEmprunt.value(2).toDate().toString("dd-MM-yyyy") ));
        this->ModelHistoriqueEmprunts->setItem(NumeroLigne, 3, new QStandardItem(RequeteHistoriqueEmprunt.value(3).toDate().toString("dd-MM-yyyy") ));
        NumeroLigne ++ ;
    }
}

/**
 * @brief Quitte la fenêtre de déclaration d'intervention
 *
 */
void F_DetailsJeux::on_Bt_Quitter_clicked()
{
    this->hide();
}
