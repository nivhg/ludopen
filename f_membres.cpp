//------------------------------------------------------------------------------
/** @file         f_membres.cpp
 *  @brief        Permet à un utilisateur de créer, de modifier ou de consulter les informations sur un membres. En administration permet de supprimer des membres
 *
 *  @author       PADIOU Nicolas
 *  @author       STS IRIS, Lyce Nicolas APPERT, ORVAULT ( FRANCE )
 *  @since        01/01/2012
 *  @version      0.1
 *  @date         22/02/2014 par William
 *
 *
 *  @todo         Voir post-it Création d'un fenètre liste membre administrateur pour pouvoir supprimer
 *
 *  @bug          aucun
 */
//------------------------------------------------------------------------------
#include "f_membres.h"
#include "ui_f_membres.h"
#include "fonctions_globale.h"
#include "f_preferences.h"

// En-têtes standards nécessaires dans ce fichier en-tête seulement ------------
#include <QMessageBox>

/** Initialisation de la classe
 *  @param  F_Membres * pRechercheMembres
 *
 */
F_Membres::F_Membres( int iMode, QWidget *parent, int nIdCollectivite ):
    QWidget( parent ),
    ui( new Ui::F_Membres )
{
    ui->setupUi( this ) ;

    this->nIdCollectivite = nIdCollectivite;
    //Indique le mode : Administration, utilisation standard ou membres associés
    this->iMode = iMode ;

    if(this->iMode!=MODE_ADMIN)
    {
        ui->Bt_SupprimerMembre->setVisible( false );
    }
    ui->Bt_SupprimerMembre->setDisabled( true ) ;

    //aucun membre sélectionné par defaut
    this->nIdMembreSelectionne = 0 ;

    qDebug()<< "F_Membres::Creation F_HistoriqueJeux";
    this->pHistoriqueJeux = new F_HistoriqueJeux ;
    this->pHistoriqueJeux->setWindowModality( Qt::ApplicationModal ) ;
    qDebug()<< "F_Membres::Creation F_AjouterCotiCarte";
    this->pAjouterCotiCarte = new F_AjouterCotiCarte ;
    this->pAjouterCotiCarte->setWindowModality( Qt::ApplicationModal ) ;

    // Création des 2 fenêtres d'ajout d'un nouveau titre et type de membre

    qDebug()<< "F_Membres::Creation F_PopUpCLESTTEM Code 5";
    this->pTypeAjMod = new F_PopUpCLESTTEM();
    this->pTypeAjMod->setWindowModality(Qt::ApplicationModal);
    qDebug()<< "F_Membres::Creation F_PopUpCLESTTEM code 6";
    this->pTitreAjMod = new F_PopUpCLESTTEM();
    this->pTitreAjMod->setWindowModality(Qt::ApplicationModal);
    qDebug()<< "F_Membres::Creation F_PopUpCLESTTEM Code 12";
    this->pDomaineEmailAjMod = new F_PopUpCLESTTEM();
    this->pDomaineEmailAjMod->setWindowModality(Qt::ApplicationModal);

    this->EffacerTousLesChamps() ;

    /////////////Fenêtre d'ajout d'une ville///////////////
    this->oFenetreAjoutVille = new QDialog(this) ;
    this->oFenetreAjoutVille->setModal( true ) ;
    this->oFenetreAjoutVille->setMaximumSize( 230, 100) ;
    this->oFenetreAjoutVille->setMinimumSize( 230, 100) ;
    this->oFenetreAjoutVille->hide() ;

    this->LE_AjoutVille = new QLineEdit( this->oFenetreAjoutVille ) ;
    this->LE_AjoutVille->setGeometry(10, 30, 210, this->LE_AjoutVille->height() );
    // Connecte l'évenement textEdited à la fonction toUpper
    connect(this->LE_AjoutVille, SIGNAL(textEdited(const QString &)), SLOT(toUpper(const QString &)));

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
    connect( this->pTitreAjMod, SIGNAL(SignalValider()), this, SLOT( slot_ChoisirNouveauTitre() ) ) ;
    connect( this->pTypeAjMod, SIGNAL(SignalValider()), this, SLOT( slot_ChoisirNouveauType() ) ) ;
    connect( this->pDomaineEmailAjMod, SIGNAL(SignalValider()), this, SLOT( slot_ChoisirNouveauDomaineEmail() ) ) ;
    connect( this->pAjouterCotiCarte, SIGNAL( SignalAjoutCotisationCarte() ), this, SLOT( slot_ActualiserAbonnements() ) ) ;
    connect( this->Bt_ValiderVille, SIGNAL( clicked() ), this, SLOT( slot_ValiderAjoutVille() ) ) ;
    connect( this->Bt_AnnuerVille, SIGNAL( clicked() ), this, SLOT(slot_AnnulerAjoutVille()) ) ;
    // Connecte l'évenement textEdited à la fonction toUpper
    connect(ui->Le_Nom, SIGNAL(textEdited(const QString &)), SLOT(toUpper(const QString &)));

    //Initialisation de la liste de titres, de types et des domaines d'email
    this->MaJTitre() ;
    this->MaJType() ;
    this->MaJDomaineEmail() ;

    // Autorise le tri des colonnes pour le tableau Abonnement souscrit par le membre
    //ui->LW_Abonnements->setSortingEnabled(true);
    // Autorise le tri des colonnes pour le tableau JeuxEmpruntes par le membre
    //ui->LW_JeuxEmpruntes->setSortingEnabled(true);
    // TODO tri pas possible car choix gérer par le vecteur !!!
    // donc clic pas possible si on modifie l'ordre dans les colonnes car on ne tombe pas sur le bon item
    // Autorise le tri des colonnes pour ce tableau
    //ui->TbW_Recherche->setSortingEnabled(true);
    // TO DO pour trier le tableau, il faudrait que l'on retrouve le membre dans le vecteur
    // ou virer le vecteur en stockant toutes les infos dans le tableau sans afficher les colonnes qu'on ne veut pas

    if(this->iMode==MODE_MEMBRE_ASSOCIE)
    {
        this->MaJListeMembres(true) ;
        setWindowTitle("Membres associés");
        ui->frame->setVisible(false);
        ui->frame_2->setVisible(false);
        ui->Fr_Associes->setVisible(true);
        ui->Lb_Associes->setVisible(true);
        ui->TbW_Recherche->blockSignals(true);
        ui->TbW_Recherche->selectRow(0);
        QDesktopWidget *desktop = QApplication::desktop();
        QRect geo_d_i = geometry();
        int x = (desktop->width() - 400) / 2;
        int y = (desktop->height() - geo_d_i.height()) / 2;
        this->setGeometry(x, y, 400, geo_d_i.height());
        this->show();
        ui->TbW_Associes->clearSpans() ;
        ui->TbW_Associes->setModel(&ModeleAssocies) ;
        ui->TbW_Associes->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->TbW_Associes->setSortingEnabled(true);
        ModeleAssocies.setColumnCount(4);
        ModeleAssocies.setRowCount(0);
        ModeleAssocies.setHorizontalHeaderItem(0, new QStandardItem("Id"));
        ModeleAssocies.setHorizontalHeaderItem(1, new QStandardItem("Nom"));
        ModeleAssocies.setHorizontalHeaderItem(2, new QStandardItem("Prénom"));
        ModeleAssocies.setHorizontalHeaderItem(3, new QStandardItem("Ville"));
        ModeleAssocies.setHorizontalHeaderItem(4, new QStandardItem("Code Membre"));
        ui->TbW_Associes->setColumnWidth(0,0);
        QSqlQuery RequeteAssocie ;
        //Enregistrement d'un nouveau membre dans la base de données
        RequeteAssocie.prepare( "SELECT Membres_IdMembre,Nom,Prenom,Ville,CodeMembre"
            " FROM membresassocies,membres WHERE Membres_IdCollectivite=:IdCollectivite"
            " AND IdMembre=Membres_IdMembre AND Ecarte=0 ORDER BY Nom") ;
        RequeteAssocie.bindValue(":IdCollectivite",this->nIdCollectivite);
        //Exectution de la requête
        if( !RequeteAssocie.exec() )
        {
            qDebug()<< "F_Membres::on_Bt_ValiderAssocies_clicked : RequeteMembre " << RequeteAssocie.lastQuery() ;
        }
        else
        {
            int i=0;
            while(RequeteAssocie.next())
            {
                ModeleAssocies.setItem(i,0,new QStandardItem(ObtenirValeurParNom(RequeteAssocie,"Membres_IdMembre").toString()));
                ModeleAssocies.setItem(i,1,new QStandardItem(ObtenirValeurParNom(RequeteAssocie,"Nom").toString()));
                ModeleAssocies.setItem(i,2,new QStandardItem(ObtenirValeurParNom(RequeteAssocie,"Prenom").toString()));
                ModeleAssocies.setItem(i,3,new QStandardItem(ObtenirValeurParNom(RequeteAssocie,"Ville").toString()));
                ModeleAssocies.setItem(i,4,new QStandardItem(ObtenirValeurParNom(RequeteAssocie,"CodeMembre").toString()));
                i++;
            }
            if(i!=0)
            {
                ui->TbW_Associes->selectRow(0);
                ui->Bt_SupprimerAssocie->setEnabled(true);
            }
        }
    }
    else
    {
        ui->Fr_Associes->setVisible(false);
        ui->Lb_Associes->setVisible(false);
        this->MaJListeMembres();
    }

    this->VecteurRechercheMembres = this->VecteurMembres ;
    this->AfficherListe(this->VecteurRechercheMembres) ;

    ui->TbW_Recherche->setModel(&ModeleRechercheMembre) ;
    ui->TbW_Recherche->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Faire défiler le tableau des membres avec les flêches du clavier
    connect(ui->TbW_Recherche->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(on_TbW_Recherche_clicked(QModelIndex)));

    slot_AfficherMembre(this->VecteurRechercheMembres[0].id);

    ui->Tw_activites->setColumnCount(2);
    ui->Tw_activites->verticalHeader()->setVisible(false);
    QStringList m_TableHeader;
    m_TableHeader<<"Activité"<<"NewsLetter";
    ui->Tw_activites->setHorizontalHeaderLabels(m_TableHeader);
    ui->Tw_activites->setColumnWidth( 0, 150 ) ;  // Jeux
    ChargerActivites(ui->CBx_Activites);
}

//==========================================================================================================
/** Détruit les objets crées
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

    if(this->pDomaineEmailAjMod)
    {
        delete this->pDomaineEmailAjMod;
    }

    delete ui ;
}

//---------------------------------------------------------------------------
// METHODEs PUBLIQUEs
//-------------------------------------------------------------------------

/**  Récupère la liste des activités et la mets dans une combobox
 *  @pre    Accès à  la base de données
 */
