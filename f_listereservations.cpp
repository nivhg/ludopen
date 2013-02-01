//------------------------------------------------------------------------------
/** @file         f_listereservations.cpp
 *  @brief        Permet la recherche des réservations selon certains critères
 *
 *  @author       SOREL William
 *  @author       STS IRIS, Lyce Nicolas APPERT, ORVAULT (FRANCE)
 *  @since        09/01/2013
 *  @version      0.1
 *  @date         09/01/2013
 *
 *  Description détaillée du fichier f_listmembres.ccp
 *
 *  @bug          aucun connu
 */
//------------------------------------------------------------------------------

#include "f_listereservations.h"
#include "ui_f_listereservations.h"

#include <iostream>
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
    ui->TbW_ListeReservations->setModel(&ModeleReservations) ;
    ui->TbW_ListeReservations->setEditTriggers(QAbstractItemView::SelectedClicked);
    // Création des caractétiques du tableau : Nom des colonnes
    ModeleReservations.setHorizontalHeaderItem( 0, new QStandardItem( "" ) ) ;
    ModeleReservations.setHorizontalHeaderItem( 1, new QStandardItem( "Code jeu" ) ) ;
    ModeleReservations.setHorizontalHeaderItem( 2, new QStandardItem( "Nom du jeu" ) ) ;
    ModeleReservations.setHorizontalHeaderItem( 3, new QStandardItem( "Statut" ) ) ;
    ModeleReservations.setHorizontalHeaderItem( 4, new QStandardItem( "Nom" ) ) ;
    ModeleReservations.setHorizontalHeaderItem( 5, new QStandardItem( "Prénom" ) ) ;
    ModeleReservations.setHorizontalHeaderItem( 6, new QStandardItem( "Date réservation" ) ) ;
    ModeleReservations.setHorizontalHeaderItem( 7, new QStandardItem( "Emprunt prévu le" ) ) ;
    ModeleReservations.setHorizontalHeaderItem( 8, new QStandardItem( "Retour prévu le" ) ) ;
    ModeleReservations.setHorizontalHeaderItem( 9, new QStandardItem( "Lieu de réservation" ) ) ;

    // Règle la largeur des colonnes
    ui->TbW_ListeReservations->setColumnWidth( 0, 20 ) ;
    ui->TbW_ListeReservations->setColumnWidth( 1, 60 ) ;
    ui->TbW_ListeReservations->setColumnWidth( 2, 200 ) ;
    ui->TbW_ListeReservations->setColumnWidth( 3, 100 ) ;
    ui->TbW_ListeReservations->setColumnWidth( 4, 150 ) ;
    ui->TbW_ListeReservations->setColumnWidth( 5, 150 ) ;
    ui->TbW_ListeReservations->setColumnWidth( 6, 120 ) ;
    ui->TbW_ListeReservations->setColumnWidth( 7, 120 ) ;
    ui->TbW_ListeReservations->setColumnWidth( 8, 120 ) ;
    ui->TbW_ListeReservations->setColumnWidth( 9, 120 ) ;

    // Permettre le tri des colonnes
    ui->TbW_ListeReservations->setSortingEnabled(true);

    //Initialisation de la liste de titres et de types
    this->MaJTitre() ;
    this->MaJLieux() ;

    // Date d'inscription initialiser à la date du jour
    ui->DtE_DateResa_Debut->setDate(QDate::currentDate());
    ui->DtE_DateResa_Fin->setDate(QDate::currentDate());
}

F_ListeReservations::~F_ListeReservations()
{
    delete ui;
}

void F_ListeReservations::TousSelectionner( bool bSelection )
{
    ui->ChBx_Lieux->setChecked( bSelection ) ;
    ui->ChBx_Titre->setChecked( bSelection ) ;
    ui->ChBx_Nom->setChecked( bSelection ) ;
    ui->ChBx_Prenom->setChecked( bSelection ) ;
    ui->ChBx_NbreRetard->setChecked( bSelection ) ;
    ui->ChBx_DateReservation->setChecked( bSelection ) ;
    ui->ChBx_DateNaissance->setChecked( bSelection ) ;
    ui->ChBx_Cotisation->setChecked( bSelection ) ;
}

