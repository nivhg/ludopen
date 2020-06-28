//------------------------------------------------------------------------------
/** @file         F_AdministrerMembres.cpp
 *  @brief        Permet l'administration des membres
 *
 *  @author       Padiou Nicolas
 *  @author       STS IRIS, Lyce Nicolas APPERT, ORVAULT (FRANCE)
 *  @since        26/04/2012
 *  @version      0.1
 *  @date         26/04/2016
 *
 *  Permet la consultation d'un membre
 *  Permet l'ajout d'un membre
 *  Permet la modification d'un membre
 *  Permet la suppression d'un membre
 *
 *  Fabrication   DevCPP, projet   .dev
 *
 *  @todo         Ajouter, supprimer, consulter et modifier
 *
 *  @bug          aucun (pour l'instant)
 */
//------------------------------------------------------------------------------
// En-ttes standards (ATTENTION : garder toujours le meme ordre) --------------
//------------------------------------------------------------------------------
// En-tte propre  l'application ----------------------------------------------
#include <QMessageBox>
// En-tte propre  l'objet ----------------------------------------------------
//------------------------------------------------------------------------------
#include "f_administrermembres.h"
#include "ui_f_administrermembres.h"


using namespace std ;

/** CONSTRUCTEURs, Initialisation de la classe
 *  @pre    Description des prconditions ncessaires  la mthode
 *  @param  Pointe sur l'objet F_RechercheMembre
 *  @test   Voir la procdure dans le fichier associ.
 */
F_AdministrerMembres::F_AdministrerMembres(F_RechercheMembres *pRechercheMembres, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::F_AdministrerMembres)
{
    ui->setupUi(this);

    this->pRechercheMembres = pRechercheMembres ;

    this->nIdMembreSelectionne = 0 ;

    this->pAjouterTitre = new F_AjouterTitreType (0) ;
    this->pAjouterType = new F_AjouterTitreType (1) ;

}

/**  DESTRUCTEURs
 *  @test   Voir la procdure dans le fichier associ.
 */
F_AdministrerMembres::~F_AdministrerMembres()
{
    delete ui;
}

//---------------------------------------------------------------------------
// METHODEs PUBLIQUEs
//---------------------------------------------------------------------------



/** Description dtaille de la mthode
 *  @pre    Accés à la base de données
 *  @test   Voir la procdure dans le fichier associ.
 */
void F_AdministrerMembres::MaJTitre ()
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
        cerr << "F_AdministrerMembres::MaJTitre () : Erreur de connexion avec la base de donne !" << endl ;
    }
}

/** Description dtaille de la mthode
 *  @pre    Accés à la base de données
 *  @test   Voir la procdure dans le fichier associ.
 */
void F_AdministrerMembres::MaJType ()
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

        //Création d'un choix permettant de créer un nouveau titre
        oType.id = 0 ;
        oType.sType = "Crer Type ..." ;

        this->VectorType.push_back( oType ) ;


        //Remplissage de la combobox grace au vecteur
        for( i = 0 ; i < VectorType.size() ; i ++ )
        {
            ui->CBx_Type->insertItem( i, VectorType[ i ].sType ) ;
        }
    }
    else //Sinon on affiche un message d'erreur et on retourne Faux
    {
        cerr << "F_AdministrerMembres::MaJType () : Erreur de connexion avec la base de donne !" << endl ;
    }
}

/** Description dtaille de la mthode
 *  @pre    Combobox remplie avec le vecteur
 *  @param  unsigned int nIdTitre
 *  @retval int
 *  @return L'emplacement du titre dans le vecteur par rapport à l'id (0 par défaut)
 *  @test   Voir la procdure dans le fichier associ.
 */
int F_AdministrerMembres::EmplacementTitreVector( unsigned int nIdTitre )
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
 *  @return L'emplacement du titre dans le vecteur par rapport à l'id (0 par défaut)
 *  @test   Voir la procdure dans le fichier associ.
 */
