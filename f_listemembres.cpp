//------------------------------------------------------------------------------
/** @file         f_listmembres.cpp
 *  @brief        Permet la recherche de membres selon certains critère
 *
 *  @author       PADIOU Nicolas
 *  @author       STS IRIS, Lyce Nicolas APPERT, ORVAULT (FRANCE)
 *  @since        01/01/2012
 *  @version      0.1
 *  @date         01/02/2013
 *
 *  Description dtaille du fichier f_listmembres.ccp
 *
 *  @bug          aucun connu
 */
//------------------------------------------------------------------------------

#include "f_listemembres.h"
#include "ui_f_listemembres.h"

#include <iostream>
#include <QStandardItem>
#include <QDialog>
#include <QFileDialog>
#include <QTableView>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>

using namespace std ;

F_ListeMembres::F_ListeMembres(bool bAdmin, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::F_ListeMembres)
{
    ui->setupUi(this);

    //Affiche les noms des colonnes dans le tableau
    ui->TbW_ListeMembre->horizontalHeader()->setVisible(true);
    ui->TbW_ListeMembre->setModel(&ModeleMembres) ;
    ui->TbW_ListeMembre->setEditTriggers(QAbstractItemView::SelectedClicked);
    // Permettre le tri des colonnes
    ui->TbW_ListeMembre->setSortingEnabled(true);
    //Mettre un nom pour toutes les colonnes
    ModeleMembres.setHorizontalHeaderItem( 0, new QStandardItem( "" ) ) ;
    ModeleMembres.setHorizontalHeaderItem( 1, new QStandardItem( "Type" ) ) ;
    ModeleMembres.setHorizontalHeaderItem( 2, new QStandardItem( "Titre" ) ) ;
    ModeleMembres.setHorizontalHeaderItem( 3, new QStandardItem( "Nom" ) ) ;
    ModeleMembres.setHorizontalHeaderItem( 4, new QStandardItem( "Prénom" ) ) ;
    ModeleMembres.setHorizontalHeaderItem( 5, new QStandardItem( "Ville" ) ) ;
    ModeleMembres.setHorizontalHeaderItem( 6, new QStandardItem( "Code" ) ) ;
    ModeleMembres.setHorizontalHeaderItem( 7, new QStandardItem( "Téléphone" ) ) ;
    ModeleMembres.setHorizontalHeaderItem( 8, new QStandardItem( "Mobile" ) ) ;
    ModeleMembres.setHorizontalHeaderItem( 9, new QStandardItem( "eMail" ) ) ;
    ModeleMembres.setHorizontalHeaderItem( 10, new QStandardItem( "Nbre de Retard" ) ) ;
    ModeleMembres.setHorizontalHeaderItem( 11, new QStandardItem( "Fin Cotisation" ) ) ;
    ModeleMembres.setHorizontalHeaderItem( 12, new QStandardItem( "Crédits restant" ) ) ;

    this->bAdmin = bAdmin ;
    if( this->bAdmin == true )
    {
        ui->Bt_DeseletionListe->setVisible( true ) ;
        ui->Bt_SelectionListe->setVisible( true ) ;
        ui->Bt_SupprimerListe->setVisible( true ) ;
    }
    else
    {
        ui->Bt_DeseletionListe->setHidden( true ) ;
        ui->Bt_SelectionListe->setHidden( true ) ;
        ui->Bt_SupprimerListe->setHidden( true ) ;
    }

    //Initialisation de la liste de titres et de types
    this->MaJTitre() ;
    this->MaJType() ;

    // Date d'inscription initialiser à la date du jour
    ui->DtE_DI_Debut->setDate(QDate::currentDate());
    ui->DtE_DI_Fin->setDate(QDate::currentDate());

    /////////////Fenetre selection abonnements///////////////
    this->pFenetreAbonnements = new QDialog(  ) ;
    this->pFenetreAbonnements->setModal( true ) ;
    this->pFenetreAbonnements->setMaximumSize( 300, 400 ) ;
    this->pFenetreAbonnements->setMinimumSize( 300, 400 ) ;
    this->pFenetreAbonnements->setHidden( true ) ;

    this->pLb_Abonnements = new QLabel ( this->pFenetreAbonnements ) ;
    this->pLb_Abonnements->setText( "Choisir les abonnements :") ;
    this->pLb_Abonnements->setGeometry( 75, 10, 200, 10 ) ;
    this->pLb_Abonnements->show() ;

    this->pListeAbonnements = new QTableView( this->pFenetreAbonnements ) ;
    this->pListeAbonnements->setGeometry( 0, 50, 300, 300);
    this->pListeAbonnements->show() ;

    this->pBt_FermerAbonnements = new QPushButton ( this->pFenetreAbonnements ) ;
    this->pBt_FermerAbonnements->setText( "Fermer" );
    this->pBt_FermerAbonnements->setGeometry( 100 , 375, 100, 25 ) ;
    this->pBt_FermerAbonnements->show() ;
    ui->ChBx_Abonnements->setHidden( true ) ;
}