/** Description détaillée de la méthode
 *  @pre    Combobox remplie avec le vecteur
 *  @param  unsigned int nIdTitre
 *  @retval int
 *  @return L'emplacement du titre dans le vecteur par rapport à  l'id (0 par défaut)
 *  @test   Voir la procédure dans le fichier associé.
 */
int F_ListeReservations::EmplacementTitreVector( unsigned int nIdTitre )
{
    int nEmplacementTitre (0) ;
    int nBoucle           (0) ;

    //Recherche de l'id dans le vecteur tant que l'on a pas fini de parcour le vecteur
    while( nBoucle < this->VectorTitre.size() && nEmplacementTitre == 0 && nIdTitre != 0 )
    {
        if( this->VectorTitre[ nBoucle ].id == nIdTitre )
        {
            nEmplacementTitre = nBoucle ;
        }
        nBoucle ++ ;
    }
    return nEmplacementTitre ;
}

/** Description détaillée de la méthode
 *  @pre    Combobox remplie avec le vecteur
 *  @param  unsigned int nIdTytre
 *  @retval int
 *  @return L'emplacement du titre dans le vecteur par rapport à  l'id (0 par défaut)
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
    int i           (0)      ;
    QSqlQuery RequeteDesReservations          ;
    Type oLieux               ;

    //Suppression du contenu du vecteur de la combobox CBx_Lieu
    this->VectorLieux.clear() ;
    ui->CBx_Lieu->clear() ;

    //Exécution de la requête qui sélectionne le contenu de la table tytremembres
    if( RequeteDesReservations.exec( "SELECT * FROM lieux ;" ) )
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
            ui->CBx_Lieu->insertItem( i, VectorLieux[ i ].sType ) ;
        }
    }
    else //Sinon on affiche un message d'erreur et on retourne Faux
    {
        qDebug() << "F_Membres::MaJLieux () : Erreur de connexion avec la base de données !"  ;
    }
}

/** Description détaillée de la méthode
 *  @pre    Accés à  la base de données
 *  @test   Voir la procédure dans le fichier associé.
 */
void F_ListeReservations::MaJTitre ()
{
    int i           (0)      ;
    QSqlQuery RequeteDesReservations          ;
    Titre oTitre             ;

    //Suppression du contenu du vecteur de la combobox CBx_Titre
    this->VectorTitre.clear() ;
    ui->CBx_Titre->clear() ;

    //Exécution de la requête qui sélectionne le contenu de la table titremembre
    if( RequeteDesReservations.exec( "SELECT * FROM titremembre" ) )
    {
        oTitre.id = 0 ;
        oTitre.sTitre = "" ;
        this->VectorTitre.push_back( oTitre ) ;

        //Remplissage du vecteur avec ce que retourne la requête
        while( RequeteDesReservations.next() )
        {
            oTitre.id = RequeteDesReservations.value(0).toInt() ;
            oTitre.sTitre = RequeteDesReservations.value(1).toString() ;

            this->VectorTitre.push_back( oTitre ) ;
        }

        //Remplissage de la combobox grace au vecteur
        for( i = 0 ; i < VectorTitre.size() ; i ++ )
        {
            ui->CBx_Titre->insertItem( i, VectorTitre[i].sTitre ) ;
        }
    }
    else //Sinon on affiche un message d'erreur et on retourne Faux
    {
        qDebug() << "F_Membres::MaJTitre () : Erreur de connexion avec la base de données !"  ;
    }
}

