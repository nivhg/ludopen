#include "f_mainwindow.h"
#include "ui_f_mainwindow.h"
#include "f_panier.h"

F_MainWindow::F_MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::F_MainWindow)
{
    ui->setupUi(this);
    qDebug()<<"Constructeur F_MainWindow = Début";

    // Création de la fenêtre de choix des préférences du logiciel
    this->pPreferences=new F_Preferences(this);

    this->VerifierConnexionBDD() ;
    this->pPreferences->ChargerPreferencesBDD();

    this->iIdBenevole=0;

    // Création de la fenêtre de choix des préférences du logiciel
    this->pPanier=new F_Panier(this);
    this->ui->Lay_Panier->addWidget(this->pPanier);
    connect(this->pPanier, SIGNAL(SignalMiseAJourNbItemsPanier(uint)), this, SLOT(slot_MiseAJourNbItemsPanier(uint)));

    // Récupère la taille de la police
    QFont font=QApplication::font();
    font.setPointSize(this->pPreferences->ObtenirValeur("TaillePolice").toInt());
    QApplication::setFont (font);

    ui->centralWidget->setLayout(ui->gridLayout);

    ui->menuImprimer->setEnabled(false);
    ui->Menu_Fichier_Quitter->setShortcut(tr("Ctrl+Q"));
    ui->Menu_Edition_Preferences->setShortcut(tr("Ctrl+P"));
    ui->Menu_Aide_Aide->setShortcut(tr("F1"));
    ui->Menu_Aide_Propos_LudOpen->setShortcut(tr("F2"));
    ui->Menu_Jeux_Imprimer_Etiquette->setShortcut(tr("Ctrl+E"));
    ui->Menu_Jeux_Imprimer_Fiche_Complete->setShortcut(tr("Ctrl+F"));

    QSqlQuery RequeteBenevoles;

    // Récupère toutes les activités
    RequeteBenevoles.prepare("SELECT Prenom,IdMembre FROM membres WHERE TypeMembres_IdTypeMembres=2 ORDER BY Prenom") ;

    //Exectution de la requête
    if( !RequeteBenevoles.exec() )
    {
        qDebug() << getLastExecutedQuery(RequeteBenevoles) << RequeteBenevoles.lastError() ;
    }
    else
    {
        while( RequeteBenevoles.next() )
        {
            // Récupère la valeurs des champs
            //CBx_Benevoles->addItem(,);
            QAction *sousmenu=ui->menuUtilisateurEnCours->addAction(ObtenirValeurParNom(RequeteBenevoles,"Prenom").toString());
            sousmenu->setData(ObtenirValeurParNom(RequeteBenevoles,"IdMembre").toInt());
            //connect(sousmenu,SIGNAL(triggered(QAction *)),this,SLOT(on_menuUtilisateurEnCours_triggered(QAction *)));
        }
    }


    //Layout///////////////

    //Widgets-onglets//////
    this->pEmprunt=0;
    this->pJeux=0;
    this->pRetour=0;
    this->pListeJeux=0;
    this->pMembres=0;
    this->pListeMembres = 0;
    this->pRetards=0;
    this->pListeReservations = 0;
    this->pMalles = 0;
    this->pReleve = 0;
    this->pCalendrierMalles = 0;

    //Widget-admin/////////
    ////Fournisseur////////
    this->pAjoutSuppModifFournisseurEditeurs=0;
    this->pAdministrerMembres=0;
    this->pListeMembresAdmin = 0;
    this->pStatistiques=0;
    this->pAjoutSuppModifJeux=0;
    this->pAbonnements=0;
//    this->pPopUpCode = 0;
    /*****************************************************************************/

/*    this->pPopUpCode = new D_PopUpCode;
    this->pPopUpCode->setWindowFlag(Qt::Dialog);
    this->pPopUpCode->setWindowFlag(Qt::WindowCloseButtonHint,false);
    this->pPopUpCode->setWindowFlag(Qt::WindowMaximizeButtonHint,false);
    //this->pPopUpCode->setWindowTitle("Identification");
    //this->pPopUpCode->setWindowModality(Qt::ApplicationModal);

    connect(this->pPopUpCode, SIGNAL(SignalMembreIdentifier(uint)), this, SLOT(slot_MembreIdentifier(uint)));
    this->pPopUpCode->exec();*/

    MAJeur *MAJ=new MAJeur(this);

    QString sCheminConfig=" " + this->pPreferences->ObtenirsCheminConfig();
    if(sCheminConfig.indexOf("config.ini",0,Qt::CaseInsensitive)!=-1)
    {
        sCheminConfig="";
    }

    //QString DateCompil=QLocale("en_US").toDate(QString(__DATE__).simplified(), "MMM d yyyy").toString("dd-MM-yyyy");
    QFileInfo info1(QCoreApplication::applicationFilePath());
    qDebug()<<info1.lastModified();
    QString DateFichier=info1.lastModified().toString("dd-MM-yyyy hh:mm");

    this->setWindowTitle(this->windowTitle()+ " v" + QString::fromLocal8Bit(VER) + " " + DateFichier + sCheminConfig);

    qDebug()<<"Création F_POSTIT";
    this->pPostIt=new F_POSTIT (this->ui->PostIt) ;

    //PostIt
    this->ui->Lay_PostIt->addWidget(this->pPostIt);

    // Afficher les post-it au démarrage de l'application
    ui->TbW_Main->setCurrentIndex(ui->TbW_Main->count()-1);

    if(this->pPreferences->ObtenirValeur("DesactiverOngletMalle").compare("true",Qt::CaseInsensitive)==0)
    {
        ui->TbW_Main->setTabEnabled(trouveOnglet("Malles"),false);
    }

    this->showMaximized();

    this->pCalendrierMalles=new F_Malles(this);
    connect(this->pPanier,SIGNAL(Signal_Nouvelle_Malle()),pCalendrierMalles,SLOT(slot_actualiserCalendrier()));
    // Si il y a plus d'un écran, on affiche le calendrier des malles aux adhérents
    if(QApplication::desktop()->screenCount() > 1)
    {
        QRect screenres = QApplication::desktop()->screenGeometry(1);
        pCalendrierMalles->move(QPoint(screenres.x(), screenres.y()));
        pCalendrierMalles->resize(screenres.width(), screenres.height());
        pCalendrierMalles->setWindowModality(Qt::NonModal);
        pCalendrierMalles->AfficherCalendrier();
        pCalendrierMalles->showFullScreen();
        pCalendrierMalles->show();
    }

    CreerReleve();
    Relevetimer = new QTimer(this);
    Relevetimer->setSingleShot(true);
    TimerProchainePermanence();
    connect(Relevetimer, SIGNAL(timeout()), SLOT(verifReleve()));

    Reservationtimer = new QTimer(this);
    Reservationtimer->setInterval(F_Preferences::ObtenirValeur("IntervalVerifResa").toInt()*60*1000);
    slot_verifReservation();
    connect(Reservationtimer, SIGNAL(timeout()), SLOT(slot_verifReservation()));
    Reservationtimer->start();
    IconeBarreTache=new QSystemTrayIcon(QIcon(":/IconeLO.png"));
    QMenu *menu = new QMenu;
    QAction *actionTray=menu->addAction("Quitter");
    connect(actionTray,SIGNAL(triggered()),this,SLOT(Quitter()));
    IconeBarreTache->setContextMenu(menu);
    IconeBarreTache->show();
    qDebug()<<"Constructeur F_MainWindow = OK";
}

