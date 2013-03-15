#ifndef F_MEMBRES_H
#define F_MEMBRES_H

#include "f_recherchemembres.h"
#include "f_historiquejeux.h"
#include "f_ajoutercoticarte.h"
#include "f_popupclesttem.h"
#include "TitreType.h"
#include <QtSql/QtSql>
#include <QtGui>

// Declaration des TYPEs UTILISATEURs ------------------------------------------

namespace Ui {
class F_Membres;
}

/** @brief Permet de gérer gérer les membres
 *
 *  Il est possible d'y ajouter et de modifier un membre.
 *  On peut consulter les abonnements en cour ainsi que les emprunts en cours et l'historique des emprunts d'un membre.
 */
class F_Membres : public QWidget
{
    Q_OBJECT
    
public:
    // CONSTRUCTEURs et DESTRUCTEURs
    /** Initialisation de la classe
     *  @param  pRechercheMembres : point sur un F_RechercheMembres, bAdmin : Indique si c'est une fenêtre administration
     *  @test   Voir la procédure dans le fichier associé.
     */
    explicit F_Membres(F_RechercheMembres * pRechercheMembres = NULL, bool bAdmin = false, QWidget *parent = 0) ;
    ~F_Membres() ;


// METHODEs -----------------------------------------------------------------

    //! Recupere l'emplacement d'un titre dans un vecteur en fonction de son id
    int RecupererEmplacementTitreVecteur (unsigned int nIdTitre) ;
    //! Recupere l'emplacement d'un type dans un vecteur en fonction de son id
    int RecupererEmplacementTypeVecteur (unsigned int nIdType) ;

    //! Permet d'ajouter un membre
    bool AjouterMembre () ;

    //! Permet de modifier un membre
    bool ModifierMembre ( unsigned int nIdMembre ) ;

    //! Met à  jour la liste des Titres depuis la base de données
    void MaJTitre () ;

    //! Met à  jour la liste des Types depuis la base de données
    void MaJType () ;

    //! Affiche les jeux empruntés dans le tableau TbW_Emprunt
    void AfficherJeuxEmpruntes (unsigned int nIdMembre) ;

    //! Affiche les informations d'un membre
    void AfficherMembre ( unsigned int nIdMembre ) ;

    //! Affiche le membre précédement selectionné par défaut
    void AfficherMembre() ;

    //! Permet de supprimer un membre
    bool SupprimerMembre(int IdMembre) ;

    //! Verrouille ou deverouille les champs contenant les informations personnelles
    void VerrouillerInfosPerso ( bool bVerrouille ) ;

    //! Efface le contenu de tous les champs
    void EffacerTousLesChamps () ;

    //! Affiche ou cache les boutons PB_AjouterMembre et PB_ModifierMembre
    void AfficherAjouterModifierMembre ( bool bAffiche ) ;

    //! Affiche ou cache les boutons PB_Valider et PB_Annuler
    void AfficherValiderAnnuler ( bool bAffiche ) ;

    //! Verrouille ou deverouille l'emplacement des emprunts
    void VerrouillerJeux (bool bVerrouille ) ;

    //! Verouille ou deverouille l'emplacement des abonnements
    void VerrouillerAbonnements ( bool bVerrouille ) ;

    //! Efface tous ce qui n'est pas un chiffre et ajoute un espace tous les 2 caractéres dans un string
    QString ModifierSyntaxeNumTelephone ( QString sNum) ;

    //! Affiche les abonnements dans le tableview abonnement
    void AfficherAbonnements( int nIdMembre ) ;

    //! Affiche les ville dans le combobox et selectionne la ville passé en paramétre
    void AfficherVilles( QString VilleSelectionne = "" ) ;

public slots:

    //! Permet l'ajout d'un type
    void slot_ChoisirNouveauType () ;

    //! Permet l'ajout d'un titre
    void slot_ChoisirNouveauTitre () ;

    //! Recupere l'id d'un membre pour l'afficher
    void slot_AfficherMembre (unsigned int nIdMembre) ;

    //! Acualise les abonnements du membre sélectionné
    void slot_ActualiserAbonnements() ;

    //! Permet l'ajout d'une ville
    void slot_ValiderAjoutVille() ;

    //! Permet l'annulation de l'ajour d'une ville
    void slot_AnnulerAjoutVille() ;
    
private slots:
    //! Permet l'ajout d'un membre
    void on_Bt_AjouterMembre_clicked() ;