void F_Membres::ChargerActivites(QComboBox * combobox)
{
    QSqlQuery RequeteActivites;

    // Récupère toutes les activités
    RequeteActivites.prepare(
                "SELECT IdActivite, Activite FROM activite " ) ;

    //Exectution de la requête
    if( !RequeteActivites.exec() )
    {
        qDebug()<< "F_Membres::ChargerActivites : " << RequeteActivites.lastQuery() ;
    }
    else
    {
        QString Activite;
        int IdActivite;
        int i = 0;
        while( RequeteActivites.next() )
        {
            // Récupère la valeurs des champs
            Activite = ObtenirValeurParNom(RequeteActivites,"Activite").toString();
            IdActivite = ObtenirValeurParNom(RequeteActivites,"IdActivite").toInt();
            // Ajoute une activité et son ID dans la partie DATA
            combobox->addItem(Activite);
            combobox->setItemData(i,IdActivite,Qt::UserRole);
            // Mets le prêt de jeux en choix par défaut
            if(Activite=="Prêt de jeux")
            {
                combobox->setCurrentIndex(i);
            }
            i++;
        }
    }
}

/**   Mise  jour de la liste des membres
 *  @pre    Accès à  la base de données
 *  @retval bool
 *  @return True si tous c'est passé correctement et false si il y a une erreur
 *  @test   Voir la procédure dans le fichier associé.
 */
bool F_Membres::MaJListeMembres(bool AfficherContact)
{
    QSqlQuery query ;
    Membre Membres ;
    bool bRetourner = true ;

    //Vidange du vecteur
    this->VecteurMembres.clear() ;

    QString requeteSQL;
    requeteSQL="SELECT IdMembre, Nom, Prenom, Ville, CodeMembre, Email FROM membres WHERE Ecarte=0 ";
    if(!AfficherContact)
    {
        requeteSQL+="AND TypeMembres_IdTypeMembres!=4 ";
    }
    requeteSQL+="ORDER BY Nom ASC";

    //Execute une requète sql qui retourne la liste des membres
    //Si la requète est correcte -> Remplissage du veteur VecteurMembres avec le résultat de la requète et on retourne vrai.
    if(query.exec(requeteSQL))
    {
        while(query.next())
        {
            Membres.id = query.value(0).toInt() ;
            Membres.sNom = query.value(1).toString() ;
            Membres.sPrenom = query.value(2).toString() ;
            Membres.sVille = query.value(3).toString() ;
            Membres.sCodeMembre = query.value(4).toString() ;
            this->VecteurMembres.push_back(Membres) ;
        }
    }
    else //Sinon on affiche un message d'erreur et on retourne Faux
    {
        qDebug()<< "RechercheMembre::MaJListeMembres() : Erreur de connexion avec la base de donnée !" << endl ;
        bRetourner = false ;
    }
    this->VecteurRechercheMembres = VecteurMembres ;
    ui->LE_Nom->clear() ;
    return bRetourner ;
}

/**  Affichage de la liste des membres
 *  @test   Voir la procédure dans le fichier associé.
*/
void F_Membres::AfficherListe()
{
    this->AfficherListe(this->VecteurMembres);
}

/** Affichage de la liste des membres
 *  @pre    Accés à  la base de données
 *  @param  QVector<Membre> VecteurMembre
 *  @test   Voir la procédure dans le fichier associé.
 */
void F_Membres::AfficherListe(QVector<Membre> VecteurMembres)
{
    int i (0) ;

    //Remise à  zero de la table recherche
    ui->TbW_Recherche->clearSpans() ;

    //Vérifie que la liste n'est pas vide
    //Si la variable VecteurMembres contient des membres alors on affiche la liste
    //Sinon on affiche rien
    if(VecteurMembres.empty() == false)
    {
        //Création des caractéristiques du tableau : -Nombre de colonnes
        //                                           -Nom des colonnes
        //                                           -Nombre de lignes
        ModeleRechercheMembre.setColumnCount(4);
        ModeleRechercheMembre.setRowCount(VecteurMembres.size());
        ModeleRechercheMembre.setHorizontalHeaderItem(0, new QStandardItem("Nom"));
        ModeleRechercheMembre.setHorizontalHeaderItem(1, new QStandardItem("Prénom"));
        ModeleRechercheMembre.setHorizontalHeaderItem(2, new QStandardItem("Ville"));
        ModeleRechercheMembre.setHorizontalHeaderItem(3, new QStandardItem("Code Membre"));

        //Remplissage du tableau avec les informations contenu dans VecteurMembres
        for(i = 0 ; i < VecteurMembres.size() ; i++)
        {
             ModeleRechercheMembre.setItem(i, 0, new QStandardItem( VecteurMembres[i].sNom ));
             ModeleRechercheMembre.setItem(i, 1, new QStandardItem( VecteurMembres[i].sPrenom ));
             ModeleRechercheMembre.setItem(i, 2, new QStandardItem( VecteurMembres[i].sVille ));
             ModeleRechercheMembre.setItem(i, 3, new QStandardItem( VecteurMembres[i].sCodeMembre));
        }
    }
}

/** Recherche soit avec le nom, soit avec le numéro ou avec les 2
 *  @pre    Accés à  la base de données
 *  @test   Voir la procédure dans le fichier associé.
 */
void F_Membres::RechercherParNomEtNumero()
{
    unsigned int i (0) ;

    //Remize à  zéro de du Vecteur VecteurRechercheMembres
    this->VecteurRechercheMembres.clear();

    //Vérification que les champs sont bien remplis
    //S'ils ne sont pas, remplis le Vecteur VecteurRechercheMembres avec le Vecteur VecteurMembre
    if(ui->LE_Nom->text() == "")
    {
        this->VecteurRechercheMembres = this->VecteurMembres ;
    }
    else// Sinon on remplis le Vecteur VecteurRechercheMembre
    {
        for(i = 0 ; i < this->VecteurMembres.size() ; i++)
        {
            //On vérifie que la suite de lettres entrées dans LE_Nom correpondent aux Noms du vecteur VecteurMembre
            //ou que la suite de chiffre entrées dans LE_Code correspondent aux CodeMembres du Vecteur VecteurMembres
            //Si le Nom, le code ou les 2 correspondent, on l'ajoute dans le vecteur VecteurRechercheMembres
            bool numeric;
            ui->LE_Nom->text().toInt(&numeric);
            // Si le texte saisie est un nombre
            if( numeric )
            {
                if( this->VecteurMembres[i].sCodeMembre.indexOf( ui->LE_Nom->text(),0,Qt::CaseInsensitive ) != string::npos )
                {
                    this->VecteurRechercheMembres.push_back(VecteurMembres[i]);
                }
            }
            else
            {
                // Retrait des accents pour pouvoir faire des recherches de membres avec accents
                QString sNom=this->VecteurMembres[i].sNom.toLower().replace(QRegExp("[éèë]"), "e");
                sNom.replace(QRegExp("[à]"), "a");
                QString sPrenom=this->VecteurMembres[i].sPrenom.toLower().replace(QRegExp("[éèë]"), "e");
                sPrenom.replace(QRegExp("[à]"), "a");
                if( sNom.indexOf( ui->LE_Nom->text().toLower() ) != string::npos )
                {
                    this->VecteurRechercheMembres.push_back(VecteurMembres[i]);
                }
                else if( sPrenom.indexOf( ui->LE_Nom->text().toLower() ) != string::npos )
                {
                    this->VecteurRechercheMembres.push_back(VecteurMembres[i]);
                }
            }
        }
    }
    this->AfficherListe(this->VecteurRechercheMembres);
}

/** Renvoie le premier code non utilisé
 *  @test   Voir la procédure dans le fichier associé.
 */
int F_Membres::RecupererProchainCodeNonUtilise ()
{
    int nCode ( 1 ) ;
    int     i ( 0 ) ;

    while( i < this->VecteurMembres.size() )
    {
        if( this->VecteurMembres[i].sCodeMembre.toInt() == nCode )
        {
            nCode ++ ;
            i = 0 ;
        }
        i++ ;
    }
    return nCode ;
}

/** Retourne le membre Selectionné dans le TableView
 *  @retval int
 *  @return Retourne la ligne de la selction
 *  @test   Voir la procédure dans le fichier associé.
 */
 int F_Membres::RecupererMembreSelectionne()
 {
     return ui->TbW_Recherche->currentIndex().row() ;
 }


//==========================================================================================================
/** Met   jour la liste des Titres depuis la base de données
 *  @pre    Accés à la base de données
 *
 */
void F_Membres::MaJTitre ()
{
   qDebug()<< "F_Membres::MaJTitre";

    int i           ( 0 )    ;
    QSqlQuery Requete        ;
    Titre oTitre             ;

   //Suppression du contenu du vecteur de la combobox CBx_Titre
    this->VectorTitre.clear() ;
    ui->CBx_Titre->clear() ;

    //Exécution de la requête qui sélectionne le contenu de la table titremembre
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

        ui->CBx_Titre->blockSignals(true);
        //Remplissage de la combobox grâce au vecteur
        for( i = 0 ; i < VectorTitre.size() ; i ++ )
        {
            ui->CBx_Titre->insertItem( i, VectorTitre[i].sTitre ) ;
        }
        ui->CBx_Titre->blockSignals(false);
    }
    else //Sinon on affiche un message d'erreur et on retourne Faux
    {
        qDebug()<< "F_Membres::MaJTitre : Erreur bdd : " << Requete.lastQuery() << endl ;
    }
}
//==========================================================================================================
/**  Met a jour la liste des Types depuis la base de données
 *  @pre    Accès à la base de données
 *  @test
 */
void F_Membres::MaJType ()
{
   qDebug()<< "F_Membres::MaJType";

    int i( 0 ) ;
    QSqlQuery Requete ;
    Type oType ;

    //Suppression du contenu du vecteur de la combobox CBx_Type
    this->VectorType.clear() ;
    ui->CBx_Type->clear() ;

    //Exécution de la requête qui sélectionne le contenu de la table tytremembres
    if( Requete.exec( "SELECT * FROM typemembres" ) )
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
        qDebug()<< "F_Membres::MaJType (): Erreur bdd :" <<  Requete.lastQuery() <<  endl ;
    }
}

//==========================================================================================================
/**  Met a jour la liste des domaines d'email depuis la base de données
 *  @pre    Accès à la base de données
 *  @test
 */