void F_MainWindow::Quitter()
{
    close();
}
void F_MainWindow::VerifierConnexionBDD()
{

    QString sNomBDD;
    QString sAdresseIP;
    QString sNomUtilisateur;
    QString sMotDePasse;
    unsigned int nPort;

    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");

    // Recupération des valeurs dans le fichier INI.
    sNomBDD = this->pPreferences->ObtenirValeur("NomDeLaBDD");
    sAdresseIP = this->pPreferences->ObtenirValeur("AdresseIP");
    sNomUtilisateur = this->pPreferences->ObtenirValeur("NomUtilisateur");
    sMotDePasse = this->pPreferences->ObtenirValeur("MotDePasse");
    nPort = this->pPreferences->ObtenirValeur("Port").toInt();

    // Ouverture de la BDD en entrant les valeurs du fichier INI.
    db.setDatabaseName(sNomBDD);
    db.setHostName(sAdresseIP);
    db.setUserName(sNomUtilisateur);
    db.setPassword(sMotDePasse);
    db.setPort(nPort);

    db.open();


    // Test d'accès à la base de données
    if(db.isOpen() == false)
    {
        QMessageBox Err(QMessageBox::Critical,"LudOpen - Erreur d'accès aux données !",
          "Impossible d'accéder à la base de données !\n\n"+ db.lastError().text(),QMessageBox::Close);
        Err.exec();
        qDebug()<< "Vous etes sur l'adresse IP " << sAdresseIP << " sur le port " << nPort <<
                   " en tant que " << sNomUtilisateur << "." << endl << endl;
        qDebug()<< endl << "La connexion à la BDD " << sNomBDD << " a échouée." << endl;

        // Création de la fenêtre de choix des préférences du logiciel
        connect( pPreferences, SIGNAL( SignalFermerFenetre() ), this, SLOT( slot_Preferences() ) ) ;
        pPreferences->exec();
        // choisir l'onglet des préférences Réseau de la fenêtre Préférences
        pPreferences->SelectionnerOnglet( 1 ) ;
    }
    else
    {
        qDebug()<< "Connexion BDD OK sur l'adresse IP " << sAdresseIP << " sur le port " << nPort << " en tant que " << sNomUtilisateur << "." << endl << endl;
        qDebug()<< endl << "Démarrage de Ludopen!" << endl ;
    }
}

F_MainWindow::~F_MainWindow()
{
    delete ui;
}

void F_MainWindow::slot_ChangerOnglet()
{
    ui->TbW_Main->setCurrentIndex( 0 ) ;
}

// Si les préférences ont été mise à jours, mettre à jour certains affichages sur certaines fenêtres
void F_MainWindow::slot_Preferences()
{
    if(this->pAdministrerMembres)
    {
        this->pAdministrerMembres->MaJTitre() ;
        this->pAdministrerMembres->MaJType() ;
        this->pAdministrerMembres->AfficherMembre() ;
    }
    if(this->pMembres)
    {
        this->pMembres->MaJTitre() ;
        this->pMembres->MaJType() ;
        this->pMembres->AfficherMembre() ;
    }
}

void F_MainWindow::on_Bt_Membre_clicked()
{
    ChangerFenetreAdmin(this->pAdministrerMembres);
}

void F_MainWindow::on_Bt_Jeux_clicked()
{
    if(!this->pAjoutSuppModifJeux)
    {
        ////Jeux////////
        qDebug()<<"Création ADMIN-F_AjoutSuppModifJeux";
        this->pAjoutSuppModifJeux=new F_AjoutSuppModifJeux (this);
        this->ui->Lay_Admin->addWidget(this->pAjoutSuppModifJeux);
        this->pAjoutSuppModifJeux->setVisible(false);
    }
    ChangerFenetreAdmin(this->pAjoutSuppModifJeux);
}

void F_MainWindow::on_Bt_FournisseurEditeur_clicked()
{
    if(!this->pAjoutSuppModifFournisseurEditeurs)
    {
        ////Fournisseur////////
        qDebug()<<"Création ADMIN-F_AjoutSuppModifFournisseursEditeurs";
        this->pAjoutSuppModifFournisseurEditeurs=new F_AjoutSuppModifFournisseursEditeurs (this->ui->admin);
        this->ui->Lay_Admin->addWidget(this->pAjoutSuppModifFournisseurEditeurs);
        this->pAjoutSuppModifFournisseurEditeurs->setVisible(false);
    }
    ChangerFenetreAdmin(this->pAjoutSuppModifFournisseurEditeurs);
}

void F_MainWindow::on_Bt_Abonnements_clicked()
{
    if(!this->pAbonnements)
    {
        ////Abonnement////////
        qDebug()<<"Création ADMIN-F_Abonnements";
        this->pAbonnements=new F_Abonnements(this->ui->admin);
        this->ui->Lay_Admin->addWidget(this->pAbonnements);
        this->pAbonnements->setVisible(false);
    }
    ChangerFenetreAdmin(this->pAbonnements);
}

void F_MainWindow::on_Bt_Statistiques_clicked()
{
    if(!this->pStatistiques)
    {
        ////Statistiques////////
        qDebug()<<"Création ADMIN-F_Statistiques";
        this->pStatistiques=new F_Statistiques (this->ui->admin);
        this->ui->Lay_Admin->addWidget(this->pStatistiques);
        this->pStatistiques->setVisible(false);
    }
    ChangerFenetreAdmin(this->pStatistiques);
}

void F_MainWindow::on_Bt_ListeMembres_clicked()
{
    CreerListeMembres();
}

/** @brief Cache les différentes fenêtres admin et affiche celle passée en argument
 *  @param  QWidget : fenêtre à afficher
 */
void F_MainWindow::ChangerFenetreAdmin(QWidget *w)
{
    if(this->pAdministrerMembres)
    {
        this->pAdministrerMembres->setVisible(false);
    }
    if(this->pAjoutSuppModifJeux)
    {
        this->pAjoutSuppModifJeux->setVisible(false);
    }
    if(this->pAjoutSuppModifFournisseurEditeurs)
    {
        this->pAjoutSuppModifFournisseurEditeurs->setVisible(false);
    }
    if(this->pAbonnements)
    {
        this->pAbonnements->setVisible(false);
    }
    if(this->pStatistiques)
    {
        this->pStatistiques->setVisible(false) ;
    }
    if(w)
    {
        w->setVisible(true);
    }
}

void F_MainWindow::ChangerFenetreListes(QWidget *w)
{
    if(this->pListeJeux)
    {
        this->pListeJeux->setVisible(false);
    }
    if(this->pListeReservations)
    {
        this->pListeReservations->setVisible(false);
    }
    if(this->pListeMembresAdmin)
    {
        this->pListeMembresAdmin->setVisible(false) ;
    }
    if(w)
    {
        w->setVisible(true);
    }
}