bool F_ListeReservations::AffichageListe()
{
    QString sRequeteSELECTFROM ;
    QString sRequeteWHERE      ;
    QString sRequete           ;
    QString sNumero            ;
    int i       ( 0 ) ;
    int nCredit ( 0 ) ;
    QSqlQuery RequeteDesReservations ;
    QSqlQuery RequeteCotisation ;
    QSqlQuery RequeteCartes ;
    QSqlQuery RequeteTitre ;
    QSqlQuery RequeteJeu ;
    QDate Date ;
    QDate DateCotisation ;
    QStandardItem * item ;


    // Création de la requête pour filtrer les réservations
    sRequeteSELECTFROM = "SELECT * FROM reservation" ;

    if( ui->ChBx_DateReservation->isChecked() )
    {
        sRequeteWHERE = "WHERE" ;
        sRequeteWHERE = sRequeteWHERE + " DateReservation>='" + ui->DtE_DateResa_Debut->dateTime().toString("yyyy-MM-dd")  + "' AND" ;
        sRequeteWHERE = sRequeteWHERE + " DateReservation<='" + ui->DtE_DateResa_Fin->dateTime().toString("yyyy-MM-dd")  + "' AND";
    }
/*
    if ( ui->ChBx_Lieu->isChecked() )
    {
        sRequeteWHERE = sRequeteWHERE + " LieuxMembres_IdLieuxMembres=" +  sNumero.setNum(this->VectorLieux[ui->CBx_Lieu->currentIndex()].id) + " AND" ;
    }

    if ( ui->ChBx_Titre->isChecked() )
    {
        sRequeteWHERE = sRequeteWHERE + " TitreMembre_IdTitreMembre=" + sNumero.setNum(this->VectorTitre[ui->CBx_Titre->currentIndex()].id) + " AND" ;
    }

    if( ui->ChBx_Nom->isChecked() )
    {
        sRequeteWHERE =  sRequeteWHERE + " Nom LIKE '%" + ui->LE_Nom->text() + "%' AND" ;
    }

    if( ui->ChBx_Prenom->isChecked() )
    {
        sRequeteWHERE = sRequeteWHERE + " Prenom LIKE '%" + ui->LE_Prenom->text() + "%' AND" ;
    }

    if( ui->ChBx_NbreRetard->isChecked() )
    {
        sRequeteWHERE = sRequeteWHERE + " NbreRetard" + ui->CBx_Retards->currentText() + ui->SBx_NbreRetard->text() + " AND" ;
    }

    if( ui->ChBx_Retard->isChecked() )
    {
        sRequeteSELECTFROM = sRequeteSELECTFROM + ", emprunts" ;
        sRequeteWHERE = sRequeteWHERE + " emprunts.DateRetour IS NULL AND emprunts.DateRetourPrevu<" + sNumero.setNum( QDateTime::currentDateTime().toTime_t() ) + " AND IDMembre=emprunts.Membres_IdMembre AND" ;
    }

    if( ui->ChBx_DateNaissance->isChecked() )
    {
        sRequeteWHERE = sRequeteWHERE + " DateNaissance>='" + ui->DtE_DN_Debut->dateTime().toString("yyyy-MM-dd") + "' AND" ;
        sRequeteWHERE = sRequeteWHERE + " DateNaissance<='" + ui->DtE_DN_Fin->dateTime().toString("yyyy-MM-dd") + "' AND" ;
    }

    if( ui->ChBx_Cotisation->isChecked() )
    {
        sRequeteSELECTFROM += " ,abonnements" ;
        switch( ui->CBx_Cotisation->currentIndex() )
        {
        case 0 :
            sRequeteWHERE = sRequeteWHERE + " DateExpiration<='" + QDate::currentDate().toString( "yyyy-MM-dd" ) + "' AND IdMembre=Membres_IdMembre AND" ;
            break ;

        case 1 :
            Date = Date.currentDate() ;
            sRequeteWHERE = sRequeteWHERE + " DateExpiration<='" + Date.addDays( 14 ).toString( "yyyy-MM-dd" ) + "' AND DateExpiration>'" + QDate::currentDate().toString( "yyyy-MM-dd" )  + "' AND IdMembre=Membres_IdMembre AND" ;
            break ;

        case 2 :
            Date = Date.currentDate() ;
            sRequeteWHERE = sRequeteWHERE + " DateExpiration<='" + Date.addMonths( 1 ).toString( "yyyy-MM-dd" ) + "' AND DateExpiration>'" + QDate::currentDate().toString( "yyyy-MM-dd" ) + "' AND IdMembre=Membres_IdMembre AND" ;
            break ;
        }
    }
*/
    // Vire le dernier mot AND dans la requête WHERE
    sRequeteWHERE.remove(sRequete.size()-3, 5) ;

    // Assemblage des différents morceaux de la requête
    sRequete = sRequeteSELECTFROM + " " + sRequeteWHERE + " GROUP BY IdReservation" ;

    qDebug() << "F_ListeReservations::AfichageListe " << sRequete ;

    //Exécution de la requête
    if( RequeteDesReservations.exec(sRequete) )
    {        
        this->VecteurListeReservations.clear() ;
        // Vidage de la liste à l'écran
        this->ModeleReservations.removeRows(0,this->ModeleReservations.rowCount());

        //Remplissage du tableau avec les informations de la table membre
        while( RequeteDesReservations.next() )
        {
            this->VecteurListeReservations.append( RequeteDesReservations.record().value( 0 ).toInt() ) ;

            // On place des case à cocher dans la première colonne.
            item = new QStandardItem() ;
            item->setCheckable( true ) ;
            ModeleReservations.setItem( i, 0, item ) ;

            // Récupérer infos sur le jeu réservé
            RequeteJeu.prepare( "SELECT CodeJeu,NomJeu,StatutJeux_IdStatutJeux FROM jeux WHERE IdJeux=:IdJeu" ) ;
            RequeteJeu.bindValue( ":IdJeu", RequeteDesReservations.record().value( 3 ).toInt() ) ;
            if( RequeteJeu.exec() )
            {
                if( RequeteJeu.next() )
                {   // Code du jeu réservé
                    ModeleReservations.setItem( i, 1, new QStandardItem( RequeteJeu.record().value( 0 ).toString() ) ) ;
                    // Nom du jeu réservé
                    ModeleReservations.setItem( i, 2, new QStandardItem( RequeteJeu.record().value( 1 ).toString() ) ) ;
                    // Statut du jeu
                    ModeleReservations.setItem( i, 3, new QStandardItem( RequeteJeu.record().value( 2 ).toString() ) ) ;
                }
            }
            else
            {
                qDebug() << "F_ListeReservations::AfichageListe RequeteJeux=" << RequeteJeu.lastError().text()  ;
            }

            //Récupérer infos sur le membre
/*
            RequeteTitre.prepare( "SELECT NomTitre FROM membre "
                                 "WHERE IdTitreMembre=:IdTitre" ) ;
            RequeteTitre.bindValue( ":IdTitre", RequeteDesReservations.record().value( 1 ).toInt() ) ;
            if( RequeteTitre.exec() )
            {
                if( RequeteTitre.next() )
                {
                    ModeleReservations.setItem( i, 2, new QStandardItem( RequeteTitre.record().value( 0 ).toString() ) ) ;
                }
            }
            else
            {
                qDebug() << "F_ListeReservations:: : RequeteTitre :" << RequeteTitre.lastError().text()  ;
            }
*/
            // Nom du membre qui a fait la réservation
            // ModeleReservations.setItem( i, 4, new QStandardItem( RequeteDesReservations.record().value( 4 ).toString() ) ) ;
            // Prénom du membre qui a fait la réservation
            //ModeleReservations.setItem( i, 5, new QStandardItem( RequeteDesReservations.record().value( 5 ).toString() ) ) ;
            // Date réservation
            ModeleReservations.setItem( i, 6, new QStandardItem( RequeteDesReservations.record().value( 4 ).toString() ) ) ;
            // Date d'emprunt prévue
            ModeleReservations.setItem( i, 7, new QStandardItem( RequeteDesReservations.record().value( 5 ).toString() ) ) ;
            // Date de retour prévue
            ModeleReservations.setItem( i, 8, new QStandardItem( RequeteDesReservations.record().value( 6 ).toString() ) ) ;
            // Lieu de réservation
            ModeleReservations.setItem( i, 9, new QStandardItem( RequeteDesReservations.record().value( 1 ).toString() ) ) ;

            RequeteCotisation.prepare( "SELECT abonnements.DateExpiration FROM abonnements "
                                       "WHERE abonnements.Membres_IdMembre=:IdMembre" ) ;
            RequeteCotisation.bindValue( ":IdMembre", RequeteDesReservations.record().value( 0 ).toInt() ) ;

            if( RequeteCotisation.exec() )
            {
                if ( RequeteCotisation.next() )
                {
                    DateCotisation = RequeteCotisation.record().value( 0 ).toDate() ;
                    ModeleReservations.setItem( i, 11, new QStandardItem( DateCotisation.toString( "dd.MM.yy" ) ) ) ;
                    if ( DateCotisation < QDate::currentDate() )
                    {
                        ModeleReservations.setData( ModeleReservations.index( i, 11 ),QColor( Qt::red ), Qt::BackgroundColorRole ) ;
                    }
                    else
                    {
                        if( DateCotisation< QDate::currentDate().addDays( 14 ) )
                        {
                            ModeleReservations.setData( ModeleReservations.index( i, 11 ),QColor( Qt::yellow ), Qt::BackgroundColorRole ) ;
                        }
                        else
                        {
                            ModeleReservations.setData( ModeleReservations.index( i, 11 ),QColor( Qt::green ), Qt::BackgroundColorRole ) ;
                        }
                    }
                }

                while( RequeteCotisation.next() )
                {
                    if( DateCotisation > RequeteCotisation.record().value( 0 ).toDate() )
                    {
                        DateCotisation = RequeteCotisation.record().value( 0 ).toDate() ;
                        ModeleReservations.setItem( i, 11, new QStandardItem( DateCotisation.toString( "dd.MM.yy" ) ) ) ;
                        if ( DateCotisation < QDate::currentDate() )
                        {
                            ModeleReservations.setData( ModeleReservations.index( i, 11 ),QColor( Qt::red ), Qt::BackgroundColorRole ) ;
                        }
                        else
                        {
                            if( DateCotisation< QDate::currentDate().addDays( 14 ) )
                            {
                                ModeleReservations.setData( ModeleReservations.index( i, 11 ),QColor( Qt::yellow ), Qt::BackgroundColorRole ) ;
                            }
                            else
                            {
                                ModeleReservations.setData( ModeleReservations.index( i, 11 ),QColor( Qt::green ), Qt::BackgroundColorRole ) ;
                            }
                        }
                    }
                }
            }
            else
            {
                qDebug() << "F_ListeReservations:: : RequeteCotisation :" << RequeteCotisation.lastError().text()  ;
            }


            RequeteCartes.prepare( "SELECT abonnements.CreditRestant FROM abonnements "
                                   "WHERE abonnements.Membres_IdMembre=:IdMembre" ) ;
            RequeteCartes.bindValue( ":IdMembre", RequeteDesReservations.record().value( 0 ).toInt() ) ;

            if( RequeteCartes.exec() )
            {
                nCredit = 0 ;
                while( RequeteCartes.next() )
                {
                    nCredit += RequeteCartes.record().value( 0 ).toInt() ;
                }
                ModeleReservations.setItem( i, 12, new QStandardItem( sNumero.setNum( nCredit ) ) ) ;
            }
            else
            {
                qDebug() << "F_ListeReservations:: : RequeteCartes :" << RequeteCartes.lastError().text()  ;
            }

            i++ ;
        }        
    }
    else //Sinon on affiche un message d'erreur et on retourne Faux
    {
        qDebug() << "F_ListeReservations:: : RequeteDesReservations :" << RequeteDesReservations.lastError().text()  ;
    }

    ui->Lb_Resultat->setNum( i ) ;

    return true;
}

