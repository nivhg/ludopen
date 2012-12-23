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
}

/** Detruit l'objet cree
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
    {
        int                i ( 0 ) ;
        QSqlQuery          queryHistorique ;
        QStandardItemModel * modele = new QStandardItemModel() ;



        ui->TVw_Historique->setModel( modele ) ;
        ui->TVw_Historique->setEditTriggers( 0 ) ;


        //Preparation de la requête SQL : Selection des Jeux emprunter par un membre ainsi que leurs date d'emprunt, de retour et leur prolongation
        queryHistorique.prepare( "SELECT jeux.NomJeu, emprunts.DateEmprunt, emprunts.DateRetour, emprunts.NbrPrologation FROM emprunts, jeux "
                                 "WHERE emprunts.Membres_IdMembre=:IdMembre AND jeux.IdJeux=emprunts.Jeux_IdJeux AND emprunts.DateRetour IS NOT NULL" ) ;

        queryHistorique.bindValue( ":IdMembre", nIdMembre ) ;

        if( queryHistorique.exec() )
        {
            /*Création des caractéristiques du tableau : -Nombre de colonnes
                                                         -Nom des colonnes
                                                         -Nombre de lignes*/
            modele->setColumnCount( 4 ) ;
            modele->setRowCount( queryHistorique.size() ) ;
            modele->setHorizontalHeaderItem( 0, new QStandardItem( "Jeux" ) ) ;
            modele->setHorizontalHeaderItem( 1, new QStandardItem( "Emprunt" ) ) ;
            modele->setHorizontalHeaderItem( 2, new QStandardItem( "Date Retour" ) ) ;
            modele->setHorizontalHeaderItem( 3, new QStandardItem( "Prologation" ) ) ;

           //Remplissage du tableau avec les informations retournées
            while( queryHistorique.next() )
            {
                 modele->setItem( i, 0, new QStandardItem( queryHistorique.record().value( 0 ).toString() ) ) ;
                 modele->setItem( i, 1, new QStandardItem( queryHistorique.record().value( 1 ).toDateTime().toString( "dd.MM.yy" ) ) ) ;
                 modele->setItem( i, 2, new QStandardItem( queryHistorique.record().value( 2 ).toDateTime().toString( "dd.MM.yy" ) ) ) ;
                 modele->setItem( i, 3, new QStandardItem( queryHistorique.record().value( 4 ).toInt() ) ) ;
                i++ ;
            }

        }
        else //Sinon on affiche un message d'erreur et on retourne Faux
        {
            cerr << " ID membre " << nIdMembre << "->" << "F_HistoriqueJeux::AfficherHistorique : queryHistorique : Erreur de connexion avec la base de donnée ! Erreur -> " << queryHistorique.lastError().databaseText().toStdString() << endl ;
        }
    }
}

/** Permet de quitter la fenêtre
 *  @test   Voir la procédure dans le fichier associé.
 */
void F_HistoriqueJeux::on_PB_Quitter_clicked()
{
    this->setVisible( false ) ;
}
