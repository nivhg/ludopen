//---------------------------------------------------
/** @file f_declarerintervention.cpp
  * @brief Classe graphique permettant d'ajouter une intervention sur un jeu
  *
  * @author Florian MARY
  * @author STS IRIS, Lycée Nicolas APPERT, ORVAULT (FRANCE)
  * @since 01/01/2012
  * @version 0.1
  * @date 24/04/2012
  *
  * Cette classe permet d'ajouter une intervention sur le jeu en consultation
  *
  * Fabrication QtCreator, LudOpen
  *
  *
*/
//-------------------------------------------------------
// EN-TETE STANDARD ----------------------------------
#include <QtSql>

// En-tête propre à l'application ----------------------------------------------
#include "f_declarerintervention.h"
#include "ui_f_declarerintervention.h"



/**
 * @brief Constructeur de la classe f_declarerintervention
 *
 * @param parent
 */
F_DeclarerIntervention::F_DeclarerIntervention(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::F_DeclarerIntervention)
{
    ui->setupUi(this);
    QDate DateActuelle ;
    DateActuelle = DateActuelle.currentDate() ;
    ui->DE_DateIntervention->setDate(DateActuelle);
    this->setWindowTitle("Déclarer intervention");
}

/**
 * @brief Destructeur de la classe f_declarerintervention
 *
 */
F_DeclarerIntervention::~F_DeclarerIntervention()
{
    delete ui;
}
/**
 * @brief Méthode qui met dans l'attribut JeuEnConsultation le code du jeu en cour de consultation
 *
 * @param CodeJeu
 */
void F_DeclarerIntervention::set_JeuEnConsultation(QString CodeJeu)
{
    this->JeuEnConsultation = CodeJeu ;
}

/**
 * @brief Méthode qui ajoute l'intervention sur le jeu
 *
 */
void F_DeclarerIntervention::on_Bt_ValiderIntervention_clicked()
{
    QSqlQuery RequeteDeclarerIntervention ;
    QSqlQuery RequeteCorrespondanceId_Jeux ;
    
    
    RequeteCorrespondanceId_Jeux.prepare("SELECT IdJeux FROM jeux WHERE CodeJeu=:CodeDuJeu") ;
    RequeteCorrespondanceId_Jeux.bindValue(":CodeDuJeu", this->JeuEnConsultation);
    RequeteCorrespondanceId_Jeux.exec() ;
    RequeteCorrespondanceId_Jeux.next() ;
    
    unsigned int IdDuJeu = RequeteCorrespondanceId_Jeux.value(0).toInt() ;
    
    RequeteDeclarerIntervention.prepare("INSERT INTO intervientionsjeu(DateIntervention, TypeIntervention,"
                             "Jeux_IdJeux) VALUES (:DateIntervention, :TypeIntervention, :Jeux_IdJeux)" ) ;
    RequeteDeclarerIntervention.bindValue(":DateIntervention", this->ui->DE_DateIntervention->date());
    RequeteDeclarerIntervention.bindValue(":TypeIntervention", this->ui->TxE_Intervention->toPlainText());
    RequeteDeclarerIntervention.bindValue(":Jeux_IdJeux", IdDuJeu);
    
    RequeteDeclarerIntervention.exec() ;
    
    
    ui->TxE_Intervention->clear();
    this->hide();
}

/**
 * @brief Méthode qui annule l'ajout de l'intervention
 *
 */
void F_DeclarerIntervention::on_Bt_AnnulerIntervention_clicked()
{
     this->hide();
}