void F_Membres::MaJDomaineEmail ()
{
    qDebug()<< "F_Membres::MaJDomaineEmail";

     int i( 0 ) ;
     QSqlQuery Requete ;
     Type oDomaineEmail ;

     //Suppression du contenu du vecteur de la combobox CBx_DomaineEmail
     this->VectorDomaineEmail.clear() ;
     ui->CBx_DomaineEmail->clear() ;

     //Exécution de la requête qui sélectionne le contenu de la table tytremembres
     if( Requete.exec( "SELECT * FROM domaineemail" ) )
     {
         oDomaineEmail.id = 0 ;
         oDomaineEmail.sType = "" ;
         this->VectorDomaineEmail.push_back( oDomaineEmail ) ;

         //Remplissage du vecteur avec ce que retourne la requête
         while( Requete.next() )
         {
             oDomaineEmail.id = Requete.value( 0 ).toInt() ;
             oDomaineEmail.sType = Requete.value( 1 ).toString() ;

             this->VectorDomaineEmail.push_back( oDomaineEmail ) ;
         }

         //Création d'un choix permettant de créer un nouveau titre
         oDomaineEmail.id = 0 ;
         oDomaineEmail.sType = "Créer domaine ..." ;

         this->VectorDomaineEmail.push_back( oDomaineEmail ) ;

         //Remplissage de la combobox grace au vecteur
         for( i = 0 ; i < VectorDomaineEmail.size() ; i ++ )
         {
             ui->CBx_DomaineEmail->insertItem( i, VectorDomaineEmail[ i ].sType ) ;
         }
     }
     else //Sinon on affiche un message d'erreur et on retourne Faux
     {
         qDebug()<< "F_Membres::MaJDomaineEmail (): Erreur bdd :" <<  Requete.lastQuery() <<  endl ;
     }
}

//==========================================================================================================
/** Recupere l'emplacement d'un titre dans un vecteur en fonction de son id
 *  @pre    Combobox remplie avec le vecteur
 *  @param  unsigned int nIdTitre
 *  @retval int
 *  @return L'emplacement du titre dans le vecteur par rapport à l'id ( 0 par défaut )
 *
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
//==========================================================================================================
/** Recupere l'emplacement d'un type dans un vecteur en fonction de son id
 *  @pre    Combobox remplie avec le vecteur
 *  @param  unsigned int nIdTytre
 *  @retval int
 *  @return L'emplacement du titre dans le vecteur par rapport à l'id ( 0 par défaut )
 *
 */
int F_Membres::RecupererEmplacementTypeVecteur( unsigned int nIdType )
{
    int nEmplacementType ( 0 ) ;
    int nBoucle          ( 0 ) ;

    //Recherche de l'id dans le vecteur tant que l'on a pas fini de parcours le vecteur
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

//==========================================================================================================
/** Recupere l'emplacement d'un domaine d'email dans un vecteur en fonction de son id
 *  @pre    Combobox remplie avec le vecteur
 *  @param  unsigned int nIdDomaineEmail
 *  @retval int
 *  @return L'emplacement du domaine d'email dans le vecteur par rapport à l'id ( 0 par défaut )
 *
 */
int F_Membres::RecupererEmplacementDomaineEmailVecteur( unsigned int nIdDomaineEmail )
{
    int nEmplacementDomaine ( 0 ) ;
    int nBoucle          ( 0 ) ;

    //Recherche de l'id dans le vecteur tant que l'on a pas fini de parcours le vecteur
    while( nBoucle < this->VectorDomaineEmail.size()&& nEmplacementDomaine == 0 && nIdDomaineEmail != 0 )
    {
        if( this->VectorDomaineEmail[ nBoucle ].id == nIdDomaineEmail )
        {
            nEmplacementDomaine = nBoucle ;
        }
        nBoucle ++ ;
    }

    return nEmplacementDomaine ;
}

//==========================================================================================================
/** Efface tous ce qui n'est pas un chiffre et ajoute un espace tous les 2 caractères dans un string
 *  @param QString sNumero
 *  @retval QString
 *  @return Retourne un QString ne contenant plus de lettre ou caractêres speciaux. Un espace est ajouté tous les 2 caractères
 *
 */
QString F_Membres::ModifierSyntaxeNumTelephone ( QString sNumero )
{
    QChar cEspace ( ' ' ) ;
    QRegExp Nombres ( "[^0-9()\+]" ) ;

    //Suppression des les caractères ormis les chiffres
    sNumero.replace( Nombres, "" ) ;

    //Ajout d'un espace tous les 2 caractères
    for( register int i = 2 ; i < sNumero.length() ; i= i+3 )
    {
        if( i != sNumero.length() )
        {
            sNumero.insert( i, cEspace ) ;
        }

    }
    return sNumero ;
}

//==========================================================================================================
/** Affiche les activités d'un membre
  *  @pre    Accés ? la base de données
  *  @param  unsigned int nIdMembre
  */
void F_Membres::AfficherActivites( unsigned int nIdMembre )
{
    qDebug()<< "F_Membres::AfficherActivite";

    QSqlQuery RequeteActivites;

    //requête permettant d'avoir les activités correspondant à l'id du membre
    RequeteActivites.prepare(
                "SELECT IdActivite, Activite, Newsletter FROM activite "
                "INNER JOIN activitemembre ON Activite_IdActivite = IdActivite "
                "INNER JOIN membres ON Membres_IdMembre = IdMembre "
                "WHERE Membres_IdMembre=:nIdMembre" ) ;
    RequeteActivites.bindValue( ":nIdMembre", nIdMembre );

    //Exectution de la requête
    if( !RequeteActivites.exec() )
    {
        qDebug()<< "F_Membres::AfficherActivite : RequeteActivite " << RequeteActivites.lastQuery() ;
    }
    else
    {
        ui->Tw_activites->setRowCount( RequeteActivites.size() ) ;
        QString Activite;
        int i=0;
        //Remplissage du tableau avec les informations de la table emprunts
        while( RequeteActivites.next() )
        {
            bool newsletter = RequeteActivites.record().value( RequeteActivites.record().indexOf("newsletter") ).toBool();
            QWidget *pWidget = new QWidget();
            QCheckBox *pCheckBox = new QCheckBox();
            pCheckBox->setChecked(newsletter);
            QString IdActivite = RequeteActivites.record().value( RequeteActivites.record().indexOf("IdActivite") ).toString();
            pCheckBox->setProperty("IdActivite",IdActivite);
            connect( pCheckBox, SIGNAL( clicked() ), this, SLOT( on_ChBx_Activites_clicked() ) ) ;
            QHBoxLayout *pLayout = new QHBoxLayout(pWidget);
            pLayout->addWidget(pCheckBox);
            pLayout->setAlignment(Qt::AlignCenter);
            pLayout->setContentsMargins(0,0,0,0);
            pWidget->setLayout(pLayout);
            ui->Tw_activites->setCellWidget(i,1,pWidget);

            Activite = RequeteActivites.record().value( RequeteActivites.record().indexOf("Activite") ).toString();
            ui->Tw_activites->setProperty(Activite.toUtf8().constData(),IdActivite);
            ui->Tw_activites->setItem(i, 0, new QTableWidgetItem(Activite));
            i++;
        }
    }
}

//==========================================================================================================
/** Affiche les jeux empruntés dans le tableau TbW_Emprunt
  *  @pre    Accés à  la base de données
  *  @param  unsigned int nIdMembre
  */
void F_Membres::AfficherJeuxEmpruntes( unsigned int nIdMembre )
{
    qDebug()<< "F_Membres::AfficherJeuxEmpruntes";

    QSqlQuery RequeteEmprunt ;
    QSqlQuery RequeteJeux    ;
    int i     ( 0 )        ;

    //Création et initialisation du Modèle
    QStandardItemModel * modele = new QStandardItemModel() ;
    ui->LW_JeuxEmpruntes->setModel( modele ) ;
    ui->LW_JeuxEmpruntes->setEditTriggers( 0 ) ;
    modele->setHorizontalHeaderItem( 0, new QStandardItem( "Code" ) ) ;
    modele->setHorizontalHeaderItem( 1, new QStandardItem( "Nom du jeu" ) ) ;
    modele->setHorizontalHeaderItem( 2, new QStandardItem( "Emprunté le" ) ) ;
    modele->setHorizontalHeaderItem( 3, new QStandardItem( "Retour prévu le" ) ) ;
    ui->LW_JeuxEmpruntes->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    /*ui->LW_JeuxEmpruntes->setColumnWidth( 0, 20 ) ;  // Code
    ui->LW_JeuxEmpruntes->setColumnWidth( 1, 200 ) ;  // Jeu
    ui->LW_JeuxEmpruntes->setColumnWidth( 2, 100 ) ;  // Date d'Emprunt
    ui->LW_JeuxEmpruntes->setColumnWidth( 3, 100 ) ;  // Date de Retour prévu*/

    //requête permettant d'avoir les jeux empruntés correspondant à l'id du membre
    RequeteEmprunt.prepare( "SELECT CodeJeu,NomJeu,DateEmprunt,DateRetourPrevu "
                            "FROM emprunts,jeux WHERE Membres_IdMembre=:nIdMembre AND "
                            "DateRetour IS NULL AND Jeux_IdJeux=IdJeux" ) ;
    RequeteEmprunt.bindValue( ":nIdMembre", nIdMembre ) ;

    //Exectution de la requête
    if( RequeteEmprunt.exec() )
    {  /*Création des caractéristiques du tableau : -Nom des colonnes
                                                    -Nombre de lignes*/
        modele->setRowCount( RequeteEmprunt.size() ) ;

        //Remplissage du tableau avec les informations de la table emprunts
        while( RequeteEmprunt.next() )
        {
            modele->setItem( i, 0, new QStandardItem(
               ObtenirValeurParNom(RequeteEmprunt,"CodeJeu").toString() ) ) ;
            modele->setItem( i, 1, new QStandardItem(
               ObtenirValeurParNom(RequeteEmprunt,"NomJeu").toString() ) ) ;
            modele->setItem( i, 2, new QStandardItem(
               ObtenirValeurParNom(RequeteEmprunt,"DateEmprunt").toDateTime().toString( "dd-MM-yyyy" ) ) ) ;
            modele->setItem( i, 3, new QStandardItem(
               ObtenirValeurParNom(RequeteEmprunt,"DateRetourPrevu").toDateTime().toString( "dd-MM-yyyy" ) ) ) ;

            // Met en rouge la ligne si le jeu emprunté est rendu en retard, sinon en vert
            // Tient compte du nombre de jour de retard toléré
            QDate DateActuelle;
            DateActuelle=DateActuelle.currentDate();
            // Calculer la date de retour avec la tolérance du nombre de jours
            DateActuelle.addDays(F_Preferences::ObtenirValeur("JourRetard").toInt() );

            QColor couleur;
            if ( RequeteEmprunt.value( 2 ).toDate() > DateActuelle )
            {
                couleur=Qt::green;
            }
            else
            {
                couleur=Qt::red;
            }
            modele->setData( modele->index( i,0 ),couleur, Qt::BackgroundColorRole ) ;
            modele->setData( modele->index( i,1 ),couleur, Qt::BackgroundColorRole ) ;
            modele->setData( modele->index( i,2 ),couleur, Qt::BackgroundColorRole ) ;
            modele->setData( modele->index( i,3 ),couleur, Qt::BackgroundColorRole ) ;
            i++ ;

        }
    }
    else //Sinon on affiche un message d'erreur et on retourne Faux
    {
        qDebug()<< "F_Membres::AfficherJeuxEmpruntes : RequeteEmprunt " << RequeteEmprunt.lastQuery() ;
    }
}

//==========================================================================================================
/** Affiche les informations d'un membre
  *  @pre    Accés à la base de données
  *  @param  unsigned int nIdMembre
  */
void F_Membres::AfficherMembre()
{
    this->AfficherMembre( this->nIdMembreSelectionne ) ;
    ui->LE_Nom->setFocus();
}
//==========================================================================================================
/** Affiche les informations d'un membre
  *  @pre    Accés à la base de données
  *  @param  unsigned int nIdMembre
  */
void F_Membres::AfficherMembre( unsigned int nIdMembre )
{
   qDebug()<< "F_Membres::AfficherMembre";

    QSqlQuery RequeteMembre ;
    QPalette palette ;
    QFont font ;

    //Effacement de tous les champs
    this->EffacerTousLesChamps() ;

    this->nIdMembreSelectionne = nIdMembre ;

    ui->Bt_ModifierAbonnement->setDisabled( true ) ;
    ui->Bt_SupprimerAbonnement->setDisabled( true ) ;

    //Si l'id du membre est différent de 0
    if( nIdMembre != 0 )
    {
        //requête permettant de récuperer tous les informations d'un membre grâce à son Id
        RequeteMembre.prepare( "SELECT *,MID(Email,1,LOCATE('@',Email)-1) as Email2 FROM membres "
                                "LEFT JOIN domaineemail ON DomaineEmail_IdDomaineEmail=IdDomaineEmail WHERE IdMembre=:id" ) ;
        RequeteMembre.bindValue( ":id", nIdMembre ) ;

        //Execution de la requête
        if( RequeteMembre.exec() )
        {
            //Remplissage des champs avec les données retournées par la base de données
            if( RequeteMembre.next() )
            {
                ui->CBx_Titre->setCurrentIndex( this->RecupererEmplacementTitreVecteur(
                               ObtenirValeurParNom(RequeteMembre,"TitreMembre_IdTitreMembre").toInt()));

                ui->CBx_Type->setCurrentIndex( this->RecupererEmplacementTypeVecteur(
                               ObtenirValeurParNom(RequeteMembre,"TypeMembres_IdTypeMembres").toInt()));

                ui->CBx_DomaineEmail->setCurrentIndex( this->RecupererEmplacementDomaineEmailVecteur(
                               ObtenirValeurParNom(RequeteMembre,"DomaineEMail_IdDomaineEMail").toInt()));

                ui->Le_Nom->setText( ObtenirValeurParNom(RequeteMembre,"Nom").toString() ) ;

                ui->Le_Prenom->setText( ObtenirValeurParNom(RequeteMembre,"Prenom").toString() ) ;

                ui->Te_Rue->setPlainText( ObtenirValeurParNom(RequeteMembre,"Rue").toString() ) ;

                ui->Le_CP->setText( ObtenirValeurParNom(RequeteMembre,"CP").toString() ) ;

                this->AfficherVilles( ObtenirValeurParNom(RequeteMembre,"Ville").toString() ) ;

                ui->Le_TelFix->setText( this->ModifierSyntaxeNumTelephone(
                                     ObtenirValeurParNom(RequeteMembre,"Telephone").toString() ) ) ;

                ui->Le_TelMobile->setText( this->ModifierSyntaxeNumTelephone(
                                     ObtenirValeurParNom(RequeteMembre,"Mobile").toString() ) ) ;

                ui->LE_Fax->setText( this->ModifierSyntaxeNumTelephone(
                                     ObtenirValeurParNom(RequeteMembre,"Fax").toString() ) ) ;

                ui->LE_Email->setText( ObtenirValeurParNom(RequeteMembre,"Email2").toString() ) ;

                ui->SBx_JeuxAutorises->setValue( ObtenirValeurParNom(RequeteMembre,"NbreJeuxAutorises").toInt() ) ;

                ui->SBx_NbrePersonne->setValue( ObtenirValeurParNom(RequeteMembre,"NbrePersonne").toInt() ) ;

                ui->DtE_Insritption->setDateTime( ObtenirValeurParNom(RequeteMembre,"DateInscription").toDateTime() ) ;

                ui->TE_Remarque->setPlainText( ObtenirValeurParNom(RequeteMembre,"Remarque").toString() ) ;

                if( ObtenirValeurParNom(RequeteMembre,"Ecarte").toBool() )
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
                    ui->ChBx_MembreEcarte->setHidden(true);

                    font.setBold( false ) ;
                    ui->Lb_MembreEcarte->setFont( font ) ;

                    palette.setColor( QPalette::WindowText, Qt::black ) ;
                    ui->Lb_MembreEcarte->setPalette( palette ) ;
                }

                ui->Le_Code->setText( ObtenirValeurParNom(RequeteMembre,"CodeMembre").toString() ) ;

                ui->SPx_NbreRetards->setValue( ObtenirValeurParNom(RequeteMembre,"NbreRetard").toInt() ) ;

                this->AfficherJeuxEmpruntes( nIdMembre ) ;

                ui->Bt_SupprimerMembre->setEnabled( true ) ;

                this->AfficherAbonnements( nIdMembre ) ;

                this->AfficherActivites( nIdMembre ) ;
            }

        }
        else //Sinon on affiche un message d'erreur et on retourne Faux
        {
            qDebug()<< "F_Membres::AfficherMembre :RequeteMembre " << RequeteMembre.lastQuery() <<  endl ;
        }
    }

}
//==========================================================================================================
/** Permet d'ajouter un membre
  *  @pre    Accès à la base de données
  *  @retval bool
  *  @return True si tous c'est bien passé et false en cas d'erreur
  */