//Changement d'onglet
void F_MainWindow::on_TbW_Main_currentChanged(int index)
{
    this->setCursor(Qt::WaitCursor);
    QString tab=ui->TbW_Main->currentWidget()->objectName().toLower();
    if(tab=="membre")
    {
        if(!this->pMembres)
        {
            CreerMembre();
        }
        ui->menuImprimer->setEnabled(false);
        this->pMembres->MaJListeMembres() ;
        this->pMembres->AfficherListe() ;
        this->pMembres->MaJTitre() ;
        this->pMembres->MaJType() ;
        this->pMembres->AfficherMembre() ;
    }
    else if(tab=="listemembre") //Liste Membres
    {
        //CreerListeMembres();
        ui->menuImprimer->setEnabled(false);
        this->pListeMembres->AffichageListe() ;
    }
    else if(tab=="jeux") //Jeux
    {
        // Rendre visible le menu jeu pour imprimer les étiquettes et les fiches des jeux
        // A_FAIRE : ne rendre possible l'impression que quand un jeu a été choisi sur l'onglet JEUX
        //if ( !this->pJeux->get_JeuEnConsultation().isEmpty() )
        CreerJeux();
        ui->menuImprimer->setEnabled(true);
        this->pJeux->ActualiserJeux();
    }
    else if(tab=="listejeux") //Liste jeux
    {
        ////CreerListeJeux();
        ui->menuImprimer->setEnabled(false);
    }
    else if(tab=="emprunt") //Emprunt
    {
        CreerEmprunt();
        ui->menuImprimer->setEnabled(true);
        this->pEmprunt->ActualiserJeu();
        this->pEmprunt->ActualiserMembre();
    }
    else if(tab=="retour") //Retour
    {
        CreerRetour();
        // Désactive le menu Jeux
        ui->menuImprimer->setEnabled(true);
        // actualise les infos affichée sur le membre sélectionné actuellement
        this->pRetour->ActualiserMembre();
        // actualise les jeux à retourner pour le membre sélectionné actuellement
        this->pRetour->ActualiserJeu();
        // Remet à jour la liste de membres ayant un retour à faire
        this->pRetour->ActualiserListeEmprunteurs();
    }
    else if(tab=="malles") //Réservation Malles
    {
        CreerMalle();
        ui->menuImprimer->setEnabled(true);
        this->pMalles->setVisible(true);
        this->pMalles->ActualiserJeu();
        this->pMalles->ActualiserMembre();
    }
    else if(tab=="listereservations") //Liste Réservations
    {
        //CreerReservations();
        // Désactive le menu Jeux
        ui->menuImprimer->setEnabled(false);
        this->pListeReservations->AffichageListe() ;
    }
    else if(tab=="retards") //retards
    {
        CreerRetards();
        ui->menuImprimer->setEnabled(false);
        this->pRetards->MaJListe();
    }
    else if(tab=="admin") //Administration
    {
        //Widget-admin/////////
        CreerAdminMembres();
        /*****************************************************************************/
        //this->pPopUpCode->show();  // Pas de code pour le moment, trop chiant"
        /*if(!this->pPopUpCode)
        {
                qDebug()<<"Création ADMIN-D_PopUpCode";
                this->pPopUpCode = new D_PopUpCode;
                this->pPopUpCode->setWindowTitle("Code d'accès");
                this->pPopUpCode->setWindowModality(Qt::ApplicationModal);

                connect(this->pPopUpCode, SIGNAL(SignalOnglet()), this, SLOT(slot_ChangerOnglet()));
        }*/
        ui->menuImprimer->setEnabled(true);
        this->pAdministrerMembres->MaJListeMembres() ;
        this->pAdministrerMembres->AfficherListe() ;
        this->pAdministrerMembres->MaJTitre() ;
        this->pAdministrerMembres->MaJType() ;
        this->pAdministrerMembres->AfficherMembre() ;
    }
    else if(tab=="listes") //Listes
    {
        CreerListeJeux();
        ui->menuImprimer->setEnabled(false);
    }
    else if(tab=="releve") //Releve caisse
    {
        CreerReleve();
        this->pReleve->setVisible(true);
        connect( this->pReleve, SIGNAL( SignalPlusTard() ), this, SLOT( slot_PlusTardReleve() ) ) ;
        connect( this->pReleve, SIGNAL( SignalReleveFini() ), this, SLOT( slot_ReleveFini() ) ) ;
    }
    else if(tab=="postit") //PostIt
    {
        ui->menuImprimer->setEnabled(false);
        this->pPostIt->setVisible(true);
    }
    this->setCursor(Qt::ArrowCursor);
}

void F_MainWindow::CreerAdminMembres()
{
    if(!this->pAdministrerMembres)
    {
        ////Membre/////////////
        qDebug()<<"Création ADMIN-F_Membres";
        this->pAdministrerMembres=new F_Membres (MODE_ADMIN,this->ui->admin,0,pPanier);
        this->pAdministrerMembres->setVisible(false);
        this->ui->Lay_Admin->addWidget(this->pAdministrerMembres);
        this->pAdministrerMembres->setVisible(true);
    }
}

void F_MainWindow::CreerRetards()
{
    if(!this->pRetards)
    {
        qDebug()<<"Création F_Retards";
        this->pRetards=new F_Retards (this->ui->Listes);
        this->ui->Lay_Listes->addWidget(this->pRetards);
    }
    ChangerFenetreListes(this->pRetards);

}

void F_MainWindow::CreerReservations()
{
    if(!this->pListeReservations)
    {
        qDebug()<<"Création F_ListeReservation";
        this->pListeReservations=new F_ListeReservations(this->ui->Listes);
        this->ui->Lay_Listes->addWidget(this->pListeReservations);
    }
    // Si l'onglet Emprunt a été crée, on connecte le signal et le slot
    if(this->pEmprunt)
    {
        connect( this->pEmprunt, SIGNAL( Signal_Nouvelle_Reservation() ), this->pListeReservations, SLOT( AffichageListe() ) ) ;
    }
    ChangerFenetreListes(this->pListeReservations);
}

void F_MainWindow::CreerMembre()
{
    if(!this->pMembres)
    {
        qDebug()<<"Création F_Membres";
        this->pMembres=new F_Membres (MODE_UTILISATEUR, this->ui->Membre,0,pPanier);
        //Membres
        this->ui->Lay_Membres->addWidget(this->pMembres);
        connect( this->pListeMembresAdmin, SIGNAL( Signal_DoubleClic_ListeMembres( uint ) ), this->pMembres, SLOT( slot_AfficherMembre( uint ) ) ) ;
        connect( this->pMembres, SIGNAL( Signal_Adherent_Cree() ), this->pEmprunt, SLOT( slot_ActualiserMembres() ) ) ;
        connect( this->pMembres, SIGNAL( Signal_Adherent_Cree() ), this->pMalles, SLOT( slot_ActualiserMembres() ) ) ;
//        connect( this->pMembres, SIGNAL( Signal_Adherent_Cree() ), this->pEmprunt, SLOT( slot_ActualiserMembres() ) ) ;
    }
}

