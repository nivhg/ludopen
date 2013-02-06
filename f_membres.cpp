//------------------------------------------------------------------------------
/** @file         f_membres.cpp
 *  @brief        Permet à un utilisateur de créer, de modifier ou de consulter les informations sur un membres. En administration permet de supprimer des membres
 *
 *  @author       PADIOU Nicolas
 *  @author       STS IRIS, Lyce Nicolas APPERT, ORVAULT ( FRANCE )
 *  @since        01/01/2012
 *  @version      0.1
 *  @date         22/09/2012 par William
 *
 *  Description dtaille du fichier f_membres.ccp
 *
 *
 *  @todo         Voir post-it Création d'un fenètre liste membre admimnistrateur pour pouvoir supprimer
 *
 *  @bug          aucun
 */
//------------------------------------------------------------------------------
#include "f_membres.h"
#include "ui_f_membres.h"

// En-ttes standards necessaires dans ce fichier en-tete seulement ------------
#include <QMessageBox>

/** Initialisation de la classe
 *  @param  F_RechercheMembres * pRechercheMembres
 *  @test   Voir la procédure dans le fichier associé.
 */
F_Membres::F_Membres( F_RechercheMembres * pRechercheMembres, bool bAdmin, QWidget *parent ):
    QWidget( parent ),
    ui( new Ui::F_Membres )
{
    ui->setupUi( this ) ;

    //Indique si c'est en administration ou pas
    this->bAdmin = bAdmin ;

    ui->Bt_SupprimerMembre->setVisible( this->bAdmin ) ;
    ui->Bt_SupprimerMembre->setDisabled( true ) ;

    //aucun membre sélectionné par defaut
    this->nIdMembreSelectionne = 0 ;

    //Initialisation des pointeur
    this->pRechercheMembres = pRechercheMembres ;
    this->pHistoriqueJeux = new F_HistoriqueJeux ;
    this->pHistoriqueJeux->setWindowModality( Qt::ApplicationModal ) ;
    this->pAjouterCotiCarte = new F_AjouterCotiCarte ;
    this->pAjouterCotiCarte->setWindowModality( Qt::ApplicationModal ) ;

    // Création des 2 fenetres d'ajout d'un nouveau titre et type de membre
    this->pTypeAjMod = new F_PopUpCLESTTEM(5);
    this->pTypeAjMod->setWindowModality(Qt::ApplicationModal);
    this->pTitreAjMod = new F_PopUpCLESTTEM(6);
    this->pTitreAjMod->setWindowModality(Qt::ApplicationModal);

    //Initialisation des tableaux, des boutons et des champs
    this->VerrouillerInfosPerso( true ) ;

    this->AfficherAjouterModifierMembre( true ) ;
    ui->Bt_ModifierMembre->setDisabled( true ) ;

    this->AfficherValiderAnnuler( false ) ;

    this->VerrouillerJeux( true ) ;

    this->VerrouillerAbonnements( true ) ;

    this->EffacerTousLesChamps() ;

    /////////////Fenêtre d'ajout d'une ville///////////////

    this->oFenetreAjoutVille = new QDialog(this) ;
    this->oFenetreAjoutVille->setModal( true ) ;
    this->oFenetreAjoutVille->setMaximumSize( 230, 100) ;
    this->oFenetreAjoutVille->setMinimumSize( 230, 100) ;
    this->oFenetreAjoutVille->hide() ;

    this->LE_AjoutVille = new QLineEdit( this->oFenetreAjoutVille ) ;
    this->LE_AjoutVille->setGeometry(10, 30, 210, this->LE_AjoutVille->height() );

    this->Bt_ValiderVille = new QPushButton ( this->oFenetreAjoutVille ) ;
    this->Bt_ValiderVille->setText( "Valider" ) ;
    this->Bt_ValiderVille->setGeometry( 10, this->LE_AjoutVille->height() + 40, Bt_ValiderVille->width(), Bt_ValiderVille->height() ) ;

    this->Bt_AnnuerVille = new QPushButton ( this->oFenetreAjoutVille ) ;
    this->Bt_AnnuerVille->setText( "Annuler" );
    this->Bt_AnnuerVille->setGeometry( 20 + Bt_ValiderVille->width(), this->LE_AjoutVille->height() + 40, Bt_AnnuerVille->width(), Bt_AnnuerVille->height() ) ;

    this->Lb_NomVille = new QLabel ( this->oFenetreAjoutVille ) ;
    this->Lb_NomVille->setText( "Nouveau nom de ville :") ;
    this->Lb_NomVille->setGeometry( 10, 0, 200, this->Lb_NomVille->height() ) ;

    //Connect--------------------------------------------------------------------------------------------------
    connect( this->pRechercheMembres, SIGNAL( SignalRenvoieIdMembre( uint ) ), this, SLOT( slot_AfficherMembre( uint ) ) ) ;
    connect( this->pTitreAjMod, SIGNAL(SignalValider()), this, SLOT( slot_ChoisirNouveauTitre() ) ) ;
    connect( this->pTypeAjMod, SIGNAL(SignalValider()), this, SLOT( slot_ChoisirNouveauType() ) ) ;
    connect( this->pAjouterCotiCarte, SIGNAL( SignalAjoutCotisationCarte() ), this, SLOT( slot_ActualiserAbonnements() ) ) ;
    connect( this->Bt_ValiderVille, SIGNAL( clicked() ), this, SLOT( slot_ValiderAjoutVille() ) ) ;
    connect( this->Bt_AnnuerVille, SIGNAL( clicked() ), this, SLOT(slot_AnnulerAjoutVille()) ) ;

    //Initialisation de la liste de titres et de types
    this->MaJTitre() ;
    this->MaJType() ;
}

/** Detruit les objets cree
 *  @test
 */
F_Membres::~F_Membres()
{
    if( this->pHistoriqueJeux )
    {
        delete this->pHistoriqueJeux  ;
    }

    if(this->pTitreAjMod)
    {
        delete this->pTitreAjMod;
    }

    if(this->pTypeAjMod)
    {
        delete this->pTypeAjMod;
    }

    delete ui ;
}

//---------------------------------------------------------------------------
// METHODEs PUBLIQUEs
//---------------------------------------------------------------------------

/** Met   jour la liste des Titres depuis la base de données
 *  @pre    Accés à la base de données
 *  @test   Voir la procédure dans le fichier associé.
 */
void F_Membres::MaJTitre ()
{
    int i           ( 0 )    ;
    QSqlQuery Requete        ;
    Titre oTitre             ;

    //Suppression du contenu du vecteur de la combobox CBx_Titre
    this->VectorTitre.clear() ;
    ui->CBx_Titre->clear() ;

    //Exécution de la requète qui sélectionne le contenu de la table titremembre
    if( Requete.exec( "SELECT * FROM titremembre" ) )
    {
        oTitre.id = 0 ;
        oTitre.sTitre = "" ;
        this->VectorTitre.push_back( oTitre ) ;

        //Remplissage du vecteur avec ce que retourne la requête
        while( Requete.next() )
        {
            oTitre.id = Requete.value( 0 ).toInt() ;
            oTitre.sTitre = Requete.value( 1 ).toString() ;
            oTitre.nJeuxAutorises = Requete.value( 2 ).toInt() ;

            this->VectorTitre.push_back( oTitre ) ;
        }

        //Création d'un choix permettant de créer un nouveau titre
        oTitre.id = 0 ;
        oTitre.sTitre = "Créer Titre ..." ;

        this->VectorTitre.push_back( oTitre ) ;

        //Remplissage de la combobox grace au vecteur
        for( i = 0 ; i < VectorTitre.size() ; i ++ )
        {
            ui->CBx_Titre->insertItem( i, VectorTitre[i].sTitre ) ;
        }
    }
    else //Sinon on affiche un message d'erreur et on retourne Faux
    {
        cerr << "F_Membres::MaJTitre (): Erreur bdd : " << Requete.lastError().text().toStdString() << endl ;
    }
}

/**  Met a jour la liste des Types depuis la base de données
 *  @pre    Accès à la base de données
 *  @test
 */
