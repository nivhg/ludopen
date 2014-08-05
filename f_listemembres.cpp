//------------------------------------------------------------------------------
/** @file         f_listmembres.cpp
 *  @brief        Permet la recherche de membres selon certains critère
 *
 *  @author       PADIOU Nicolas
 *  @author       STS IRIS, Lyce Nicolas APPERT, ORVAULT (FRANCE)
 *  @since        01/01/2012
 *  @version      0.1
 *  @date         02/02/2013 Nicolas
 *
 *  Description détaillée du fichier f_listmembres.ccp
 *
 *  @bug          aucun connu
 */
//------------------------------------------------------------------------------

#include "f_listemembres.h"
#include "f_membres.h"
#include "ui_f_listemembres.h"

#include <QtDebug>
#include <QStandardItem>
#include <QtWidgets>

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
    ModeleMembres.setHorizontalHeaderItem( 1, new QStandardItem( "" ) ) ;
    ModeleMembres.setHorizontalHeaderItem( 2, new QStandardItem( "Titre" ) ) ;
    ModeleMembres.setHorizontalHeaderItem( 3, new QStandardItem( "Type" ) ) ;
    ModeleMembres.setHorizontalHeaderItem( 4, new QStandardItem( "Nom" ) ) ;
    ModeleMembres.setHorizontalHeaderItem( 5, new QStandardItem( "Prénom" ) ) ;
    ModeleMembres.setHorizontalHeaderItem( 6, new QStandardItem( "Ville" ) ) ;
    ModeleMembres.setHorizontalHeaderItem( 7, new QStandardItem( "Code" ) ) ;
    ModeleMembres.setHorizontalHeaderItem( 8, new QStandardItem( "Téléphone" ) ) ;
    ModeleMembres.setHorizontalHeaderItem( 9, new QStandardItem( "Mobile" ) ) ;
    ModeleMembres.setHorizontalHeaderItem( 10, new QStandardItem( "eMail" ) ) ;
    ModeleMembres.setHorizontalHeaderItem( 11, new QStandardItem( "Retards" ) ) ;
    ModeleMembres.setHorizontalHeaderItem( 12, new QStandardItem( "Fin Cotisation" ) ) ;
    ModeleMembres.setHorizontalHeaderItem( 13, new QStandardItem( "Crédits" ) ) ;
    ModeleMembres.setHorizontalHeaderItem( 14, new QStandardItem( "Inscription" ) ) ;

    // Régler la taille des colonnes
    ui->TbW_ListeMembre->setColumnWidth(  0,   0 ) ; // pour l'ID du membre
    ui->TbW_ListeMembre->setColumnWidth(  1,   0 ) ; // coche pour la suppression
    ui->TbW_ListeMembre->setColumnWidth(  2, 100 ) ; // Type
    ui->TbW_ListeMembre->setColumnWidth(  3,  70 ) ; // Titre
    ui->TbW_ListeMembre->setColumnWidth(  4, 160 ) ; // Nom
    ui->TbW_ListeMembre->setColumnWidth(  5, 150 ) ; // Prénom
    ui->TbW_ListeMembre->setColumnWidth(  6, 150 ) ; // Ville
    ui->TbW_ListeMembre->setColumnWidth(  7,  50 ) ; // Code
    ui->TbW_ListeMembre->setColumnWidth(  8,  90 ) ; // Téléphone
    ui->TbW_ListeMembre->setColumnWidth(  9,  90 ) ; // Mobile
    ui->TbW_ListeMembre->setColumnWidth( 10, 200 ) ; // eMail
    ui->TbW_ListeMembre->setColumnWidth( 11,  50 ) ; // Nbre de Retard
    ui->TbW_ListeMembre->setColumnWidth( 12, 100 ) ; // Fin Cotisation
    // si je veux la bonne date d'expiration d'une cotisation, ne pas prendre en compte les dates des cartes pré-payées
    // On affiche pas les crédits restant car la requête qui calcule la SUM(CreditRestant) prend aussi la date d'expiration
    // des cartes pré-payées.
    // TO DO  Séparé la table des cotisations et celles des cartes prépayées !
    ui->TbW_ListeMembre->setColumnWidth( 13,  0 ) ; // Crédits restant
    ui->TbW_ListeMembre->setColumnWidth( 14, 100 ) ; // Inscription

    this->bAdmin = bAdmin ;
    if( this->bAdmin == true )
    {
        ui->Bt_DeselectionListe->setVisible( true ) ;
        ui->Bt_SelectionListe->setVisible( true ) ;
        ui->Bt_SupprimerListe->setVisible( true ) ;
    }
    else
    {
        ui->Bt_DeselectionListe->setHidden( true ) ;
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

    QStandardItemModel *Model = new QStandardItemModel(2, 1);

    // Liste des champs de la combobox d'export
    QString* sChamps = new QString[13] { "Titre", "Type", "Nom", "Prénom","Ville",
            "Code Membre","Téléphone","Mobile","Courriel","Retards","Fin cotisation",
            "Crédits","Inscription"};
    // Ajout des checkboxes dans la combobox d'export
    for (int r = 0; r < 13; ++r)
        {
            QStandardItem* item;
            item = new QStandardItem(sChamps[r]);

            item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
            item->setData(Qt::Unchecked, Qt::CheckStateRole);
            item->setCheckState(Qt::Checked);

            Model->setItem(r, 0, item);
            this->Items.push_back(item);
    }

    ui->CBx_Exporter->setModel(Model);
    // Nécessaire pour l'affichage des combobox sous Linux (Bug ?)
    ui->CBx_Exporter->setStyle(new QCommonStyle);
    ui->CBx_Exporter->setStyleSheet("border-style: outset;");
    ui->CBx_Exporter->setCurrentText("Champs à exporter");
    //qDebug() << ui->ChBx_Abonnements->;
    F_Membres::ChargerActivites(ui->CBx_Activites);
}

