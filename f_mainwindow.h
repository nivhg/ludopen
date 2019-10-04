#ifndef F_MAINWINDOW_H
#define F_MAINWINDOW_H

class F_Retour;
class F_Jeux;
class F_AjoutSuppModifFournisseursEditeurs;

#include <QDebug>
#include <QMainWindow>
#include <QHostInfo>

#include "f_emprunt.h"
#include "f_jeux.h"
#include "f_retour.h"
#include "f_membres.h"
#include "f_listejeux.h"
#include "f_ajoutsuppmodiffournisseursediteurs.h"
#include "f_statistiques.h"
#include "f_ajoutsuppmodifjeux.h"
#include "f_abonnements.h"
#include "f_paiement.h"
#include "f_preferences.h"
#include "f_retards.h"
#include "f_listemembres.h"
#include "d_popupcode.h"
#include "f_postit.h"
#include "f_listereservations.h"
#include "f_malles.h"
#include "f_imprimeretiquettejeu.h"
#include "f_imprimerfichecompletejeu.h"
#include "f_imprimermalle.h"
#include "f_panier.h"
#include "d_releve.h"
#include "d_resamisdecote.h"
#include "fonctions_globale.h"
#include "majeur.h"

#define PERM_JOUR 0
#define PERM_DEBUT 1
#define PERM_FIN 2

namespace Ui {
class F_MainWindow;
}

class F_MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit F_MainWindow(QWidget *parent = 0);
    ~F_MainWindow();
    void VerifierConnexionBDD() ;
    /** @brief Cache les différentes fenêtres admin et affiche celle passée en argument
     */
    void ChangerFenetreAdmin(QWidget *w=NULL);
    void ChangerFenetreListes(QWidget *w=NULL);

    // Fonction de création des interfaces
    void CreerAdminMembres();
    void CreerRetards();
    void CreerMembre();
    void CreerListeMembres();
    void CreerJeux();
    void CreerListeJeux();
    void CreerEmprunt();
    void CreerRetour();
    void CreerReservations();
    void CreerMalle();
    void CreerReleve();
    uint RecupereIdBenevole();

public slots:
    void slot_ChangerOnglet();
    void slot_Preferences() ;
    void slot_DoubleClic_ListeJeux(QString CodeJeu);
    void slot_DoubleClic_ListeMembres(uint IdMembre) ;
    void slot_DoubleClic_ListeMembresAdmin(uint IdMembre) ;
    void slot_Clic_Reserve(int IdReservation);
    void slot_Reservation_Malle(int iIdMalle);
    void slot_PlusTardReleve();
    void slot_ReleveFini();
    void slot_MembreIdentifier(uint iIdMembre);
    void slot_MiseAJourNbItemsPanier(uint iNbItems);
    void slot_verifReservation();
    void slot_AfficherJeu(QString CodeJeu);

private slots:
    void on_Bt_Membre_clicked();
    void on_Bt_Jeux_clicked();
    void on_Bt_FournisseurEditeur_clicked();
    void on_Bt_Abonnements_clicked();
    void on_Bt_Statistiques_clicked();
    //void on_Bt_PostIt_clicked();
    void on_TbW_Main_currentChanged(int index);
    void on_Menu_Fichier_Quitter_triggered();
    void on_Menu_Aide_Propos_LudOpen_triggered();
    void on_Menu_Edition_Preferences_triggered();
    void on_Menu_Aide_Aide_triggered();
    void on_Menu_Jeux_Imprimer_Etiquette_triggered();
    void on_Menu_Jeux_Imprimer_Fiche_Complete_triggered();
    void on_Bt_ListeMembres_clicked();
    void on_Menu_Imprimer_Malle_Reservee_triggered();
    void on_Menu_Imprimer_Malle_Empruntee_triggered();
    void verifReleve();
    void on_Bt_ListeJeux_clicked();
    void on_Bt_ListeReservations_clicked();
    void on_menuUtilisateurEnCours_triggered(QAction *);
    void on_Bt_Retards_clicked();
    void Quitter();

private:
    Ui::F_MainWindow *ui;
    F_Emprunt * pEmprunt ;
    F_Jeux * pJeux ;
    F_Retour * pRetour;
    F_Membres * pMembres;
    F_ListeJeux * pListeJeux;
    F_AjoutSuppModifFournisseursEditeurs * pAjoutSuppModifFournisseurEditeurs;
    F_Statistiques * pStatistiques;
    F_AjoutSuppModifJeux * pAjoutSuppModifJeux;
    F_Abonnements * pAbonnements;
    F_ListeMembres * pListeMembres ;
    F_ListeMembres * pListeMembresAdmin ;
    F_Membres * pAdministrerMembres;
    F_Paiement * pPaiement;
    F_Retards * pRetards;
    F_Preferences * pPreferences;
    D_PopUpCode * pPopUpCode;
    F_POSTIT * pPostIt ;
    F_ListeReservations * pListeReservations ;
    F_Emprunt * pMalles;
    F_Malles * pCalendrierMalles;
    D_Releve * pReleve;
    F_Panier * pPanier;
    QTimer *Relevetimer;
    QTimer *Reservationtimer;
    uint iIdBenevole;
    QSystemTrayIcon *IconeBarreTache;
    QDateTime FuturPerm;

    void TimerProchainePermanence();
    QDateTime TrouverProchainePerm(QDateTime LaDate,QList<QList <QVariant>> Permanences,int *iFuturPerm);
    int trouveOnglet(QString NomOnglet);
    void closeEvent(QCloseEvent *event);
};

#endif // F_MAINWINDOW_H
