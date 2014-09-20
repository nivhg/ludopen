//------------------------------------------------------------------------------
/** @file         f_historiquejeux.cpp
 *  @brief        Permet de consulter les jeux empruntés et rendus
 *
 *  @author       PADIOU Nicolas
 *  @author       STS IRIS, Lyce Nicolas APPERT, ORVAULT (FRANCE)
 *  @since        01/01/2012
 *  @version      0.1
 *  @date         29/08/2012
 *
 *  @bug          aucun
 */
//------------------------------------------------------------------------------
// En-tête propre  l'objet ----------------------------------------------------
#include "f_historiquejeux.h"
#include "ui_f_historiquejeux.h"

using namespace std ;

/** Initialise l'interface
  */
F_HistoriqueJeux::F_HistoriqueJeux(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::F_HistoriqueJeux)
{
    ui->setupUi(this);

    ui->TVw_Historique->setModel( &ModeleHistoriqueJeuxEmprunter ) ;
    /*Création des caractéristiques du tableau :  -Nombre de colonnes
                                                  -Nom des colonnes
                                                  -Nombre de colonnes*/

    ui->TVw_Historique->setEditTriggers( 0 ) ;
    // Permettre le tri des colonnes
    ui->TVw_Historique->setSortingEnabled(true);
        ModeleHistoriqueJeuxEmprunter.setColumnCount( 4 ) ;
    ModeleHistoriqueJeuxEmprunter.setHorizontalHeaderItem( 0, new QStandardItem( "Nom du jeu" ) ) ;
    ModeleHistoriqueJeuxEmprunter.setHorizontalHeaderItem( 1, new QStandardItem( "Date emprunt" ) ) ;
    ModeleHistoriqueJeuxEmprunter.setHorizontalHeaderItem( 2, new QStandardItem( "Date retour" ) ) ;
    ModeleHistoriqueJeuxEmprunter.setHorizontalHeaderItem( 3, new QStandardItem( "Nb prologation" ) ) ;
    // Taille des colonnes
    ui->TVw_Historique->setColumnWidth(0, 250 );  // colonne Nom du jeu
    ui->TVw_Historique->setColumnWidth(1, 100 );  // colonne Date emprunt
    ui->TVw_Historique->setColumnWidth(2, 100 );  // colonne Date retour
    ui->TVw_Historique->setColumnWidth(3, 100 );  // colonne Nb prologation
}

/** Détruit l'objet crée
 */
F_HistoriqueJeux::~F_HistoriqueJeux()
{
    delete ui;
}

/** Affiche les jeux deja empruntés
 *  @param  unsigned int nIdMembre
 */
void F_HistoriqueJeux::AfficherHistorique( unsigned int nIdMembre )
{
    int                i ( 0 ) ;
    QSqlQuery          queryHistorique ;

    //Preparation de la requête SQL : Selection des Jeux emprunter par un membre ainsi que leurs date d'emprunt, de retour et leur prolongation
    queryHistorique.prepare( "SELECT jeux.NomJeu, emprunts.DateEmprunt, emprunts.DateRetour, emprunts.NbrPrologation FROM emprunts, jeux "
                             "WHERE emprunts.Membres_IdMembre=:IdMembre AND jeux.IdJeux=emprunts.Jeux_IdJeux AND emprunts.DateRetour IS NOT NULL" ) ;

    queryHistorique.bindValue( ":IdMembre", nIdMembre ) ;

    if( queryHistorique.exec() )
    {
        //Remplissage du tableau avec les informations retournées
        while( queryHistorique.next() )
        {
            // Fixe le nombre de lignes du tableau
            ModeleHistoriqueJeuxEmprunter.setRowCount( queryHistorique.size() ) ;
            ModeleHistoriqueJeuxEmprunter.setItem( i, 0, new QStandardItem( queryHistorique.record().value( 0 ).toString() ) ) ;
            ModeleHistoriqueJeuxEmprunter.setItem( i, 1, new QStandardItem( queryHistorique.record().value( 1 ).toDateTime().toString( "dd-MM-yyyy" ) ) ) ;
            ModeleHistoriqueJeuxEmprunter.setItem( i, 2, new QStandardItem( queryHistorique.record().value( 2 ).toDateTime().toString( "dd-MM-yyyy" ) ) ) ;
            ModeleHistoriqueJeuxEmprunter.setItem( i, 3, new QStandardItem( queryHistorique.record().value( 4 ).toInt() ) ) ;
            i++ ;
        }
    }
    else //Sinon on affiche un message d'erreur et on retourne Faux
    {
        qDebug()<< " ID membre " << nIdMembre << "->" << "F_HistoriqueJeux::AfficherHistorique : queryHistorique : Erreur de connexion avec la base de donnée ! Erreur -> " << queryHistorique.lastError().databaseText() << endl ;
    }
}

/** Permet de quitter la fenêtre
 *  @test   Voir la procédure dans le fichier associé.
 */
void F_HistoriqueJeux::on_PB_Quitter_clicked()
{
    this->setVisible( false ) ;
}
