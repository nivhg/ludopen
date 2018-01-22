//------------------------------------------------------------------------------
/** @file         f_listereservations.cpp
 *  @brief        Permet la recherche des réservations selon certains critères
 *
 *  @author       SOREL William
 *  @author       STS IRIS, Lyce Nicolas APPERT, ORVAULT (FRANCE)
 *  @since        09/01/2013
 *  @version      0.1
 *  @date         01/02/2013 William
 *
 *  Description détaillée du fichier f_listereservations.ccp
 *
 *  @bug          aucun connu
 */
//------------------------------------------------------------------------------

#include "f_listereservations.h"
#include "ui_f_listereservations.h"
#include "fonctions_globale.h"

#include <QtDebug>
#include <QStandardItem>
#include <QDialog>
#include <QFileDialog>
#include <QTableView>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>

using namespace std ;

F_ListeReservations::F_ListeReservations(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::F_ListeReservations)
{
    ui->setupUi(this);

    // Création du tableau pour afficher les réservations
    ui->Tv_ListeReservations->setModel(&ModeleReservations) ;
    ui->Tv_ListeReservations->setEditTriggers(QAbstractItemView::SelectedClicked);
    //Affiche les noms des colonnes dans le tableau
    //ui->Tv_ListeReservations->horizontalHeader()->setVisible(true);
    // Création des caractétiques du tableau : Nom des colonnes
    ModeleReservations.setHorizontalHeaderItem( 0, new QStandardItem( "" ) ) ;  // case à cocher pour la suppression
    ModeleReservations.setHorizontalHeaderItem( 1, new QStandardItem( "Code jeu" ) ) ;
    ModeleReservations.setHorizontalHeaderItem( 2, new QStandardItem( "Nom du jeu" ) ) ;
    ModeleReservations.setHorizontalHeaderItem( 3, new QStandardItem( "Statut" ) ) ;
    ModeleReservations.setHorizontalHeaderItem( 4, new QStandardItem( "Nom" ) ) ;
    ModeleReservations.setHorizontalHeaderItem( 5, new QStandardItem( "Prénom" ) ) ;
    ModeleReservations.setHorizontalHeaderItem( 6, new QStandardItem( "Date réservation" ) ) ;
    ModeleReservations.setHorizontalHeaderItem( 7, new QStandardItem( "Emprunt prévu le" ) ) ;
    ModeleReservations.setHorizontalHeaderItem( 8, new QStandardItem( "Retour prévu le" ) ) ;
    ModeleReservations.setHorizontalHeaderItem( 9, new QStandardItem( "Lieu de réservation" ) ) ;
    ModeleReservations.setHorizontalHeaderItem( 10, new QStandardItem( "Lieu de retrait" ) ) ;
    ModeleReservations.setHorizontalHeaderItem( 11, new QStandardItem( "Confirmé?" ) ) ;
    ModeleReservations.setHorizontalHeaderItem( 12, new QStandardItem( "Type de malle" ) ) ;

    // Règle la largeur des colonnes
    /*ui->Tv_ListeReservations->setColumnWidth( 0, 20 ) ;  // case à cocher pour la suppression
    ui->Tv_ListeReservations->setColumnWidth( 1, 60 ) ;
    ui->Tv_ListeReservations->setColumnWidth( 2, 200 ) ;
    ui->Tv_ListeReservations->setColumnWidth( 3, 100 ) ;
    ui->Tv_ListeReservations->setColumnWidth( 4, 150 ) ;
    ui->Tv_ListeReservations->setColumnWidth( 5, 150 ) ;
    ui->Tv_ListeReservations->setColumnWidth( 6, 120 ) ;
    ui->Tv_ListeReservations->setColumnWidth( 7, 120 ) ;
    ui->Tv_ListeReservations->setColumnWidth( 8, 120 ) ;
    ui->Tv_ListeReservations->setColumnWidth( 9, 120 ) ;
    ui->Tv_ListeReservations->setColumnWidth( 11, 120 ) ;
    ui->Tv_ListeReservations->setColumnWidth( 12, 60 ) ;*/
    //ui->Tv_ListeReservations->resizeColumnsToContents();

    // Permettre le tri des colonnes
    ui->Tv_ListeReservations->setSortingEnabled(true);

    //Initialisation de la liste de lieux de réservation et de retrait
    //this->MiseAJourStatutJeu() ;
    this->MaJLieux() ;

    // Date d'inscription initialiser à la date du jour
    ui->DtE_DateResa_Debut->setDate(QDate::currentDate());
    ui->DtE_DateResa_Fin->setDate(QDate::currentDate());

    // Date d'emprunt initialiser à la date du jour
    ui->DtE_DateEmpruntPrevue_Debut->setDate(QDate::currentDate());
    ui->DtE_DateEmpruntPrevue_Fin->setDate(QDate::currentDate());

    // Cache les widgets qui ne servent pas pour l'instant repris de F_ListeMembre
    ui->CBx_StatutJeu->setVisible(false) ;
    ui->ChBx_StatutJeu->setVisible(false) ;
    ui->Lb_DateInscription->setVisible(false);
    ui->Lb_DateNaissance->setVisible(false) ;
    ui->Bt_Exporter->setVisible(false);
}