F_ListeMembres::~F_ListeMembres()
{
    delete ui;
}

void F_ListeMembres::DecocherTout( bool bSelection )
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
    this->AffichageListe();
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
    if( query.exec( "SELECT * FROM typemembres" ) )
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

/** Affiche le résultat du filtrage
 *  @pre    Accés à  la base de données
 *  @test   Voir la procédure dans le fichier associé.
 */
bool F_ListeMembres::AffichageListe()
{
    QString sRequeteSELECTFROM ;
    QString sRequeteWHERE      ;
    QString sRequete           ;
    QString sNumero            ;
    int     i            ( 0 ) ;
    QSqlQuery RequeteListemembres ;
    QDate Date ;
    QDate DateCotisation ;
    QStandardItem * item ;

    sRequeteSELECTFROM = "SELECT IdMembre,NomTitre,TypeMembre,Nom,Prenom,Ville,CodeMembre,Telephone,Mobile,"
            "Email,NbreRetard,DateInscription,DateExpiration,SUM(CreditRestant) as CreditRestant FROM membres,typemembres,"
            "titremembre,abonnements,activitemembre " ;
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
        sRequeteSELECTFROM = sRequeteSELECTFROM + ",emprunts " ;
        //sRequeteWHERE = sRequeteWHERE + " emprunts.DateRetour IS NULL AND emprunts.DateRetourPrevu<" + sNumero.setNum( QDateTime::currentDateTime().toTime_t() ) + " AND IDMembre=emprunts.Membres_IdMembre AND" ;
        sRequeteWHERE = sRequeteWHERE + " emprunts.DateRetour IS NULL AND IDMembre=emprunts.Membres_IdMembre AND" ;
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
        sRequeteWHERE = sRequeteWHERE + " DateInscription<='" + ui->DtE_DI_Fin->dateTime().toString("yyyy-MM-dd")    + "' AND" ;
    }

    if( ui->ChBx_DateNaissance->isChecked() )
    {
        sRequeteWHERE = sRequeteWHERE + " DateNaissance>='" + ui->DtE_DN_Debut->dateTime().toString("yyyy-MM-dd") + "' AND" ;
        sRequeteWHERE = sRequeteWHERE + " DateNaissance<='" + ui->DtE_DN_Fin->dateTime().toString("yyyy-MM-dd") + "' AND" ;
    }

    if( ui->ChBx_Activite->isChecked() )
    {
        int index = ui->CBx_Activites->currentIndex();
        QString sIdActivite = ui->CBx_Activites->itemData(index,Qt::UserRole).toString();
        sRequeteWHERE = sRequeteWHERE + " Activite_IdActivite = " + sIdActivite
                + " AND IdMembre = activitemembre.Membres_IdMembre AND";
    }
    if( ui->ChBx_Cotisation->isChecked() )
    {
        switch( ui->CBx_Cotisation->currentIndex() )
        {
        case 0 : // cotisation expirée
            sRequeteWHERE = sRequeteWHERE + " DateExpiration<='" + QDate::currentDate().toString( "yyyy-MM-dd" )
                                          + "' AND IdMembre=Membres_IdMembre AND" ;
            break ;

        case 1 : // cotisation expirant dans 2 semaines (14 jours)
            Date = Date.currentDate() ;
            sRequeteWHERE = sRequeteWHERE + " DateExpiration<='" + Date.addDays( 14 ).toString( "yyyy-MM-dd" )
                                          + "' AND DateExpiration>'" + QDate::currentDate().toString( "yyyy-MM-dd" )
                                          + "' AND IdMembre=Membres_IdMembre AND" ;
            break ;

        case 2 : // cotisation expirant dans 1 mois
            Date = Date.currentDate() ;
            sRequeteWHERE = sRequeteWHERE + " DateExpiration<='" + Date.addMonths( 1 ).toString( "yyyy-MM-dd" )
                                          + "' AND DateExpiration>'" + QDate::currentDate().toString( "yyyy-MM-dd" )
                                          + "' AND IdMembre=Membres_IdMembre AND" ;
            break ;
        case 3 :  // cotisation à jour
            Date = Date.currentDate() ;
            sRequeteWHERE = sRequeteWHERE + " DateExpiration<='" + Date.addYears( 1 ).toString( "yyyy-MM-dd" )
                                          + "' AND DateExpiration>'" + QDate::currentDate().toString( "yyyy-MM-dd" )
                                          + "' AND IdMembre=Membres_IdMembre AND" ;
            break ;
        }
    }

    sRequeteWHERE += " CartesPrepayees_IdCarte IS NULL AND IdTypeMembres=TypeMembres_IdTypeMembres AND IdTitreMembre=TitreMembre_IdTitreMembre AND IdMembre=abonnements.Membres_IdMembre " ;  //IdMembre=abonnements.Membres_IdMembre
    sRequete = sRequeteSELECTFROM + sRequeteWHERE + " GROUP BY IdMembre " ;

    qDebug() << "F_listemembre::AffichageListe() :" << sRequete ;
    ui->LE_sql->setText(sRequete);

    //Exécution de la requête
    if( RequeteListemembres.exec(sRequete) )
    {
        // Vidage de la liste à l'écran
        this->ModeleMembres.removeRows(0,this->ModeleMembres.rowCount());

        //Remplissage du tableau avec les informations de la table membre
        while( RequeteListemembres.next() )
        {

            // mettre l'ID du membre dans le tableau pour le retrouver + facilement
            ModeleMembres.setItem( i, 0, new QStandardItem(
                RequeteListemembres.record().value( RequeteListemembres.record().indexOf("IdMembre") ).toString() ) ) ;

            if ( this->bAdmin == true )
            {
                item = new QStandardItem() ;
                item->setCheckable( true ) ;
                ModeleMembres.setItem( i, 1, item ) ;
            }

            ModeleMembres.setItem( i, 2, new QStandardItem( RequeteListemembres.record().value( RequeteListemembres.record().indexOf("NomTitre") ).toString() ) ) ;
            ModeleMembres.setItem( i, 3, new QStandardItem( RequeteListemembres.record().value( RequeteListemembres.record().indexOf("TypeMembre") ).toString() ) ) ;
            ModeleMembres.setItem( i, 4, new QStandardItem( RequeteListemembres.record().value( RequeteListemembres.record().indexOf("Nom") ).toString() ) ) ;
            ModeleMembres.setItem( i, 5, new QStandardItem( RequeteListemembres.record().value( RequeteListemembres.record().indexOf("Prenom") ).toString() ) ) ;
            ModeleMembres.setItem( i, 6, new QStandardItem( RequeteListemembres.record().value( RequeteListemembres.record().indexOf("Ville") ).toString() ) ) ;
            ModeleMembres.setItem( i, 7, new QStandardItem( RequeteListemembres.record().value( RequeteListemembres.record().indexOf("CodeMembre") ).toString() ) ) ;
            ModeleMembres.setItem( i, 8, new QStandardItem( this->ModifierSyntaxeNumTelephone( RequeteListemembres.record().value( RequeteListemembres.record().indexOf("Telephone") ).toString() ) ) ) ;
            ModeleMembres.setItem( i, 9, new QStandardItem( this->ModifierSyntaxeNumTelephone( RequeteListemembres.record().value( RequeteListemembres.record().indexOf("Mobile") ).toString() ) ) ) ;
            ModeleMembres.setItem( i, 10, new QStandardItem( RequeteListemembres.record().value( RequeteListemembres.record().indexOf("Email") ).toString() ) ) ;
            ModeleMembres.setItem( i, 11, new QStandardItem( sNumero.setNum( RequeteListemembres.record().value( RequeteListemembres.record().indexOf("NbreRetard") ).toInt() ) ) ) ;

            DateCotisation = RequeteListemembres.record().value( RequeteListemembres.record().indexOf("DateExpiration") ).toDate() ;
            ModeleMembres.setItem( i, 12, new QStandardItem( DateCotisation.toString( "yyyy-MM-dd" ) ) ) ;
            if ( DateCotisation < QDate::currentDate() )
            {
                ModeleMembres.setData( ModeleMembres.index( i, 12 ),QColor( Qt::red ), Qt::BackgroundColorRole ) ;
            }
            else
            {
                if( DateCotisation < QDate::currentDate().addDays( 14 ) )
                {
                    ModeleMembres.setData( ModeleMembres.index( i, 12 ),QColor( Qt::yellow ), Qt::BackgroundColorRole ) ;
                }
                else
                {
                    ModeleMembres.setData( ModeleMembres.index( i, 12 ),QColor( Qt::green ), Qt::BackgroundColorRole ) ;
                }
            }

            if( DateCotisation > RequeteListemembres.record().value( RequeteListemembres.record().indexOf("DateExpiration") ).toDate() )
            {
                DateCotisation = RequeteListemembres.record().value( RequeteListemembres.record().indexOf("DateExpiration") ).toDate() ;
                ModeleMembres.setItem( i, 12, new QStandardItem( DateCotisation.toString( "yyyy-MM-dd" ) ) ) ;
                if ( DateCotisation < QDate::currentDate() )
                {
                    ModeleMembres.setData( ModeleMembres.index( i, 12 ),QColor( Qt::red ), Qt::BackgroundColorRole ) ;
                }
                else
                {
                    if( DateCotisation< QDate::currentDate().addDays( 14 ) )
                    {
                        ModeleMembres.setData( ModeleMembres.index( i, 12 ),QColor( Qt::yellow ), Qt::BackgroundColorRole ) ;
                    }
                    else
                    {
                        ModeleMembres.setData( ModeleMembres.index( i, 12 ),QColor( Qt::green ), Qt::BackgroundColorRole ) ;
                    }
                }
            }
            //",NbreRetard,DateInscription,DateExpiration,SUM(CreditRestant) as CreditRestant
            ModeleMembres.setItem( i, 13, new QStandardItem( sNumero.setNum( RequeteListemembres.record().value( RequeteListemembres.record().indexOf("CreditRestant") ).toInt() ) ) ) ;
            ModeleMembres.setItem( i, 14, new QStandardItem( RequeteListemembres.record().value( RequeteListemembres.record().indexOf("DateInscription") ).toDate().toString( "yyyy-MM-dd" ) ) ) ;

            i++ ;
        }

        if( this->bAdmin == true )
        {
            ui->TbW_ListeMembre->setColumnWidth( 1, 30 ) ;
        }
        else
        {
            ui->TbW_ListeMembre->setColumnWidth( 1, 0 ) ;
        }
    }
    else //Sinon on affiche un message d'erreur et on retourne Faux
    {
        qDebug() << "F_ListeMembres:: : RequeteListeMembres :" << RequeteListemembres.lastQuery()  ;
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

    this->DecocherTout( false ) ;
}

