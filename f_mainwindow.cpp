#include "f_mainwindow.h"
#include "ui_f_mainwindow.h"
#include "f_imprimeretiquettejeu.h"
#include "f_imprimerfichecompletejeu.h"

F_MainWindow::F_MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::F_MainWindow)
{
    ui->setupUi(this);
    qDebug()<<"Constructeur F_MainWindow = Début";

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

    //Widget-admin/////////
    ////Fournisseur////////
    this->pAjoutSuppModifFournisseurEditeurs=0;
    this->pAdministrerMembres=0;
    this->pListeMembresAdmin = 0;
    this->pStatistiques=0;
    this->pAjoutSuppModifJeux=0;
    this->pAbonnements=0;
    this->pPopUpCode = 0;
    /*****************************************************************************/

    this->VerifierConnexionBDD() ;

    qDebug()<<"Création F_POSTIT";
    this->pPostIt=new F_POSTIT (this->ui->PostIt) ;

    //PostIt
    this->ui->Lay_PostIt->addWidget(this->pPostIt);

    // Afficher les post-it au démarrage de l'application
    ui->TbW_Main->setCurrentIndex(9);

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

    // Ouverture du fichier INI.
    QSettings settings("config.ini", QSettings::IniFormat);

    // Recupération des valeurs dans le fichier INI.
    sNomBDD = settings.value("BaseDeDonnees/NomDeLaBDD", "config").toString();
    sAdresseIP = settings.value("BaseDeDonnees/AdresseIP", "config").toString();
    sNomUtilisateur = settings.value("BaseDeDonnees/NomUtilisateur", "config").toString();
    sMotDePasse = settings.value("BaseDeDonnees/MotDePasse", "config").toString();
    nPort = settings.value("BaseDeDonnees/Port", "config").toInt();

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
        F_Preferences f_Preferences;
        connect( &f_Preferences, SIGNAL( SignalFermerFenetre() ), this, SLOT( slot_Preferences() ) ) ;
        f_Preferences.exec();
        // choisir l'onglet des préférences Réseau de la fenêtre Préférences
        f_Preferences.SelectionnerOnglet( 1 ) ;
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
    ChangerFenetre(this->pAdministrerMembres);
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
    ChangerFenetre(this->pAjoutSuppModifJeux);
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
    ChangerFenetre(this->pAjoutSuppModifFournisseurEditeurs);
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
    ChangerFenetre(this->pAbonnements);
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
    ChangerFenetre(this->pStatistiques);
}

void F_MainWindow::on_Bt_ListeMembres_clicked()
{
    if(!this->pListeMembresAdmin)
    {
        ////Liste Membres//////
        qDebug()<<"Création ADMIN-F_ListeMembres";
        this->pListeMembresAdmin = new F_ListeMembres( true, this->ui->admin ) ;
        this->pListeMembresAdmin->setVisible( false ) ;
        this->ui->Lay_Admin->addWidget( this->pListeMembresAdmin ) ;
        // Si double clic dans la liste des membres sur un membre,
        // affiche la fiche détaillée du membre sélectionné
        connect( this->pListeMembresAdmin,
                 SIGNAL( Signal_DoubleClic_ListeMembres( uint ) ), this,
                 SLOT( slot_DoubleClic_ListeMembresAdmin ( uint )) ) ;
        this->pListeMembresAdmin->AffichageListe() ;
    }
    ChangerFenetre(this->pListeMembresAdmin);
}

/** @brief Cache les différentes fenêtres admin et affiche celle passée en argument
 *  @param  QWidget : fenêtre à afficher
 */