F_ListeReservations::~F_ListeReservations()
{
    delete ui;
}

void F_ListeReservations::TousSelectionner( bool bSelection )
{
    ui->ChBx_LieuxDeReservation->setChecked( bSelection ) ;
    ui->ChBx_StatutJeu->setChecked( bSelection ) ;
    ui->ChBx_LieuxDeRetrait->setChecked( bSelection ) ;
    ui->ChBx_DateReservation->setChecked( bSelection ) ;
    ui->ChBx_DateEmpruntPrevue->setChecked( bSelection ) ;
    ui->ChBx_NonConfirme->setChecked( bSelection ) ;
}

/** Description détaillée de la méthode
 *  @pre    Combobox remplie avec le vecteur
 *  @param  unsigned int nIdStatutDuJeu
 *  @retval int
 *  @return L'emplacement du StatutDuJeu dans le vecteur par rapport à  l'id (0 par défaut)
 *  @test   Voir la procédure dans le fichier associé.
 */
int F_ListeReservations::EmplacementStatutDuJeuVector( unsigned int nIdStatutDuJeu )
{
    int nEmplacementStatutDuJeu (0) ;
    int nBoucle           (0) ;

    //Recherche de l'id dans le vecteur tant que l'on a pas fini de parcourir le vecteur
    while( nBoucle < this->VectorStatutDuJeu.size() && nEmplacementStatutDuJeu == 0 && nIdStatutDuJeu != 0 )
    {
        if( this->VectorStatutDuJeu[ nBoucle ].id == nIdStatutDuJeu )
        {
            nEmplacementStatutDuJeu = nBoucle ;
        }
        nBoucle ++ ;
    }
    return nEmplacementStatutDuJeu ;
}

/** Description détaillée de la méthode
 *  @pre    Combobox remplie avec le vecteur
 *  @param  unsigned int nIdTytre
 *  @retval int
 *  @return L'emplacement du StatutDuJeu dans le vecteur par rapport à  l'id (0 par défaut)
 *  @test   Voir la procédure dans le fichier associé.
 */
int F_ListeReservations::EmplacementLieuxVector(unsigned int nIdLieux )
{
    int nEmplacementLieux (0) ;
    int nBoucle          (0) ;

    //Recherche de l'id dans le vecteur tant que l'on a pas fini de parcour le vecteur
    while( nBoucle < this->VectorLieux.size() && nEmplacementLieux == 0 && nIdLieux != 0 )
    {
        if( this->VectorLieux[ nBoucle ].id == nIdLieux )
        {
            nEmplacementLieux = nBoucle ;
        }
        nBoucle ++ ;
    }

    return nEmplacementLieux ;
}

/** Description détaillée de la méthode
 *  @pre    Accés à  la base de données
 *  @test   Voir la procédure dans le fichier associé.
 */