void F_ListeMembres::on_Bt_ToutDecocher_clicked()
{
    this->DecocherTout( false ) ;
}

void F_ListeMembres::on_Bt_SelectionListe_clicked()
{
    //Permet mettre toute les checkbox en état "Check"
    for (register int i (0) ; i < ui->TbW_ListeMembre->model()->rowCount() ; i++ )
    {
        ui->TbW_ListeMembre->model()->setData( ui->TbW_ListeMembre->model()->index(i ,0), Qt::Checked, Qt::CheckStateRole) ;
    }
}

void F_ListeMembres::on_Bt_DeselectionListe_clicked()
{
    //Parcour la liste pour "decheck" toute toutes les checkbox
    for (register int i (0) ; i < ui->TbW_ListeMembre->model()->rowCount() ; i++ )
    {
        ui->TbW_ListeMembre->model()->setData( ui->TbW_ListeMembre->model()->index(i ,0), Qt::Unchecked, Qt::CheckStateRole) ;
    }
}

void F_ListeMembres::on_Bt_SupprimerListe_clicked()
{
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
                RequeteEmprunts.bindValue( ":IdMembre", this->ModeleMembres.data( this->ModeleMembres.index( i, 0 )).toInt() ) ;

                if( !RequeteEmprunts.exec() )
                {
                    qDebug()<< "F_AdministrerMembres::SupprimerMembre : RequeteEmprunts : Erreur de connexion avec la base de donne !" << RequeteEmprunts.lastQuery()<< endl ;
                }

                //S'il n'y a pas d'emprunt en cour
                if( RequeteEmprunts.size()== 0 )
                {
                    //Vérification que la personne veux bien supprimer le membre
                    //Préparation de la requête permettant la suppression dans la table de membre --------------------------
                    RequeteSupprimer.prepare( "DELETE FROM membres WHERE IdMembre=:IdMembre " ) ;
                    RequeteSupprimer.bindValue( ":IdMembre", this->ModeleMembres.data( this->ModeleMembres.index( i, 0 )).toInt() ) ;

                    //Execution de la requête, si elle fonctionne on met la variable de retoure à  vrai
                    if( !RequeteSupprimer.exec() )
                    {
                        qDebug()<< "F_AdministrerMembres::SupprimerMembre : RequeteSupprimer : Erreur de connexion avec la base de donne !" << RequeteSupprimer.lastQuery()<< endl ;
                    }

                    //Préparation de la requête permettant la suppression dans la table reservation -------------------------------
                    RequeteSupprimer.prepare( "DELETE FROM reservation WHERE Membres_IdMembre=:IdMembre " ) ;
                    RequeteSupprimer.bindValue( ":IdMembre", this->ModeleMembres.data( this->ModeleMembres.index( i, 0 )).toInt() ) ;

                    //Execution de la requête, si elle fonctionne on met la variable de retoure à  vrai
                    if( !RequeteSupprimer.exec() )
                    {
                        qDebug()<< "F_AdministrerMembres::SupprimerMembre : RequeteSupprimer : Erreur de connexion avec la base de donne !" << RequeteSupprimer.lastQuery()<< endl ;
                    }

                    //Préparation de la requête permettant la suppression dans la table emprunts ------------------------
                    RequeteSupprimer.prepare( "DELETE FROM emprunts WHERE Membres_IdMembre=:IdMembre " ) ;
                    RequeteSupprimer.bindValue( ":IdMembre", this->ModeleMembres.data( this->ModeleMembres.index( i, 0 )).toInt() ) ;

                    //Execution de la requête, si elle fonctionne on met la variable de retoure à  vrai
                    if( !RequeteSupprimer.exec() )
                    {
                        qDebug()<< "F_AdministrerMembres::SupprimerMembre : RequeteSupprimer : Erreur de connexion avec la base de donne !" << RequeteSupprimer.lastQuery()<< endl ;
                    }

                    //Préparation de la requête permettant la suppression dans la table abonnements-----------------------
                    RequeteSupprimer.prepare( "DELETE FROM abonnements WHERE Membres_IdMembre=:IdMembre " ) ;
                    RequeteSupprimer.bindValue( ":IdMembre", this->ModeleMembres.data( this->ModeleMembres.index( i, 0 )).toInt() ) ;

                    //Execution de la requête, si elle fonctionne on met la variable de retoure à  vrai
                    if( !RequeteSupprimer.exec() )
                    {
                        qDebug()<< "F_AdministrerMembres::SupprimerMembre : RequeteSupprimer : Erreur de connexion avec la base de donne !" << RequeteSupprimer.lastQuery()<< endl ;
                    }
                }
                else
                {
                    QMessageBox::information( this, "Suppression Membre","Impossible de supprimer " + ui->TbW_ListeMembre->model()->data( ui->TbW_ListeMembre->model()->index(i ,1) ).toString()  + " " + ui->TbW_ListeMembre->model()->data( ui->TbW_ListeMembre->model()->index(i ,2) ).toString() + " membre.\nIl a encore des jeux en cours d'emprunts.",  "Ok" ) ;
                }
            }
        }
    }
    this->AffichageListe() ;
}

