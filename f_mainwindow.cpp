#include "f_mainwindow.h"
#include "ui_f_mainwindow.h"
#include "f_imprimeretiquettejeu.h"
#include "f_imprimerfichecompletejeu.h"

F_MainWindow::F_MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::F_MainWindow)
{
    ui->setupUi(this);
    //qDebug()<<"Constructeur F_MainWindow = Début";
    ui->centralWidget->setLayout(ui->gridLayout);

    ui->menuImprimer->setEnabled(false);
    ui->Menu_Fichier_Quitter->setShortcut(tr("Ctrl+Q"));
    ui->Menu_Edition_Preferences->setShortcut(tr("Ctrl+P"));
    ui->Menu_Aide_Aide->setShortcut(tr("F1"));
    ui->Menu_Aide_Propos_LudOpen->setShortcut(tr("F2"));
    ui->Menu_Jeux_Imprimer_Etiquette->setShortcut(tr("Ctrl+E"));
    ui->Menu_Jeux_Imprimer_Fiche_Complete->setShortcut(tr("Ctrl+F"));

    this->pPreferences = new F_Preferences;
    this->pPreferences->setWindowTitle("Préférences");
    this->pPreferences->setWindowModality(Qt::ApplicationModal);

    //Layout///////////////

    //Widgets-onglets//////
    this->pEmprunt=new F_Emprunt (this->ui->Emprunt);
    this->pJeux=new F_Jeux (this->ui->Jeux);
    this->pRetour=new F_Retour (this->ui->Retour);
    this->pListeJeux=new F_ListeJeux (this->ui->ListeJeux);
    this->pRechercheMembres = new F_RechercheMembres (this->ui->Membres) ;
    this->pMembres=new F_Membres (this->pRechercheMembres, false, this->ui->Membres);
    this->pListeMembres = new F_ListeMembres( false ,ui->ListeMembres ) ;
    this->pRetards=new F_Retards (this->ui->Retards);
    this->pListeReservations = new F_ListeReservations( ui->ListeReservations ) ;
    this->pPostIt=new F_POSTIT (this->ui->PostIt) ;

    //Widget-admin/////////
    ////Fournisseur////////
    this->pAjoutSuppModifFournisseurEditeurs=new F_AjoutSuppModifFournisseursEditeurs (this->ui->admin);
    this->ui->Lay_Admin->addWidget(this->pAjoutSuppModifFournisseurEditeurs);
    this->pAjoutSuppModifFournisseurEditeurs->setVisible(false);

    ////Membre/////////////
    this->pRechercheMembresAdmin = new F_RechercheMembres (this->ui->admin) ;
    this->pAdministrerMembres=new F_Membres (this->pRechercheMembresAdmin, true,this->ui->admin);
    this->pAdministrerMembres->setVisible(false);
    this->ui->Lay_Admin->addWidget(this->pRechercheMembresAdmin);
    this->ui->Lay_Admin->addWidget(this->pAdministrerMembres);
    this->pRechercheMembresAdmin->setLayout(ui->Lay_Admin);
    this->pAdministrerMembres->setVisible(true);


    ////Liste Membres//////
    this->pListeMembresAdmin = new F_ListeMembres( true, this->ui->admin ) ;
    this->pListeMembresAdmin->setVisible( false ) ;
    this->ui->Lay_Admin->addWidget( this->pListeMembresAdmin ) ;
    this->pListeMembresAdmin->setLayout( ui->Lay_Admin ) ;

    ////Statistiques////////
    this->pStatistiques=new F_Statistiques (this->ui->admin);
    this->ui->Lay_Admin->addWidget(this->pStatistiques);
    this->pStatistiques->setVisible(false);

    ////Jeux////////
    this->pAjoutSuppModifJeux=new F_AjoutSuppModifJeux (this->ui->admin);
    this->ui->Lay_Admin->addWidget(this->pAjoutSuppModifJeux);
    this->pAjoutSuppModifJeux->setVisible(false);

    ////Abonnement////////
    this->pAbonnements=new F_Abonnements(this->ui->admin);
    this->ui->Lay_Admin->addWidget(this->pAbonnements);
    this->pAbonnements->setVisible(false);

    this->pPopUpCode = new F_PopUpCode;
    this->pPopUpCode->setWindowTitle("Code d'accés");
    this->pPopUpCode->setWindowModality(Qt::ApplicationModal);
    /*****************************************************************************/

    //Membres
    this->ui->Lay_Membres->addWidget(this->pRechercheMembres);
    this->ui->Lay_Membres->addWidget(this->pMembres);

    //Liste Membres
    ui->Lay_ListeMembres->addWidget( this->pListeMembres ) ;

    //Jeu
    this->ui->Lay_Jeux->addWidget(this->pJeux);

    //Liste Jeux
    this->ui->Layout_ListeJeux->addWidget(this->pListeJeux);

    //Emprunt
    this->ui->Lay_Emprunt->addWidget(this->pEmprunt);

    //Liste Réservations
    ui->Lay_ListeReservations->addWidget( this->pListeReservations ) ;

    //Retour
    this->ui->Lay_Retour->addWidget(this->pRetour);

    //Retards
    this->ui->Layout_Retards->addWidget(this->pRetards);

    //PostIt
    this->ui->Lay_PostIt->addWidget(this->pPostIt);

    connect(this->pPopUpCode, SIGNAL(SignalOnglet()), this, SLOT(on_ChangerOnglet()));
    connect( this->pListeMembres, SIGNAL( SignalSelectionMembre( uint ) ), this->pMembres, SLOT( on_AfficherMembre( uint ) ) ) ;
    connect( this->pListeMembresAdmin, SIGNAL( SignalSelectionMembre( uint ) ), this->pAdministrerMembres, SLOT( on_AfficherMembre( uint ) ) ) ;
    connect( this->pListeMembres, SIGNAL( SignalSelectionMembre( uint ) ), this, SLOT( on_ChangerOnglet() ) ) ;
    connect( this->pListeMembresAdmin, SIGNAL( SignalSelectionMembre( uint ) ), this, SLOT( on_Bt_Membre_clicked() ) ) ;
    connect( this->pPreferences, SIGNAL( SignalFermerFenetre() ), this, SLOT( on_Preference() ) ) ;
    connect( this->pListeJeux, SIGNAL( Signal_DoubleClic_ListeJeux( QString ) ), this, SLOT( on_DoubleClic_ListeJeux(QString) )) ;

    // Afficher les post-it au démarage de l'application
    ui->TbW_Main->setCurrentIndex(9);

    //qDebug()<<"Constructeur F_MainWindow = OK";
}