void F_Membres::MaJType ()
{
    int i( 0 ) ;
    QSqlQuery Requete ;
    Type oType ;

    //Suppression du contenu du vecteur de la combobox CBx_Type
    this->VectorType.clear() ;
    ui->CBx_Type->clear() ;

    //Exécution de la requète qui sélectionne le contenu de la table tytremembres
    if( Requete.exec( "SELECT * FROM typemembres ;" ) )
    {
        oType.id = 0 ;
        oType.sType = "" ;
        this->VectorType.push_back( oType ) ;

        //Remplissage du vecteur avec ce que retourne la requête
        while( Requete.next() )
        {
            oType.id = Requete.value( 0 ).toInt() ;
            oType.sType = Requete.value( 1 ).toString() ;

            this->VectorType.push_back( oType ) ;
        }

        //Création d'un choix permettant de créer un nouveau titre
        oType.id = 0 ;
        oType.sType = "Créer Type ..." ;

        this->VectorType.push_back( oType ) ;


        //Remplissage de la combobox grace au vecteur
        for( i = 0 ; i < VectorType.size() ; i ++ )
        {
            ui->CBx_Type->insertItem( i, VectorType[ i ].sType ) ;
        }
    }
    else //Sinon on affiche un message d'erreur et on retourne Faux
    {
        cerr << "F_Membres::MaJType (): Erreur bdd :" <<  Requete.lastError().text().toStdString() <<  endl ;
    }
}

/** Recupere l'emplacement d'un titre dans un vecteur en fonction de son id
 *  @pre    Combobox remplie avec le vecteur
 *  @param  unsigned int nIdTitre
 *  @retval int
 *  @return L'emplacement du titre dans le vecteur par rapport à l'id ( 0 par défaut )
 *  @test   Voir la procédure dans le fichier associé.
 */
int F_Membres::RecupererEmplacementTitreVecteur( unsigned int nIdTitre )
{
    int nEmplacementTitre ( 0 ) ;
    int nBoucle           ( 0 ) ;

    //Recherche de l'id dans le vecteur tant que l'on a pas fini de parcour le vecteur
    while( nBoucle < this->VectorTitre.size()&& nEmplacementTitre == 0 && nIdTitre != 0 )
    {
        if( this->VectorTitre[ nBoucle ].id == nIdTitre )
        {
            nEmplacementTitre = nBoucle ;
        }
        nBoucle ++ ;
    }
    return nEmplacementTitre ;
}

/** Recupere l'emplacement d'un type dans un vecteur en fonction de son id
 *  @pre    Combobox remplie avec le vecteur
 *  @param  unsigned int nIdTytre
 *  @retval int
 *  @return L'emplacement du titre dans le vecteur par rapport à l'id ( 0 par défaut )
 *  @test   Voir la procédure dans le fichier associé.
 */
int F_Membres::RecupererEmplacementTypeVecteur( unsigned int nIdType )
{
    int nEmplacementType ( 0 ) ;
    int nBoucle          ( 0 ) ;

    //Recherche de l'id dans le vecteur tant que l'on a pas fini de parcour le vecteur
    while( nBoucle < this->VectorType.size()&& nEmplacementType == 0 && nIdType != 0 )
    {
        if( this->VectorType[ nBoucle ].id == nIdType )
        {
            nEmplacementType = nBoucle ;
        }
        nBoucle ++ ;
    }

    return nEmplacementType ;
}

/** Efface tous ce qui n'est pas un chiffre et ajoute un espace tous les 2 caractéres dans un string
 *  @param QString sNumero
 *  @retval QString
 *  @return Retourne un QString ne contenant plus de lettre ou caractêres speciaux. Un espace est ajouté tous les 2 caractéres
 *  @test   Voir la procédure dans le fichier associé.
 */
QString F_Membres::ModifierSyntaxeNumTelephone ( QString sNumero )
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

/** Affiche les jeux empruntés dans le tableau TbW_Emprunt
  *  @pre    Accés à  la base de données
  *  @param  unsigned int nIdMembre
  *  @test   Voir la procédure dans le fichier associé.
  */
void F_Membres::AfficherJeuxEmpruntes( unsigned int nIdMembre )
{
    QSqlQuery RequeteEmprunt ;
    QSqlQuery RequeteJeux    ;
    int i     ( 0 )        ;

    //Création et initialisation du Model
    QStandardItemModel * modele = new QStandardItemModel() ;
    ui->LW_JeuxEmpruntes->setModel( modele ) ;
    ui->LW_JeuxEmpruntes->setEditTriggers( 0 ) ;

    //requète permettant d'avoir les jeux empruntés correspondant à l'id du membre
    RequeteEmprunt.prepare( "SELECT Jeux_IdJeux, DateEmprunt, DateRetourPrevu FROM emprunts WHERE Membres_IdMembre=:nIdMembre AND DateRetour IS NULL ;" ) ;
    RequeteEmprunt.bindValue( "nIdMembre", nIdMembre ) ;

    //Exectution de la requète
    if( RequeteEmprunt.exec() )
    {

        /*Cration des caractristiques du tableau : -Nombre de colonnes
                                                      -Nom des colonnes
                                                      -Nombre de lignes*/
        modele->setColumnCount( 3 ) ;
        modele->setRowCount( RequeteEmprunt.size() ) ;
        modele->setHorizontalHeaderItem( 0, new QStandardItem( "Jeux" ) ) ;
        modele->setHorizontalHeaderItem( 1, new QStandardItem( "Emprunt" ) ) ;
        modele->setHorizontalHeaderItem( 2, new QStandardItem( "Retour prévu" ) ) ;


        //Remplissage du tableau avec les informations de la table emprunts
        while( RequeteEmprunt.next() )
        {

            //Requete pour rcuperer le nom du jeux
            RequeteJeux.prepare( "SELECT NomJeu FROM jeux WHERE IdJeux=:nIdJeux ;" ) ;
            RequeteJeux.bindValue( "nIdJeux", RequeteEmprunt.record().value( 0 ).toInt() ) ;

            if( RequeteJeux.exec() )
            {
                RequeteJeux.next() ;
                modele->setItem( i, 0, new QStandardItem( RequeteJeux.record().value( 0 ).toString() ) ) ;
            }
            else
            {
                cerr << "F_Membres::AfficherJeuxEmpruntes : RequeteJeux : Erreur de connexion avec la base de donne !" << endl ;
            }

            modele->setItem( i, 1, new QStandardItem( RequeteEmprunt.record().value( 1 ).toDateTime().toString( "dd.MM.yy" ) ) ) ;
            modele->setItem( i, 2, new QStandardItem( RequeteEmprunt.record().value( 2 ).toDateTime().toString( "dd.MM.yy" ) ) ) ;

            if ( RequeteEmprunt.value( 2 ).toDate()>QDate::currentDate() )
            {
                modele->setData( modele->index( i,2 ),QColor( Qt::green ), Qt::BackgroundColorRole ) ;
            }
            else
            {
                modele->setData( modele->index( i,2 ),QColor( Qt::red ), Qt::BackgroundColorRole ) ;
            }
            i++ ;

        }
    }
    else //Sinon on affiche un message d'erreur et on retourne Faux
    {
        cerr << "F_Membres::AfficherJeuxEmpruntes : RequeteEmprunt : Erreur de connexion avec la base de donne !" << endl ;
    }
}

/** Affiche les informations d'un membre
  *  @pre    Accés ? la base de données
  *  @param  unsigned int nIdMembre
  *  @test   Voir la procédure dans le fichier associé.
  */
void F_Membres::AfficherMembre()
{
    this->AfficherMembre( this->nIdMembreSelectionne ) ;
}


/** Affiche les informations d'un membre
  *  @pre    Accés ? la base de données
  *  @param  unsigned int nIdMembre
  *  @test   Voir la procédure dans le fichier associé.
  */