void F_ListeReservations::on_Bt_RAZ_clicked()
{
    ui->CBx_Lieu->clear() ;
    this->MaJLieux() ;

    ui->CBx_Titre->clear() ;
    this->MaJTitre() ;

    ui->LE_Nom->clear() ;
    ui->LE_Prenom->clear() ;
    ui->SBx_NbreRetard->clear() ;
    // Date d'inscription initialiser à la date du jour
    ui->DtE_DateResa_Debut->setDate(QDate::currentDate());
    ui->DtE_DateResa_Fin->setDate(QDate::currentDate());
    // Date de naissance
    ui->DtE_DN_Debut->clear() ;
    ui->DtE_DN_Fin->clear() ;

    this->TousSelectionner( false ) ;
}

void F_ListeReservations::on_Bt_Deselection_clicked()
{
    this->TousSelectionner( false ) ;
}

void F_ListeReservations::on_Bt_SupprimerListe_clicked()
{
    bool bRetourne ( true ) ;
    QSqlQuery RequeteSupprimer ;
    QSqlQuery RequeteEmprunts ;
    QSqlQuery RequeteMembre ;


    if ( QMessageBox::information( this, "Suppression Membre","Voulez vous vraiment supprimer tous les membres", "Supprimer", "Annuler" ) == 0 )
    {
        for (register int i (0) ; i < ui->TbW_ListeReservations->model()->columnCount() ; i++ )
        {
            //Si le checkbox est "check" on prépare le mail et on l'envoie
            if(ui->TbW_ListeReservations->model()->data( ui->TbW_ListeReservations->model()->index(i ,0), Qt::CheckStateRole).toBool() )
            {
                //Préparation de la requête la recherche des emprunts
                RequeteEmprunts.prepare( "SELECT IdEmprunts FROM emprunts WHERE Membres_IdMembre=:IdMembre AND DateRetour IS NULL  " ) ;
                RequeteEmprunts.bindValue( ":IdMembre", this->VecteurListeReservations.at( i ) ) ;

                if( !RequeteEmprunts.exec() )
                {
                    cerr << "F_AdministrerMembres::SupprimerMembre : RequeteEmprunts : Erreur de connexion avec la base de données !" << RequeteEmprunts.lastError().text().toStdString()<< endl ;
                    bRetourne = false ;
                }

                //S'il n'y a pas d'emprunt en cour
                if( RequeteEmprunts.size()== 0 )
                {
                    //Vérification que la personne veux bien supprimer le membre
                    //Préparation de la requête permettant la suppression dans la table de membre --------------------------
                    RequeteSupprimer.prepare( "DELETE FROM membres WHERE IdMembre=:IdMembre " ) ;
                    RequeteSupprimer.bindValue( ":IdMembre", this->VecteurListeReservations.at( i ) ) ;

                    //Execution de la requête, si elle fonctionne on met la variable de retoure à  vrai
                    if( RequeteSupprimer.exec() )
                    {
                        bRetourne = true ;
                    }
                    else//Sinon on affiche un message d'erreur et on met la variable de retoure à  faux
                    {
                        cerr << "F_AdministrerMembres::SupprimerMembre : RequeteSupprimer : Erreur de connexion avec la base de données !" << RequeteSupprimer.lastError().text().toStdString()<< endl ;
                        bRetourne = false ;
                    }

                    //Préparation de la requête permettant la suppression dans la table reservation -------------------------------
                    RequeteSupprimer.prepare( "DELETE FROM reservation WHERE Membres_IdMembre=:IdMembre " ) ;
                    RequeteSupprimer.bindValue( ":IdMembre", this->VecteurListeReservations.at( i ) ) ;

                    //Execution de la requête, si elle fonctionne on met la variable de retoure à  vrai
                    if( RequeteSupprimer.exec() )
                    {
                        bRetourne = true ;
                    }
                    else//Sinon on affiche un message d'erreur et on met la variable de retoure à  faux
                    {
                        cerr << "F_AdministrerMembres::SupprimerMembre : RequeteSupprimer : Erreur de connexion avec la base de données !" << RequeteSupprimer.lastError().text().toStdString()<< endl ;
                        bRetourne = false ;
                    }

                    //Préparation de la requête permettant la suppression dans la table emprunts ------------------------
                    RequeteSupprimer.prepare( "DELETE FROM emprunts WHERE Membres_IdMembre=:IdMembre " ) ;
                    RequeteSupprimer.bindValue( ":IdMembre", this->VecteurListeReservations.at( i ) ) ;

                    //Execution de la requête, si elle fonctionne on met la variable de retoure à  vrai
                    if( RequeteSupprimer.exec() )
                    {
                        bRetourne = true ;
                    }
                    else//Sinon on affiche un message d'erreur et on met la variable de retoure à  faux
                    {
                        cerr << "F_AdministrerMembres::SupprimerMembre : RequeteSupprimer : Erreur de connexion avec la base de données !" << RequeteSupprimer.lastError().text().toStdString()<< endl ;
                        bRetourne = false ;
                    }


                    //Préparation de la requête permettant la suppression dans la table abonnements-----------------------
                    RequeteSupprimer.prepare( "DELETE FROM abonnements WHERE Membres_IdMembre=:IdMembre " ) ;
                    RequeteSupprimer.bindValue( ":IdMembre", this->VecteurListeReservations.at( i ) ) ;

                    //Execution de la requête, si elle fonctionne on met la variable de retoure à  vrai
                    if( RequeteSupprimer.exec() )
                    {
                        bRetourne = true ;
                    }
                    else//Sinon on affiche un message d'erreur et on met la variable de retoure à  faux
                    {
                        cerr << "F_AdministrerMembres::SupprimerMembre : RequeteSupprimer : Erreur de connexion avec la base de données !" << RequeteSupprimer.lastError().text().toStdString()<< endl ;
                        bRetourne = false ;
                    }
                }
                else
                {
                    bRetourne = false ;
                    QMessageBox::information( this, "Suppression Membre","Impossible de supprimer " + ui->TbW_ListeReservations->model()->data( ui->TbW_ListeReservations->model()->index(i ,1) ).toString()  + " " + ui->TbW_ListeReservations->model()->data( ui->TbW_ListeReservations->model()->index(i ,2) ).toString() + " membre.\nIl a encore des jeux en cours d'emprunts.",  "Ok" ) ;
                }
            }
        }
    }
    else
    {
        bRetourne = false ;
    }

    this->AffichageListe() ;
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
    for(nNombreLigne = 0; nNombreLigne<ui->TbW_ListeReservations->model()->rowCount(); nNombreLigne++)
    {
        for(nNombreColonne = 1; nNombreColonne<ui->TbW_ListeReservations->model()->columnCount(); nNombreColonne++)
        {
            sCaractere = ui->TbW_ListeReservations->model()->data( ui->TbW_ListeReservations->model()->index( nNombreLigne, nNombreColonne ) ).toString() ;
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


void F_ListeReservations::on_CBx_Lieux_activated(int index)
{
    this->AffichageListe() ;
}

void F_ListeReservations::on_ChBx_Lieux_clicked()
{
    this->AffichageListe() ;
}

void F_ListeReservations::on_ChBx_Nom_clicked()
{
    this->AffichageListe() ;
}

void F_ListeReservations::on_ChBx_Prenom_clicked()
{
    this->AffichageListe() ;
}

void F_ListeReservations::on_ChBx_Titre_clicked()
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

void F_ListeReservations::on_ChBx_Retard_clicked()
{
    this->AffichageListe() ;
}

void F_ListeReservations::on_ChBx_Cotisation_clicked()
{
    this->AffichageListe() ;
}

void F_ListeReservations::on_ChBx_DateNaissance_clicked()
{
    this->AffichageListe() ;
}

void F_ListeReservations::on_LE_Nom_textEdited(const QString &arg1)
{
    this->AffichageListe() ;
}

void F_ListeReservations::on_LE_Prenom_textEdited(const QString &arg1)
{
    this->AffichageListe() ;
}

void F_ListeReservations::on_CBx_Titre_currentIndexChanged(int index)
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

void F_ListeReservations::on_DtE_DN_Debut_dateChanged(const QDate &date)
{
    this->AffichageListe() ;
}

void F_ListeReservations::on_DtE_DN_Fin_dateChanged(const QDate &date)
{
    this->AffichageListe() ;
}

void F_ListeReservations::on_CBx_Cotisation_currentIndexChanged(int index)
{
    this->AffichageListe() ;
}

void F_ListeReservations::on_TbW_ListeReservations_clicked(const QModelIndex &index)
{
}

void F_ListeReservations::on_TbW_ListeReservations_doubleClicked(const QModelIndex &index)
{
    emit( this->SignalSelectionMembre( this->VecteurListeReservations.at( index.row() ) ) ) ;
}