F_MainWindow::~F_MainWindow()
{
    delete ui;
}

void F_MainWindow::on_ChangerOnglet()
{
    ui->TbW_Main->setCurrentIndex( 0 ) ;
}

void F_MainWindow::on_Preference()
{
    this->pAdministrerMembres->MaJTitre() ;
    this->pAdministrerMembres->MaJType() ;
    this->pAdministrerMembres->AfficherMembre() ;
    this->pMembres->MaJTitre() ;
    this->pMembres->MaJType() ;
    this->pMembres->AfficherMembre() ;
}

void F_MainWindow::on_Bt_Membre_clicked()
{
    this->pAjoutSuppModifJeux->setVisible(false);
    this->pListeMembresAdmin->setVisible( false ) ;
    this->pAjoutSuppModifFournisseurEditeurs->setVisible(false);
    this->pAbonnements->setVisible(false);
    this->pStatistiques->setVisible(false);
    this->pRechercheMembresAdmin->setVisible(true);
    this->pAdministrerMembres->setVisible(true);
}

void F_MainWindow::on_Bt_Jeux_clicked()
{
    this->pAdministrerMembres->setVisible(false);
    this->pListeMembresAdmin->setVisible( false ) ;
    this->pAjoutSuppModifFournisseurEditeurs->setVisible(false);
    this->pAbonnements->setVisible(false);
    this->pStatistiques->setVisible(false);
    this->pRechercheMembresAdmin->setVisible(false);
    this->pAjoutSuppModifJeux->setVisible(true);
}

void F_MainWindow::on_Bt_FournisseurEditeur_clicked()
{
    this->pAdministrerMembres->setVisible(false);
    this->pListeMembresAdmin->setVisible( false ) ;
    this->pAjoutSuppModifJeux->setVisible(false);
    this->pAbonnements->setVisible(false);
    this->pStatistiques->setVisible(false);
    this->pRechercheMembresAdmin->setVisible(false);
    this->pAjoutSuppModifFournisseurEditeurs->setVisible(true);
}

void F_MainWindow::on_Bt_Abonnements_clicked()
{
    this->pAdministrerMembres->setVisible(false);
    this->pListeMembresAdmin->setVisible( false ) ;
    this->pAjoutSuppModifFournisseurEditeurs->setVisible(false);
    this->pAjoutSuppModifJeux->setVisible(false);
    this->pStatistiques->setVisible(false);
    this->pRechercheMembresAdmin->setVisible(false);
    this->pAbonnements->setVisible(true);
}

void F_MainWindow::on_Bt_Statistiques_clicked()
{
    this->pAdministrerMembres->setVisible(false);
    this->pListeMembresAdmin->setVisible( false ) ;
    this->pAjoutSuppModifJeux->setVisible(false);
    this->pAjoutSuppModifFournisseurEditeurs->setVisible(false);
    this->pAbonnements->setVisible(false);
    this->pRechercheMembresAdmin->setVisible(false);
    this->pStatistiques->setVisible(true);
}