void F_Membres::AfficherMembre( unsigned int nIdMembre )
{
    QSqlQuery RequeteMembre ;
    QPalette palette ;
    QFont font ;

    //Effacement de tous les champs
    this->EffacerTousLesChamps() ;

    this->nIdMembreSelectionne = nIdMembre ;

    ui->Bt_Modifier->setDisabled( true ) ;
    ui->Bt_Supprimer->setDisabled( true ) ;

    //Si l'id du membre est diffêrent de 0
    if( nIdMembre != 0 )
    {
        //requète permettant de récuperer tous les informations d'un membre grâce à son Id
        RequeteMembre.prepare( "SELECT * FROM membres WHERE IdMembre=:id ;" ) ;
        RequeteMembre.bindValue( ":id", nIdMembre ) ;

        //Execution de la requète
        if( RequeteMembre.exec() )
        {
            //Remplissage des champs avec les données retournées par la base de données
            if( RequeteMembre.next() )
            {
                ui->CBx_Titre->setCurrentIndex( this->RecupererEmplacementTitreVecteur( RequeteMembre.record().value( 1 ).toInt() ) ) ;

                ui->CBx_Type->setCurrentIndex( this->RecupererEmplacementTypeVecteur( RequeteMembre.record().value( 2 ).toInt() ) ) ;

                ui->Le_Nom->setText( RequeteMembre.record().value( 3 ).toString() ) ;

                ui->Le_Prenom->setText( RequeteMembre.value( 4 ).toString() ) ;

                ui->Te_Rue->setPlainText( RequeteMembre.record().value( 5 ).toString() ) ;

                ui->Le_CP->setText( RequeteMembre.record().value( 6 ).toString() ) ;

                this->AfficherVilles( RequeteMembre.record().value( 7 ).toString() ) ;

                ui->Le_TelFix->setText( this->ModifierSyntaxeNumTelephone( RequeteMembre.value( 8 ).toString() ) ) ;

                ui->Le_TelMobile->setText( this->ModifierSyntaxeNumTelephone( RequeteMembre.value( 9 ).toString() ) ) ;

                ui->LE_Fax->setText( this->ModifierSyntaxeNumTelephone( RequeteMembre.value( 10 ).toString() ) ) ;

                ui->LE_Email->setText( RequeteMembre.value( 11 ).toString() ) ;

                ui->SBx_JeuxAutorises->setValue( RequeteMembre.record().value( 12 ).toInt() ) ;

                ui->DtE_Insritption->setDateTime( RequeteMembre.record().value( 13 ).toDateTime() ) ;

                ui->TE_Remarque->setPlainText( RequeteMembre.record().value( 14 ).toString() ) ;

                if( RequeteMembre.record().value( 15 ).toBool() )
                {
                    ui->Lb_MembreEcarte->setVisible( true ) ;
                    ui->ChBx_MembreEcarte->setChecked( true ) ;

                    font.setBold( true ) ;
                    ui->Lb_MembreEcarte->setFont( font ) ;

                    palette.setColor( QPalette::WindowText, Qt::red ) ;
                    ui->Lb_MembreEcarte->setPalette( palette ) ;
                }
                else
                {
                    ui->Lb_MembreEcarte->setHidden( true ) ;
                    ui->ChBx_MembreEcarte->setChecked( false ) ;

                    font.setBold( false ) ;
                    ui->Lb_MembreEcarte->setFont( font ) ;

                    palette.setColor( QPalette::WindowText, Qt::black ) ;
                    ui->Lb_MembreEcarte->setPalette( palette ) ;
                }

                ui->DtE_Naissance->setDateTime( RequeteMembre.record().value( 16 ).toDateTime() ) ;

                ui->Le_Code->setText( RequeteMembre.record().value( 17 ).toString() ) ;

                ui->LE_MembreAssocie->setText( RequeteMembre.record().value( 21 ).toString() ) ;

                ui->SPx_NbreRetards->setValue( RequeteMembre.record().value( 22 ).toInt() ) ;


                this->AfficherJeuxEmpruntes( nIdMembre ) ;

                ui->Bt_ModifierMembre->setEnabled( true ) ;
                ui->Bt_SupprimerMembre->setEnabled( true ) ;

                this->AfficherAbonnements( nIdMembre ) ;
            }

        }
        else //Sinon on affiche un message d'erreur et on retourne Faux
        {
            cerr << "F_Membres::AfficherMembre : Erreur de connexion avec la base de données !" << RequeteMembre.lastError().text().toStdString() <<  endl ;
            ui->Bt_ModifierMembre->setDisabled( true ) ;
        }
    }
    else
    {
        ui->Bt_ModifierMembre->setDisabled( true ) ;
    }

}

/** Permet d'ajouter un membre
  *  @pre    Accês à la base de données
  *  @retval bool
  *  @return True si tous c'est bien passé et false en cas d'erreur
  *  @test   Voir la procédure dans le fichier associé.
  */
bool F_Membres::AjouterMembre()
{
    bool bRetourne( true ) ;
    QSqlQuery RequeteMembre ;

    //Enregistrement d'un nouveau membre dans la base de données
    RequeteMembre.prepare( "INSERT INTO membres ( TitreMembre_IdTitreMembre, TypeMembres_IdTypeMembres, Nom, Prenom, Rue, CP, Ville, Telephone, Mobile, Fax, Email, NbreJeuxAutorises, DateInscription, DateNaissance, Remarque, Ecarte, CodeMembre, MembreAssocie )"
                           "VALUES ( :TitreMembre_IdTitreMembre, :TypeMembres_IdTypeMembres, :Nom, :Prenom, :Rue, :CP, :Ville, :Telephone, :Mobile, :Fax, :Email, :NbreJeuxAutorises, :DateInscription, :DateNaissance, :Remarque, :Ecarte, :CodeMembre, :MembreAssocie )" ) ;

    //Titre Membre
    RequeteMembre.bindValue( ":TitreMembre_IdTitreMembre", this->VectorTitre[ui->CBx_Titre->currentIndex()].id ) ;

    //Type Membre
    RequeteMembre.bindValue( ":TypeMembres_IdTypeMembres", this->VectorType[ui->CBx_Type->currentIndex()].id ) ;

    //Nom
    RequeteMembre.bindValue( ":Nom", ui->Le_Nom->text() ) ;

    //Prenom
    RequeteMembre.bindValue( ":Prenom", ui->Le_Prenom->text() ) ;

    //Rue
    RequeteMembre.bindValue( ":Rue", ui->Te_Rue->toPlainText() ) ;

    //Code Postale
    RequeteMembre.bindValue( ":CP", ui->Le_CP->text().toInt() ) ;

    //Ville
    RequeteMembre.bindValue( ":Ville", ui->CBx_Ville->currentText() ) ;

    //Telephone
    RequeteMembre.bindValue( ":Telephone", ui->Le_TelFix->text() ) ;

    //Mobile
    RequeteMembre.bindValue( ":Mobile", ui->Le_TelMobile->text() ) ;

    //Fax
    RequeteMembre.bindValue( ":Fax", ui->LE_Fax->text() ) ;

    //Email
    RequeteMembre.bindValue( ":Email", ui->LE_Email->text() ) ;

    //Nombre de jeux Autorisés
    RequeteMembre.bindValue( ":NbreJeuxAutorises", ui->SBx_JeuxAutorises->text().toInt() ) ;

    //Date d'inscription
    RequeteMembre.bindValue( ":DateInscription", ui->DtE_Insritption->date() ) ;

    //Date Naissance
    RequeteMembre.bindValue( ":DateNaissance", ui->DtE_Naissance->date() ) ;

    //Remarque
    RequeteMembre.bindValue( ":Remarque", ui->TE_Remarque->toPlainText() ) ;

    //Ecarte
    RequeteMembre.bindValue( ":Ecarte", ui->ChBx_MembreEcarte->isChecked() ) ;

    //Code Membre
    RequeteMembre.bindValue( ":CodeMembre", ui->Le_Code->text() ) ;

    //Membre associé
    RequeteMembre.bindValue( ":MembreAssocie", ui->LE_MembreAssocie->text().toInt() ) ;

    //Si le membre à bien était enregistrer this->nIdMembreSelectionne prend pour valeur l'id du membre créé
    if( RequeteMembre.exec() )
    {
        this->nIdMembreSelectionne = RequeteMembre.lastInsertId().toInt() ;
    }
    else//Sinon on affiche un message d'erreur et on retourne Faux
    {
        cerr << "F_Membres::AjouterMembre : Erreur de connexion avec la base de donne !" << RequeteMembre.lastError().text().toStdString()<< endl ;
        bRetourne = false ;
    }

    return bRetourne ;
}