bool F_Membres::AjouterMembre()
{
   qDebug()<< "F_Membres::AjouterMembre";

    bool bRetourne( true ) ;
    QSqlQuery RequeteMembre ;

    //Enregistrement d'un nouveau membre dans la base de données
    RequeteMembre.prepare( "INSERT INTO membres (TitreMembre_IdTitreMembre,TypeMembres_IdTypeMembres,"
        "Nom,Prenom,Rue,CP,Ville,Telephone,Mobile,Fax,Email,DomaineEmail_IdDomaineEmail,NbreJeuxAutorises,DateInscription,"
        "Remarque,Ecarte,CodeMembre,NbreRetard,NbrePersonne) "
        "VALUES (:TitreMembre_IdTitreMembre,:TypeMembres_IdTypeMembres,:Nom,:Prenom,:Rue,:CP,:Ville,"
        ":Telephone,:Mobile,:Fax,:Email,:DomaineEmail_IdDomaineEmail,:NbreJeuxAutorises,:DateInscription,:Remarque,"
        ":Ecarte,:CodeMembre,:NbreRetard,:NbrePersonne)" ) ;

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

    //Code Postal
    RequeteMembre.bindValue( ":CP",  ui->Le_CP->text() ) ;

    //Ville
    RequeteMembre.bindValue( ":Ville", ui->CBx_Ville->currentText() ) ;

    //Telephone
    RequeteMembre.bindValue( ":Telephone", ui->Le_TelFix->text() ) ;

    //Mobile
    RequeteMembre.bindValue( ":Mobile", ui->Le_TelMobile->text() ) ;

    //Fax
    RequeteMembre.bindValue( ":Fax", ui->LE_Fax->text() ) ;

    //Email
    RequeteMembre.bindValue( ":Email", QString(ui->LE_Email->text()+"@"+ui->CBx_DomaineEmail->currentText()) ) ;

    //Domaine Email
    RequeteMembre.bindValue( ":DomaineEmail_IdDomaineEmail", this->VectorDomaineEmail[ui->CBx_DomaineEmail->currentIndex()].id );

    //Nombre de jeux Autorisés
    RequeteMembre.bindValue( ":NbreJeuxAutorises", ui->SBx_JeuxAutorises->text().toInt() ) ;

    //Nombre de personne
    RequeteMembre.bindValue( ":NbrePersonne", ui->SBx_NbrePersonne->text().toInt() ) ;

    //Date d'inscription
    RequeteMembre.bindValue( ":DateInscription", ui->DtE_Insritption->date() ) ;

    //Remarque
    RequeteMembre.bindValue( ":Remarque", ui->TE_Remarque->toPlainText() ) ;

    //Ecarte
    RequeteMembre.bindValue( ":Ecarte", ui->ChBx_MembreEcarte->isChecked() ) ;

    //Code Membre
    RequeteMembre.bindValue( ":CodeMembre", ui->Le_Code->text() ) ;

    //Nombre de retards
    RequeteMembre.bindValue( ":NbreRetard", ui->SPx_NbreRetards->text() ) ;

    //Si le membre a bien été enregistré, this->nIdMembreSelectionne prend pour valeur l'id du membre créé
    if( RequeteMembre.exec() )
    {
        this->nIdMembreSelectionne = RequeteMembre.lastInsertId().toInt();

        QList <QStandardItem *> ListStandardItem;
        ListStandardItem.append(new QStandardItem(QString::number(this->nIdMembreSelectionne)));
        ListStandardItem.append(new QStandardItem(ui->Le_Nom->text()));
        ListStandardItem.append(new QStandardItem(ui->Le_Prenom->text()));
        ListStandardItem.append(new QStandardItem(ui->CBx_Ville->currentText()));
        ListStandardItem.append(new QStandardItem(ui->Le_Code->text()));

        AjouterAssocie(ListStandardItem);
    }
    else//Sinon on affiche un message d'erreur et on retourne Faux
    {
        qDebug()<< "F_Membres::AjouterMembre : RequeteMembre " << RequeteMembre.lastQuery()<< endl ;
        bRetourne = false ;
    }

    return bRetourne ;
}
//==========================================================================================================
/** Permet de modifier le membre dans la base de données
  *  @pre    Accés à la base de données, Base de données contenant minimum un membre
  *  @param  unsigned int nIdMembre
  *  @retval bool
  *  @return True si tous c'est bien passé et false en cas d'erreur
  *
  */