F_ListeMembres::~F_ListeMembres()
{
    delete ui;
}

void F_ListeMembres::TousSelectionner( bool bSelection )
{
    ui->ChBx_Type->setChecked( bSelection ) ;
    ui->ChBx_Titre->setChecked( bSelection ) ;
    ui->ChBx_Nom->setChecked( bSelection ) ;
    ui->ChBx_Prenom->setChecked( bSelection ) ;
    ui->ChBx_Ville->setChecked( bSelection ) ;
    ui->ChBx_CodePostal->setChecked( bSelection ) ;
    ui->ChBx_NbreRetard->setChecked( bSelection ) ;
    ui->ChBx_AncienMembre->setChecked( bSelection ) ;
    ui->ChBx_DateInscription->setChecked( bSelection ) ;
    ui->ChBx_DateNaissance->setChecked( bSelection ) ;
    ui->ChBx_Cotisation->setChecked( bSelection ) ;
    ui->ChBx_Abonnements->setChecked( bSelection ) ;
}

/** Description dtaille de la mthode
 *  @pre    Combobox remplie avec le vecteur
 *  @param  unsigned int nIdTitre
 *  @retval int
 *  @return L'emplacement du titre dans le vecteur par rapport à  l'id (0 par défaut)
 *  @test   Voir la procédure dans le fichier associé.
 */
int F_ListeMembres::EmplacementTitreVector( unsigned int nIdTitre )
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

/** Description dtaille de la mthode
 *  @pre    Combobox remplie avec le vecteur
 *  @param  unsigned int nIdTytre
 *  @retval int
 *  @return L'emplacement du titre dans le vecteur par rapport à  l'id (0 par défaut)
 *  @test   Voir la procédure dans le fichier associé.
 */
int F_ListeMembres::EmplacementTypeVector(unsigned int nIdType )
{
    int nEmplacementType (0) ;
    int nBoucle          (0) ;

    //Recherche de l'id dans le vecteur tant que l'on a pas fini de parcour le vecteur
    while( nBoucle < this->VectorType.size() && nEmplacementType == 0 && nIdType != 0 )
    {
        if( this->VectorType[ nBoucle ].id == nIdType )
        {
            nEmplacementType = nBoucle ;
        }
        nBoucle ++ ;
    }

    return nEmplacementType ;
}

/** Description dtaille de la mthode
 *  @pre    Accés à  la base de données
 *  @test   Voir la procédure dans le fichier associé.
 */
void F_ListeMembres::MaJType ()
{
    int i           (0)      ;
    QSqlQuery query          ;
    Type oType               ;

    //Suppression du contenu du vecteur de la combobox CBx_Type
    this->VectorType.clear() ;
    ui->CBx_Type->clear() ;

    //Exécution de la requête qui sélectionne le contenu de la table tytremembres
    if( query.exec( "SELECT * FROM typemembres ;" ) )
    {
        oType.id = 0 ;
        oType.sType = "" ;
        this->VectorType.push_back( oType ) ;

        //Remplissage du vecteur avec ce que retourne la requête
        while( query.next() )
        {
            oType.id = query.value(0).toInt() ;
            oType.sType = query.value(1).toString() ;

            this->VectorType.push_back(oType) ;
        }

        //Remplissage de la combobox grace au vecteur
        for( i = 0 ; i < VectorType.size() ; i ++ )
        {
            ui->CBx_Type->insertItem( i, VectorType[ i ].sType ) ;
        }
    }
    else //Sinon on affiche un message d'erreur et on retourne Faux
    {
        qDebug() << "F_Membres::MaJType () : Erreur de connexion avec la base de donne !"  ;
    }
}