/** Permet de modifier le membre dans la base de données
  *  @pre    Accés à la base de données, Base de données contenant minimum un membre
  *  @param  unsigned int nIdMembre
  *  @retval bool
  *  @return True si tous c'est bien passé et false en cas d'erreur
  *  @test   Voir la procédure dans le fichier associé.
  */
bool F_Membres::ModifierMembre( unsigned int nIdMembre )
{

    bool bRetourne ( true ) ;
    QSqlQuery RequeteMembre ;

    //On vérifie que l'id est présent dans la base donnée
    RequeteMembre.prepare( " SELECT IdMembre FROM membres WHERE IdMembre=:id " ) ;
    RequeteMembre.bindValue( ":id", nIdMembre ) ;

    //Exécution de la requête
    if( RequeteMembre.exec() )
    {
        //Si l'id n'est pas présent dans la table on met nId Membre à 0
        if( RequeteMembre.size()== 0 )
        {
            nIdMembre = 0 ;
        }
    }
    else //Sinon on affiche un message d'erreur et on retourne Faux
    {
        cerr << "F_Membres::ModifierMembre : Erreur de connexion avec la base de donne !" << RequeteMembre.lastError().text().toStdString()<< endl ;
    }



    //S'il l'id est différent de 0 on ne mofifie pas le membre
    if( nIdMembre != 0 )
    {
        //Mise à des nouvelles informations sur le membre sélectionné dans la base de données
        RequeteMembre.prepare( "UPDATE membres SET membres.TitreMembre_IdTitreMembre=:TitreMembre_IdTitreMembre, membres.TypeMembres_IdTypeMembres=:TypeMembres_IdTypeMembres, membres.Nom=:Nom, membres.Prenom=:Prenom, membres.Rue=:Rue, membres.CP=:CP, membres.Ville=:Ville, membres.Telephone=:Telephone, membres.Mobile=:Mobile, membres.Fax=:Fax, membres.Email=:Email, membres.NbreJeuxAutorises=:NbreJeuxAutorises, membres.DateInscription=:DateInscription, DateNaissance=:DateNaissance, membres.Remarque=:Remarque, membres.Ecarte=:Ecarte, membres.CodeMembre=:CodeMembre ,membres.MembreAssocie=:MembreAssocie "
                               "WHERE membres.IdMembre=:IdMembre" ) ;

        //ID Membre
        RequeteMembre.bindValue( ":IdMembre", this->nIdMembreSelectionne ) ;

        //Titre Membre
        RequeteMembre.bindValue( ":TitreMembre_IdTitreMembre", this->VectorTitre[ui->CBx_Titre->currentIndex()].id ) ;

        //Type Membre
        RequeteMembre.bindValue( ":TypeMembres_IdTypeMembres", this->VectorType[ui->CBx_Type->currentIndex()].id ) ;

        //Nom
        RequeteMembre.bindValue( ":Nom", ui->Le_Nom->text() ) ;

        //Prenom
        RequeteMembre.bindValue( ":Prenom", ui->Le_Prenom->text() ) ;

        //Rue
        RequeteMembre.bindValue( ":Rue", ui->Te_Rue->toPlainText() ) ;

        //Code Postale
        RequeteMembre.bindValue( ":CP", ui->Le_CP->text().toInt() ) ;

        //Ville
        RequeteMembre.bindValue( ":Ville", ui->CBx_Ville->currentText() ) ;

        //Telephone
        RequeteMembre.bindValue( ":Telephone", ui->Le_TelFix->text() ) ;

        //Mobile
        RequeteMembre.bindValue( ":Mobile", ui->Le_TelMobile->text() ) ;

        //Fax
        RequeteMembre.bindValue( ":Fax", ui->LE_Fax->text() ) ;

        //Email
        RequeteMembre.bindValue( ":Email", ui->LE_Email->text() ) ;

        //Nombre de jeux Autorisés
        RequeteMembre.bindValue( ":NbreJeuxAutorises", ui->SBx_JeuxAutorises->text().toInt() ) ;

        //Date d'inscription
        RequeteMembre.bindValue( ":DateInscription", ui->DtE_Insritption->date() ) ;

        //Date Naissance
        RequeteMembre.bindValue( ":DateNaissance", ui->DtE_Naissance->date() ) ;

        //Remarque
        RequeteMembre.bindValue( ":Remarque", ui->TE_Remarque->toPlainText() ) ;

        //Ecarte
        RequeteMembre.bindValue( ":Ecarte", ui->ChBx_MembreEcarte->isChecked() ) ;

        //Code Membre
        RequeteMembre.bindValue( ":CodeMembre", ui->Le_Code->text() ) ;

        //Membre associé
        RequeteMembre.bindValue( ":MembreAssocie", ui->LE_MembreAssocie->text() ) ;

        //Si le membre n'a pas été enregisté on indique l'erreur qu'il y a une erreur et on retourne l'erreur de sql
        if( RequeteMembre.exec()== false )
        {
            cerr << "F_Membres::ModifierMembre : Erreur de connexion avec la base de donne !" << endl <<  RequeteMembre.lastError().text().toStdString()<< endl ;
            bRetourne = false ;
        }
    }
    else
    {
        bRetourne = false ;
    }
    return bRetourne ;
}

/** Permet la suppression du dont l'id est passé en paramêtre
 *  @pre    Connexion à la base de données, Membre présent dans la base de données
 *  @param  Indique l'id du membre à supprimer
 *  @retval bool
 *  @return Retourne vrai si la requête a bien été executer sinon elle retourne faux
 *  @test   Voir la procédure dans le fichier associé.
 */