bool F_Membres::ModifierMembre( unsigned int nIdMembre )
{
   qDebug()<< "F_Membres::ModifierMembre";

    bool bRetourne ( true ) ;
    QSqlQuery RequeteMembre ;

    //On vérifie que l'id est présent dans la base donnée
    RequeteMembre.prepare( "SELECT IdMembre FROM membres WHERE IdMembre=:id" ) ;
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
        qDebug()<< "F_Membres::ModifierMembre : RequeteMembre " << RequeteMembre.lastQuery()<< endl ;
    }

    //S'il l'id est différent de 0 on ne mofifie pas le membre
    if( nIdMembre != 0 )
    {
        //Mise à des nouvelles informations sur le membre sélectionné dans la base de données
        RequeteMembre.prepare( "UPDATE membres SET TitreMembre_IdTitreMembre=:TitreMembre_IdTitreMembre,"
                               "TypeMembres_IdTypeMembres=:TypeMembres_IdTypeMembres,Nom=:Nom,"
                               "Prenom=:Prenom,Rue=:Rue,CP=:CP,Ville=:Ville,"
                               "Telephone=:Telephone,Mobile=:Mobile,Fax=:Fax,"
                               "Email=:Email,DomaineEmail_IdDomaineEmail=:DomaineEmail_IdDomaineEmail,"
                               "NbreJeuxAutorises=:NbreJeuxAutorises,"
                               "DateInscription=:DateInscription,NbreRetard=:NbreRetard,"
                               "DateNaissance=:DateNaissance,Remarque=:Remarque,"
                               "NbrePersonne=:NbrePersonne,Ecarte=:Ecarte,CodeMembre=:CodeMembre "
                               "WHERE IdMembre=:IdMembre" ) ;

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
        RequeteMembre.bindValue( ":CP", ui->Le_CP->text() ) ;

        //Ville
        RequeteMembre.bindValue( ":Ville", ui->CBx_Ville->currentText() ) ;

        //Telephone
        RequeteMembre.bindValue( ":Telephone", ui->Le_TelFix->text() ) ;

        //Mobile
        RequeteMembre.bindValue( ":Mobile", ui->Le_TelMobile->text() ) ;

        //Fax
        RequeteMembre.bindValue( ":Fax", ui->LE_Fax->text() ) ;

        //Email
        RequeteMembre.bindValue( ":Email", QString(ui->LE_Email->text()+"@"+ui->CBx_DomaineEmail->currentText()) ) ;

        //Domaine Email
        RequeteMembre.bindValue( ":DomaineEmail_IdDomaineEmail", this->VectorDomaineEmail[ui->CBx_DomaineEmail->currentIndex()].id ) ;

        //Nombre de jeux Autorisés
        RequeteMembre.bindValue( ":NbreJeuxAutorises", ui->SBx_JeuxAutorises->text().toInt() ) ;

        //Nombre de personne
        RequeteMembre.bindValue( ":NbrePersonne", ui->SBx_NbrePersonne->text().toInt() ) ;

        //Date d'inscription
        RequeteMembre.bindValue( ":DateInscription", ui->DtE_Insritption->date() ) ;

        //Remarque
        RequeteMembre.bindValue( ":Remarque", ui->TE_Remarque->toPlainText() ) ;

        //Ecarte
        RequeteMembre.bindValue( ":Ecarte", ui->ChBx_MembreEcarte->isChecked() ) ;

        //Code Membre
        RequeteMembre.bindValue( ":CodeMembre", ui->Le_Code->text() ) ;

        //Nombre de retards
        RequeteMembre.bindValue( ":NbreRetard", ui->SPx_NbreRetards->value() ) ;

        //Si le membre n'a pas été enregisté on indique l'erreur qu'il y a une erreur et on retourne l'erreur de sql
        if( RequeteMembre.exec()== false )
        {
            qDebug()<< "F_Membres::ModifierMembre : RequeteMembre" << endl <<  RequeteMembre.lastQuery()<< endl ;
            bRetourne = false ;
        }
    }
    else
    {
        bRetourne = false ;
    }
    return bRetourne ;
}
//==========================================================================================================
/** Permet la suppression du dont l'id est passé en paramêtre
 *  @pre    Connexion à la base de données, Membre présent dans la base de données
 *  @param  Indique l'id du membre à supprimer
 *  @retval bool
 *  @return Retourne vrai si la requête a bien été exécuter sinon elle retourne faux
 */
bool F_Membres::SupprimerMembre( int nIdMembre )
{
   qDebug()<< "F_Membres::SupprimerMembre";

    bool bRetourne ( true ) ;
    QSqlQuery RequeteSupprimer ;
    QSqlQuery RequeteEmprunts ;

    //Préparation de la requête la recherche des emprunts
    RequeteEmprunts.prepare( "SELECT IdEmprunts FROM emprunts WHERE Membres_IdMembre=:IdMembre AND DateRetour IS NULL" ) ;
    RequeteEmprunts.bindValue( ":IdMembre", nIdMembre ) ;

    if( !RequeteEmprunts.exec() )
    {
        qDebug()<< "F_AdministrerMembres::SupprimerMembre : RequeteEmprunts " << RequeteEmprunts.lastQuery()<< endl ;
        bRetourne = false ;
    }

    //S'il n'y a pas d'emprunt en cours
    if( RequeteEmprunts.size()== 0 )
    {
        //Vérification que la personne veux bien supprimer le membre
        if ( QMessageBox::information( this, "Suppression Membre","Voulez vraiment supprimer ce membre ?", "Supprimer", "Annuler" ) == 0 )
        {
            //Préparation de la requête permettant la suppression dans la table de membre --------------------------
            RequeteSupprimer.prepare( "DELETE FROM membres WHERE IdMembre=:IdMembre" ) ;
            RequeteSupprimer.bindValue( ":IdMembre", nIdMembre ) ;

            if( !RequeteSupprimer.exec() )
            {
                qDebug()<< "F_AdministrerMembres::SupprimerMembre : RequeteSupprimer : Erreur de connexion avec la base de donne !" << RequeteSupprimer.lastQuery()<< endl ;
                bRetourne = false ;
            }

            //Préparation de la requête permettant la suppression dans la table reservation -------------------------------
            RequeteSupprimer.prepare( "DELETE FROM reservation WHERE Membres_IdMembre=:IdMembre" ) ;
            RequeteSupprimer.bindValue( ":IdMembre", nIdMembre ) ;

            //Execution de la requête, si elle fonctionne on met la variable de retoure à vrai
            if( !RequeteSupprimer.exec() )
            {
                qDebug()<< "F_AdministrerMembres::SupprimerMembre : RequeteSupprimer " << RequeteSupprimer.lastQuery()<< endl ;
                bRetourne = false ;
            }

            //Préparation de la requête permettant la suppression dans la table emprunts ------------------------
            RequeteSupprimer.prepare( "DELETE FROM emprunts WHERE Membres_IdMembre=:IdMembre" ) ;
            RequeteSupprimer.bindValue( ":IdMembre", nIdMembre ) ;

            //Execution de la requête, si elle fonctionne on met la variable de retoure à vrai
            if( !RequeteSupprimer.exec() )
            {
                qDebug()<< "F_AdministrerMembres::SupprimerMembre : RequeteSupprimer " << RequeteSupprimer.lastQuery()<< endl ;
                bRetourne = false ;
            }

            //Préparation de la requête permettant la suppression dans la table abonnements-----------------------
            RequeteSupprimer.prepare( "DELETE FROM abonnements WHERE Membres_IdMembre=:IdMembre" ) ;
            RequeteSupprimer.bindValue( ":IdMembre", nIdMembre ) ;

            //Execution de la requête, si elle fonctionne on met la variable de retoure à vrai
            if( !RequeteSupprimer.exec() )
            {
                qDebug()<< "F_AdministrerMembres::SupprimerMembre : RequeteSupprimer " << RequeteSupprimer.lastQuery()<< endl ;
                bRetourne = false ;
            }
            //Préparation de la requête permettant la suppression dans la table abonnements-----------------------
            RequeteSupprimer.prepare( "DELETE FROM abonnements WHERE Membres_IdMembre=:IdMembre" ) ;
            RequeteSupprimer.bindValue( ":IdMembre", nIdMembre ) ;

            //Execution de la requête, si elle fonctionne on met la variable de retoure à vrai
            if( !RequeteSupprimer.exec() )
            {
                qDebug()<< "F_AdministrerMembres::SupprimerMembre : RequeteSupprimer " << RequeteSupprimer.lastQuery()<< endl ;
                bRetourne = false ;
            }
            //Préparation de la requête permettant la suppression dans la table abonnements-----------------------
            RequeteSupprimer.prepare( "DELETE FROM activitemembre WHERE Membres_IdMembre=:IdMembre" ) ;
            RequeteSupprimer.bindValue( ":IdMembre", nIdMembre ) ;

            //Execution de la requête, si elle fonctionne on met la variable de retoure à vrai
            if( !RequeteSupprimer.exec() )
            {
                qDebug()<< "F_AdministrerMembres::SupprimerMembre : RequeteSupprimer " << RequeteSupprimer.lastQuery()<< endl ;
                bRetourne = false ;
            }
            //Préparation de la requête permettant la suppression dans la table abonnements-----------------------
            RequeteSupprimer.prepare( "DELETE FROM membresassocies WHERE Membres_IdMembre=:IdMembre OR "
                                      "Membres_IdCollectivite=:IdMembre") ;
            RequeteSupprimer.bindValue( ":IdMembre", nIdMembre ) ;

            //Execution de la requête, si elle fonctionne on met la variable de retoure à vrai
            if( !RequeteSupprimer.exec() )
            {
                qDebug()<< "F_AdministrerMembres::SupprimerMembre : RequeteSupprimer " << RequeteSupprimer.lastQuery()<< endl ;
                bRetourne = false ;
            }        }
        else
        {
            bRetourne = false ;
        }
    }
    else
    {
        bRetourne = false ;
        QMessageBox::information( this, "Suppression Membre","Impossible de supprimer ce membre.\n"
                                  "Il a des jeux encore en cours d'emprunts.",  "Ok" ) ;
    }

    this->MaJListeMembres() ;
    this->AfficherListe() ;

    return bRetourne ;
}
//==========================================================================================================
/** Verrouille ou deverouille les champs contenant les informations personnelles
 *  @param  bool bVerrouille
 */
void F_Membres::VerrouillerInfosPerso ( bool bVerrouille )
{
    ui->CBx_Type->setDisabled( bVerrouille ) ;
    ui->CBx_Titre->setDisabled( bVerrouille ) ;
    ui->CBx_DomaineEmail->setDisabled( bVerrouille );

    ui->Le_Nom->setReadOnly( bVerrouille ) ;
    ui->Le_Prenom->setReadOnly( bVerrouille ) ;
    ui->Le_TelFix->setReadOnly( bVerrouille ) ;
    ui->Le_TelMobile->setReadOnly( bVerrouille ) ;
    ui->LE_Fax->setReadOnly( bVerrouille ) ;
    ui->LE_Email->setReadOnly( bVerrouille ) ;
    ui->Le_Code->setReadOnly( bVerrouille ) ;
    ui->Te_Rue->setReadOnly( bVerrouille ) ;
    ui->Le_CP->setReadOnly( bVerrouille ) ;
    ui->CBx_Ville->setDisabled( bVerrouille ) ;
    ui->SBx_JeuxAutorises->setReadOnly( bVerrouille ) ;
    ui->SBx_NbrePersonne->setReadOnly( bVerrouille ) ;
    ui->SPx_NbreRetards->setReadOnly( bVerrouille ) ;
    ui->TE_Remarque->setReadOnly( bVerrouille ) ;
    ui->DtE_Insritption->setReadOnly( bVerrouille ) ;
    ui->ChBx_MembreEcarte->setVisible( !bVerrouille ) ;
}
//==========================================================================================================
/** Efface le contenu de tous les champs
 */