void F_MainWindow::on_Bt_PostIt_clicked()
{
    this->pAdministrerMembres->setVisible(false);
    this->pListeMembresAdmin->setVisible( false ) ;
    this->pAjoutSuppModifJeux->setVisible(false);
    this->pAjoutSuppModifFournisseurEditeurs->setVisible(false);
    this->pAbonnements->setVisible(false);
    this->pRechercheMembresAdmin->setVisible(false);
    this->pStatistiques->setVisible(false);
    this->pPostIt->setVisible(true);
}

void F_MainWindow::on_Bt_ListeMembres_clicked()
{
    this->pAdministrerMembres->setVisible(false);
    this->pAjoutSuppModifJeux->setVisible(false);
    this->pAjoutSuppModifFournisseurEditeurs->setVisible(false);
    this->pAbonnements->setVisible(false);
    this->pRechercheMembresAdmin->setVisible(false);
    this->pStatistiques->setVisible(false) ;
    this->pListeMembresAdmin->setVisible( true ) ;
}

//Changement d'onglet
void F_MainWindow::on_TbW_Main_currentChanged(int index)
{
    switch(index)
    {
    case 0 : //Membre
        ui->menuImprimer->setEnabled(false);
        this->pRechercheMembres->MaJListeMembres() ;
        this->pRechercheMembres->AfficherListe() ;
        this->pMembres->MaJTitre() ;
        this->pMembres->MaJType() ;
        this->pMembres->AfficherMembre() ;
        break;

    case 1 : //Liste Membres
        this->pListeMembres->AffichageListe() ;
        break ;
    case 2 : //Jeux
        // Rendre visible le menu jeu pour imprimer les étiquettes et les fiches des jeux
        // A_FAIRE : ne rendre possible l'impression que quand un jeu a été choisi sur l'onglet JEUX
        //if ( !this->pJeux->get_JeuEnConsultation().isEmpty() )
        {
            ui->menuImprimer->setEnabled(true);
        }
        //this->pJeux->ActualiserJeux();
        break;
    case 3 : //Liste jeux
        ui->menuImprimer->setEnabled(false);
        break;
    case 4 : //Emprunt
        ui->menuImprimer->setEnabled(false);
        this->pEmprunt->ActualiserJeu();
        this->pEmprunt->ActualiserMembre();
        break;
    case 5 : //Retour
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
        // Désactive le menu Jeux
        ui->menuImprimer->setEnabled(false);
        this->pListeReservations->AffichageListe() ;
        break;
    case 7 : //retards
        ui->menuImprimer->setEnabled(false);
        this->pRetards->MaJListe();
        break;
    case 8 : //Administration
        //this->pPopUpCode->show();  // Pas de code pour le moment, trop chiant"
        ui->menuImprimer->setEnabled(false);
        this->pRechercheMembresAdmin->MaJListeMembres() ;
        this->pRechercheMembresAdmin->AfficherListe() ;
        this->pListeMembresAdmin->AffichageListe() ;
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

void F_MainWindow::on_Menu_Fichier_Quitter_triggered()
{
    this->close();
}

void F_MainWindow::on_Menu_Edition_Preferences_triggered()
{
    this->pPreferences->show();
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
void F_MainWindow::on_DoubleClic_ListeJeux(QString CodeJeu)
{
    // Indiquer à l'onglet jeu quel code jeu afficher
    this->pJeux->set_JeuEnConsultation(CodeJeu);
    // Afficher le détail du jeu choisi
    this->pJeux->ActualiserJeux();
    // Faire apparaître l'onglet Jeu
    ui->TbW_Main->setCurrentIndex(2);
}

void F_MainWindow::on_Menu_Jeux_Imprimer_Etiquette_triggered()
{
    F_ImprimerEtiquetteJeu f_ImprimerEtiquetteJeu ;
    f_ImprimerEtiquetteJeu.ImprimerEtiquetteJeu(this->pJeux->get_JeuEnConsultation() ) ;
    f_ImprimerEtiquetteJeu.exec() ;
}

void F_MainWindow::on_Menu_Jeux_Imprimer_Fiche_Complete_triggered()
{
    F_ImprimerFicheCompleteJeu f_ImprimerFicheCompleteJeu ;
    f_ImprimerFicheCompleteJeu.ImprimerFicheJeuComplete(this->pJeux->get_JeuEnConsultation()) ;
    f_ImprimerFicheCompleteJeu.exec() ;
}

void F_MainWindow::on_Menu_Aide_Propos_LudOpen_triggered()
{
    QMessageBox APropos;

    APropos.about(this, "A propos de ...", "<center><IMG SRC=\"Ludopen.png\" ALIGN=\"MIDDLE\" ALT=\"LudOpen\"></center><br>"
                  "LudOpen version 2012.12.22<br>"
                  "Programme créé avec Qt Creator 2.4.1 - Qt 4.8.1<br><br>"
                  "Copyright © BOTHEREL Phillipe, MARY Florian, NORMAND Julien, PADIOU Nicolas, SOREL William. Tous droits réservés.");
    //APropos.setWindowIcon(QIcon(""));
}