bool F_Membres::SupprimerMembre( int nIdMembre )
{
    bool bRetourne ( true ) ;
    QSqlQuery RequeteSupprimer ;
    QSqlQuery RequeteEmprunts ;

    //Préparation de la requête la recherche des emprunts
    RequeteEmprunts.prepare( "SELECT IdEmprunts FROM emprunts WHERE Membres_IdMembre=:IdMembre AND DateRetour IS NULL  " ) ;
    RequeteEmprunts.bindValue( ":IdMembre", nIdMembre ) ;

    if( !RequeteEmprunts.exec() )
    {
        cerr << "F_AdministrerMembres::SupprimerMembre : RequeteEmprunts : Erreur de connexion avec la base de donne !" << RequeteEmprunts.lastError().text().toStdString()<< endl ;
        bRetourne = false ;
    }

    //S'il n'y a pas d'emprunt en cour
    if( RequeteEmprunts.size()== 0 )
    {
        //Vérification que la personne veux bien supprimer le membre
        if ( QMessageBox::information( this, "Suppression Membre","Voulez vraiment supprimer ce membre ?", "Supprimer", "Annuler" ) == 0 )
        {
            //Préparation de la requête permettant la suppression dans la table de membre --------------------------
            RequeteSupprimer.prepare( "DELETE FROM membres WHERE IdMembre=:IdMembre " ) ;
            RequeteSupprimer.bindValue( ":IdMembre", nIdMembre ) ;

            //Execution de la requête, si elle fonctionne on met la variable de retoure à vrai
            if( RequeteSupprimer.exec() )
            {
                bRetourne = true ;
            }
            else//Sinon on affiche un message d'erreur et on met la variable de retoure à faux
            {
                cerr << "F_AdministrerMembres::SupprimerMembre : RequeteSupprimer : Erreur de connexion avec la base de donne !" << RequeteSupprimer.lastError().text().toStdString()<< endl ;
                bRetourne = false ;
            }

            //Préparation de la requête permettant la suppression dans la table reservation -------------------------------
            RequeteSupprimer.prepare( "DELETE FROM reservation WHERE Membres_IdMembre=:IdMembre " ) ;
            RequeteSupprimer.bindValue( ":IdMembre", nIdMembre ) ;

            //Execution de la requête, si elle fonctionne on met la variable de retoure à vrai
            if( RequeteSupprimer.exec() )
            {
                bRetourne = true ;
            }
            else//Sinon on affiche un message d'erreur et on met la variable de retoure à faux
            {
                cerr << "F_AdministrerMembres::SupprimerMembre : RequeteSupprimer : Erreur de connexion avec la base de donne !" << RequeteSupprimer.lastError().text().toStdString()<< endl ;
                bRetourne = false ;
            }

            //Préparation de la requête permettant la suppression dans la table emprunts ------------------------
            RequeteSupprimer.prepare( "DELETE FROM emprunts WHERE Membres_IdMembre=:IdMembre " ) ;
            RequeteSupprimer.bindValue( ":IdMembre", nIdMembre ) ;

            //Execution de la requête, si elle fonctionne on met la variable de retoure à vrai
            if( RequeteSupprimer.exec() )
            {
                bRetourne = true ;
            }
            else//Sinon on affiche un message d'erreur et on met la variable de retoure à faux
            {
                cerr << "F_AdministrerMembres::SupprimerMembre : RequeteSupprimer : Erreur de connexion avec la base de donne !" << RequeteSupprimer.lastError().text().toStdString()<< endl ;
                bRetourne = false ;
            }


            //Préparation de la requête permettant la suppression dans la table abonnements-----------------------
            RequeteSupprimer.prepare( "DELETE FROM abonnements WHERE Membres_IdMembre=:IdMembre " ) ;
            RequeteSupprimer.bindValue( ":IdMembre", nIdMembre ) ;

            //Execution de la requête, si elle fonctionne on met la variable de retoure à vrai
            if( RequeteSupprimer.exec() )
            {
                bRetourne = true ;
            }
            else//Sinon on affiche un message d'erreur et on met la variable de retoure à faux
            {
                cerr << "F_AdministrerMembres::SupprimerMembre : RequeteSupprimer : Erreur de connexion avec la base de donne !" << RequeteSupprimer.lastError().text().toStdString()<< endl ;
                bRetourne = false ;
            }

        }
        else
        {
            bRetourne = false ;
        }

    }
    else
    {
        bRetourne = false ;
        QMessageBox::information( this, "Suppression Membre","Impossible de supprimer ce membre.\nIl a encore des jeux en cours d'emprunts.",  "Ok" ) ;
    }


    this->pRechercheMembres->MaJListeMembres() ;
    this->pRechercheMembres->AfficherListe() ;

    return bRetourne ;
}

/** Verrouille ou deverouille les champs contenant les informations personnelles
 *  @param  bool bVerrouille
 *  @test   Voir la procédure dans le fichier associé.
 */
void F_Membres::VerrouillerInfosPerso ( bool bVerrouille )
{
    ui->CBx_Type->setDisabled( bVerrouille ) ;
    ui->CBx_Titre->setDisabled( bVerrouille ) ;


    ui->Le_Nom->setReadOnly( bVerrouille ) ;
    ui->Le_Prenom->setReadOnly( bVerrouille ) ;
    ui->Le_TelFix->setReadOnly( bVerrouille ) ;
    ui->Le_TelMobile->setReadOnly( bVerrouille ) ;
    ui->LE_Fax->setReadOnly( bVerrouille ) ;
    ui->LE_Email->setReadOnly( bVerrouille ) ;
    ui->Le_Code->setReadOnly( bVerrouille ) ;
    ui->LE_MembreAssocie->setReadOnly( bVerrouille ) ;
    ui->Te_Rue->setReadOnly( bVerrouille ) ;
    ui->Le_CP->setReadOnly( bVerrouille ) ;
    ui->CBx_Ville->setDisabled( bVerrouille ) ;
    ui->SBx_JeuxAutorises->setReadOnly( bVerrouille ) ;
    ui->SPx_NbreRetards->setReadOnly( bVerrouille ) ;
    ui->TE_Remarque->setReadOnly( bVerrouille ) ;
    ui->DtE_Insritption->setReadOnly( bVerrouille ) ;
    ui->DtE_Naissance->setReadOnly( bVerrouille ) ;
    ui->ChBx_MembreEcarte->setVisible( !bVerrouille ) ;
}

/** Efface le contenu de tous les champs
 *  @test   Voir la procédure dans le fichier associé.
 */
void F_Membres::EffacerTousLesChamps ()
{
    ui->CBx_Type->setCurrentIndex( 0 ) ;
    ui->CBx_Titre->setCurrentIndex( 0 ) ;

    ui->Le_Nom->clear() ;
    ui->Le_Prenom->clear() ;
    ui->Le_TelFix->clear() ;
    ui->Le_TelMobile->clear() ;
    ui->LE_Fax->clear() ;
    ui->LE_Email->clear() ;
    ui->Le_Code->clear() ;
    ui->LE_MembreAssocie->clear() ;
    ui->Te_Rue->clear() ;
    ui->Le_CP->clear() ;
    ui->CBx_Ville->setCurrentIndex( 0 ) ;
    ui->SBx_JeuxAutorises->clear() ;
    ui->SPx_NbreRetards->clear() ;
    ui->DtE_Insritption->clear() ;
    ui->DtE_Naissance->clear() ;
    ui->TE_Remarque->clear() ;
    ui->LW_JeuxEmpruntes->clearSpans() ;
    ui->SBx_JeuxAutorises->clear() ;

    ui->Lb_MembreEcarte->setHidden( true ) ;
}

/** Affiche ou cache les boutons PB_AjouterMembre et PB_ModifierMembre
 *  @test   Voir la procédure dans le fichier associé.
 */
void F_Membres::AfficherAjouterModifierMembre ( bool bAffiche )
{
    ui->Bt_AjouterMembre->setVisible( bAffiche ) ;
    ui->Bt_ModifierMembre->setVisible( bAffiche ) ;
    if ( bAdmin == true )
    {
        ui->Bt_SupprimerMembre->setVisible( bAffiche ) ;
    }

}

/** Affiche ou cache les boutons PB_Valider et PB_Annuler
 *  @test   Voir la procédure dans le fichier associé.
 */
void F_Membres::AfficherValiderAnnuler ( bool bAffiche )
{
    ui->Bt_Valider->setVisible( bAffiche ) ;
    ui->Bt_Annuler->setVisible( bAffiche ) ;
    ui->ChBx_MembreEcarte->setVisible( bAffiche ) ;
    ui->Lb_MembreEcarte->setVisible( bAffiche ) ;
}

/** Verrouille ou deverouille l'emplacement des emprunts
 *  @test   Voir la procédure dans le fichier associé.
 */
void F_Membres::VerrouillerJeux ( bool bVerrouille )
{
    ui->LW_JeuxEmpruntes->setDisabled( bVerrouille ) ;
    ui->Bt_Historique->setDisabled( bVerrouille ) ;
}

/** Verouille ou deverouille l'emplacement des abonnements
 *  @test   Voir la procédure dans le fichier associé.
 */
void F_Membres::VerrouillerAbonnements ( bool bVerrouille )
{
    ui->LW_Abonnements->setDisabled( bVerrouille ) ;
    ui->Bt_Ajouter->setDisabled( bVerrouille ) ;
    ui->Bt_Modifier->setDisabled( bVerrouille ) ;
    ui->Bt_Supprimer->setDisabled( bVerrouille ) ;
}

/** Permet l'affichage d'un membre
 *  @pre    Connexion à la base de données, Membre présent dans la base de données
 *  @param  Indique l'id du membre pour afficher ses abonnements
 *  @test   Voir la procédure dans le fichier associé.
 */