void F_MainWindow::CreerListeMembres()
{
    if(!this->pListeMembres)
    {
        ////Liste Membres//////
        qDebug()<<"Création ADMIN-F_ListeMembres";
        this->pListeMembresAdmin = new F_ListeMembres( true, this->ui->Listes ) ;
        //Liste Membres
        this->ui->Lay_Listes->addWidget( this->pListeMembresAdmin ) ;
        // Si double clic dans la liste des membres sur un membre,
        // affiche la fiche détaillée du membre sélectionné
        connect( this->pListeMembresAdmin,
                 SIGNAL( Signal_DoubleClic_ListeMembres( uint ) ), this,
                 SLOT( slot_DoubleClic_ListeMembresAdmin ( uint )) ) ;
        this->pListeMembresAdmin->AffichageListe() ;
        connect( this->pListeMembres, SIGNAL( Signal_DoubleClic_ListeMembres( uint ) ), this , SLOT( slot_DoubleClic_ListeMembres( uint ) ) ) ;
    }
    ChangerFenetreListes(this->pListeMembresAdmin);
}

void F_MainWindow::CreerJeux()
{
    if(!this->pJeux)
    {
        qDebug()<<"Création F_Jeux";
        this->pJeux=new F_Jeux (this);
        //Jeux
        this->ui->Lay_Jeux->addWidget(this->pJeux);
        // Si clic dans le bouton réservé, affiche la liste des réservation avec le jeu sélectionné
        connect( this->pJeux, SIGNAL( Signal_Clic_Reserve( int ) ), this, SLOT( slot_Clic_Reserve(int) )) ;
    }
}

void F_MainWindow::CreerListeJeux()
{
    if(!this->pListeJeux)
    {
        qDebug()<<"Création F_ListeJeux";
        this->pListeJeux=new F_ListeJeux (this->ui->Listes);
        //Liste Jeux
        this->ui->Lay_Listes->addWidget(this->pListeJeux);

        // Si double clic dans la liste des jeux sur un jeu, affiche la fiche détaillée du jeu sélectionné
        connect( this->pListeJeux, SIGNAL( Signal_DoubleClic_ListeJeux( QString ) ), this, SLOT( slot_DoubleClic_ListeJeux(QString) )) ;
    }
}

void F_MainWindow::CreerEmprunt()
{
    if(!this->pEmprunt)
    {
        CreerJeux();
        qDebug()<<"Création F_Emprunt";
        this->pEmprunt=new F_Emprunt (MODE_EMPRUNT,this,pCalendrierMalles);
        //Emprunt
        this->ui->Lay_Emprunt->addWidget(this->pEmprunt);
        connect( this->pEmprunt, SIGNAL( Signal_Reservation_Malle(int) ), this, SLOT( slot_Reservation_Malle(int) )) ;
        connect( this->pEmprunt, SIGNAL( Signal_AjouterAuPanier(QString,uint,double,int,QString,QList<QSqlQuery *> *) ), this->pPanier,
                 SLOT( slot_AjouterAuPanier(QString,uint,double,int,QString,QList<QSqlQuery *> *)) );
        connect( this->pEmprunt, SIGNAL( Signal_VerifMembrePanier(uint)), this->pPanier,SLOT( slot_VerifMembrePanier(uint)) );
        if(pCalendrierMalles)
            connect(this->pEmprunt,SIGNAL(Signal_Nouvelle_Malle()),pCalendrierMalles,SLOT(slot_actualiserCalendrier()));
        connect( this->pEmprunt, SIGNAL( Signal_AfficherJeu(QString)), this,
                 SLOT( slot_AfficherJeu(QString)) );
    }
    // Si l'onglet Liste réservations a été crée, on connecte le signal et le slot
    if(this->pListeReservations)
    {
        connect( this->pEmprunt, SIGNAL( Signal_Nouvelle_Reservation() ), this->pListeReservations, SLOT( AffichageListe() ) ) ;
    }
}

void F_MainWindow::CreerRetour()
{
    if(!this->pRetour)
    {
        CreerJeux();
        qDebug()<<"Création F_Retour";
        this->pRetour=new F_Retour (this);
        //Retour
        this->ui->Lay_Retour->addWidget(this->pRetour);
        connect( this->pRetour, SIGNAL( Signal_AjouterAuPanier(QString,uint,double,int,QString,QList<QSqlQuery *> *) ), this->pPanier,
                 SLOT( slot_AjouterAuPanier(QString,uint,double,int,QString,QList<QSqlQuery *> *)) ) ;
        connect( this->pRetour, SIGNAL( Signal_AfficherJeu(QString)), this,
                 SLOT( slot_AfficherJeu(QString)) );

    }
}

void F_MainWindow::CreerMalle()
{
    if(!this->pMalles)
    {
        qDebug()<<"Création F_Emprunt pour onglet réservation Malles";
        this->pMalles=new F_Emprunt (MODE_MALLES, this,pCalendrierMalles);
        this->ui->Lay_Malles->addWidget(this->pMalles);
        connect( this->pMalles, SIGNAL( Signal_Reservation_Malle(int) ), this, SLOT( slot_Reservation_Malle(int) )) ;
        connect( this->pMalles, SIGNAL( Signal_AjouterAuPanier(QString,uint,double,int,QString,QList<QSqlQuery *> *) ), this->pPanier,
                 SLOT( slot_AjouterAuPanier(QString,uint,double,int,QString,QList<QSqlQuery *> *)) ) ;
        connect( this->pMalles, SIGNAL( Signal_VerifMembrePanier(uint IdDuMembre)), this->pPanier,
                 SLOT( slot_VerifMembrePanier(uint IdDuMembre)) );
        if(!pCalendrierMalles)
            connect(this->pMalles,SIGNAL(Signal_Nouvelle_Malle()),pCalendrierMalles,SLOT(slot_actualiserCalendrier()));
    }
}

void F_MainWindow::CreerReleve()
{
    if(!this->pReleve)
    {
        qDebug()<<"Création D_Releve";
        this->pReleve=new D_Releve(this);
        this->ui->Lay_Releve->addWidget(this->pReleve);
    }
}

void F_MainWindow::on_Menu_Fichier_Quitter_triggered()
{
    this->close();
}

void F_MainWindow::on_Menu_Edition_Preferences_triggered()
{
   connect( pPreferences, SIGNAL( SignalFermerFenetre() ), this, SLOT( slot_Preferences() ) ) ;
   pPreferences->exec();
}

void F_MainWindow::on_Menu_Aide_Aide_triggered()
{
    QString sAide;

    sAide = QDir::currentPath() + "/Manuel.pdf";
    QDesktopServices::openUrl(QUrl::fromLocalFile(sAide));
}


/**
 * Quand double clic sur un jeu dans l'onglet Liste de jeux, affiche l'onglet jeu avec le jeu concerné pour avoir le détail
 * grâce à un signal envoyé par f_listejeux.
 * @param index Ligne choisie dans le modèle associé au tableau TbV_Recherche
 */