/** Description dtaille de la mthode
 *  @pre    Accés à  la base de données
 *  @test   Voir la procédure dans le fichier associé.
 */
void F_ListeMembres::MaJTitre ()
{
    int i           (0)      ;
    QSqlQuery query          ;
    Titre oTitre             ;

    //Suppression du contenu du vecteur de la combobox CBx_Titre
    this->VectorTitre.clear() ;
    ui->CBx_Titre->clear() ;

    //Exécution de la requête qui sélectionne le contenu de la table titremembre
    if( query.exec( "SELECT * FROM titremembre" ) )
    {
        oTitre.id = 0 ;
        oTitre.sTitre = "" ;
        this->VectorTitre.push_back( oTitre ) ;

        //Remplissage du vecteur avec ce que retourne la requête
        while( query.next() )
        {
            oTitre.id = query.value(0).toInt() ;
            oTitre.sTitre = query.value(1).toString() ;

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
        qDebug() << "F_Membres::MaJTitre () : Erreur de connexion avec la base de donne !"  ;
    }
}

bool F_ListeMembres::AffichageListe()
{
    QString sRequeteSELECTFROM ;
    QString sRequeteWHERE      ;
    QString sRequete           ;
    QString sNumero            ;
    int     i            ( 0 ) ;
    int nCredit ( 0 ) ;
    QSqlQuery query ;
    QSqlQuery RequeteCotisation ;
    QSqlQuery RequeteCartes ;
    QSqlQuery RequeteTitre ;
    QSqlQuery RequeteType ;
    QDate Date ;
    QDate DateCotisation ;
    QStandardItem * item ;


    sRequeteSELECTFROM = "SELECT membres.IdMembre, membres.TitreMembre_IdTitreMembre ,membres.TypeMembres_IdTypeMembres ,membres.Nom, membres.Prenom, membres.Ville, membres.CodeMembre, membres.Telephone, membres.Mobile, membres.Email, membres.NbreRetard, membres.DateInscription FROM membres" ;
    sRequeteWHERE = "WHERE" ;

    if ( ui->ChBx_Type->isChecked() )
    {
        sRequeteWHERE = sRequeteWHERE + " TypeMembres_IdTypeMembres=" +  sNumero.setNum(this->VectorType[ui->CBx_Type->currentIndex()].id) + " AND" ;
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

    if( ui->ChBx_Ville->isChecked() )
    {
        sRequeteWHERE = sRequeteWHERE + " Ville LIKE '%" + ui->LE_Ville->text() + "%' AND" ;
    }

    if( ui->ChBx_Retard->isChecked() )
    {
        sRequeteSELECTFROM = sRequeteSELECTFROM + ", emprunts" ;
        sRequeteWHERE = sRequeteWHERE + " emprunts.DateRetour IS NULL AND emprunts.DateRetourPrevu<" + sNumero.setNum( QDateTime::currentDateTime().toTime_t() ) + " AND IDMembre=emprunts.Membres_IdMembre AND" ;
    }

    if( ui->ChBx_CodePostal->isChecked() )
    {
        sRequeteWHERE = sRequeteWHERE + " CP LIKE '%" + ui->LE_CodePostal->text() + "%' AND" ;
    }

    if( ui->ChBx_AncienMembre->isChecked() )
    {
        sRequeteWHERE = sRequeteWHERE + " Ecarte=TRUE AND" ;
    }

    if( ui->ChBx_DateInscription->isChecked() )
    {
        sRequeteWHERE = sRequeteWHERE + " DateInscription>='" + ui->DtE_DI_Debut->dateTime().toString("yyyy-MM-dd")  + "' AND" ;
        sRequeteWHERE = sRequeteWHERE + " DateInscription<='" + ui->DtE_DI_Fin->dateTime().toString("yyyy-MM-dd")  + "' AND";
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

    // Vire le dernier mot AND dans la requête WHERE
    sRequeteWHERE.remove(sRequete.size()-3, 5) ;
    sRequete = sRequeteSELECTFROM + " " + sRequeteWHERE + " GROUP BY IdMembre ORDER BY IdMembre ASC" ;

    // qDebug() << sRequete ;

    //Exécution de la requête
    if( query.exec(sRequete) )
    {
        this->VecteurListeMembres.clear() ;
        // Vidage de la liste à l'écran
        this->ModeleMembres.removeRows(0,this->ModeleMembres.rowCount());

        //Remplissage du tableau avec les informations de la table membre
        while( query.next() )
        {
            if ( this->bAdmin == true )
            {
                item = new QStandardItem() ;
                item->setCheckable( true ) ;
                ModeleMembres.setItem( i, 0, item ) ;
            }

            this->VecteurListeMembres.append( query.record().value( 0 ).toInt() ) ;

            //Type//
            RequeteType.prepare( "SELECT TypeMembre FROM typemembres "
                                 "WHERE IdTypeMembres=:IdType" ) ;
            RequeteType.bindValue( ":IdType", query.record().value( 2 ).toInt() ) ;
            if( RequeteType.exec() )
            {
                if( RequeteType.next() )
                {
                    ModeleMembres.setItem( i, 1, new QStandardItem( RequeteType.record().value( 0 ).toString() ) ) ;
                }
            }
            else
            {
                qDebug() << "F_ListeMembres:: : RequeteType :" << RequeteType.lastError().text()  ;
            }

            //Titre//
            RequeteTitre.prepare( "SELECT NomTitre FROM titremembre "
                                 "WHERE IdTitreMembre=:IdTitre" ) ;
            RequeteTitre.bindValue( ":IdTitre", query.record().value( 1 ).toInt() ) ;
            if( RequeteTitre.exec() )
            {
                if( RequeteTitre.next() )
                {
                    ModeleMembres.setItem( i, 2, new QStandardItem( RequeteTitre.record().value( 0 ).toString() ) ) ;
                }
            }
            else
            {
                qDebug() << "F_ListeMembres:: : RequeteTitre :" << RequeteTitre.lastError().text()  ;
            }

            ModeleMembres.setItem( i, 3, new QStandardItem( query.record().value( 3 ).toString() ) ) ;
            ModeleMembres.setItem( i, 4, new QStandardItem( query.record().value( 4 ).toString() ) ) ;
            ModeleMembres.setItem( i, 5, new QStandardItem( query.record().value( 5 ).toString() ) ) ;
            ModeleMembres.setItem( i, 6, new QStandardItem( query.record().value( 6 ).toString() ) ) ;
            ModeleMembres.setItem( i, 7, new QStandardItem( this->ModifierSyntaxeNumTelephone( query.record().value( 7 ).toString() ) ) ) ;
            ModeleMembres.setItem( i, 8, new QStandardItem( this->ModifierSyntaxeNumTelephone( query.record().value( 8 ).toString() ) ) ) ;
            ModeleMembres.setItem( i, 9, new QStandardItem( query.record().value( 9 ).toString() ) ) ;
            ModeleMembres.setItem( i, 10, new QStandardItem( sNumero.setNum( query.record().value( 10 ).toInt() ) ) ) ;

            RequeteCotisation.prepare( "SELECT abonnements.DateExpiration FROM abonnements "
                                       "WHERE abonnements.Membres_IdMembre=:IdMembre" ) ;
            RequeteCotisation.bindValue( ":IdMembre", query.record().value( 0 ).toInt() ) ;

            if( RequeteCotisation.exec() )
            {
                if ( RequeteCotisation.next() )
                {
                    DateCotisation = RequeteCotisation.record().value( 0 ).toDate() ;
                    ModeleMembres.setItem( i, 11, new QStandardItem( DateCotisation.toString( "dd.MM.yy" ) ) ) ;
                    if ( DateCotisation < QDate::currentDate() )
                    {
                        ModeleMembres.setData( ModeleMembres.index( i, 11 ),QColor( Qt::red ), Qt::BackgroundColorRole ) ;
                    }
                    else
                    {
                        if( DateCotisation< QDate::currentDate().addDays( 14 ) )
                        {
                            ModeleMembres.setData( ModeleMembres.index( i, 11 ),QColor( Qt::yellow ), Qt::BackgroundColorRole ) ;
                        }
                        else
                        {
                            ModeleMembres.setData( ModeleMembres.index( i, 11 ),QColor( Qt::green ), Qt::BackgroundColorRole ) ;
                        }
                    }
                }

                while( RequeteCotisation.next() )
                {
                    if( DateCotisation > RequeteCotisation.record().value( 0 ).toDate() )
                    {
                        DateCotisation = RequeteCotisation.record().value( 0 ).toDate() ;
                        ModeleMembres.setItem( i, 11, new QStandardItem( DateCotisation.toString( "dd.MM.yy" ) ) ) ;
                        if ( DateCotisation < QDate::currentDate() )
                        {
                            ModeleMembres.setData( ModeleMembres.index( i, 11 ),QColor( Qt::red ), Qt::BackgroundColorRole ) ;
                        }
                        else
                        {
                            if( DateCotisation< QDate::currentDate().addDays( 14 ) )
                            {
                                ModeleMembres.setData( ModeleMembres.index( i, 11 ),QColor( Qt::yellow ), Qt::BackgroundColorRole ) ;
                            }
                            else
                            {
                                ModeleMembres.setData( ModeleMembres.index( i, 11 ),QColor( Qt::green ), Qt::BackgroundColorRole ) ;
                            }
                        }
                    }
                }
            }
            else
            {
                qDebug() << "F_ListeMembres:: : RequeteCotisation :" << RequeteCotisation.lastError().text()  ;
            }


            RequeteCartes.prepare( "SELECT abonnements.CreditRestant FROM abonnements "
                                   "WHERE abonnements.Membres_IdMembre=:IdMembre" ) ;
            RequeteCartes.bindValue( ":IdMembre", query.record().value( 0 ).toInt() ) ;

            if( RequeteCartes.exec() )
            {
                nCredit = 0 ;
                while( RequeteCartes.next() )
                {
                    nCredit += RequeteCartes.record().value( 0 ).toInt() ;
                }
                ModeleMembres.setItem( i, 12, new QStandardItem( sNumero.setNum( nCredit ) ) ) ;
            }
            else
            {
                qDebug() << "F_ListeMembres:: : RequeteCartes :" << RequeteCartes.lastError().text()  ;
            }

            i++ ;
        }
        if( this->bAdmin == true )
        {
            ui->TbW_ListeMembre->setColumnWidth( 0 ,30 ) ;
        }
        else
        {
            ui->TbW_ListeMembre->setColumnWidth( 0, 0 ) ;
        }
        ui->TbW_ListeMembre->setColumnWidth( 6, 70 ) ;
        ui->TbW_ListeMembre->setColumnWidth( 9, 200 ) ;
    }
    else //Sinon on affiche un message d'erreur et on retourne Faux
    {
        qDebug() << "F_ListeMembres:: : query :" << query.lastError().text()  ;
    }

    ui->Lb_Resultat->setNum( i ) ;

    return true;
}

/** Efface tous ce qui n'est pas un chiffre et ajoute un espace tous les 2 caractéres dans un string
 *  @param QString sNumero
 *  @retval QString
 *  @return Retourne un QString ne contenant plus de lettre ou caractêres speciaux. Un espace est ajouté tous les 2 caractéres
 *  @test   Voir la procédure dans le fichier associé.
 */
QString F_ListeMembres::ModifierSyntaxeNumTelephone ( QString sNumero )
{
    QChar cEspace ( ' ' ) ;
    QRegExp Nombres ( "[^0-9]" ) ;

    //Suppression des les caractéres ormis les chiffres
    sNumero.replace( Nombres, "" ) ;

    //Ajout d'un espace tous les 2 caractéres
    for( register int i = 2 ; i < sNumero.length() ; i= i+3 )
    {
        if( i != sNumero.length() )
        {
            sNumero.insert( i, cEspace ) ;
        }
    }
    return sNumero ;
}

void F_ListeMembres::on_Bt_RAZ_clicked()
{
    ui->CBx_Type->clear() ;
    this->MaJType() ;

    ui->CBx_Titre->clear() ;
    this->MaJTitre() ;

    ui->LE_Nom->clear() ;
    ui->LE_Prenom->clear() ;
    ui->LE_Ville->clear() ;
    ui->LE_CodePostal->clear() ;
    ui->SBx_NbreRetard->clear() ;
    // Date d'inscription initialiser à la date du jour
    ui->DtE_DI_Debut->setDate(QDate::currentDate());
    ui->DtE_DI_Fin->setDate(QDate::currentDate());
    // Date de naissance
    ui->DtE_DN_Debut->clear() ;
    ui->DtE_DN_Fin->clear() ;

    this->TousSelectionner( false ) ;
}

void F_ListeMembres::on_Bt_Deselection_clicked()
{
    this->TousSelectionner( false ) ;
}

void F_ListeMembres::on_Bt_SelectionListe_clicked()
{
    //Permet mettre toute les checkbox en état "Check"
    for (register int i (0) ; i < ui->TbW_ListeMembre->model()->rowCount() ; i++ )
    {
        ui->TbW_ListeMembre->model()->setData( ui->TbW_ListeMembre->model()->index(i ,0), Qt::Checked, Qt::CheckStateRole) ;
    }
}

void F_ListeMembres::on_Bt_DeseletionListe_clicked()
{
    //Parcour la liste pour "decheck" toute toutes les checkbox
    for (register int i (0) ; i < ui->TbW_ListeMembre->model()->rowCount() ; i++ )
    {
        ui->TbW_ListeMembre->model()->setData( ui->TbW_ListeMembre->model()->index(i ,0), Qt::Unchecked, Qt::CheckStateRole) ;
    }
}

void F_ListeMembres::on_Bt_SupprimerListe_clicked()
{
    bool bRetourne ( true ) ;
    QSqlQuery RequeteSupprimer ;
    QSqlQuery RequeteEmprunts ;
    QSqlQuery RequeteMembre ;


    if ( QMessageBox::information( this, "Suppression Membre","Voulez vous vraiment supprimer tous les membres", "Supprimer", "Annuler" ) == 0 )
    {
        for (register int i (0) ; i < ui->TbW_ListeMembre->model()->columnCount() ; i++ )
        {
            //Si le checkbox est "check" on prépare le mail et on l'envoie
            if(ui->TbW_ListeMembre->model()->data( ui->TbW_ListeMembre->model()->index(i ,0), Qt::CheckStateRole).toBool() )
            {
                //Préparation de la requête la recherche des emprunts
                RequeteEmprunts.prepare( "SELECT IdEmprunts FROM emprunts WHERE Membres_IdMembre=:IdMembre AND DateRetour IS NULL  " ) ;
                RequeteEmprunts.bindValue( ":IdMembre", this->VecteurListeMembres.at( i ) ) ;

                if( !RequeteEmprunts.exec() )
                {
                    cerr << "F_AdministrerMembres::SupprimerMembre : RequeteEmprunts : Erreur de connexion avec la base de donne !" << RequeteEmprunts.lastError().text().toStdString()<< endl ;
                    bRetourne = false ;
                }

                //S'il n'y a pas d'emprunt en cour
                if( RequeteEmprunts.size()== 0 )
                {
                    //Vérification que la personne veux bien supprimer le membre
                    //Préparation de la requête permettant la suppression dans la table de membre --------------------------
                    RequeteSupprimer.prepare( "DELETE FROM membres WHERE IdMembre=:IdMembre " ) ;
                    RequeteSupprimer.bindValue( ":IdMembre", this->VecteurListeMembres.at( i ) ) ;

                    //Execution de la requête, si elle fonctionne on met la variable de retoure à  vrai
                    if( RequeteSupprimer.exec() )
                    {
                        bRetourne = true ;
                    }
                    else//Sinon on affiche un message d'erreur et on met la variable de retoure à  faux
                    {
                        cerr << "F_AdministrerMembres::SupprimerMembre : RequeteSupprimer : Erreur de connexion avec la base de donne !" << RequeteSupprimer.lastError().text().toStdString()<< endl ;
                        bRetourne = false ;
                    }

                    //Préparation de la requête permettant la suppression dans la table reservation -------------------------------
                    RequeteSupprimer.prepare( "DELETE FROM reservation WHERE Membres_IdMembre=:IdMembre " ) ;
                    RequeteSupprimer.bindValue( ":IdMembre", this->VecteurListeMembres.at( i ) ) ;

                    //Execution de la requête, si elle fonctionne on met la variable de retoure à  vrai
                    if( RequeteSupprimer.exec() )
                    {
                        bRetourne = true ;
                    }
                    else//Sinon on affiche un message d'erreur et on met la variable de retoure à  faux
                    {
                        cerr << "F_AdministrerMembres::SupprimerMembre : RequeteSupprimer : Erreur de connexion avec la base de donne !" << RequeteSupprimer.lastError().text().toStdString()<< endl ;
                        bRetourne = false ;
                    }

                    //Préparation de la requête permettant la suppression dans la table emprunts ------------------------
                    RequeteSupprimer.prepare( "DELETE FROM emprunts WHERE Membres_IdMembre=:IdMembre " ) ;
                    RequeteSupprimer.bindValue( ":IdMembre", this->VecteurListeMembres.at( i ) ) ;

                    //Execution de la requête, si elle fonctionne on met la variable de retoure à  vrai
                    if( RequeteSupprimer.exec() )
                    {
                        bRetourne = true ;
                    }
                    else//Sinon on affiche un message d'erreur et on met la variable de retoure à  faux
                    {
                        cerr << "F_AdministrerMembres::SupprimerMembre : RequeteSupprimer : Erreur de connexion avec la base de donne !" << RequeteSupprimer.lastError().text().toStdString()<< endl ;
                        bRetourne = false ;
                    }


                    //Préparation de la requête permettant la suppression dans la table abonnements-----------------------
                    RequeteSupprimer.prepare( "DELETE FROM abonnements WHERE Membres_IdMembre=:IdMembre " ) ;
                    RequeteSupprimer.bindValue( ":IdMembre", this->VecteurListeMembres.at( i ) ) ;

                    //Execution de la requête, si elle fonctionne on met la variable de retoure à  vrai
                    if( RequeteSupprimer.exec() )
                    {
                        bRetourne = true ;
                    }
                    else//Sinon on affiche un message d'erreur et on met la variable de retoure à  faux
                    {
                        cerr << "F_AdministrerMembres::SupprimerMembre : RequeteSupprimer : Erreur de connexion avec la base de donne !" << RequeteSupprimer.lastError().text().toStdString()<< endl ;
                        bRetourne = false ;
                    }
                }
                else
                {
                    bRetourne = false ;
                    QMessageBox::information( this, "Suppression Membre","Impossible de supprimer " + ui->TbW_ListeMembre->model()->data( ui->TbW_ListeMembre->model()->index(i ,1) ).toString()  + " " + ui->TbW_ListeMembre->model()->data( ui->TbW_ListeMembre->model()->index(i ,2) ).toString() + " membre.\nIl a encore des jeux en cours d'emprunts.",  "Ok" ) ;
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

void F_ListeMembres::on_Bt_Exporter_clicked()
{
    int nNombreColonne (0);
    int nNombreLigne (0);

    QString sCaractere;
    QString nomFichier = QFileDialog::getSaveFileName(this, "Enregistrer sous ...", "Sans titre 1.csv");
    QFile fichier(nomFichier);
    QTextStream ecrire (&fichier);
    fichier.open(QIODevice::WriteOnly);

    ecrire << "Nom, Prenom, Ville, Code Membre, Telephone, Email, Nombre de retard \r\n\r\n" ;
    for(nNombreLigne = 0; nNombreLigne<ui->TbW_ListeMembre->model()->rowCount(); nNombreLigne++)
    {
        for(nNombreColonne = 1; nNombreColonne<ui->TbW_ListeMembre->model()->columnCount(); nNombreColonne++)
        {
            sCaractere = ui->TbW_ListeMembre->model()->data( ui->TbW_ListeMembre->model()->index( nNombreLigne, nNombreColonne ) ).toString() ;
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


void F_ListeMembres::on_CBx_Type_currentIndexChanged(int index)
{
    this->AffichageListe() ;
}

void F_ListeMembres::on_ChBx_Type_clicked()
{
    this->AffichageListe() ;
}

void F_ListeMembres::on_ChBx_Nom_clicked()
{
    this->AffichageListe() ;
}

void F_ListeMembres::on_ChBx_Prenom_clicked()
{
    this->AffichageListe() ;
}

void F_ListeMembres::on_ChBx_Ville_clicked()
{
    this->AffichageListe() ;
}

void F_ListeMembres::on_ChBx_CodePostal_clicked()
{
    this->AffichageListe() ;
}

void F_ListeMembres::on_ChBx_Titre_clicked()
{
    this->AffichageListe() ;
}

void F_ListeMembres::on_ChBx_AncienMembre_clicked()
{
    this->AffichageListe() ;
}

void F_ListeMembres::on_ChBx_Abonnements_clicked()
{
    this->AffichageListe() ;
}

void F_ListeMembres::on_ChBx_DateInscription_clicked()
{
    this->AffichageListe() ;
}

void F_ListeMembres::on_ChBx_NbreRetard_clicked()
{
    this->AffichageListe() ;
}

void F_ListeMembres::on_ChBx_Retard_clicked()
{
    this->AffichageListe() ;
}

void F_ListeMembres::on_ChBx_Cotisation_clicked()
{
    this->AffichageListe() ;
}

void F_ListeMembres::on_ChBx_DateNaissance_clicked()
{
    this->AffichageListe() ;
}

void F_ListeMembres::on_LE_Nom_textEdited(const QString &arg1)
{
    this->AffichageListe() ;
}

void F_ListeMembres::on_LE_Prenom_textEdited(const QString &arg1)
{
    this->AffichageListe() ;
}

void F_ListeMembres::on_LE_Ville_textEdited(const QString &arg1)
{
    this->AffichageListe() ;
}

void F_ListeMembres::on_LE_CodePostal_textEdited(const QString &arg1)
{
    this->AffichageListe() ;
}

void F_ListeMembres::on_CBx_Titre_currentIndexChanged(int index)
{
    this->AffichageListe() ;
}

void F_ListeMembres::on_CBx_Retards_currentIndexChanged(int index)
{
    this->AffichageListe() ;
}

void F_ListeMembres::on_SBx_NbreRetard_valueChanged(int arg1)
{
    this->AffichageListe() ;
}

void F_ListeMembres::on_DtE_DI_Debut_dateChanged(const QDate &date)
{
    this->AffichageListe() ;
}

void F_ListeMembres::on_DtE_DI_Fin_dateChanged(const QDate &date)
{
    this->AffichageListe() ;
}

void F_ListeMembres::on_DtE_DN_Debut_dateChanged(const QDate &date)
{
    this->AffichageListe() ;
}

void F_ListeMembres::on_DtE_DN_Fin_dateChanged(const QDate &date)
{
    this->AffichageListe() ;
}

void F_ListeMembres::on_CBx_Cotisation_currentIndexChanged(int index)
{
    this->AffichageListe() ;
}

void F_ListeMembres::on_TbW_ListeMembre_clicked(const QModelIndex &index)
{
}

void F_ListeMembres::on_TbW_ListeMembre_doubleClicked(const QModelIndex &index)
{
    emit( this->SignalSelectionMembre( this->VecteurListeMembres.at( index.row() ) ) ) ;
}