void F_Membres::AfficherAbonnements( int nIdMembre )
{
    QSqlQuery RequetePrestation ;
    QSqlQuery RequeteCartes ;
    int i     ( 0 )        ;

    //Création et initialisation du Model
    QStandardItemModel * modele = new QStandardItemModel() ;

    this->VecteurAbonnements.clear() ;


    /*Cration des caractristiques du tableau : -Nombre de colonnes
                                               -Nom des colonnes*/
    modele->setColumnCount( 3 ) ;
    modele->setHorizontalHeaderItem( 0, new QStandardItem( "Abonnement" ) ) ;
    modele->setHorizontalHeaderItem( 1, new QStandardItem( "Crédit" ) ) ;
    modele->setHorizontalHeaderItem( 2, new QStandardItem( "Date" ) ) ;



    ui->LW_Abonnements->setModel( modele ) ;
    ui->LW_Abonnements->setEditTriggers( 0 ) ;
    ui->LW_Abonnements->verticalHeader()->setVisible( false ) ;
    ui->LW_Abonnements->horizontalHeader()->setVisible( true ) ;
    ui->LW_Abonnements->setColumnWidth( 0, 138 ) ;
    ui->LW_Abonnements->setColumnWidth( 1, 50 ) ;
    ui->LW_Abonnements->setColumnWidth( 2, 73 ) ;

    //Préparation de la requête
    RequetePrestation.prepare( "SELECT prestations.NomPrestation, abonnements.DateExpiration, abonnements.IdAbonnements FROM abonnements, prestations "
                               "WHERE abonnements.Membres_IdMembre=:IdMembre AND prestations.IdPrestation=abonnements.Prestations_IdPrestation  " ) ;
    RequetePrestation.bindValue( ":IdMembre", nIdMembre ) ;

    //Exectution de la requète
    if( RequetePrestation.exec() )
    {
        while( RequetePrestation.next() )
        {
            modele->setItem( i, 0, new QStandardItem( RequetePrestation.record().value( 0 ).toString() ) ) ;
            modele->setItem( i, 2, new QStandardItem( RequetePrestation.record().value( 1 ).toDateTime().toString( "dd.MM.yyyy" ) ) ) ;
            this->VecteurAbonnements.append( RequetePrestation.record().value( 2 ).toInt() ) ;

            if ( RequetePrestation.value( 1 ).toDate()< QDate::currentDate() )
            {
                modele->setData( modele->index( i, 2 ),QColor( Qt::red ), Qt::BackgroundColorRole ) ;
            }
            else
            {
                if( RequetePrestation.value( 1 ).toDate()< QDate::currentDate().addDays( 14 ) )
                {
                    modele->setData( modele->index( i, 2 ),QColor( Qt::yellow ), Qt::BackgroundColorRole ) ;
                }
                else
                {
                    modele->setData( modele->index( i, 2 ),QColor( Qt::green ), Qt::BackgroundColorRole ) ;
                }
            }

            i++ ;
        }


    }
    else //Sinon on affiche un message d'erreur et on retourne Faux
    {
        cerr << "F_Membres::AfficherAbonnements : RequetePrestation : Erreur de connexion avec la base de donne !"<< RequetePrestation.lastError().text().toStdString()<< endl ;
    }

    //Préparation de la requête
    RequeteCartes.prepare( "SELECT cartesprepayees.NomCarte, abonnements.DateExpiration, abonnements.CreditRestant, abonnements.IdAbonnements FROM abonnements, cartesprepayees "
                           "WHERE abonnements.Membres_IdMembre=:IdMembre AND cartesprepayees.IdCarte=abonnements.CartesPrepayees_IdCarte  " ) ;
    RequeteCartes.bindValue( ":IdMembre", nIdMembre ) ;

    //Exectution de la requète
    if( RequeteCartes.exec() )
    {
        while( RequeteCartes.next() )
        {
            modele->setItem( i, 0, new QStandardItem( RequeteCartes.record().value( 0 ).toString() ) ) ;
            modele->setItem( i, 1, new QStandardItem( RequeteCartes.record().value( 2 ).toString() ) ) ;
            modele->setItem( i, 2, new QStandardItem( RequeteCartes.record().value( 1 ).toDateTime().toString( "dd.MM.yyyy" ) ) ) ;
            this->VecteurAbonnements.append( RequeteCartes.record().value( 3 ).toInt() ) ;

            if ( RequeteCartes.value( 1 ).toDate()< QDate::currentDate() )
            {
                modele->setData( modele->index( i, 2 ),QColor( Qt::red ), Qt::BackgroundColorRole ) ;
            }
            else
            {
                if( RequeteCartes.value( 1 ).toDate()< QDate::currentDate().addDays( 14 ) )
                {
                    modele->setData( modele->index( i, 2 ),QColor( Qt::yellow ), Qt::BackgroundColorRole ) ;
                }
                else
                {
                    modele->setData( modele->index( i, 2 ),QColor( Qt::green ), Qt::BackgroundColorRole ) ;
                }
            }

            i++ ;
        }
    }
    else //Sinon on affiche un message d'erreur et on retourne Faux
    {
        cerr << "F_Membres::AfficherAbonnements : RequeteCartes : Erreur de connexion avec la base de donne !"<< RequeteCartes.lastError().text().toStdString()<< endl ;
    }

}

/** Affiche les ville dans le combobox et selectionne la ville passé en paramétre
 *  @param  QString VilleSelectionne
 *  @test   Voir la procédure dans le fichier associé.
 */
void F_Membres::AfficherVilles( QString VilleSelectionne )
{
    QSqlQuery Requete ;
    int i ( 1 ) ;

    ui->CBx_Ville->clear() ;

    if( Requete.exec( "SELECT DISTINCT Ville FROM membres ORDER BY Ville ASC" ) )
    {
        ui->CBx_Ville->insertItem(0, "");
        while( Requete.next() )
        {
            if( Requete.record().value( 0 ).toString() != "" )
            {
                ui->CBx_Ville->insertItem( i, Requete.record().value( 0 ).toString()) ;
                i++ ;
            }
        }
        ui->CBx_Ville->insertItem( i, "Ajouter une ville ..." ) ;
    }
    else
    {
        cerr << "F_Membres::AfficherVilles : Erreur de connexion avec la base de donnée !"<< Requete.lastError().text().toStdString() << endl ;
    }

    i = 0 ;
    while (ui->CBx_Ville->itemText( i ) != VilleSelectionne && ( i ) < ui->CBx_Ville->count() )
    {
        i++ ;
    }

    if( i != ui->CBx_Ville->count() )
    {
        ui->CBx_Ville->setCurrentIndex( i ) ;
    }
    else
    {
        ui->CBx_Ville->setCurrentIndex( 0 ) ;
    }
}

//---------------------------------------------------------------------------
// SLOTS PUBLIQUES
//---------------------------------------------------------------------------

/** Permet l'ajout d'un type
 *  @test   Voir la procédure dans le fichier associé.
 */
void F_Membres::slot_ChoisirNouveauType ()
{
    this->MaJType() ;
    ui->CBx_Type->setCurrentIndex( this->VectorType.size()- 2 ) ;
}

/** Permet l'ajout d'un titre
 *  @test   Voir la procédure dans le fichier associé.
 */
void F_Membres::slot_ChoisirNouveauTitre ()
{
    this->MaJTitre() ;
    ui->CBx_Titre->setCurrentIndex( this->VectorTitre.size()- 2 ) ;
}

/** Recupere l'id d'un membre pour l'afficher
 *  @pre    Recevoir un signal avec l'id d'un membre
 *  @param  unsigned int nIdMembre
 *  @test   Voir la procédure dans le fichier associé.
 */
void F_Membres::slot_AfficherMembre( unsigned int nIdMembre )
{
    this->VerrouillerAbonnements( false ) ;
    this->VerrouillerJeux( false ) ;
    this->AfficherMembre( nIdMembre ) ;
}

/** Acualise les abonnements du membre sélectionné
 *  @pre    Recevoir un signal avec l'id d'un membre
 *  @param  unsigned int nIdMembre
 *  @test   Voir la procédure dans le fichier associé.
 */
void F_Membres::slot_ActualiserAbonnements()
{
    this->AfficherAbonnements( this->nIdMembreSelectionne ) ;
}

/** Permet l'ajout d'une ville
 *  @test   Voir la procédure dans le fichier associé.
 */