void F_ListeMembres::on_Bt_Exporter_clicked()
{
//    trayIcon->contextMenu()->popup(QCursor::pos());
    int nNombreColonne (0);
    int nNombreLigne (0);

    QString sCaractere;
    QString nomFichier = QFileDialog::getSaveFileName(this, "Enregistrer sous ...", "Sans titre 1.csv");
    QFile fichier(nomFichier);
    QTextStream ecrire (&fichier);
    fichier.open(QIODevice::WriteOnly);

    // Récupération des titres de colonnes et concaténation dans la variable titre si les checkboxes sont cochées
    QString titre;
    for (int i = 0; i < this->Items.size(); ++i) {
        QStandardItem * item = this->Items[i];
        if(item->checkState() == Qt::Checked)
        {
            if(titre.size()==0)
            {
                titre+=item->text();
            }
            else
            {
                titre+=";"+item->text();
            }
        }
    }

    ecrire << titre + "\r\n";

    for(nNombreLigne = 0; nNombreLigne<ui->TbW_ListeMembre->model()->rowCount(); nNombreLigne++)
    {
        for(nNombreColonne = 2; nNombreColonne<ui->TbW_ListeMembre->model()->columnCount(); nNombreColonne++)
        {
            QStandardItem * item = this->Items[nNombreColonne-2];
            if(item->checkState() == Qt::Checked)
            {
                titre=ui->TbW_ListeMembre->model()->headerData(nNombreColonne,Qt::Horizontal,Qt::DisplayRole).toString();
                sCaractere = ui->TbW_ListeMembre->model()->data( ui->TbW_ListeMembre->model()->index( nNombreLigne, nNombreColonne ) ).toString() ;
                // On rejete les valeurs à caractère unique et on le remplace par un champs vide
                sCaractere.replace(" ", "\ ") ;
                if(sCaractere == "-" || sCaractere == "_" || sCaractere == ".")
                {
                    sCaractere = "";
                }
                ecrire << "\"" << sCaractere << "\";";
            }
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
    emit( this->Signal_DoubleClic_ListeMembres( this->ModeleMembres.data( this->ModeleMembres.index( index.row(), 0 )).toInt() ) ) ;
}

void F_ListeMembres::on_ChBx_Activite_clicked()
{
    this->AffichageListe() ;
}

void F_ListeMembres::on_CBx_Activites_currentIndexChanged(int index)
{
    this->AffichageListe() ;
}