void F_Membres::EffacerTousLesChamps ()
{
    ui->CBx_Type->setCurrentIndex( 0 ) ;
    ui->CBx_Titre->setCurrentIndex( 0 ) ;
    ui->CBx_DomaineEmail->setCurrentIndex( 0 ) ;

    ui->Le_Nom->clear() ;
    ui->Le_Prenom->clear() ;
    ui->Le_TelFix->clear() ;
    ui->Le_TelMobile->clear() ;
    ui->LE_Fax->clear() ;
    ui->LE_Email->clear() ;
    ui->Le_Code->clear() ;
    ui->Te_Rue->clear() ;
    ui->Le_CP->clear() ;
    ui->CBx_Ville->setCurrentIndex( 0 ) ;
    ui->SBx_JeuxAutorises->clear() ;
    ui->SBx_NbrePersonne->clear() ;
    ui->SPx_NbreRetards->clear() ;
    ui->DtE_Insritption->clear() ;
    ui->TE_Remarque->clear() ;
    QStandardItemModel* modeleVide = new QStandardItemModel() ;
    ui->LW_JeuxEmpruntes->setModel( modeleVide ) ;
    ui->LW_Abonnements->setModel( modeleVide ) ;
    ui->Tw_activites->clearContents();
    ui->Tw_activites->setRowCount(0);

    ui->Lb_MembreEcarte->setHidden( true ) ;
}
//==========================================================================================================
/** Affiche ou cache les boutons PB_AjouterMembre et PB_ModifierMembre
 */
void F_Membres::AfficherAjouterModifierMembre ( bool bAffiche )
{
    // VV : Ne voit pas l'utilité de masquer le bouton de création de membre
    //ui->Bt_AjouterMembre->setVisible( bAffiche ) ;
    if ( iMode == MODE_ADMIN )
    {
        ui->Bt_SupprimerMembre->setVisible( bAffiche ) ;
    }
}
//==========================================================================================================
/** Affiche ou cache les boutons PB_Valider et PB_Annuler
 */
void F_Membres::AfficherValiderAnnuler ( bool bAffiche )
{
    ui->Bt_ValiderMembre->setVisible( bAffiche ) ;
    ui->Bt_AnnulerMembre->setVisible( bAffiche ) ;
    ui->ChBx_MembreEcarte->setVisible( bAffiche ) ;
    ui->Lb_MembreEcarte->setVisible( bAffiche ) ;
}
//==========================================================================================================
/** Verrouille ou deverouille l'emplacement des emprunts
 */
void F_Membres::VerrouillerJeux ( bool bVerrouille )
{
    ui->LW_JeuxEmpruntes->setDisabled( bVerrouille ) ;
    ui->Bt_Historique->setDisabled( bVerrouille ) ;
}
//==========================================================================================================
/** Verouille ou deverouille l'emplacement des abonnements
 */
void F_Membres::VerrouillerAbonnements ( bool bVerrouille )
{
    ui->LW_Abonnements->setDisabled( bVerrouille ) ;
    ui->Bt_AjouterAbonnement->setDisabled( bVerrouille ) ;
    ui->Bt_ModifierAbonnement->setDisabled( bVerrouille ) ;
    ui->Bt_SupprimerAbonnement->setDisabled( bVerrouille ) ;
}
//==========================================================================================================
/** Verouille ou deverouille l'emplacement des activités
 */
void F_Membres::VerrouillerActivite ( bool bVerrouille )
{
    ui->Bt_AjouterActivite->setDisabled( bVerrouille ) ;
    ui->Bt_SupprimerActivite->setDisabled( bVerrouille ) ;
}
//==========================================================================================================
/** Permet l'affichage d'un membre
 *  @pre    Connexion à la base de données, Membre présent dans la base de données
 *  @param  Indique l'id du membre pour afficher ses abonnements
 */
void F_Membres::AfficherAbonnements( int nIdMembre )
{
   qDebug()<< "F_Membres::AfficherAbonnements";

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
    modele->setHorizontalHeaderItem( 3, new QStandardItem( "EstCeAbo" ) ) ;

    ui->LW_Abonnements->setModel( modele ) ;
    ui->LW_Abonnements->setEditTriggers( 0 ) ;
    ui->LW_Abonnements->verticalHeader()->setVisible( false ) ;
    ui->LW_Abonnements->horizontalHeader()->setVisible( true ) ;
    ui->LW_Abonnements->setColumnWidth( 0, 138 ) ;  // Abonnement
    ui->LW_Abonnements->setColumnWidth( 1,  40 ) ;  // Crédit
    ui->LW_Abonnements->setColumnWidth( 2, 100 ) ;  // Date
    ui->LW_Abonnements->setColumnWidth( 3, 0 ) ;  // Est-ce un abonnement ?

    //Préparation de la requête
    RequetePrestation.prepare( "SELECT prestations.NomPrestation,abonnements.DateExpiration,abonnements.IdAbonnements "
                               "FROM abonnements,prestations "
                               "WHERE abonnements.Membres_IdMembre=:IdMembre "
                               "AND prestations.IdPrestation=abonnements.Prestations_IdPrestation AND "
                               "supprimer=0" ) ;
    RequetePrestation.bindValue( ":IdMembre", nIdMembre ) ;

    //Exectution de la requête
    if( RequetePrestation.exec() )
    {
        while( RequetePrestation.next() )
        {
            modele->setItem( i, 0, new QStandardItem( RequetePrestation.record().value( 0 ).toString() ) ) ;
            modele->setItem( i, 2, new QStandardItem( RequetePrestation.record().value( 1 ).toDateTime().toString( "dd-MM-yyyy" ) ) ) ;
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
            modele->setData( modele->index( i, 3 ),true) ;

            i++ ;
        }
    }
    else //Sinon on affiche un message d'erreur et on retourne Faux
    {
        qDebug()<< "F_Membres::AfficherAbonnements : RequetePrestation "<< RequetePrestation.lastQuery()<< endl ;
    }

    //Préparation de la requête
    RequeteCartes.prepare( "SELECT cartesprepayees.NomCarte,abonnements.DateExpiration,abonnements.CreditRestant,"
                           "abonnements.IdAbonnements FROM abonnements,cartesprepayees "
                           "WHERE abonnements.Membres_IdMembre=:IdMembre AND "
                           "cartesprepayees.IdCarte=abonnements.CartesPrepayees_IdCarte AND Supprimer=0" ) ;
    RequeteCartes.bindValue( ":IdMembre", nIdMembre ) ;

    //Exectution de la requête
    if( RequeteCartes.exec() )
    {
        while( RequeteCartes.next() )
        {
            modele->setItem( i, 0, new QStandardItem( RequeteCartes.record().value( 0 ).toString() ) ) ;
            modele->setItem( i, 1, new QStandardItem( RequeteCartes.record().value( 2 ).toString() ) ) ;
            modele->setItem( i, 2, new QStandardItem( RequeteCartes.record().value( 1 ).toDateTime().toString( "dd-MM-yyyy" ) ) ) ;
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
            modele->setData( modele->index( i, 3 ),false) ;

            i++ ;
        }
    }
    else //Sinon on affiche un message d'erreur et on retourne Faux
    {
        qDebug()<< "F_Membres::AfficherAbonnements : RequeteCartes "<< RequeteCartes.lastQuery()<< endl ;
    }

}
//==========================================================================================================
/** Affiche les villes dans le combobox et sélectionne la ville passée en paramétre
 *  @param  QString VilleSelectionne
 */
void F_Membres::AfficherVilles( QString VilleSelectionne )
{
   qDebug()<< "F_Membres::AfficherVilles";

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
        qDebug()<< "F_Membres::AfficherVilles : RequeteVille "<< Requete.lastQuery();
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

/**
 *  @brief Mets en majuscule le texte saisi dans un QLineEdit
 *
 *  @param text
 */

void F_Membres::toUpper(const QString &text)
{
    QLineEdit *le = qobject_cast<QLineEdit *>(sender());
    if (!le)
    return;
    le->setText(text.toUpper());
}

//==========================================================================================================
/** Récupère l'id d'un membre pour l'afficher
 *  @pre    Recevoir un signal avec l'id d'un membre
 *  @param  unsigned int nIdMembre
 */
void F_Membres::slot_AfficherMembre( unsigned int nIdMembre )
{
    this->VerrouillerAbonnements( false ) ;
    this->VerrouillerActivite(false);
    ui->Bt_Associe->setDisabled(false);
    this->VerrouillerJeux( false ) ;
    this->AfficherMembre( nIdMembre ) ;
}

//==========================================================================================================
/** Permet l'ajout d'un type
 */
void F_Membres::slot_ChoisirNouveauType ()
{
    this->MaJType() ;
    ui->CBx_Type->setCurrentIndex( this->VectorType.size()- 2 ) ;
}
//==========================================================================================================
/** Permet l'ajout d'un domaine d'email
 */
void F_Membres::slot_ChoisirNouveauDomaineEmail ()
{
    this->MaJDomaineEmail() ;
    ui->CBx_DomaineEmail->setCurrentIndex( this->VectorDomaineEmail.size()- 2 ) ;
}

//==========================================================================================================
/** Permet l'ajout d'un titre
 */
void F_Membres::slot_ChoisirNouveauTitre ()
{
    this->MaJTitre() ;
    ui->CBx_Titre->setCurrentIndex( this->VectorTitre.size()- 2 ) ;
}
//==========================================================================================================
/** Acualise les abonnements du membre sélectionné
 *  @pre    Recevoir un signal avec l'id d'un membre
 *  @param  unsigned int nIdMembre
 */
void F_Membres::slot_ActualiserAbonnements()
{
    this->AfficherAbonnements( this->nIdMembreSelectionne ) ;
}
//==========================================================================================================
/** Permet l'ajout d'une ville
 */
void F_Membres::slot_ValiderAjoutVille()
{
    ui->CBx_Ville->insertItem( ui->CBx_Ville->count(), this->LE_AjoutVille->text() ) ;
    ui->CBx_Ville->setCurrentIndex( ui->CBx_Ville->count() - 1 );
    this->LE_AjoutVille->clear() ;
    this->oFenetreAjoutVille->hide() ;
}
//==========================================================================================================
/** Permet l'annulation de l'ajour d'une ville
 */
void F_Membres::slot_AnnulerAjoutVille()
{
    this->LE_AjoutVille->clear() ;
    this->oFenetreAjoutVille->hide() ;
}
//==========================================================================================================
/** Met en forme (du type "00 00 00 00 00")  le numéro de télephone quand il est modifié
 */
void F_Membres::on_Le_TelFix_textEdited( const QString &arg1 )
{
    ui->Le_TelFix->setText( this->ModifierSyntaxeNumTelephone( arg1 ) ) ;
}
//==========================================================================================================
/** Met en forme (du type "00 00 00 00 00")  le numéro de télephone quand il est modifié
 */
void F_Membres::on_Le_TelMobile_textEdited( const QString &arg1 )
{
    ui->Le_TelMobile->setText( this->ModifierSyntaxeNumTelephone( arg1 ) ) ;
}
//==========================================================================================================
/** Met en forme (du type "00 00 00 00 00")  le numéro de télephone quand il est modifié
 */
void F_Membres::on_LE_Fax_textEdited( const QString &arg1 )
{
    ui->LE_Fax->setText( this->ModifierSyntaxeNumTelephone( arg1 ) ) ;
}
//==========================================================================================================
/** Active ou désactive le bouton valider en fonction de lineedit nom
 */
void F_Membres::on_Le_Nom_textEdited( const QString &arg1 )
{
    if( arg1 == "" )
    {
        ui->Bt_ValiderMembre->setDisabled( true ) ;
    }
    else
    {
        ui->Bt_ValiderMembre->setEnabled( true ) ;
    }
}
//==========================================================================================================
/**  Permet de modifier un abonnement
 *  @param index : numéro de la ligne qui a été sélectionnée
 */
void F_Membres::on_LW_Abonnements_doubleClicked(const QModelIndex &index)
{
    // Si ce n'est pas un abonnement
    if(!ui->LW_Abonnements->model()->index(index.row(),3).data().toBool())
    {
        this->on_Bt_ModifierAbonnement_clicked();
    }
}
//==========================================================================================================
/** Selectionne une ligne entière dans le tableau LW_Abonnememnt
 *
 */
void F_Membres::on_LW_Abonnements_clicked( const QModelIndex &index )
{
    // Si c'est un abonnement
    if(ui->LW_Abonnements->model()->index(index.row(),3).data().toBool())
    {
        ui->Bt_ModifierAbonnement->setEnabled( false );
    }
    else
    {
        ui->Bt_ModifierAbonnement->setEnabled( true );
    }
    ui->Bt_SupprimerAbonnement->setEnabled( true );
}
//==========================================================================================================
/** Selectionne une ligne entière dans le tableau LW_Empruntes
 */
void F_Membres::on_LW_JeuxEmpruntes_clicked( const QModelIndex &index )
{
}
//==========================================================================================================
/** Affiche ou cache membre écarté
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
//==========================================================================================================
/**  Change la couleur et met en gras un membre écarté quand le checkbox est coché
 *
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
//==========================================================================================================
/**  Permet l'ajout de Ajouter une ville dans le combox Ville
 *
 */
void F_Membres::on_CBx_Ville_currentIndexChanged(const QString &arg1)
{
    if( arg1 == "Ajouter une ville ..." )
    {
        this->oFenetreAjoutVille->show() ;
    }
}
//==========================================================================================================
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
        this->pTypeAjMod->Ajouter(5) ;
        ui->CBx_Type->setCurrentIndex( 0 ) ;
    }
}