void F_MainWindow::slot_DoubleClic_ListeJeux(QString CodeJeu)
{
    CreerJeux();
    // Indiquer à l'onglet jeu quel code jeu afficher
    this->pJeux->set_JeuEnConsultation(CodeJeu);
    // Afficher le détail du jeu choisi
    this->pJeux->ActualiserJeux();
    // Faire apparaître l'onglet Jeu
    ui->TbW_Main->setCurrentIndex(2);
}

/**
 * Quand clic sur le bouton réservé dans l'onglet Jeux, affiche l'onglet liste des réservation
 * avec le jeu sélectionné grâce à un signal envoyé par f_jeux
 * @param CodeJeu
 */
void F_MainWindow::slot_Clic_Reserve(int IdReservation)
{
    //CreerReservations();
    this->pListeReservations->TousSelectionner( false ) ;
    ui->TbW_Main->setCurrentIndex(6);
    this->pListeReservations->SelectionnerReservation(IdReservation);
    // Indiquer à l'onglet jeu quel code jeu afficher
}
/**
 * Quand double clic sur un membre dans l'onglet Liste des membres,
 * affiche l'onglet Membres avec le membre concerné pour avoir le détail
 * grâce à un signal envoyé par f_listemembres.
 * @param index Ligne choisie dans le modèle associé au tableau TbV_Recherche
 */
void F_MainWindow::slot_DoubleClic_ListeMembres(uint IdMembre)
{
    CreerMembre();
    // Indiquer à l'onglet Membre quel membre afficher
    this->pMembres->slot_AfficherMembre( IdMembre );
    // Faire apparaître l'onglet Membre
    ui->TbW_Main->setCurrentIndex(this->trouveOnglet("membre"));
}

/**
 * Quand double clic sur un membre dans l'onglet Admin/Liste des membres,
 * affiche l'onglet Admin/Membres avec le membre concerné pour avoir le détail
 * grâce à un signal envoyé par f_listemembres.
 * @param index Ligne choisie dans le modèle associé au tableau TbV_Recherche
 */
void F_MainWindow::slot_DoubleClic_ListeMembresAdmin(uint IdMembre)
{
    CreerAdminMembres();
    // Indiquer à l'onglet Membre quel membre afficher
    this->pAdministrerMembres->slot_AfficherMembre( IdMembre );
    // Faire apparaître l'onglet Admin/Membre
    ui->TbW_Main->setCurrentIndex(this->trouveOnglet("membre"));
}

void F_MainWindow::on_Menu_Jeux_Imprimer_Etiquette_triggered()
{
    QString CodeJeu;
    int index=ui->TbW_Main->currentIndex();
    if(index==trouveOnglet("jeux")) CodeJeu=this->pJeux->get_JeuEnConsultation();
    else if(index==trouveOnglet("admin")) CodeJeu=this->pAjoutSuppModifJeux->get_JeuEnConsultation();
    else if(index==trouveOnglet("retour")) CodeJeu=this->pRetour->get_JeuEnConsultation();
    else if(index==trouveOnglet("emprunt")) CodeJeu=this->pEmprunt->get_JeuEnConsultation();

    if(!CodeJeu.isEmpty())
    {
        F_ImprimerEtiquetteJeu f_ImprimerEtiquetteJeu;
        f_ImprimerEtiquetteJeu.ImprimerEtiquetteJeu(CodeJeu);
        f_ImprimerEtiquetteJeu.exec();
    }
    else
    {
        QMessageBox::information(this, "Pas de jeu sélectionné !", "Vous n'avez choisi aucun jeu dans la liste des jeux.\nVeuillez en sélectionner un avant de lancer l'impression de son étiquette.", "OK") ;
    }
}

void F_MainWindow::on_Menu_Jeux_Imprimer_Fiche_Complete_triggered()
{
   if ( !this->pJeux->get_JeuEnConsultation().isEmpty() )
   {
      F_ImprimerFicheCompleteJeu f_ImprimerFicheCompleteJeu ;
      f_ImprimerFicheCompleteJeu.ImprimerFicheJeuComplete(this->pJeux->get_JeuEnConsultation()) ;
      f_ImprimerFicheCompleteJeu.exec() ;
   }
   else
   {
      QMessageBox::information(this, "Pas de jeu sélectionné !", "Vous n'avez choisi aucun jeu dans la liste des jeux.\nVeuillez en sélectionner "
                               "un avant de lancer l'impression de sa fiche complête.", "OK") ;
   }
}

void F_MainWindow::on_Menu_Aide_Propos_LudOpen_triggered()
{
    QMessageBox APropos;
    APropos.about(this, "A propos de ...", "<center><IMG SRC=\":LudOpen.png\" ALIGN=\"MIDDLE\" ALT=\"LudOpen\"></center><br>"
      "LudOpen version " + QString::fromLocal8Bit(VER) + "<br><br>Date de compilation : " +
      QString::number(BUILD_DAY) + "-" + QString::number(BUILD_MONTH) + "-" + QString::number(BUILD_YEAR) + " " +
      QString::number(BUILD_HOUR) + ":" + QString::number(BUILD_MIN) + ":" + QString::number(BUILD_SEC) +
      "<br><br>Programme créé avec Qt Creator 3.0.1 - Qt 5.2.1<br><br>"
      "<a href='https://github.com/nivhg/ludopen'>https://github.com/nivhg/ludopen</a><br><br>"
      "Copyright © BOTHEREL Phillipe, MARY Florian, NORMAND Julien, PADIOU Nicolas, SOREL William, VICTORIN Vincent. Tous droits réservés.");
    //APropos.setWindowIcon(QIcon(""));
}

void F_MainWindow::on_Menu_Imprimer_Malle_Reservee_triggered()
{
    int iIdMalle;
    int index=ui->TbW_Main->currentIndex();
    if(index==trouveOnglet("malles"))
    {
        iIdMalle=this->pMalles->get_MalleReserveeSelectionnee();
    }
    else if(index==trouveOnglet("emprunt"))
    {
        iIdMalle=this->pEmprunt->get_MalleReserveeSelectionnee();
    }
    else if(index==trouveOnglet("retour"))
    {
        iIdMalle=this->pRetour->get_MalleReserveeSelectionnee();
    }

    if ( iIdMalle!=0 )
    {
       F_ImprimerMalle F_ImprimerMalle;
       F_ImprimerMalle.ImprimerMalle(iIdMalle,false) ;
       F_ImprimerMalle.exec() ;
    }
    else
    {
       QMessageBox::information(this, "Pas de malle sélectionnée !", "Vous n'avez pas sélectionné de malle dans la liste des jeux/malles réservés.\n"
                                "Veuillez en sélectionner une avant de lancer l'impression de son récapitulatif.", "OK") ;
    }
}