    //! Permet d'ajout un nouveau type si "Créer type..." a été sélectionné
    void on_CBx_Type_activated(int index);

    //! Permet d'ajout un nouveau titre si "Créer titre..." a été sélectionné
    void on_CBx_Titre_activated(int index);

    //! Annule l'ajout ou la modification d'un membre
    void on_Bt_AnnulerMembre_clicked() ;

    //! Valide la modification ou l'ajout d'un membre
    void on_Bt_ValiderMembre_clicked() ;

    //! Met en forme (du type "00 00 00 00 00")  le numéro de télephone quand il est modifié
    void on_Le_TelFix_textEdited(const QString &arg1) ;

    //! Met en forme (du type "00 00 00 00 00")  le numéro de télephone quand il est modifié
    void on_Le_TelMobile_textEdited(const QString &arg1) ;

    //! Met en forme (du type "00 00 00 00 00")  le numéro de télephone quand il est modifié
    void on_LE_Fax_textEdited(const QString &arg1) ;

    //! Affiche l'historique des jeux empruntés
    void on_Bt_Historique_clicked() ;

    //! Affiche ou cache membre écarté
    void on_ChBx_MembreEcarte_toggled(bool checked) ;

    //! Permet de modifier un membre
    void on_Bt_ModifierMembre_clicked() ;

    //! Active ou désactive le bouton valider en fonction de lineedit nom
    void on_Le_Nom_textEdited(const QString &arg1);

    //! Validation suppression d'un membre
    void on_Bt_SupprimerMembre_clicked();

    //! Selectionne une ligne entière dans le tableau LW_Abonnememnt
    void on_LW_Abonnements_clicked(const QModelIndex &index);

    //! Selectionne une ligne entière dans e tableau LW_Empruntes
    void on_LW_JeuxEmpruntes_clicked(const QModelIndex &index);

    //! Permet l'ajout d'un abonnement
    void on_Bt_AjouterAbonnement_clicked();

    //! Permet la modifiaction d'un abonnement
    void on_Bt_ModifierAbonnement_clicked();

    //! Permet de supprimer un abonnement
    void on_Bt_SupprimerAbonnement_clicked();

    //! Change de la couleur et met en gras membre  écarter quand le checkbox et check
    void on_ChBx_MembreEcarte_stateChanged(int arg1);

    //! Permet l'ajout de Ajouter une ville dans le combox Ville
    void on_CBx_Ville_currentIndexChanged(const QString &arg1);

    void on_LW_Abonnements_doubleClicked(const QModelIndex &index);

private:
    Ui::F_Membres * ui;

    F_RechercheMembres * pRechercheMembres ; //! Pointeur sur la classe F_RechercheMembres
    F_HistoriqueJeux *   pHistoriqueJeux ;   //! Pointeur sur la classe F_HistoriqueJeux
    F_AjouterCotiCarte * pAjouterCotiCarte ; //! Pointeur sur la classe F_AjouterCoticarte

    F_PopUpCLESTTEM * pTypeAjMod ;      //! Pointeur sur la classe F_AjouterTitreType avec comme paramètre (0)
    F_PopUpCLESTTEM * pTitreAjMod ;     //! Pointeur sur la classe F_AjouterTitreType avec comme paramètre (1)

    QVector<Type>    VectorType ;            //! Vecteur contenant la liste des types (id, nom)
    QVector<Titre>   VectorTitre ;           //! Vecteur contenant la liste des titres (id, nom)
    QVector<QString> VecteurVille ;          //! Vecteur contenant la liste des villes
    QVector<int>     VecteurAbonnements ;    //! Vecteur contenant la liste des des Id des abonnements présent dans le tableau

    unsigned int nIdMembreSelectionne ;      //! Permet de savoir quelle membre est sélectionné

    bool bAdmin ;                            //! Indique les droits d'adminstration (vrai = admin, faux = simple bénévole)



    /////////////Fenêtre d'ajout d'une ville///////////////
    QDialog * oFenetreAjoutVille ;                     //! Fenêtre pour l'ajout d'une ville
    QLineEdit * LE_AjoutVille ;                   //! LigneEdit permettant d'indiquer la ville
    QPushButton * Bt_ValiderVille ;          //! Bouton Valider pour la fenêtre d'ajout d'une ville
    QPushButton * Bt_AnnuerVille ;           //! Bouton Annuler pour la fenêtre d'ajout d'une ville
    QLabel * Lb_NomVille ;                   //! Label pour la fenêtre d'ajout d'une ville
};

#endif // F_MEMBRES_H