void F_Membres::slot_ValiderAjoutVille()
{
    ui->CBx_Ville->insertItem( ui->CBx_Ville->count(), this->LE_AjoutVille->text() ) ;
    ui->CBx_Ville->setCurrentIndex( ui->CBx_Ville->count() - 1 );
    this->LE_AjoutVille->clear() ;
    this->oFenetreAjoutVille->hide() ;
}

/** Permet l'annulation de l'ajour d'une ville
 *  @test   Voir la procédure dans le fichier associé.
 */
void F_Membres::slot_AnnulerAjoutVille()
{
    this->LE_AjoutVille->clear() ;
    this->oFenetreAjoutVille->hide() ;
}

//---------------------------------------------------------------------------
// SLOTS PRIVEES
//---------------------------------------------------------------------------
/** Permet l'ajout d'un membre
 *  @test   Voir la procédure dans le fichier associé.
 */
void F_Membres::on_Bt_AjouterMembre_clicked()
{
    QString sNombre      ;
    QDate   DateActuelle ;
    QStandardItemModel * modeleVide ;

    this->nIdMembreSelectionne = 0 ;

    this->AfficherVilles( "" ) ;

    modeleVide = new QStandardItemModel() ;
    ui->LW_JeuxEmpruntes->setModel( modeleVide ) ;

    ui->LW_Abonnements->setModel( modeleVide ) ;

    this->pRechercheMembres->setDisabled( true ) ;

    this->AfficherAjouterModifierMembre( false ) ;

    this->EffacerTousLesChamps() ;

    this->AfficherValiderAnnuler( true ) ;

    this->VerrouillerInfosPerso( false ) ;

    this->VerrouillerJeux( true ) ;

    this->VerrouillerAbonnements( false ) ;

    /*
    // Ancien code qui ne marche pas
    sNombre.setNum( this->pRechercheMembres->RecupererProchainCodeNonUtilise() ) ;
    ui->Le_Code->setText( sNombre ) ;
    */
    //Sélectionne le premier CodeMembre qui est libre (supérieur au plus grand)
    QSqlQuery RequetePremierCodeLibre ;
    RequetePremierCodeLibre.exec("SELECT MIN(CodeMembre+1) FROM membres WHERE (CodeMembre+1) NOT IN (SELECT CodeMembre FROM membres)") ;
    RequetePremierCodeLibre.next() ;
    //ui->LE_Code->setText(CodeJeu);
    ui->Le_Code->setText( RequetePremierCodeLibre.value(0).toString()) ;

    ui->DtE_Insritption->setDate( DateActuelle.currentDate() ) ;

    ui->Bt_Valider->setDisabled( true ) ;

    ui->Bt_Valider->setText( "Valider ce nouveau membre" ) ;

    ui->Lb_MembreEcarte->hide() ;

    ui->ChBx_MembreEcarte->hide() ;

    ui->SPx_NbreRetards->setDisabled( true ) ;

    ui->DtE_Naissance->setDate( QDate( 1900, 1, 1 ) ) ;
    ui->Bt_Ajouter->setDisabled( true ) ;
    ui->Bt_Modifier->setDisabled( true ) ;
    ui->Bt_Supprimer->setDisabled( true ) ;
}


/** Pas utilise
 *  @test   Voir la procédure dans le fichier associé.
 */
void F_Membres::on_CBx_Type_highlighted( int index )
{
}

/** Annule l'ajout ou la modification d'un membre
 *  @test   Voir la procédure dans le fichier associé.
 */
void F_Membres::on_Bt_Annuler_clicked()
{
    this->pRechercheMembres->setDisabled( false ) ;
    ui->LW_JeuxEmpruntes->setDisabled( true ) ;
    ui->Bt_Historique->setDisabled( true ) ;

    this->AfficherAjouterModifierMembre( true ) ;

    this->AfficherValiderAnnuler( false ) ;

    this->EffacerTousLesChamps() ;

    this->VerrouillerInfosPerso( true ) ;

    this->VerrouillerAbonnements( true ) ;

    this->VerrouillerJeux( true ) ;

    ui->Bt_ModifierMembre->setDisabled( true ) ;

    this->AfficherMembre( this->nIdMembreSelectionne ) ;

    ui->Bt_Valider->setEnabled( true ) ;

    ui->LW_Abonnements->setModel( NULL ) ;
    ui->LW_JeuxEmpruntes->setModel( NULL ) ;
}

/** Valide la modification ou l'ajout d'un membre
 *  @test   Voir la procédure dans le fichier associé.
 */
void F_Membres::on_Bt_Valider_clicked()
{
    QSqlQuery RequeteCodeMembre ;

    RequeteCodeMembre.prepare( "SELECT IdMembre FROM membres "
                               "WHERE CodeMembre=:CodeMembre " ) ;
    RequeteCodeMembre.bindValue( ":CodeMembre", ui->Le_Code->text().toInt() ) ;
    qDebug() << ui->Le_Code->text().toInt();

    if( RequeteCodeMembre.exec() )
    {
        if( this->nIdMembreSelectionne == 0 )
        {
            if( !RequeteCodeMembre.next() )
            {
                if ( this->AjouterMembre() )
                {
                    this->pRechercheMembres->MaJListeMembres() ;
                    this->pRechercheMembres->AfficherListe() ;
                    this->pRechercheMembres->setEnabled( true ) ;
                    this->VerrouillerAbonnements( false ) ;
                    this->VerrouillerJeux( false ) ;
                    this->VerrouillerInfosPerso( true ) ;
                    this->AfficherValiderAnnuler( false ) ;
                    this->AfficherAjouterModifierMembre( true ) ;
                    ui->Bt_ModifierMembre->setEnabled( true ) ;
                    this->AfficherMembre( this->nIdMembreSelectionne ) ;
                }
            }
            else
            {
                QMessageBox::information( this, "Code Membre","Le code membre que vous avez choisi existe déjà !", "Ok" ) ;
            }
        }
        else
        {
            if( !RequeteCodeMembre.next() )
            {

                this->ModifierMembre( this->nIdMembreSelectionne ) ;
                this->pRechercheMembres->MaJListeMembres() ;
                this->pRechercheMembres->AfficherListe() ;
                this->pRechercheMembres->setEnabled( true ) ;
                this->VerrouillerInfosPerso( true ) ;
                this->AfficherValiderAnnuler( false ) ;
                this->AfficherAjouterModifierMembre( true ) ;
                this->AfficherMembre( this->nIdMembreSelectionne ) ;                
            }
            else
            {
                if( RequeteCodeMembre.record().value(0).toInt() == this->nIdMembreSelectionne )
                {
                    this->ModifierMembre( this->nIdMembreSelectionne ) ;
                    this->pRechercheMembres->MaJListeMembres() ;
                    this->pRechercheMembres->AfficherListe() ;
                    this->pRechercheMembres->setEnabled( true ) ;
                    this->VerrouillerInfosPerso( true ) ;
                    this->AfficherValiderAnnuler( false ) ;
                    this->AfficherAjouterModifierMembre( true ) ;
                    this->AfficherMembre( this->nIdMembreSelectionne ) ;
                }
                else
                {
                    QMessageBox::information( this, "Code Membre","Le code membre que vous avez choisi existe déjà !", "Ok" ) ;
                }
            }
        }
    }
    else
    {
        qDebug() << "F_Membres::AjouterMembre() : RequeteCodeMembre :" << RequeteCodeMembre.lastError().text()  ;
    }
}


/** Met en forme (du type "00 00 00 00 00")  le numéro de télephone quand il est modifié
 *  @test   Voir la procédure dans le fichier associé.
 */
void F_Membres::on_Le_TelFix_textEdited( const QString &arg1 )
{
    ui->Le_TelFix->setText( this->ModifierSyntaxeNumTelephone( arg1 ) ) ;
}

/** Met en forme (du type "00 00 00 00 00")  le numéro de télephone quand il est modifié
 *  @test   Voir la procédure dans le fichier associé.
 */
void F_Membres::on_Le_TelMobile_textEdited( const QString &arg1 )
{
    ui->Le_TelMobile->setText( this->ModifierSyntaxeNumTelephone( arg1 ) ) ;
}