void F_MainWindow::on_Menu_Imprimer_Malle_Empruntee_triggered()
{
    int iIdMalle;
    int index=ui->TbW_Main->currentIndex();
    if(index==trouveOnglet("malles"))
    {
        iIdMalle=this->pMalles->get_MalleEmprunteeSelectionnee();
    }
    else if(index==trouveOnglet("emprunt"))
    {
        iIdMalle=this->pEmprunt->get_MalleEmprunteeSelectionnee();
    }
    else if(index==trouveOnglet("retour"))
    {
        iIdMalle=this->pRetour->get_MalleEmprunteeSelectionnee();
    }

    if ( iIdMalle!=0 )
    {
       F_ImprimerMalle F_ImprimerMalle;
       F_ImprimerMalle.ImprimerMalle(iIdMalle,true);
       F_ImprimerMalle.exec() ;
    }
    else
    {
       QMessageBox::information(this, "Pas de malle sélectionnée !", "Vous n'avez pas sélectionné de malle dans la liste des jeux/malles empruntés.\n"
                                "Veuillez en sélectionner une avant de lancer l'impression de son récapitulatif.", "OK") ;
    }
}

void F_MainWindow::slot_Reservation_Malle(int iIdMalle)
{
    CreerMalle();
    this->pMalles->AfficherMalle(iIdMalle);
    // Faire apparaître l'onglet Malle
    ui->TbW_Main->setCurrentIndex(trouveOnglet("malles"));
}

void F_MainWindow::verifReleve()
{
    QStringList Postes=F_Preferences::ObtenirValeur("PosteReleveCaisse").split(",");
    if(Postes.contains(QHostInfo::localHostName(),Qt::CaseInsensitive))
    {
        QDateTime Maintenant=QDateTime::currentDateTime();
        int Difference=Maintenant.secsTo(FuturPerm);

        if(Difference>0)
        {
            IconeBarreTache->showMessage("Relevé de caisse imminent","Il reste "+QString::number(Difference/60)
                                         +" minutes avant le relevé de caisse.");
            Relevetimer->start(F_Preferences::ObtenirValeur("RelanceReleveCaisse").toInt()*60*1000);
        }
        else
        {
            ui->TbW_Main->blockSignals(true);
            for(int i=0;i<ui->TbW_Main->count();i++)
            {
                ui->TbW_Main->setTabEnabled(i,false);
            }
            ui->TbW_Main->blockSignals(false);
            int IndexReleve=trouveOnglet("releve");
            ui->TbW_Main->setTabEnabled(IndexReleve,true);
            ui->TbW_Main->setCurrentIndex(IndexReleve);
        }
    }
}

int F_MainWindow::trouveOnglet(QString NomOnglet)
{
    for(int i=0;i<ui->TbW_Main->count();i++)
    {
        if(ui->TbW_Main->widget(i)->objectName().compare(NomOnglet,Qt::CaseInsensitive)==0)
        {
            return i;
        }
    }
}
void F_MainWindow::slot_PlusTardReleve()
{
    for(int i=0;i<ui->TbW_Main->count();i++)
    {
        ui->TbW_Main->setTabEnabled(i,true);
    }
    // On redemandera le relevé dans X minutes
    Relevetimer->start(F_Preferences::ObtenirValeur("RelanceReleveCaisse").toInt()*60*1000);
}

void F_MainWindow::TimerProchainePermanence()
{
    QSqlQuery Requete;
    QList<QList <QVariant>> Permanences;

    Requete.prepare("SELECT * FROM permanences as p");
    //Exectution de la requête
    if( !Requete.exec() )
    {
        qDebug() << getLastExecutedQuery(Requete) << Requete.lastError();
        return;
    }
    // Si on obtient un résultat, c'est qu'il y a eu un relevé, on attends
    int i=0;
    while(Requete.next())
    {
        QList<QVariant> *Liste=new QList<QVariant>();
        Liste->append(ObtenirValeurParNom(Requete,"JourPermanence"));
        Liste->append(ObtenirValeurParNom(Requete,"HeureDebut"));
        Liste->append(ObtenirValeurParNom(Requete,"HeureFin"));
        Permanences.append(*Liste);
    }
    qDebug()<<Permanences;


    QDateTime Maintenant=QDateTime::currentDateTime();
//    QString sMaintenant="2018-11-24 17:00";
    QString sMaintenant="NOW()";
//    Maintenant="'"+Maintenant.fromString(sMaintenant,"yyyy-MM-dd hh:mm")+"'";
    qDebug()<<Maintenant;
    // Récupère toutes les relevés de caisse
    Requete.prepare("SELECT * FROM (SELECT @d:="+sMaintenant+") as d,relevescaisse as r WHERE DATE_FORMAT(@d,'%y-%m-%d')="
                    "DATE_FORMAT(DateHeureReleve,'%y-%m-%d')");

    //Exectution de la requête
    if( !Requete.exec() )
    {
        qDebug() << getLastExecutedQuery(Requete) << Requete.lastError();
        return;
    }
    bool Retour=Requete.next();
    int Difference;
    int iFuturPerm=0;
    // Recherche de la prochaine permanence
    FuturPerm=TrouverProchainePerm(Maintenant,Permanences,&iFuturPerm);
    int HeurePerm;
    // Si il y a eu un relevé aujourd'hui, on active le timer pour s'enclencher 30 minutes avant la fin de la permanence, sinon au début
    if(Retour)
    {
        HeurePerm=PERM_FIN;
        do
        {
            QTime FinPerm=QTime::fromString(Permanences.at(iFuturPerm).at(HeurePerm).toString());
            Difference=FinPerm.secsTo(ObtenirValeurParNom(Requete,"DateHeureReleve").toDateTime().time());
            Difference=Difference+F_Preferences::ObtenirValeur("IntervalReleveCaisse").toInt()*60;
            // Si il y a eu un relevé de fin de permanence, on programme le timer pour la prochaine permanence
            if(Difference>0)
            {
                HeurePerm=PERM_DEBUT;
                // On commence la recherche à partir de demain
                FuturPerm=TrouverProchainePerm(Maintenant.addDays(1),Permanences,&iFuturPerm);
                break;
            }
        }
        while(Requete.next());
    }
    else
    {
        HeurePerm=PERM_DEBUT;
    }
    // On mets l'heure de la future perm (début ou fin de permanence)
    FuturPerm.setTime(QTime::fromString(Permanences.at(iFuturPerm).at(HeurePerm).toString()));
    // On calcule la différence de temps entre la futur perm et maintenant et on retire l'interval défini pour le relevé de caisse (30 min)
    Difference=Maintenant.secsTo(FuturPerm);
    Difference=Difference-F_Preferences::ObtenirValeur("IntervalReleveCaisse").toInt()*60;
    if(Difference<0) Difference=0;

    Relevetimer->start(Difference*1000);
}