void F_ListeReservations::MaJLieux ()
{
    int i (0) ;
    QSqlQuery RequeteDesReservations ;
    Type oLieux ;

    //Suppression du contenu du vecteur de la combobox CBx_Lieu
    this->VectorLieux.clear() ;
    ui->CBx_LieuxDeReservation->clear() ;
    ui->CBx_LieuxDeRetrait->clear() ;

    //Exécution de la requête qui sélectionne le contenu de la table titremembres
    if( RequeteDesReservations.exec( "SELECT * FROM lieux" ) )
    {
        oLieux.id = 0 ;
        oLieux.sType = "" ;
        this->VectorLieux.push_back( oLieux ) ;

        //Remplissage du vecteur avec ce que retourne la requête
        while( RequeteDesReservations.next() )
        {
            oLieux.id = RequeteDesReservations.value(0).toInt() ;
            oLieux.sType = RequeteDesReservations.value(1).toString() ;

            this->VectorLieux.push_back(oLieux) ;
        }

        //Remplissage de la combobox grace au vecteur
        for( i = 0 ; i < VectorLieux.size() ; i ++ )
        {
            ui->CBx_LieuxDeReservation->insertItem( i, VectorLieux[ i ].sType ) ;
            ui->CBx_LieuxDeRetrait->insertItem( i, VectorLieux[ i ].sType ) ;
        }
    }
    else //Sinon on affiche un message d'erreur et on retourne Faux
    {
        qDebug() << "F_Membres::MaJLieux () : RequeteDesReservations="<<RequeteDesReservations.lastQuery()  ;
    }
}

/** Description détaillée de la méthode
 *  @pre    Accés à  la base de données
 *  @test   Voir la procédure dans le fichier associé.
 */
void F_ListeReservations::MiseAJourStatutJeu()
{
    int i           (0)      ;
    QSqlQuery RequeteDesReservations          ;
    StatutDuJeu oStatutDuJeu             ;

    //Suppression du contenu du vecteur de la combobox CBx_StatutJeu
    this->VectorStatutDuJeu.clear() ;
    ui->CBx_StatutJeu->clear() ;

    //Exécution de la requête qui sélectionne le contenu de la table statutjeux
    if( RequeteDesReservations.exec( "SELECT * FROM statutjeux" ) )
    {
        oStatutDuJeu.id = 0 ;
        oStatutDuJeu.sStatutDuJeu = "" ;
        this->VectorStatutDuJeu.push_back( oStatutDuJeu ) ;

        //Remplissage du vecteur avec ce que retourne la requête
        while( RequeteDesReservations.next() )
        {
            oStatutDuJeu.id = RequeteDesReservations.value(0).toInt() ;
            oStatutDuJeu.sStatutDuJeu = RequeteDesReservations.value(1).toString() ;
            this->VectorStatutDuJeu.push_back( oStatutDuJeu ) ;
        }

        //Remplissage de la combobox grace au vecteur
        for( i = 0 ; i < VectorStatutDuJeu.size() ; i ++ )
        {
            ui->CBx_StatutJeu->insertItem( i, VectorStatutDuJeu[i].sStatutDuJeu ) ;
        }
    }
    else //Sinon on affiche un message d'erreur et on retourne Faux
    {
        qDebug() << "F_ListeReservations::MiseAJourStatutJeu => " << RequeteDesReservations.lastQuery() ;
    }
}