void F_Membres::on_CBx_Titre_activated(int index)
{

}
//==========================================================================================================
/** Permet l'ajout d'un membre
 *
 */
void F_Membres::on_Bt_AjouterMembre_clicked()
{
    qDebug()<< "F_Membres::on_Bt_AjouterMembre_clicked";

    QString sNombre      ;
    QDate   DateActuelle ;
    QStandardItemModel * modeleVide ;

    if(this->iMode==MODE_MEMBRE_ASSOCIE)
    {
        ui->frame->setVisible(true);
        ui->frame_2->setVisible(true);
        this->showMaximized();
    }

    this->nIdMembreSelectionne = 0 ;

    this->AfficherVilles( "" ) ;

    this->AfficherAjouterModifierMembre( false ) ;

    this->EffacerTousLesChamps() ;

    this->AfficherValiderAnnuler( true ) ;

    this->VerrouillerInfosPerso( false ) ;

    this->VerrouillerJeux( true ) ;

    //Sélectionne le premier CodeMembre qui est libre (supérieur au plus grand)
    QSqlQuery RequetePremierCodeLibre ;
    if ( !RequetePremierCodeLibre.exec("SELECT MIN(CodeMembre+1) FROM membres WHERE (CodeMembre+1) NOT IN (SELECT CodeMembre FROM membres)"))
    {
       qDebug()<< "F_Membres::AfficherVilles : on_Bt_AjouterMembre_clicked "<< RequetePremierCodeLibre.lastQuery();
    }
    RequetePremierCodeLibre.next() ;
    ui->Le_Code->setText( RequetePremierCodeLibre.value(0).toString()) ;

    ui->DtE_Insritption->setDate( DateActuelle.currentDate() ) ;

    ui->Bt_ValiderMembre->setDisabled( true ) ;

    ui->Lb_MembreEcarte->hide() ;

    ui->ChBx_MembreEcarte->hide() ;

    //ui->SPx_NbreRetards->setDisabled( true ) ;

    this->VerrouillerAbonnements( true );

    this->VerrouillerActivite( true ) ;

    ui->Bt_Associe->setDisabled(true);

    ui->Le_Nom->setFocus();
}
//==========================================================================================================
/** Annule l'ajout ou la modification d'un membre
 *
 */
void F_Membres::on_Bt_AnnulerMembre_clicked()
{
    this->AfficherMembre( this->nIdMembreSelectionne );
    this->AfficherAjouterModifierMembre( true ) ;
}
//==========================================================================================================
/** Valide la modification ou l'ajout d'un membre
 *
 */
void F_Membres::on_Bt_ValiderMembre_clicked()
{
    if( this->nIdMembreSelectionne != 0 && ui->Tw_activites->rowCount() == 0)
    {
        QMessageBox::information( this, "Manque activité","Vous devez choisir au moins une activité.",  "Ok" ) ;
        return;
    }
   qDebug()<< "F_Membres::on_Bt_ValiderMembre_clicked";

    QSqlQuery RequeteCodeMembre ;

    RequeteCodeMembre.prepare( "SELECT IdMembre FROM membres "
                               "WHERE CodeMembre=:CodeMembre" ) ;
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
                    this->MaJListeMembres() ;
                    this->AfficherListe() ;
                    this->VerrouillerAbonnements( false ) ;
                    this->VerrouillerJeux( false ) ;
                    this->VerrouillerActivite(false);
                    ui->Bt_Associe->setDisabled(false);
                    //this->VerrouillerInfosPerso( true ) ;
                    //this->AfficherValiderAnnuler( false ) ;
                    this->AfficherAjouterModifierMembre( true ) ;
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
                this->MaJListeMembres() ;
                this->AfficherListe() ;
                this->AfficherMembre( this->nIdMembreSelectionne ) ;
            }
            else
            {
                if( RequeteCodeMembre.record().value(0).toUInt() == this->nIdMembreSelectionne )
                {
                    this->ModifierMembre( this->nIdMembreSelectionne ) ;
                    this->MaJListeMembres() ;
                    this->AfficherListe() ;
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
        qDebug() << "F_Membres::AjouterMembre() : RequeteCodeMembre :" << RequeteCodeMembre.lastQuery()  ;
    }
}
//==========================================================================================================
/** Affiche l'historique des jeux empruntés
 *
 */
void F_Membres::on_Bt_Historique_clicked()
{
    this->pHistoriqueJeux->AfficherHistorique( this->nIdMembreSelectionne ) ;
    this->pHistoriqueJeux->show() ;
}
//==========================================================================================================
/** Permet de modifier un membre
 *
 */
void F_Membres::on_Bt_ModifierMembre_clicked()
{
    this->VerrouillerInfosPerso( false ) ;
    this->AfficherValiderAnnuler( true ) ;
    this->AfficherAjouterModifierMembre( false ) ;
    ui->Bt_ValiderMembre->setText( "Valider Modifications" ) ;
    ui->LE_Email->setFocus();
}
//==========================================================================================================
/** Validation suppression d'un membre
 *
 */
void F_Membres::on_Bt_SupprimerMembre_clicked()
{
    QStandardItemModel * modeleVide ;

    if( this->SupprimerMembre( this->nIdMembreSelectionne ) )
    {
        this->MaJListeMembres() ;
        this->AfficherListe() ;
        this->AfficherMembre( 0 ) ;
        ui->Bt_SupprimerMembre->setDisabled( true ) ;
        ui->LW_Abonnements->clearSpans() ;
        ui->LW_JeuxEmpruntes->clearSpans() ;

        modeleVide = new QStandardItemModel() ;
        ui->LW_JeuxEmpruntes->setModel( modeleVide ) ;
        ui->LW_Abonnements->setModel( modeleVide ) ;
    }
}
//==========================================================================================================
/** Permet l'ajout d'un abonnement
 *
 */
void F_Membres::on_Bt_AjouterAbonnement_clicked()
{
    this->pAjouterCotiCarte->MaJListeAbonnements() ;
    this->pAjouterCotiCarte->AjouterAbonnement( this->nIdMembreSelectionne ) ;
}
//==========================================================================================================
/** Permet la modification d'un abonnement
 *
 */
void F_Membres::on_Bt_ModifierAbonnement_clicked()
{
    this->pAjouterCotiCarte->MaJListeAbonnements() ;
    this->pAjouterCotiCarte->ModifierAbonnement( this->VecteurAbonnements.at( ui->LW_Abonnements->currentIndex().row() ) ) ;

   // masquer les boutons Modifier/Supprimer abonnement
    ui->Bt_ModifierAbonnement->setEnabled(false);
    ui->Bt_SupprimerAbonnement->setEnabled(false);
}
//==========================================================================================================
/**  Permet de supprimer un abonnement
 *
 */
void F_Membres::on_Bt_SupprimerAbonnement_clicked()
{
   qDebug()<< "F_Membres::on_Bt_SupprimerAbonnement_clicked";

    QSqlQuery RequeteSupprimer ;

    if ( QMessageBox::information( this, "Suppression d'un abonnement","Voulez vraiment supprimer cet abonnement ?", "Supprimer", "Garder" )== 0 )
    {
        //Préparation de la requête permettant la suppression dans la table abonnements
        RequeteSupprimer.prepare( "UPDATE abonnements SET Supprimer=1 WHERE IdAbonnements=:nIdAbonnement" ) ;
        RequeteSupprimer.bindValue( ":nIdAbonnement", this->VecteurAbonnements.at( ui->LW_Abonnements->currentIndex().row() ) ) ;

        //Execution de la requête
        if( !RequeteSupprimer.exec() )
        {
            qDebug()<< "F_AdministrerMembres::SupprimerMembre : RequeteSupprimer " << RequeteSupprimer.lastQuery()<< endl ;
        }
        else
        {
           this->AfficherAbonnements( this->nIdMembreSelectionne ) ;
           // masquer les boutons Modifier/Supprimer abonnement
           ui->Bt_ModifierAbonnement->setDisabled( true ) ;
           ui->Bt_SupprimerAbonnement->setDisabled( true ) ;
        }
    }
}

