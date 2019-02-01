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

    this->pPopUpCode = new D_PopUpCode;
    this->pPopUpCode->setWindowFlag(Qt::Dialog);
    this->pPopUpCode->setWindowFlag(Qt::WindowCloseButtonHint,false);
    this->pPopUpCode->setWindowFlag(Qt::WindowMaximizeButtonHint,false);
    //this->pPopUpCode->setWindowTitle("Identification");
    //this->pPopUpCode->setWindowModality(Qt::ApplicationModal);

    connect(this->pPopUpCode, SIGNAL(SignalMembreIdentifier(uint)), this, SLOT(slot_MembreIdentifier(uint)));
    this->pPopUpCode->exec();

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

    // Si il y a plus d'un écran, on affiche le calendrier des malles aux adhérents
    if(QApplication::desktop()->screenCount() > 1)
    {
        this->pCalendrierMalles=new F_Malles(this);
        QRect screenres = QApplication::desktop()->screenGeometry(1);
        pCalendrierMalles->move(QPoint(screenres.x(), screenres.y()));
        pCalendrierMalles->resize(screenres.width(), screenres.height());
        pCalendrierMalles->setWindowModality(Qt::NonModal);
        pCalendrierMalles->AfficherCalendrier();
        pCalendrierMalles->showFullScreen();
        pCalendrierMalles->show();
        connect(this->pPanier,SIGNAL(Signal_Nouvelle_Malle()),pCalendrierMalles,SLOT(slot_actualiserCalendrier()));
    }
    else
    {
        this->pCalendrierMalles=0;
    }

    CreerReleve();
    Relevetimer = new QTimer(this);
    Relevetimer->setSingleShot(true);
    TimerProchainePermanence();
    connect(Relevetimer, SIGNAL(timeout()), SLOT(verifReleve()));
    qDebug()<<"Constructeur F_MainWindow = OK";
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
    if(this->pJeux)
    {

    }
    if(!this->pAjoutSuppModifJeux)
    {

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
        this->pAjoutSuppModifJeux=new F_AjoutSuppModifJeux (this->ui->admin);
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
    else if(tab=="jeu") //Jeux
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
        ui->menuImprimer->setEnabled(false);
        this->pEmprunt->ActualiserJeu();
        this->pEmprunt->ActualiserMembre();
    }
    else if(tab=="retour") //Retour
    {
        CreerRetour();
        // Désactive le menu Jeux
        ui->menuImprimer->setEnabled(false);
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
        //CreerRetards();
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
        ui->menuImprimer->setEnabled(false);
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

/*void F_MainWindow::CreerRetards()
{
    if(!this->pRetards)
    {
        qDebug()<<"Création F_Retards";
        this->pRetards=new F_Retards (this->ui->Retards);
        //Retards
        this->ui->Layout_Retards->addWidget(this->pRetards);
        // Si double clic dans la liste des retards sur un membre, affiche la fiche détaillée du membre sélectionné
        connect( this->pRetards, SIGNAL( Signal_DoubleClic_ListeMembres( uint ) ), this , SLOT( slot_DoubleClic_ListeMembres ( uint )) ) ;
    }
}*/

void F_MainWindow::CreerReservations()
{
    if(!this->pListeReservations)
    {
        qDebug()<<"Création F_ListeReservation";
        this->pListeReservations=new F_ListeReservations(this->ui->Listes);
        this->ui->Lay_Listes->addWidget(this->pListeReservations);
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
        this->pJeux=new F_Jeux (this->ui->Jeu);
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
        qDebug()<<"Création F_Emprunt";
        this->pEmprunt=new F_Emprunt (MODE_EMPRUNT,this->ui->Emprunt);
        //Emprunt
        this->ui->Lay_Emprunt->addWidget(this->pEmprunt);
        connect( this->pEmprunt, SIGNAL( Signal_Reservation_Malle(int) ), this, SLOT( slot_Reservation_Malle(int) )) ;
        connect( this->pEmprunt, SIGNAL( Signal_AjouterAuPanier(QString,uint,double,int,QString,QList<QSqlQuery *> *) ), this->pPanier,
                 SLOT( slot_AjouterAuPanier(QString,uint,double,int,QString,QList<QSqlQuery *> *)) );
        connect( this->pEmprunt, SIGNAL( Signal_VerifMembrePanier(uint IdDuMembre)), this->pPanier,
                 SLOT( slot_VerifMembrePanier(uint IdDuMembre)) );
        if(!pCalendrierMalles)
            connect(this->pEmprunt,SIGNAL(Signal_Nouvelle_Malle()),pCalendrierMalles,SLOT(slot_actualiserCalendrier()));
    }
}

void F_MainWindow::CreerRetour()
{
    if(!this->pRetour)
    {
        qDebug()<<"Création F_Retour";
        this->pRetour=new F_Retour (this->ui->Retour);
        //Retour
        this->ui->Lay_Retour->addWidget(this->pRetour);
        connect( this->pRetour, SIGNAL( Signal_AjouterAuPanier(QString,uint,double,int,QString,QList<QSqlQuery *> *) ), this->pPanier,
                 SLOT( slot_AjouterAuPanier(QString,uint,double,int,QString,QList<QSqlQuery *> *)) ) ;
    }
}

void F_MainWindow::CreerMalle()
{
    if(!this->pMalles)
    {
        qDebug()<<"Création F_Emprunt pour onglet réservation Malles";
        this->pMalles=new F_Emprunt (MODE_MALLES, this->ui->Malles,pCalendrierMalles);
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
        this->pReleve=new D_Releve(NULL,iIdBenevole);
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
   // ne rendre possible l'impression que quand un jeu a été choisi sur l'onglet JEUX
   if ( !this->pJeux->get_JeuEnConsultation().isEmpty() )
   {
       F_ImprimerEtiquetteJeu f_ImprimerEtiquetteJeu ;
       f_ImprimerEtiquetteJeu.ImprimerEtiquetteJeu(this->pJeux->get_JeuEnConsultation() ) ;
       f_ImprimerEtiquetteJeu.exec() ;
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
    int iIdMalle=this->pMalles->get_MalleReserveeSelectionnee();
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
    int iIdMalle=this->pMalles->get_MalleEmprunteeSelectionnee();
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
    ui->TbW_Main->setCurrentIndex(6);
}

void F_MainWindow::verifReleve()
{
    QStringList Postes=F_Preferences::ObtenirValeur("PosteReleveCaisse").split(",");
    if(Postes.contains(QHostInfo::localHostName(),Qt::CaseInsensitive))
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
    qDebug() << getLastExecutedQuery(Requete);
    bool Retour=Requete.next();
    int Difference;
    QDateTime FuturPerm;
    int iFuturPerm=0;
    // Recherche de la prochaine permanence
    FuturPerm=TrouverProchainePerm(Maintenant,Permanences,&iFuturPerm);
    qDebug()<<FuturPerm;
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
    qDebug()<<FuturPerm;
    // On calcule la différence de temps entre la futur perm et maintenant et on retire l'interval défini pour le relevé de caisse (30 min)
    Difference=Maintenant.secsTo(FuturPerm);
    Difference=Difference-F_Preferences::ObtenirValeur("IntervalReleveCaisse").toInt()*60;
    qDebug()<<Difference;
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

    if(this->pRetour && this->pRetour->ModelJeuEmpruntes->rowCount()>0)
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