bool F_ListeReservations::AffichageListe()
{
    QString sRequeteSELECTFROM ;
    QString sRequeteWHERE      ;
    QString sRequete           ;
    QString sNumero            ;
    int i       ( 0 ) ;
    QSqlQuery RequeteDesReservations ;
    QSqlQuery RequeteStatutDuJeu ;
    QSqlQuery RequeteJeu ;
    QDate Date ;
    QDate DateCotisation ;
    QStandardItem * item ;

    // Création de la requête pour filtrer les réservations
    sRequeteSELECTFROM = "SELECT CodeJeu,NomJeu,StatutJeu,idReservation, r.Membres_IdMembre,Jeux_IdJeux, "
            "r.DateReservation, Nom, Prenom, r.DatePrevuEmprunt, r.DatePrevuRetour, JeuEmprunte, JeuMisDeCote, "
            "ConfirmationReservation, L1.NomLieux as NomReservation, L2.NomLieux as NomRetrait, "
            "IFNULL(TypeEmprunt,'Aucune') as TypeEmprunt, IdMalle "
            "FROM membres,statutjeux,jeux,reservation as r "
            "LEFT JOIN lieux as L1 ON L1.IdLieux=Lieux_IdLieuxReservation "
            "LEFT JOIN malles as m ON m.IdMalle=r.Malles_IdMalle "
            "LEFT JOIN typeemprunt as t ON t.IdTypeEmprunt=m.TypeEmprunt_IdTypeEmprunt LEFT JOIN lieux as L2 ON "
            "L2.IdLieux=Lieux_IdLieuxRetrait ";
    sRequeteWHERE = "WHERE IdJeux=Jeux_IdJeux AND IdStatutJeux=StatutJeux_IdStatutJeux AND "
            "IdMembre=r.Membres_IdMembre AND " ;

    if( ui->ChBx_DateReservation->isChecked() )
    {       
        sRequeteWHERE += " DateReservation>='" + ui->DtE_DateResa_Debut->dateTime().toString("yyyy-MM-dd") + "' AND " ;
        sRequeteWHERE += " DateReservation<='" + ui->DtE_DateResa_Fin->dateTime().toString("yyyy-MM-dd") + "' AND ";
    }

    if ( ui->ChBx_LieuxDeReservation->isChecked() )
    {
        sRequeteWHERE += " Lieux_IdLieuxReservation=" +  sNumero.setNum(this->VectorLieux[ui->CBx_LieuxDeReservation->currentIndex()].id) + " AND " ;
    }

    if ( ui->ChBx_LieuxDeRetrait->isChecked() )
    {
        sRequeteWHERE += " Lieux_IdLieuxRetrait=" +  QString::number(this->VectorLieux[ui->CBx_LieuxDeRetrait->currentIndex()].id) + " AND " ;
    }

    if ( ui->ChBx_StatutJeu->isChecked() )
    {
        sRequeteWHERE = sRequeteWHERE + " StatutDuJeuMembre_IdStatutDuJeuMembre=" + sNumero.setNum(this->VectorStatutDuJeu[ui->CBx_StatutJeu->currentIndex()].id) + " AND " ;
    }

    if( ui->ChBx_DateEmpruntPrevue->isChecked() )
    {
        sRequeteWHERE = sRequeteWHERE + " DatePrevuEmprunt>='" + ui->DtE_DateEmpruntPrevue_Debut->dateTime().toString("yyyy-MM-dd") + "' AND" ;
        sRequeteWHERE = sRequeteWHERE + " DatePrevuRetour<='" + ui->DtE_DateEmpruntPrevue_Fin->dateTime().toString("yyyy-MM-dd") + "' AND " ;
    }

    sRequeteWHERE = sRequeteWHERE + " ConfirmationReservation="+QString::number(!ui->ChBx_NonConfirme->isChecked())+" AND ";

    // Vire le dernier mot AND dans la requête WHERE ou le WHERE si requête sans WHERE nécessaire
    sRequeteWHERE.remove( sRequeteWHERE.size()-5 , 5) ;

    // Assemblage des différents morceaux de la requête
    sRequete = sRequeteSELECTFROM + " " + sRequeteWHERE + " GROUP BY IdReservation" ;

    qDebug() << "F_ListeReservations::AffichageListe " << sRequete ;

    ui->Le_SQL->setText(sRequete);

    //Exécution de la requête
    if( RequeteDesReservations.exec(sRequete) )
    {        
        // Vider le vecteur qui contient les ID de toutes les réservations (sert pour la suppression des réservations)
        this->VecteurListeReservations.clear() ;
        // Vidage de la liste à l'écran
        this->ModeleReservations.removeRows(0,this->ModeleReservations.rowCount());
        //Remplissage du tableau avec les informations issues des tables jeux et membres
        while( RequeteDesReservations.next() )
        {
            // Ajouter au vecteur l'ID de cette réservation (sert pour la suppression des réservations)
            this->VecteurListeReservations.append( ObtenirValeurParNom(RequeteDesReservations,"idReservation").toInt() ) ;

            // On place des case à cocher dans la première colonne.
            item = new QStandardItem() ;
            item->setCheckable( true ) ;
            ModeleReservations.setItem( i, 0, item ) ;

            // Code du jeu réservé
            ModeleReservations.setItem( i, 1, new QStandardItem(
                 ObtenirValeurParNom(RequeteDesReservations,"CodeJeu").toString() ) ) ;
            // Nom du jeu réservé
            ModeleReservations.setItem( i, 2, new QStandardItem(
                 ObtenirValeurParNom(RequeteDesReservations,"NomJeu").toString() ) ) ;
            ModeleReservations.setItem( i, 3, new QStandardItem(
                 ObtenirValeurParNom(RequeteDesReservations,"StatutJeu").toString() ) ) ;
            // Nom de l'adhérent qui a réservé
            ModeleReservations.setItem( i,4, new QStandardItem(
                 ObtenirValeurParNom(RequeteDesReservations,"Nom").toString() ) ) ;
            // Prénom de l'adhérent qui a réservé
            ModeleReservations.setItem( i,5, new QStandardItem(
                 ObtenirValeurParNom(RequeteDesReservations,"Prenom").toString() ) ) ;
            // Date réservation
            ModeleReservations.setItem( i, 6, new QStandardItem(
                 ObtenirValeurParNom(RequeteDesReservations,"DateReservation").toDate().toString("dd-MM-yyyy" ) ) ) ;
            // Date d'emprunt prévue
            ModeleReservations.setItem( i, 7, new QStandardItem(
                 ObtenirValeurParNom(RequeteDesReservations,"DatePrevuEmprunt").toDate().toString("dd-MM-yyyy" ) ) ) ;
            // Date de retour prévue
            ModeleReservations.setItem( i, 8, new QStandardItem(
                 ObtenirValeurParNom(RequeteDesReservations,"DatePrevuRetour").toDate().toString( "dd-MM-yyyy" ) ) ) ;
            // Lieu de réservation
            ModeleReservations.setItem( i, 9, new QStandardItem(
                 ObtenirValeurParNom(RequeteDesReservations,"NomReservation").toString() ) ) ;
            ModeleReservations.setItem( i, 10, new QStandardItem(
                 ObtenirValeurParNom(RequeteDesReservations,"NomRetrait").toString() ) ) ;
            ModeleReservations.setItem( i, 11, new QStandardItem(
                 (ObtenirValeurParNom(RequeteDesReservations,"ConfirmationReservation").toString()=="1")?"OUI":"NON") );
            ModeleReservations.setItem( i, 12, new QStandardItem(
                 ObtenirValeurParNom(RequeteDesReservations,"TypeEmprunt").toString() ) );
            i++ ;
        }
    }
    else //Sinon on affiche un message d'erreur et on retourne Faux
    {
        qDebug() << "F_ListeReservations:: : RequeteDesReservations :" << RequeteDesReservations.lastQuery()  ;
    }

    ui->Lb_Resultat->setNum( i ) ;

    return true;
}