QDateTime F_MainWindow::TrouverProchainePerm(QDateTime LaDate,QList<QList <QVariant>> Permanences,int *iFuturPerm)
{
    QDateTime FuturPerm=LaDate;
    // Si le jour d'aujourd'hui est supérieur au dernier jour de permanence, on prends la première permanence
    if(LaDate.date().dayOfWeek()-1 > Permanences.at(Permanences.count()-1).at(PERM_JOUR).toInt())
    {
        // On part de la semaine prochaine pour trouver le jour correspondant
        FuturPerm=FuturPerm.addDays(8-LaDate.date().dayOfWeek());
        FuturPerm=FuturPerm.addDays(Permanences.at(0).at(PERM_JOUR).toInt());
    }
    // Sinon, on recherche la prochaine permanence
    else
    {
        for(int i=0;i<Permanences.count();i++)
        {
            if(Permanences.at(i).at(PERM_JOUR)>=LaDate.date().dayOfWeek()-1)
            {
                FuturPerm=FuturPerm.addDays(1-LaDate.date().dayOfWeek());
                FuturPerm=FuturPerm.addDays(Permanences.at(i).at(PERM_JOUR).toInt());
                *iFuturPerm=i;
                break;
            }
        }
    }
    return FuturPerm;
}

void F_MainWindow::slot_MembreIdentifier(uint iIdMembre)
{
    iIdBenevole=iIdMembre;
}

void F_MainWindow::slot_ReleveFini()
{
    for(int i=0;i<ui->TbW_Main->count();i++)
    {
        ui->TbW_Main->setTabEnabled(i,true);
    }
    TimerProchainePermanence();
}

void F_MainWindow::closeEvent(QCloseEvent *event)
{
    if(this->pEmprunt && this->pEmprunt->ModeleEmpruntsAValider->rowCount()>0)
    {
        if(QMessageBox::question(this, "Confirmation", "Il reste des emprunts à valider. Êtes-vous sûr que vouloir fermer LudOpen ?", "Oui", "Non") != 0)
        {
            event->ignore();
            return;
        }
    }

    if(this->pMalles && this->pMalles->ModeleEmpruntsAValider->rowCount()>0)
    {
        if(QMessageBox::question(this, "Confirmation", "Il reste une malle à valider. Êtes-vous sûr que vouloir fermer LudOpen ?", "Oui", "Non") != 0)
        {
            event->ignore();
            return;
        }
    }

    if(this->pRetour && this->pRetour->ModeleJeuxEmpruntes->rowCount()>0)
    {
        if(QMessageBox::question(this, "Confirmation", "Il reste des retours de jeux à valider. Êtes-vous sûr que vouloir fermer LudOpen ?", "Oui", "Non") != 0)
        {
            event->ignore();
            return;
        }
    }

    if(this->pPanier && this->pPanier->ModelePanier.rowCount()>0)
    {
        if(QMessageBox::question(this, "Confirmation", "Il reste des éléments dans le panier. Êtes-vous sûr que vouloir fermer LudOpen ?", "Oui", "Non") != 0)
        {
            event->ignore();
            return;
        }
    }
}

void F_MainWindow::on_Bt_ListeJeux_clicked()
{
    ChangerFenetreListes(this->pListeJeux);
}

void F_MainWindow::on_Bt_ListeReservations_clicked()
{
    CreerReservations();
}

void F_MainWindow::slot_MiseAJourNbItemsPanier(uint iNbItems)
{
    ui->TbW_Main->setTabText(trouveOnglet("Panier"),"Panier ("+QString::number(iNbItems)+")");
}


void F_MainWindow::on_Bt_Retards_clicked()
{
    CreerRetards();
}

void F_MainWindow::slot_verifReservation()
{
    // Si on n'est pas sur le lieu où se trouve les jeux, on ne demande pas de mettre de coté les jeux
    if(F_Preferences::ObtenirValeur("IdLieux")!=F_Preferences::ObtenirValeur("LieuDesJeux"))
    {
        return;
    }
    // Recherche des réservations à mettre de côté
    QSqlQuery Requete;
    // On recherche les réservations pas encore mise de côté, non supprimer par l'utilisateur et disponible hors jeux spéciaux et uniquement pour
    // les réservations de jeux à venir retirer dans le lieu où se trouve les jeux (hors ludobus)
    QString RequeteStr="SELECT IdMembre,Email,NomJeu,CodeJeu,IdJeux FROM reservation as r LEFT JOIN membres as m ON m.IdMembre=r.Membres_IdMembre "
                       "LEFT JOIN jeux as j ON IdJeux=Jeux_IdJeux WHERE StatutJeux_IdStatutJeux = "+QString::number(STATUTJEUX_DISPONIBLE)+
                       " AND ASupprimer=0 AND Lieux_IdLieuxRetrait="+F_Preferences::ObtenirValeur("IdLieux")+" AND "+
                        F_Preferences::ObtenirValeur("FiltreJeuxSpeciauxNomChamps")+"!="+F_Preferences::ObtenirValeur("FiltreJeuxSpeciauxValeur")+
                        " AND Malles_IdMalle IS NULL GROUP BY IdJeux ORDER BY DateReservation";
    Requete.prepare(RequeteStr);
    //Exectution de la requête
    if( !Requete.exec() )
    {
        qDebug() << getLastExecutedQuery(Requete) << Requete.lastError();
        return;
    }
    qDebug() << getLastExecutedQuery(Requete) << Requete.lastError();
    // Si on obtient un résultat, c'est qu'il y a eu un relevé, on attends
    while(Requete.next())
    {
        QString NomJeu=ObtenirValeurParNom(Requete,"NomJeu").toString();
        QString CodeJeu=ObtenirValeurParNom(Requete,"CodeJeu").toString();
        int IdMembre=ObtenirValeurParNom(Requete,"IdMembre").toInt();
        int IdJeu=ObtenirValeurParNom(Requete,"IdJeux").toInt();
        QString Email=ObtenirValeurParNom(Requete,"Email").toString();

        // On affiche le dialog demandant si le jeu a été mis de coté ou non
        D_ResaMisDeCote D_ResaMisDeCote(this,CodeJeu,NomJeu,IdMembre,IdJeu,Email);
        D_ResaMisDeCote.exec();
    }

    // Recherche des réservations à ranger
    // On recherche les réservations mise de côté, et supprimer ainsi que les réservations qui ont dépassé la durée de réservation
    Requete.prepare("SELECT Idreservation,DateReservation,IdMembre,Email,NomJeu,CodeJeu,IdJeux FROM reservation as r LEFT JOIN membres as m ON "
                    "m.IdMembre=r.Membres_IdMembre LEFT JOIN jeux as j ON IdJeux=Jeux_IdJeux WHERE StatutJeux_IdStatutJeux = "+
                    QString::number(STATUTJEUX_ENRESERVATION)+" AND (ASupprimer=1 OR DATEDIFF(NOW(),DatePrevuEmprunt)>:DelaiJeuMisDeCote) AND "+
                    F_Preferences::ObtenirValeur("FiltreJeuxSpeciauxNomChamps")+"!="+F_Preferences::ObtenirValeur("FiltreJeuxSpeciauxValeur")+
                    " AND Malles_IdMalle IS NULL");
    Requete.bindValue(":DelaiJeuMisDeCote",F_Preferences::ObtenirValeur("DelaiJeuMisDeCote").toInt()*7);
    //Exectution de la requête
    if( !Requete.exec() )
    {
        qDebug() << getLastExecutedQuery(Requete) << Requete.lastError();
        return;
    }
    while(Requete.next())
    {
        QString NomJeu=ObtenirValeurParNom(Requete,"NomJeu").toString();
        QString CodeJeu=ObtenirValeurParNom(Requete,"CodeJeu").toString();
        int IdMembre=ObtenirValeurParNom(Requete,"IdMembre").toInt();
        int IdJeu=ObtenirValeurParNom(Requete,"IdJeux").toInt();
        QString Email=ObtenirValeurParNom(Requete,"Email").toString();

        if(QMessageBox::question(this, "Jeu à ranger", "Le jeu "+NomJeu+" ("+CodeJeu+")\nqui a été mis de côté pour une réservation,\nest à ranger, "
                "le membre aillant supprimé sa réservation\nou la réservation aillant expirée.", "OK", "Plus tard") != 0)
        {
            return;
        }
        QSqlQuery RequeteSuppressionResa;
        RequeteSuppressionResa.prepare("DELETE FROM reservation WHERE IdReservation=:IdReservation");
        RequeteSuppressionResa.bindValue(":IdReservation",ObtenirValeurParNom(Requete,"Idreservation").toInt());
        //Exectution de la requête
        if( !RequeteSuppressionResa.exec() )
        {
            qDebug() << getLastExecutedQuery(RequeteSuppressionResa) << RequeteSuppressionResa.lastError();
            return;
        }

        // On passe le jeu dans le statut "Disponible"
        QSqlQuery Requete;
        Requete.prepare("UPDATE jeux SET StatutJeux_IdStatutJeux="+QString::number(STATUTJEUX_DISPONIBLE)+" WHERE IdJeux=:Jeux_IdJeux");
        Requete.bindValue(":Membres_IdMembre",IdMembre);
        Requete.bindValue(":Jeux_IdJeux",IdJeu);
        //Exectution de la requête
        if( !Requete.exec() )
        {
            qDebug() << getLastExecutedQuery(Requete) << Requete.lastError();
        }
    }
}