int F_AdministrerMembres::EmplacementTypeVector(unsigned int nIdType )
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
 *  @param QString sNumero
 *  @retval QString
 *  @return Retourne un QString ne contenant plus de lettre ou caractères speciaux. Un espace est ajouté tous les 2 caractéres
 *  @test   Voir la procdure dans le fichier associ.
 */
 QString F_AdministrerMembres::SyntaxeNumTelephone ( QString sNumero)
 {
     QChar cEspace (' ') ;
     QRegExp Nombres ("[^0-9]") ;

     //Suppression des les caractéres ormis les chiffres
     sNumero.replace(Nombres, "") ;

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

 /** Description dtaille de la mthode
  *  @pre    Accés à la base de données
  *  @param  unsigned int nIdMembre
  *  @test   Voir la procdure dans le fichier associ.
  */
 void F_AdministrerMembres::AfficherJeuxEmpruntes(unsigned int nIdMembre)
 {
     QSqlQuery queryEmprunt ;
     QSqlQuery queryJeux    ;
     int i     (0)          ;

     //Création et initialisation du Model
     QStandardItemModel * modele = new QStandardItemModel() ;
     ui->LW_JeuxEmpruntes->setModel(modele) ;
     ui->LW_JeuxEmpruntes->setEditTriggers(0) ;

     //requête permettant d'avoir les jeux empruntés correspondant à l'id du membre
     queryEmprunt.prepare( "SELECT * FROM emprunts WHERE Membres_IdMembre=:nIdMembre ;" ) ;
     queryEmprunt.bindValue( "nIdMembre", nIdMembre ) ;

     //Exectution de la requête
     if( queryEmprunt.exec() )
     {

         /*Cration des caractristiques du tableau : -Nombre de colonnes
                                                      -Nom des colonnes
                                                      -Nombre de lignes*/
         modele->setColumnCount( 3 ) ;
         modele->setRowCount( queryEmprunt.size() ) ;
         modele->setHorizontalHeaderItem( 0, new QStandardItem("Jeux") ) ;
         modele->setHorizontalHeaderItem( 1, new QStandardItem( "Emprunt" ) ) ;
         modele->setHorizontalHeaderItem( 2, new QStandardItem( "Retour prévu" ) ) ;


        //Remplissage du tableau avec les informations de la table emprunts
         while( queryEmprunt.next() )
         {

             //Requete pour rcuperer le nom du jeux
             queryJeux.prepare( "SELECT * FROM jeux WHERE IdJeux=:nIdJeux ;" ) ;
             queryJeux.bindValue( "nIdJeux", queryEmprunt.record().value( 2 ).toInt() ) ;

             if( queryJeux.exec())
             {
                 queryJeux.next() ;
                 modele->setItem( i, 0, new QStandardItem( queryJeux.record().value( 5 ).toString() ) ) ;
             }
             else
             {
                 cerr << "F_AdministrerMembres::AfficherJeuxEmpruntes : queryJeux : Erreur de connexion avec la base de donne !" << endl ;
             }

             modele->setItem( i, 1, new QStandardItem( queryEmprunt.record().value( 6 ).toDateTime().toString( "dd.MM.yy" ) ) ) ;
             modele->setItem( i, 2, new QStandardItem( queryEmprunt.record().value( 7 ).toDateTime().toString( "dd.MM.yy" ) ) ) ;
             i++ ;
         }
     }
     else //Sinon on affiche un message d'erreur et on retourne Faux
     {
         cerr << "F_AdministrerMembres::AfficherJeuxEmpruntes : queryEmprunt : Erreur de connexion avec la base de donne !" << endl ;
     }

     //Suppression du contenu pointé par modele
    /* if ( modele )
     {
         delete modele ;
         modele = NULL ;
     }*/
 }

 /** Description dtaille de la mthode
  *  @pre    Accés à la base de données
  *  @param  unsigned int nIdMembre
  *  @test   Voir la procdure dans le fichier associ.
  */
 void F_AdministrerMembres::AfficherMembre(unsigned int nIdMembre)
 {
     QSqlQuery queryMembre ;

     //Effacement de tous les champs
     this->EffacerTousLesChamps();

     this->nIdMembreSelectionne = nIdMembre ;

     //Si l'id du membre est diffèrent de 0
     if(nIdMembre != 0)
     {
         //requête permettant de récuperer tous les informations d'un membre grâce à son Id
         queryMembre.prepare( "SELECT * FROM membres WHERE IdMembre=:id ;" ) ;
         queryMembre.bindValue( ":id", nIdMembre ) ;

         //Execution de la requête
         if( queryMembre.exec() )
         {
             //Remplissage des champs avec les données retournées par la base de données
             if( queryMembre.next() )
             {
                 ui->CBx_Titre->setCurrentIndex( this->EmplacementTitreVector( queryMembre.record().value( 1 ).toInt() ) ) ;

                 ui->CBx_Type->setCurrentIndex( this->EmplacementTypeVector( queryMembre.record().value( 2 ).toInt() ) ) ;

                 ui->Le_Nom->setText( queryMembre.record().value( 3 ).toString() ) ;

                 ui->Le_Prenom->setText( queryMembre.value( 4 ).toString() ) ;

                 ui->Te_Rue->setPlainText( queryMembre.record().value( 5 ).toString() ) ;

                 ui->Le_CP->setText( queryMembre.record().value( 6 ).toString() )  ;

                 ui->Le_Ville->setText( queryMembre.record().value( 7 ).toString() ) ;

                 ui->Le_TelFix->setText( this->SyntaxeNumTelephone( queryMembre.value( 8 ).toString() ) ) ;

                 ui->Le_TelMobile->setText( this->SyntaxeNumTelephone( queryMembre.value( 9 ).toString() ) ) ;

                 ui->LE_Fax->setText( this->SyntaxeNumTelephone( queryMembre.value( 10 ).toString() ) ) ;

                 ui->LE_Email->setText( queryMembre.value( 11 ).toString() ) ;

                 ui->SBx_JeuxAutorises->setValue( queryMembre.record().value( 12 ).toInt() ) ;

                 ui->DtE_Insritption->setDateTime( queryMembre.record().value( 13 ).toDateTime() ) ;

                 ui->TE_Remarque->setPlainText( queryMembre.record().value( 14 ).toString() ) ;

                 if( queryMembre.record().value( 15 ).toBool() )
                 {
                      ui->Lb_MembreEcarte->setVisible( true ) ;
                      ui->ChBx_MembreEcarte->setChecked( true ) ;
                 }
                 else
                 {
                     ui->Lb_MembreEcarte->setHidden( true ) ;
                     ui->ChBx_MembreEcarte->setChecked( false ) ;
                 }

                 ui->DtE_Naissance->setDateTime( queryMembre.record().value( 16 ).toDateTime() ) ;

                 ui->Le_Code->setText( queryMembre.record().value( 17 ).toString() ) ;

                 ui->SPx_NbreRetards->setValue( queryMembre.record().value( 22 ).toInt() ) ;


                 this->AfficherJeuxEmpruntes( nIdMembre ) ;

                 ui->Bt_ModifierMembre->setEnabled( true ) ;
             }

         }
         else //Sinon on affiche un message d'erreur et on retourne Faux
         {
             cerr << "F_AdministrerMembres::AfficherMembre : Erreur de connexion avec la base de donne !" << queryMembre.lastError().text().toStdString() <<  endl ;
             ui->Bt_ModifierMembre->setDisabled( true ) ;
         }
     }
     else
     {
         ui->Bt_ModifierMembre->setDisabled( true ) ;
     }

 }

 /** Description dtaille de la mthode
  *  @pre    Accès à la base de données
  *  @retval bool
  *  @return True si tous c'est bien passé et false en cas d'erreur
  *  @test   Voir la procdure dans le fichier associ.
  */
 bool F_AdministrerMembres::AjouterMembre()
 {
     bool bRetourne (true) ;
     QSqlQuery queryMembre ;


     //Enregistrement d'un nouveau membre dans la base de données
     queryMembre.prepare("INSERT INTO membres (TitreMembre_IdTitreMembre, TypeMembres_IdTypeMembres, Nom, Prenom, Rue, CP, Ville, Telephone, Mobile, Fax, Email, NbreJeuxAutorises, DateInscription, DateNaissance, Remarque, Ecarte, CodeMembre)"
                         "VALUES (:TitreMembre_IdTitreMembre, :TypeMembres_IdTypeMembres, :Nom, :Prenom, :Rue, :CP, :Ville, :Telephone, :Mobile, :Fax, :Email, :NbreJeuxAutorises, :DateInscription, :DateNaissance, :Remarque, :Ecarte, :CodeMembre)" ) ;

     //Titre Membre
     queryMembre.bindValue( ":TitreMembre_IdTitreMembre", this->VectorTitre[ui->CBx_Titre->currentIndex()].id ) ;

     //Type Membre
     queryMembre.bindValue( ":TypeMembres_IdTypeMembres", this->VectorType[ui->CBx_Type->currentIndex()].id ) ;

     //Nom
     queryMembre.bindValue( ":Nom", ui->Le_Nom->text() ) ;

     //Prenom
     queryMembre.bindValue( ":Prenom", ui->Le_Prenom->text() ) ;

     //Rue
     queryMembre.bindValue( ":Rue", ui->Te_Rue->toPlainText() ) ;

     //Code Postale
     queryMembre.bindValue( ":CP", ui->Le_CP->text().toInt() ) ;

     //Ville
     queryMembre.bindValue( ":Ville", ui->Le_Ville->text() ) ;

     //Telephone
     queryMembre.bindValue( ":Telephone", ui->Le_TelFix->text() ) ;

     //Mobile
     queryMembre.bindValue( ":Mobile", ui->Le_TelMobile->text() ) ;

     //Fax
     queryMembre.bindValue( ":Fax", ui->LE_Fax->text() ) ;

     //Email
     queryMembre.bindValue( ":Email", ui->LE_Email->text() ) ;

     //Nombre de jeux Autorisés
     queryMembre.bindValue( ":NbreJeuxAutorises", ui->SBx_JeuxAutorises->text().toInt() ) ;

     //Date d'inscription
     queryMembre.bindValue( ":DateInscription", ui->DtE_Insritption->dateTime() ) ;

     //Date Naissance
     queryMembre.bindValue( ":DateNaissance", ui->DtE_Naissance->dateTime() ) ;

     //Remarque
     queryMembre.bindValue( ":Remarque", ui->TE_Remarque->toPlainText() ) ;

     //Ecarte
     queryMembre.bindValue( ":Ecarte", ui->ChBx_MembreEcarte->isChecked() ) ;

     //Code Membre
     queryMembre.bindValue( ":CodeMembre", ui->Le_Code->text() ) ;

     //Si le membre à bien était enregistrer this->nIdMembreSelectionne prend pour valeur l'id du membre créé
     if( queryMembre.exec() )
     {
        this->nIdMembreSelectionne = queryMembre.lastInsertId().toInt() ;
     }
     else//Sinon on affiche un message d'erreur et on retourne Faux
     {
         cerr << "F_AdministrerMembres::AjouterMembre : Erreur de connexion avec la base de donne !" << queryMembre.lastError().text().toStdString() << endl ;
         bRetourne = false ;
     }
     return bRetourne ;
 }

 /** Description dtaille de la mthode
  *  @pre    Accés à la base de données, Base de données contenant minimum un membre
  *  @param  unsigned int nIdMembre
  *  @retval bool
  *  @return True si tous c'est bien passé et false en cas d'erreur
  *  @test   Voir la procdure dans le fichier associ.
  */
bool F_AdministrerMembres::ModifierMembre( unsigned int nIdMembre )
{

    bool bRetourne (true) ;
    QSqlQuery queryMembre ;

    //On vérifie que l'id est présent dans la base donnée
    queryMembre.prepare( " SELECT IdMembre FROM membres WHERE IdMembre=:id ") ;
    queryMembre.bindValue( ":id", nIdMembre);

    //Exécution de la requète
    if( queryMembre.exec() )
    {
        //Si l'id n'est pas présent dans la table on met nId Membre à 0
        if( queryMembre.size() == 0 )
        {
            nIdMembre = 0 ;
        }
    }
    else //Sinon on affiche un message d'erreur et on retourne Faux
    {
        cerr << "F_AdministrerMembres::ModifierMembre : Erreur de connexion avec la base de donne !" << queryMembre.lastError().text().toStdString() << endl ;
    }



    //S'il l'id est différent de 0 on ne mofifie pas le membre
    if( nIdMembre != 0 )
    {
        //Mise à des nouvelles informations sur le membre sélectionné dans la base de données
        queryMembre.prepare( "UPDATE membres SET membres.TitreMembre_IdTitreMembre=:TitreMembre_IdTitreMembre, membres.TypeMembres_IdTypeMembres=:TypeMembres_IdTypeMembres, membres.Nom=:Nom, membres.Prenom=:Prenom, membres.Rue=:Rue, membres.CP=:CP, membres.Ville=:Ville, membres.Telephone=:Telephone, membres.Mobile=:Mobile, membres.Fax=:Fax, membres.Email=:Email, membres.NbreJeuxAutorises=:NbreJeuxAutorises, membres.DateInscription=:DateInscription, DateNaissance=:DateNaissance, membres.Remarque=:Remarque, membres.Ecarte=:Ecarte, membres.CodeMembre=:CodeMembre "
                             "WHERE membres.IdMembre=:IdMembre" ) ;

        //ID Membre
        queryMembre.bindValue( ":IdMembre", this->nIdMembreSelectionne ) ;

        //Titre Membre
        queryMembre.bindValue( ":TitreMembre_IdTitreMembre", this->VectorTitre[ui->CBx_Titre->currentIndex()].id ) ;

        //Type Membre
        queryMembre.bindValue( ":TypeMembres_IdTypeMembres", this->VectorType[ui->CBx_Type->currentIndex()].id ) ;

        //Nom
        queryMembre.bindValue( ":Nom", ui->Le_Nom->text() ) ;

        //Prenom
        queryMembre.bindValue( ":Prenom", ui->Le_Prenom->text() ) ;

        //Rue
        queryMembre.bindValue( ":Rue", ui->Te_Rue->toPlainText() ) ;

        //Code Postale
        queryMembre.bindValue( ":CP", ui->Le_CP->text().toInt() ) ;

        //Ville
        queryMembre.bindValue( ":Ville", ui->Le_Ville->text() ) ;

        //Telephone
        queryMembre.bindValue( ":Telephone", ui->Le_TelFix->text() ) ;

        //Mobile
        queryMembre.bindValue( ":Mobile", ui->Le_TelMobile->text() ) ;

        //Fax
        queryMembre.bindValue( ":Fax", ui->LE_Fax->text() ) ;

        //Email
        queryMembre.bindValue( ":Email", ui->LE_Email->text() ) ;

        //Nombre de jeux Autorisés
        queryMembre.bindValue( ":NbreJeuxAutorises", ui->SBx_JeuxAutorises->text().toInt() ) ;

        //Date d'inscription
        queryMembre.bindValue( ":DateInscription", ui->DtE_Insritption->dateTime() ) ;

        //Date Naissance
        queryMembre.bindValue( ":DateNaissance", ui->DtE_Naissance->dateTime() ) ;

        //Remarque
        queryMembre.bindValue( ":Remarque", ui->TE_Remarque->toPlainText() ) ;

        //Ecarte
        queryMembre.bindValue( ":Ecarte", ui->ChBx_MembreEcarte->isChecked() ) ;

        //Code Membre
        queryMembre.bindValue( ":CodeMembre", ui->Le_Code->text() ) ;

        //Si le membre à bien était enregistrer this->nIdMembreSelectionne prend pour valeur l'id du membre créé
        if( queryMembre.exec() )
        {
            this->nIdMembreSelectionne = queryMembre.lastInsertId().toInt() ;
        }
        else //Sinon on affiche un message d'erreur et on retourne Faux
        {
            cerr << "F_AdministrerMembres::ModifierMembre : Erreur de connexion avec la base de donne !" << queryMembre.lastError().text().toStdString() << endl ;
            bRetourne = false ;
        }
    }
    else
    {
        bRetourne = false ;
    }
    return bRetourne ;
}

/** Permet la suppression du dont l'id est passé en paramètre
 *  @pre    Connexion à la base de données, Membre présent dans la base de données
 *  @param  Indique l'id du membre à supprimer
 *  @retval bool
 *  @return Retourne vrai si la requète a bien été executer sinon elle retourne faux
 *  @test   Voir la procdure dans le fichier associ.
 */
bool F_AdministrerMembres::SupprimerMembre(int IdMembre)
{
    bool bRetourne ( true ) ;
    QSqlQuery querySupprimer ;

    //Si l'id est différent de 0
    if( IdMembre != 0 )
    {
        //Préparation de la requète permettant la suppression d'un membre
        querySupprimer.prepare( "DELETE FROM `membres` WHERE IdMembre=:IdMembre " ) ;
        querySupprimer.bindValue( ":IdMembre", IdMembre) ;

        //Execution de la requète, si elle fonctionne on met la variable de retoure à vrai
        if( querySupprimer.exec() )
        {
            bRetourne = true ;
        }
        else//Sinon on affiche un message d'erreur et on met la variable de retoure à faux
        {
            cerr << "F_AdministrerMembres::SupprimerMembre : Erreur de connexion avec la base de donne !" << querySupprimer.lastError().text().toStdString() << endl ;
            bRetourne = false ;
        }
    }
    else // Si l'id est égale à 0 on met la variable de retoure à faux
    {
        bRetourne = false ;
    }

    return bRetourne ;
}

/** Validation suppression d'un membre
 *  @test   Voir la procdure dans le fichier associé.
 */
void F_AdministrerMembres::on_commandLinkButton_3_clicked()
{
    //Message pour confirmer la suppression
    if( QMessageBox::information( this, "Suppression Membre","Voulez vraiment supprimer ce membre ?", "Supprimer", "Annuler" ) == 0 )
    {
        this->SupprimerMembre( this->nIdMembreSelectionne ) ;
    }
}
