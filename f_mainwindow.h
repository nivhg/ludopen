#ifndef F_MAINWINDOW_H
#define F_MAINWINDOW_H

#include <QDebug>
#include <QtWidgets\QMainWindow>
#include "f_emprunt.h"
#include "f_jeux.h"
#include "f_retour.h"
#include "f_membres.h"
#include "f_listejeux.h"
#include "f_ajoutsuppmodiffournisseursediteurs.h"
#include "f_statistiques.h"
#include "f_ajoutsuppmodifjeux.h"
#include "f_abonnements.h"
#include "f_recherchemembres.h"
#include "f_paiement.h"
#include "f_preferences.h"
#include "f_retards.h"
#include "f_listemembres.h"
#include "f_popupcode.h"
#include "f_postit.h"
#include "f_listereservations.h"

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

public slots:
    void slot_ChangerOnglet();
    void slot_Preferences() ;
    void slot_DoubleClic_ListeJeux(QString CodeJeu);
    void slot_DoubleClic_ListeMembres(uint IdMembre) ;

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
    F_RechercheMembres * pRechercheMembres ;
    F_RechercheMembres * pRechercheMembresAdmin ;
    F_ListeMembres * pListeMembres ;
    F_ListeMembres * pListeMembresAdmin ;
    F_Membres * pAdministrerMembres;
    F_Paiement * pPaiement;
    F_Retards * pRetards;
    F_PopUpCode * pPopUpCode;
    F_POSTIT * pPostIt ;
    F_ListeReservations * pListeReservations ;
};

#endif // F_MAINWINDOW_H