/** Met en forme (du type "00 00 00 00 00")  le numéro de télephone quand il est modifié
 *  @test   Voir la procédure dans le fichier associé.
 */
void F_Membres::on_LE_Fax_textEdited( const QString &arg1 )
{
    ui->LE_Fax->setText( this->ModifierSyntaxeNumTelephone( arg1 ) ) ;
}

/** Affiche l'historique des jeux empruntés
 *  @test   Voir la procédure dans le fichier associé.
 */
void F_Membres::on_Bt_Historique_clicked()
{
    this->pHistoriqueJeux->AfficherHistorique( this->nIdMembreSelectionne ) ;
    this->pHistoriqueJeux->show() ;

}

/** Affiche ou cache membre écarté
 *  @test   Voir la procédure dans le fichier associé.
 */
void F_Membres::on_ChBx_MembreEcarte_toggled( bool checked )
{
    if( checked == true )
    {
        ui->ChBx_MembreEcarte->setPalette( QPalette( Qt::red ) ) ;
    }
    else
    {
        ui->ChBx_MembreEcarte->setPalette( QPalette( Qt::black ) ) ;
    }
}

/** Non utilisé
 *  @test   Voir la procédure dans le fichier associé.
 */
void F_Membres::on_Lb_MembreEcarte_linkActivated( const QString &link )
{

}

/** Permet de modifier un membre
 *  @test   Voir la procédure dans le fichier associé.
 */
void F_Membres::on_Bt_ModifierMembre_clicked()
{
    this->VerrouillerInfosPerso( false ) ;
    this->AfficherValiderAnnuler( true ) ;
    this->AfficherAjouterModifierMembre( false ) ;
    ui->Bt_Valider->setText( "Valider les Modifications" ) ;
    this->pRechercheMembres->setDisabled( true ) ;
}

/** Active ou désactive le bouton valider en fonction de lineedit nom
 *  @test   Voir la procédure dans le fichier associé.
 */
void F_Membres::on_Le_Nom_textEdited( const QString &arg1 )
{
    if( arg1 == "" )
    {
        ui->Bt_Valider->setDisabled( true ) ;
    }
    else
    {
        ui->Bt_Valider->setEnabled( true ) ;
    }
}

/** Validation suppression d'un membre
 *  @test   Voir la procédure dans le fichier associé.
 */
void F_Membres::on_Bt_SupprimerMembre_clicked()
{
    QStandardItemModel * modeleVide ;

    if( this->SupprimerMembre( this->nIdMembreSelectionne ) )
    {
        this->pRechercheMembres->MaJListeMembres() ;
        this->pRechercheMembres->AfficherListe() ;
        this->AfficherMembre( 0 ) ;
        ui->Bt_SupprimerMembre->setDisabled( true ) ;
        ui->LW_Abonnements->clearSpans() ;
        ui->LW_JeuxEmpruntes->clearSpans() ;

        modeleVide = new QStandardItemModel() ;
        ui->LW_JeuxEmpruntes->setModel( modeleVide ) ;

        ui->LW_Abonnements->setModel( modeleVide ) ;
    }
}

/** Permet l'ajout d'un abonnement
 *  @test   Voir la procédure dans le fichier associé.
 */
void F_Membres::on_Bt_Ajouter_clicked()
{
    this->pAjouterCotiCarte->MaJListeAbonnements() ;
    this->pAjouterCotiCarte->AjouterMembre( this->nIdMembreSelectionne ) ;
}

/** Selectionne une ligne entière dans le tableau LW_Abonnememnt
 *  @test   Voir la procédure dans le fichier associé.
 */
void F_Membres::on_LW_Abonnements_clicked( const QModelIndex &index )
{
    ui->Bt_Modifier->setEnabled( true ) ;
    ui->Bt_Supprimer->setEnabled( true ) ;
}

/** Selectionne une ligne entière dans le tableau LW_Empruntes
 *  @test   Voir la procédure dans le fichier associé.
 */
void F_Membres::on_LW_JeuxEmpruntes_clicked( const QModelIndex &index )
{
}

/** Permet la modifiaction d'un abonnement
 *  @test   Voir la procédure dans le fichier associé.
 */
void F_Membres::on_Bt_Modifier_clicked()
{
    this->pAjouterCotiCarte->MaJListeAbonnements() ;
    this->pAjouterCotiCarte->ModifierMembre( this->VecteurAbonnements.at( ui->LW_Abonnements->currentIndex().row() ) ) ;
}

/**  Permet de supprimer un abonnement
 *  @test   Voir la procédure dans le fichier associé.
 */
void F_Membres::on_Bt_Supprimer_clicked()
{
    QSqlQuery RequeteSupprimer ;

    if ( QMessageBox::information( this, "Suppression Abonnement","Voulez vraiment supprimer cet abonnement?", "Supprimer", "Annuler" )== 0 )
    {
        //Préparation de la requête permettant la suppression dans la table abonnements-----------------------
        RequeteSupprimer.prepare( "DELETE FROM abonnements WHERE IdAbonnements=:nIdAbonnement " ) ;
        RequeteSupprimer.bindValue( ":nIdAbonnement", this->VecteurAbonnements.at( ui->LW_Abonnements->currentIndex().row() ) ) ;

        //Execution de la requête, si elle fonctionne on met la variable de retoure à vrai
        if( !RequeteSupprimer.exec() )
        {
            cerr << "F_AdministrerMembres::SupprimerMembre : RequeteSupprimer : Erreur de connexion avec la base de donne !" << RequeteSupprimer.lastError().text().toStdString()<< endl ;
        }
        this->AfficherAbonnements( this->nIdMembreSelectionne ) ;

        ui->Bt_Modifier->setDisabled( true ) ;
        ui->Bt_Supprimer->setDisabled( true ) ;
    }
}

/**  Change de la couleur et met en gras membre écarter quand le checkbox et check
 *  @test   Voir la procédure dans le fichier associé.
 */
void F_Membres::on_ChBx_MembreEcarte_stateChanged( int arg1 )
{
    QFont font ;
    QPalette palette ;

    if( arg1 == 0 )
    {
        font.setBold( false ) ;
        ui->Lb_MembreEcarte->setFont( font ) ;
        palette.setColor( QPalette::WindowText, Qt::black ) ;
        ui->Lb_MembreEcarte->setPalette( palette ) ;
    }
    else
    {
        font.setBold( true ) ;
        ui->Lb_MembreEcarte->setFont( font ) ;
        palette.setColor( QPalette::WindowText, Qt::red ) ;
        ui->Lb_MembreEcarte->setPalette( palette ) ;
    }
}

/**  Permet l'ajout de Ajouter une ville dans le combox Ville
 *  @test   Voir la procédure dans le fichier associé.
 */
void F_Membres::on_CBx_Ville_currentIndexChanged(const QString &arg1)
{
    if( arg1 == "Ajouter une ville ..." )
    {
        this->oFenetreAjoutVille->show() ;
    }
}

/** Permet d'ajout un nouveau type si "Créer type..." a été sélectionné
 *  @pre    l'utilisateur a choisi Créer type...
 *  @post   Ouverture d'un popup pour l'ajout d'un type
 *  @param  int index
 *  @test
 */
void F_Membres::on_CBx_Type_activated(int index)
{
    if( ( this->VectorType.size() - 1 )== index )
    {
        this->pTypeAjMod->Ajouter() ;
        ui->CBx_Type->setCurrentIndex( 0 ) ;
    }
}

/** Permet d'ajout un nouveau titre si "Créer titre..." a été sélectionné
 *  @pre    l'utilisateur a choisi Créer titre...
 *  @post   Ouverture d'un popup pour l'ajout d'un titre
 *  @param  int index
 *  @test
 */
void F_Membres::on_CBx_Titre_activated(int index)
{
    if( ( this->VectorTitre.size() - 1 ) == index )
    {
        this->pTitreAjMod->Ajouter() ;
        ui->CBx_Titre->setCurrentIndex( 0 ) ;
    }
    else
    {
        ui->SBx_JeuxAutorises->setValue( this->VectorTitre.at( index ).nJeuxAutorises ) ;
    }
}