//==========================================================================================================
/** Permet d'ajouter une activité
 *
 */
void F_Membres::on_Bt_AjouterActivite_clicked()
{
    // Retrouve l'id d'une activité stockée dans l'objet combobox
    int index = ui->CBx_Activites->currentIndex();
    int nIdActivite = ui->CBx_Activites->itemData(index,Qt::UserRole).toInt();

    // Vérifie si l'activité n'est pas déjà présente dans la tablewidget
    if(ui->Tw_activites->findItems(ui->CBx_Activites->currentText(),Qt::MatchExactly).size() != 0)
    {
        QMessageBox::information( this, "Activité déjà existante","Ce membre possède déjà cette activité");
        return;
    }

    QSqlQuery RequeteActivites;

    RequeteActivites.prepare(
                "INSERT INTO activitemembre (Activite_IdActivite, Membres_IdMembre, Newsletter) "
                "VALUES (:nIdActivite, :nIdMembre, 1)" ) ;
    RequeteActivites.bindValue( ":nIdMembre", this->nIdMembreSelectionne );
    RequeteActivites.bindValue( ":nIdActivite", nIdActivite );

    //Exectution de la requête
    if( !RequeteActivites.exec() )
    {
        qDebug()<< "F_Membres::AfficherActivite : RequeteActivite " << RequeteActivites.lastQuery() ;
    }
    else
    {
        AfficherActivites(this->nIdMembreSelectionne);
    }
}

//==========================================================================================================
/** Active la newsletter d'une des activités
 *
 */
void F_Membres::on_ChBx_Activites_clicked()
{
    QSqlQuery RequeteActivites;
    QCheckBox * checkbox = (QCheckBox *) QObject::sender();
    QString nIdActivite=checkbox->property("IdActivite").toString();

    // Ajoute une nouvelle entrée ou mets à jour l'entrée existante si elle existe déjà avec ou sans la newsletter d'actif
    RequeteActivites.prepare(
                "INSERT INTO activitemembre (Activite_IdActivite, Membres_IdMembre, NewsLetter) "
                "VALUES (:nIdActivite, :nIdMembre, :Newsletter) ON DUPLICATE KEY UPDATE NewsLetter=:Newsletter" );
    RequeteActivites.bindValue( ":nIdMembre", this->nIdMembreSelectionne );
    RequeteActivites.bindValue( ":nIdActivite", nIdActivite );
    RequeteActivites.bindValue( ":Newsletter", (checkbox->checkState() == Qt::Checked) );

    //Exectution de la requête
    if( !RequeteActivites.exec() )
    {
        qDebug()<< "F_Membres::on_ChBx_Activites_clicked : RequeteActivite " << RequeteActivites.lastQuery() ;
    }
}

//==========================================================================================================
/** Permet de supprimer une activité
 *
 */
void F_Membres::on_Bt_SupprimerActivite_clicked()
{
    // Vérifie si une activité a été sélectionnée dans la tablewidget
    if( ui->Tw_activites->selectedItems().size() == 0)
    {
        QMessageBox::information( this, "Aucune activité de sélectionnée",
                                  "Vous devez sélectionner une activité pour pouvoir la supprimer.", "Ok" );
        return;
    }

    QSqlQuery RequeteActivites;

    // Récupére l'ID d'une activité
    QString Activite = ui->Tw_activites->selectedItems()[0]->text();
    int nIdActivite = ui->Tw_activites->property(Activite.toUtf8().constData()).toInt();

    // Supprime une activité
    RequeteActivites.prepare("DELETE FROM activitemembre WHERE Activite_IdActivite=:nIdActivite AND "
                "Membres_IdMembre=:nIdMembre" );
    RequeteActivites.bindValue( ":nIdMembre", this->nIdMembreSelectionne );
    RequeteActivites.bindValue( ":nIdActivite", nIdActivite );

    //Exectution de la requête
    if( !RequeteActivites.exec() )
    {
        qDebug()<< "F_Membres::on_ChBx_Activites_clicked : RequeteActivite " << RequeteActivites.lastQuery() ;
    }
    AfficherActivites(this->nIdMembreSelectionne);
}

//---------------------------------------------------------------------------
// SLOTS PRIVEES
//---------------------------------------------------------------------------
/** Sélectionne un membre avec un double click
 *  @pre    Double clique sur un membre
 *  @param  const QModelIndex &index
 *  @test   Voir la procédure dans le fichier associé.
 */
void F_Membres::on_TbW_Recherche_doubleClicked(const QModelIndex &index)
{
    on_TbW_Recherche_clicked(index);
}

/** Sélectionne un membre avec un click
 *  @param  const QModelIndex &index
 *  @test   Voir la procédure dans le fichier associé.
 */
void F_Membres::on_TbW_Recherche_clicked(const QModelIndex &index)
{
    if(this->VecteurRechercheMembres.count()!=0)
    {
        slot_AfficherMembre(this->VecteurRechercheMembres[index.row()].id);
    }
}

/** Recherche les membres correspondant au champs chaque fois que le champs est modifié
*  @param  const QString &arg1
*  @test   Voir la procédure dans le fichier associé.
*/
void F_Membres::on_LE_Nom_textEdited(const QString &arg1)
{
   // Effectue la recherche
   this->RechercherParNomEtNumero() ;
   // Sélectionne le premier résultat
   ui->TbW_Recherche->selectRow( 0 ) ;
}

void F_Membres::on_Bt_Associe_clicked()
{
    pAssocies = new F_Membres(MODE_MEMBRE_ASSOCIE,0,this->nIdMembreSelectionne);
}

void F_Membres::on_Bt_AJouterAssocie_clicked()
{
    if(ModeleAssocies.findItems(QString::number(this->nIdMembreSelectionne),Qt::MatchExactly,0).count()!=0)
    {
        return;
    }
    QModelIndexList ModelIndexList;
    QList <QStandardItem *> ListStandardItem;
    ListStandardItem.append(new QStandardItem(QString::number(this->nIdMembreSelectionne)));
    for(int i=0;i<ModeleRechercheMembre.columnCount();i++)
    {
        ModelIndexList=ui->TbW_Recherche->selectionModel()->selectedRows(i);
        ListStandardItem.append(new QStandardItem(ModelIndexList.first().data().toString()));
    }
    AjouterAssocie(ListStandardItem);
}

void F_Membres::AjouterAssocie(QList <QStandardItem *> ListStandardItem)
{
    ModeleAssocies.appendRow(ListStandardItem);
    QSqlQuery RequeteAssocie ;
    //Enregistrement d'un nouveau membre dans la base de données
    RequeteAssocie.prepare( "INSERT INTO membresassocies "
        "(Membres_IdCollectivite,Membres_IdMembre) VALUES (:IdCollectivite,:IdMembre)");
    RequeteAssocie.bindValue(":IdCollectivite",this->nIdCollectivite);
    RequeteAssocie.bindValue( ":IdMembre", ListStandardItem.at(0)->text() );

    //Exectution de la requête
    if( !RequeteAssocie.exec() )
    {
        qDebug()<< "F_Membres::on_Bt_ValiderAssocies_clicked : RequeteMembre " << RequeteAssocie.lastQuery() ;
    }
    ui->TbW_Associes->selectRow(0);
    ui->Bt_SupprimerAssocie->setEnabled(true);
}

void F_Membres::on_Bt_SupprimerAssocie_clicked()
{
    int row=ui->TbW_Associes->selectionModel()->selectedRows().first().row();
    QSqlQuery RequeteAssocie ;
    //Enregistrement d'un nouveau membre dans la base de données
    RequeteAssocie.prepare( "DELETE FROM membresassocies WHERE Membres_IdCollectivite=:IdCollectivite AND "
                            "Membres_IdMembre=:IdMembre");
    RequeteAssocie.bindValue(":IdCollectivite",this->nIdCollectivite);
    RequeteAssocie.bindValue( ":IdMembre", ModeleAssocies.index(row, 0 ).data().toString() );

    //Exectution de la requête
    if( !RequeteAssocie.exec() )
    {
        qDebug()<< "F_Membres::on_Bt_ValiderAssocies_clicked : RequeteMembre " << RequeteAssocie.lastQuery() ;
    }
    ModeleAssocies.removeRow(row);
    if(ModeleAssocies.rowCount()==0)
    {
        ui->Bt_SupprimerAssocie->setEnabled(false);
    }
}

void F_Membres::on_Bt_ValiderAssocies_clicked()
{
    this->close();
}

//==========================================================================================================
/** Permet d'ajout un nouveau titre si "Créer titre..." a été sélectionné
 *  @pre    l'utilisateur a choisi Créer titre...
 *  @post   Ouverture d'un popup pour l'ajout d'un titre
 *  @param  int index
 *  @test
 */
void F_Membres::on_CBx_Titre_currentIndexChanged(int index)
{
    if(index == -1)
    {
        return;
    }
    if( ( this->VectorTitre.size() - 1 ) == index )
    {
        this->pTitreAjMod->Ajouter(6) ;
        ui->CBx_Titre->setCurrentIndex( 0 ) ;
    }
    else
    {
        ui->SBx_JeuxAutorises->setValue( this->VectorTitre.at( index ).nJeuxAutorises ) ;
    }
    // Si c'est collectivité ou association ou école qui a été choisie
    if(RecupererEmplacementTitreVecteur(2) == index ||
            RecupererEmplacementTitreVecteur(3) == index ||
            RecupererEmplacementTitreVecteur(6) == index )
    {
        ui->Gb_Prenom->setVisible(false);
        ui->Bt_Associe->setVisible(true);
    }
    else
    {
        ui->Gb_Prenom->setVisible(true);
        ui->Bt_Associe->setVisible(false);
    }
}

void F_Membres::on_LE_Email_textEdited(const QString &arg1)
{
    ui->LE_Email->setText( this->ModifierSyntaxeEmail( arg1 ) ) ;
}

//==========================================================================================================
/** Efface tous ce qui n'est pas autorisé dans une adresse email
 *  @param QString sNumero
 *  @retval QString
 *  @return Retourne un QString ne contenant plus de lettre ou caractêres speciaux. Un espace est ajouté tous les 2 caractères
 *
 */
QString F_Membres::ModifierSyntaxeEmail( QString sEmail )
{
    QRegExp CaracteresAutorises ( "[^\\+0-9a-zA-Z_.-]" ) ;

    //Suppression des les caractères ormis les chiffres
    sEmail.replace( CaracteresAutorises, "" ) ;

    return sEmail ;
}

void F_Membres::on_CBx_DomaineEmail_currentIndexChanged(int index)
{
    if( ( this->VectorDomaineEmail.size() - 1 )== index )
    {
        this->pDomaineEmailAjMod->Ajouter(12) ;
        ui->CBx_DomaineEmail->setCurrentIndex( 0 ) ;
    }
}