void F_MainWindow::ChangerFenetre(QWidget *w)
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
    switch(index)
    {
    case 0 : //Membre
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
        break;

    case 1 : //Liste Membres
        CreerListeMembres();
        ui->menuImprimer->setEnabled(false);
        this->pListeMembres->AffichageListe() ;
        break ;
    case 2 : //Jeux
        // Rendre visible le menu jeu pour imprimer les étiquettes et les fiches des jeux
        // A_FAIRE : ne rendre possible l'impression que quand un jeu a été choisi sur l'onglet JEUX
        //if ( !this->pJeux->get_JeuEnConsultation().isEmpty() )
    {
        CreerJeux();
        ui->menuImprimer->setEnabled(true);
        this->pJeux->ActualiserJeux();
    }
        break;
    case 3 : //Liste jeux
        CreerListeJeux();
        ui->menuImprimer->setEnabled(false);
        break;
    case 4 : //Emprunt
        CreerEmprunt();
        ui->menuImprimer->setEnabled(false);
        this->pEmprunt->ActualiserJeu();
        this->pEmprunt->ActualiserMembre();
        break;
    case 5 : //Retour
        CreerRetour();
        // Désactive le menu Jeux
        ui->menuImprimer->setEnabled(false);
        // actualise les infos afficher sur le membre sélectionné actuellement
        this->pRetour->ActualiserMembre();
        // actualise les jeux à retourner pour le membre sélectionné actuellement
        this->pRetour->ActualiserJeu();
        // Remet à jour la liste de membres ayant un retour à faire
        this->pRetour->ActualiserListeEmprunteurs();
        break;
    case 6 : //Liste Réservations
        CreerReservations();
        // Désactive le menu Jeux
        ui->menuImprimer->setEnabled(false);
        this->pListeReservations->AffichageListe() ;
        break;
    case 7 : //retards
        CreerRetards();
        ui->menuImprimer->setEnabled(false);
        this->pRetards->MaJListe();
        break;
    case 8 : //Administration
        //Widget-admin/////////
        CreerAdminMembres();

        /*****************************************************************************/
        //this->pPopUpCode->show();  // Pas de code pour le moment, trop chiant"
        /*if(!this->pPopUpCode)
        {
                qDebug()<<"Création ADMIN-F_PopUpCode";
                this->pPopUpCode = new F_PopUpCode;
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
        break;
    case 9 : //PostIt
        ui->menuImprimer->setEnabled(false);
        this->pPostIt->setVisible(true);
        break;
    }
}

void F_MainWindow::CreerAdminMembres()
{
    if(!this->pAdministrerMembres)
    {
        ////Membre/////////////
        qDebug()<<"Création ADMIN-F_Membres";
        this->pAdministrerMembres=new F_Membres (true,this->ui->admin);
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
        this->pRetards=new F_Retards (this->ui->Retards);
        //Retards
        this->ui->Layout_Retards->addWidget(this->pRetards);
        // Si double clic dans la liste des retards sur un membre, affiche la fiche détaillée du membre sélectionné
        connect( this->pRetards, SIGNAL( Signal_DoubleClic_ListeMembres( uint ) ), this , SLOT( slot_DoubleClic_ListeMembres ( uint )) ) ;
    }
}

void F_MainWindow::CreerReservations()
{
    if(!this->pListeReservations)
    {
        qDebug()<<"Création F_ListeReservation";
        this->pListeReservations = new F_ListeReservations( ui->ListeReservations ) ;
        //Liste Réservations
        ui->Lay_ListeReservations->addWidget( this->pListeReservations ) ;
    }
}

void F_MainWindow::CreerMembre()
{
    if(!this->pMembres)
    {
        qDebug()<<"Création F_Membres";
        this->pMembres=new F_Membres (false, this->ui->Membre);
        //Membres
        this->ui->Lay_Membres->addWidget(this->pMembres);
        connect( this->pListeMembresAdmin, SIGNAL( Signal_DoubleClic_ListeMembres( uint ) ), this->pMembres, SLOT( slot_AfficherMembre( uint ) ) ) ;
    }
}

void F_MainWindow::CreerListeMembres()
{
    if(!this->pListeMembres)
    {
        qDebug()<<"Création F_ListeMembres";
        this->pListeMembres = new F_ListeMembres( false ,ui->ListeMembres ) ;
        //Liste Membres
        ui->Lay_ListeMembres->addWidget( this->pListeMembres ) ;
        connect( this->pListeMembres, SIGNAL( Signal_DoubleClic_ListeMembres( uint ) ), this , SLOT( slot_DoubleClic_ListeMembres( uint ) ) ) ;
    }
}

void F_MainWindow::CreerJeux()
{
    if(!this->pJeux)
    {
        qDebug()<<"Création F_Jeux";
        this->pJeux=new F_Jeux (this->ui->Jeu);
        //Jeux
        this->ui->Lay_Jeux->addWidget(this->pJeux);
    }
}

void F_MainWindow::CreerListeJeux()
{
    if(!this->pListeJeux)
    {
        qDebug()<<"Création F_ListeJeux";
        this->pListeJeux=new F_ListeJeux (this->ui->ListeJeux);
        //Liste Jeux
        this->ui->Layout_ListeJeux->addWidget(this->pListeJeux);

        // Si double clic dans la liste des jeux sur un jeu, affiche la fiche détaillée du jeu sélectionné
        connect( this->pListeJeux, SIGNAL( Signal_DoubleClic_ListeJeux( QString ) ), this, SLOT( slot_DoubleClic_ListeJeux(QString) )) ;
    }
}

void F_MainWindow::CreerEmprunt()
{
    if(!this->pEmprunt)
    {
        qDebug()<<"Création F_Emprunt";
        this->pEmprunt=new F_Emprunt (this->ui->Emprunt);
        //Emprunt
        this->ui->Lay_Emprunt->addWidget(this->pEmprunt);
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
    }
}

void F_MainWindow::on_Menu_Fichier_Quitter_triggered()
{
    this->close();
}

void F_MainWindow::on_Menu_Edition_Preferences_triggered()
{
   // Création de la fenêtre de choix des préférences du logiciel
   F_Preferences f_Preferences;
   connect( &f_Preferences, SIGNAL( SignalFermerFenetre() ), this, SLOT( slot_Preferences() ) ) ;
   f_Preferences.exec();
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
    ui->TbW_Main->setCurrentIndex(0);
}

/**
 * Quand double clic sur un membre dans l'onglet Admin/Liste des membres,
 * affiche l'onglet Admin/Membres avec le membre concerné pour avoir le détail
 * grâce à un signal envoyé par f_listemembres.
 * @param index Ligne choisie dans le modèle associé au tableau TbV_Recherche
 */
void F_MainWindow::slot_DoubleClic_ListeMembresAdmin(uint IdMembre)
{
    CreerMembre();
    // Indiquer à l'onglet Membre quel membre afficher
    this->pAdministrerMembres->slot_AfficherMembre( IdMembre );
    // Faire apparaître l'onglet Admin/Membre
    ChangerFenetre(this->pAdministrerMembres);
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
      QMessageBox::information(this, "Pas de jeu sélectionné !", "Vous n'avez choisi aucun jeu dans la liste des jeux.\nVeuillez en sélectionner un avant de lancer l'impression de sa fiche complête.", "OK") ;
   }
}

void F_MainWindow::on_Menu_Aide_Propos_LudOpen_triggered()
{
    QMessageBox APropos;

    APropos.about(this, "A propos de ...", "<center><IMG SRC=\":LudOpen.png\" ALIGN=\"MIDDLE\" ALT=\"LudOpen\"></center><br>"
                  "LudOpen version 2014.09.01<br><br>"
                  "Programme créé avec Qt Creator 3.0.1 - Qt 5.2.1<br><br>"
                  "<a href='http://code.google.com/p/ludopen'>http://code.google.com/p/ludopen</a><br><br>"
                  "Copyright © BOTHEREL Phillipe, MARY Florian, NORMAND Julien, PADIOU Nicolas, SOREL William, VICTORIN Vincent. Tous droits réservés.");
    //APropos.setWindowIcon(QIcon(""));
}