void F_ListeReservations::on_Bt_RAZ_clicked()
{
    ui-> CBx_LieuxDeReservation->clear() ;
    ui->CBx_LieuxDeRetrait->clear() ;

    this->MaJLieux() ;

    ui->CBx_StatutJeu->clear() ;
    this->MiseAJourStatutJeu() ;

    // Date d'inscription initialiser à la date du jour
    ui->DtE_DateResa_Debut->setDate(QDate::currentDate());
    ui->DtE_DateResa_Fin->setDate(QDate::currentDate());
    // Date d'emprunt
    ui->DtE_DateEmpruntPrevue_Debut->setDate(QDate::currentDate());
    ui->DtE_DateEmpruntPrevue_Fin->setDate(QDate::currentDate());

    this->TousSelectionner( false ) ;
    // Remettre à jour la liste des réservations affichées
    this->AffichageListe();
}

void F_ListeReservations::on_Bt_Deselection_clicked()
{
    this->TousSelectionner( false ) ;
    this->AffichageListe() ;
}

void F_ListeReservations::on_Bt_SupprimerListe_clicked()
{
    QSqlQuery RequeteSupprimer ;

    //Vérification que la personne veut bien supprimer les réservations choisies
    if ( QMessageBox::information( this, "Suppression de réservations","Voulez vous vraiment supprimer toutes les réservations sélectionnées ?", "Supprimer", "Garder" ) == 0 )
    {
        for (register int i (0) ; i < ui->Tv_ListeReservations->model()->columnCount() ; i++ )
        {
            //Si le checkbox est "check" dans la liste des résa, on vire cette résa
            if(ui->Tv_ListeReservations->model()->data( ui->Tv_ListeReservations->model()->index(i ,0), Qt::CheckStateRole).toBool() )
            {
                //Préparation de la requête permettant la suppression dans la table reservation
                RequeteSupprimer.prepare( "DELETE FROM reservation WHERE idReservation=:IdReservation " ) ;
                RequeteSupprimer.bindValue( ":IdReservation", this->VecteurListeReservations.at( i ) ) ;
                if( ! RequeteSupprimer.exec() )
                {
                    qDebug() << "F_ListeReservations::on_Bt_SupprimerListe_clicked => RequeteSupprimer" << RequeteSupprimer.lastQuery()<< endl ;
                }
            }
        }
        // Suppression des réservations effectuée. Mettre à jour la liste affichée
        this->AffichageListe();
    }
}