void F_MainWindow::on_menuUtilisateurEnCours_triggered(QAction *caller)
{
    ui->menuUtilisateur->setTitle(caller->text());
    iIdBenevole=caller->data().toInt();
}

uint F_MainWindow::RecupereIdBenevole()
{
    return iIdBenevole;
}

void F_MainWindow::slot_AfficherJeu(QString CodeJeu)
{
    ui->TbW_Main->setCurrentIndex(this->trouveOnglet("admin"));
    on_Bt_Jeux_clicked();
    pAjoutSuppModifJeux->AfficherJeu(CodeJeu);
}

void F_MainWindow::on_Menu_Exporter_inventaire_triggered()
{
    /*QXlsx::Document xModele("ModeleInventaire.xlsx");
    QXlsx::Document xInventaire;
    QString CellAdr;
    QString CellAdr2;

    for(int i =1;i<23;i++)
    {
        CellAdr="A"+QString::number(i);
        Cell* cell=xModele.cellAt(CellAdr);
        QXlsx::Format fmt;
        fmt=cell->format();
        QXlsx::Format fmt2;
        fmt2.setFontBold(fmt.fontBold());
        xInventaire.write(CellAdr, xModele.read(CellAdr),fmt2);
        if(i<12)
        {
            CellAdr2="A"+QString::number(i)+":J"+QString::number(i);
            xInventaire.mergeCells(CellAdr2, fmt2);
        }
        else
        {
            CellAdr="B"+QString::number(i);
            xInventaire.write(CellAdr, xModele.read(CellAdr),fmt2);
        }
    }
    xInventaire.saveAs("Inventaire.xlsx");*/
    QSqlQuery Requete;
    QXlsx::Document xInventaire("Inventaire.xlsx");
    QDate DernierSeptembre=QDate::currentDate();
    int annee=DernierSeptembre.year();
    if(DernierSeptembre.month()<9)
    {
        annee--;
    }
    DernierSeptembre.setDate(annee,9,1);
    Requete.prepare("select CodeJeu,StatutJeux_IdStatutJeux,(Jeux_IdJeux IS NOT NULL) as Sortie from jeux LEFT JOIN emprunts ON IdJeux=Jeux_IdJeux AND DateEmprunt > '"+
                    DernierSeptembre.toString("yyyy-MM-dd")+"' order by CAST(CodeJeu as UNSIGNED)");
    qDebug() << getLastExecutedQuery(Requete) << Requete.lastError();
    //Exectution de la requête
    if( !Requete.exec() )
    {
        qDebug() << getLastExecutedQuery(Requete) << Requete.lastError();
        return;
    }
    int i=0;
    QString CellAdr;
    QStringList Extensions;
    QList<int> ExtStatut;
    QList<bool> ExtSorti;
    while(Requete.next())
    {
        QString CodeJeu=ObtenirValeurParNom(Requete,"CodeJeu").toString();
        QXlsx::Format format;
        format.setBorderStyle(Format::BorderThin);
        bool ok;
        CodeJeu.toInt(&ok, 10);
        if(!ok)
        {
            Extensions << CodeJeu;
            ExtStatut << ObtenirValeurParNom(Requete,"StatutJeux_IdStatutJeux").toInt();
            ExtSorti << ObtenirValeurParNom(Requete,"Sortie").toBool();
            continue;
        }
        // Si il y a des extensions de sauvegarder et qu'on est au début ou à la fin de la ligne
        if(Extensions.size()>0 && i%10==9)
        {
            for(int j = 1; j <= Extensions.size();j++)
            {
                CellAdr=char(('A'+((i % 10)+j)))+QString::number(24+int(i/10));
                format=formatCell(format, ExtStatut.at(j-1),ExtSorti.at(j-1));
                xInventaire.write(CellAdr, Extensions.at(j-1),format);
            }
            Extensions.clear();
            ExtStatut .clear();
            ExtSorti .clear();
        }
        CellAdr=char('A'+((i++) % 10))+QString::number(24+int(i/10));
        qDebug()<<CodeJeu<<ObtenirValeurParNom(Requete,"StatutJeux_IdStatutJeux").toInt()<<ObtenirValeurParNom(Requete,"Sortie").toBool();
        format=formatCell(format,ObtenirValeurParNom(Requete,"StatutJeux_IdStatutJeux").toInt(),ObtenirValeurParNom(Requete,"Sortie").toBool());
        xInventaire.write(CellAdr, CodeJeu, format);
    }
    xInventaire.saveAs("Inventaire.xlsx");
}

Format F_MainWindow::formatCell(Format format,int StatutJeu, bool Sorti)
{
    switch(StatutJeu)
    {
        // En réparation
        case 4 :
        {
            format.setPatternBackgroundColor(QColor(Qt::black));
            break;
        }
        // Inventorié
        case 10 :
        {
            format.setPatternBackgroundColor(QColor(Qt::green));
            break;
        }
        // Emprunté
        case 3 :
        {
            format.setPatternBackgroundColor(QColor(Qt::magenta));
            break;
        }
        default :
        {
            if(Sorti)
            {
                format.setPatternBackgroundColor(QColor(Qt::white));
            }
            else
            {
                format.setPatternBackgroundColor(QColor(Qt::gray));
            }
        }
    }
    return format;
}