void F_ListeReservations::on_Bt_Exporter_clicked()
{
    int nNombreColonne (0);
    int nNombreLigne (0);

    QString sCaractere;
    QString nomFichier = QFileDialog::getSaveFileName(this, "Enregistrer sous ...", "Sans titre 1.csv");
    QFile fichier(nomFichier);
    QTextStream ecrire (&fichier);
    fichier.open(QIODevice::WriteOnly);

    ecrire << "Nom, Prenom, Ville, Code Membre, Telephone, Email, Nombre de retard \r\n\r\n" ;
    for(nNombreLigne = 0; nNombreLigne<ui->Tv_ListeReservations->model()->rowCount(); nNombreLigne++)
    {
        for(nNombreColonne = 1; nNombreColonne<ui->Tv_ListeReservations->model()->columnCount(); nNombreColonne++)
        {
            sCaractere = ui->Tv_ListeReservations->model()->data( ui->Tv_ListeReservations->model()->index( nNombreLigne, nNombreColonne ) ).toString() ;
            // On rejete les valeurs à  caractère unique et on le remplace par un champs vide
            sCaractere.replace(" ", "\ ") ;
            if(sCaractere == "-" || sCaractere == "_" || sCaractere == ".")
            {
                sCaractere = "";
            }
            ecrire << "\"" << sCaractere << "\",";
        }
        ecrire << "\r\n";
        nNombreColonne = 0;
    }
}


void F_ListeReservations::on_CBx_LieuxDeReservation_activated(int index)
{
    this->AffichageListe() ;
}

void F_ListeReservations::on_ChBx_LieuxDeReservation_clicked()
{
    this->AffichageListe() ;
}

void F_ListeReservations::on_ChBx_LieuxDeRetrait_clicked()
{
    this->AffichageListe() ;
}

void F_ListeReservations::on_ChBx_StatutJeu_clicked()
{
    this->AffichageListe() ;
}

void F_ListeReservations::on_ChBx_DateReservation_clicked()
{
    this->AffichageListe() ;
}

void F_ListeReservations::on_ChBx_NbreRetard_clicked()
{
    this->AffichageListe() ;
}

/* A virer car pas de Chexbox de ce nom
void F_ListeReservations::on_ChBx_Retard_clicked()
{
    this->AffichageListe() ;
}
*/

void F_ListeReservations::on_ChBx_Cotisation_clicked()
{
    this->AffichageListe() ;
}

void F_ListeReservations::on_ChBx_DateEmpruntPrevue_clicked()
{
    this->AffichageListe() ;
}

void F_ListeReservations::on_CBx_StatutJeu_currentIndexChanged(int index)
{
    this->AffichageListe() ;
}

void F_ListeReservations::on_CBx_Retards_currentIndexChanged(int index)
{
    this->AffichageListe() ;
}

void F_ListeReservations::on_SBx_NbreRetard_valueChanged(int arg1)
{
    this->AffichageListe() ;
}

void F_ListeReservations::on_DtE_DateResa_Debut_dateChanged(const QDate &date)
{
    this->AffichageListe() ;
}

void F_ListeReservations::on_DtE_DateResa_Fin_dateChanged(const QDate &date)
{
    this->AffichageListe() ;
}

void F_ListeReservations::on_DtE_DateEmpruntPrevue_Debut_dateChanged(const QDate &date)
{
    this->AffichageListe() ;
}

void F_ListeReservations::on_DtE_DateEmpruntPrevue_Fin_dateChanged(const QDate &date)
{
    this->AffichageListe() ;
}

void F_ListeReservations::on_CBx_Cotisation_currentIndexChanged(int index)
{
    this->AffichageListe() ;
}

void F_ListeReservations::on_Tv_ListeReservations_clicked(const QModelIndex &index)
{
}

void F_ListeReservations::on_Tv_ListeReservations_doubleClicked(const QModelIndex &index)
{
    emit( this->Signal_DoubleClic_ListeMembres( this->VecteurListeReservations.at( index.row() ) ) ) ;
}

void F_ListeReservations::SelectionnerReservation(int IdReservation)
{
    int index=this->VecteurListeReservations.indexOf(IdReservation);
    //ui->Tv_ListeReservations->selectRow(index);
}

void F_ListeReservations::on_CBx_LieuxDeRetrait_activated(int index)
{
    this->AffichageListe();
}

void F_ListeReservations::on_ChBx_NonConfirme_clicked()
{
    this->AffichageListe();
}
