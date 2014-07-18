// f_statistiques.h	1.0	<02/2013>	<NORMAND Julien><Padiou Nicolas>
#ifndef F_PREFERENCES_H
#define F_PREFERENCES_H

// En-têtes standards necessaires dans ce fichier en-tête seulement ------------
#include <QtWidgets>
#include <QtWidgets>

// En-têtes propres à  l'application necessaires dans ce fichier en-tête --------
#include "f_popupclesttem.h"

namespace Ui {
class F_Preferences;
}

/**
 *  @brief F_Preferences permet de modifier les options du programme.
 *
 *  Les options disponibles dans cette classe sont:
 *      - Préférences Générales avec :
 *          - Nombre de jeux autorisés par défaut.
 *          - Prix d'une unité de location.
 *          - Nombre de jours de retard toléré.
 *          - Nombre de jours avant l'envoi d'un mail.
 *          - Modification du code administrateur.
 *      - Réseaux avec :
 *          - Information de la BDD.
 *          - Messagerie.
 *      - Membres avec :
 *          - Titre.
 *          - Type.
 *      - Jeux avec :
 *          - Type.
 *          - Etat.
 *          - Statut.
 *          - Emplacement.
 *          - Chemin des règles des jeux.
 *          - Chemin des images des jeux.
 *      - Informations de la ludothèque avec :
 *          - Lieux.
 */
class F_Preferences : public QDialog
{
    Q_OBJECT
    
public:
    // METHODEs ----------------------------------------------------------------
    // CONSTRUCTEURs et DESTRUCTEURs
    /**
     *  @brief Constructeur de la classe.
     *
     *  @param parent
     */
    explicit F_Preferences(QWidget *parent = 0);

    /**
     *  @brief Destructeur de la classe.
     *
     */
    ~F_Preferences();

    void closeEvent( QCloseEvent * event ) ;

    /**
     *  @brief Permet de changer d'anglais.
     *
     */
    void SelectionnerOnglet( int nOnglet ) ;

signals:
    void SignalFermerFenetre() ;

public slots:
    // METHODEs ----------------------------------------------------------------
    /**
     *  @brief Slot qui récupère un signal pour la classe F_Preferences.
     *
     */
    void slot_Valider();

private slots:
    // METHODEs ----------------------------------------------------------------
    /**
     *  @brief Permet d'enregistrer les actions effectuées depuis l'ouverture de la F_Preferences.
     *
     */
    void on_Bt_Enregistrer_clicked();

    /**
     *  @brief Permet d'annuler tous les actions effectuées depuis l'ouverture de la F_Preferences.
     *
     */
    void on_Bt_Annuler_clicked();

    /**
     *  @brief Sélectionne la ligne entière du tableau titre de membre.
     *
     *  @param index
     */
    void on_TbV_MembresTitre_clicked(const QModelIndex &index);

    /**
     *  @brief Sélectionne la ligne entière du tableau type de membre.
     *
     *  @param index
     */
    void on_TbV_MembresType_clicked(const QModelIndex &index);

    /**
     *  @brief Sélectionne la ligne entière du tableau paiement de membre.
     *
     *  @param index
     */
    void on_TbV_MembresPaiement_clicked(const QModelIndex &index);

    /**
     *  @brief Sélectionne la ligne entière du tableau type d'emprunt.
     *
     *  @param index
     */
    void on_TbV_EmpruntType_clicked(const QModelIndex &index);

    /**
     *  @brief Sélectionne la ligne entière du tableau type de jeu.
     *
     *  @param index
     */
    void on_TbV_JeuxType_clicked(const QModelIndex &index);

    /**
     *  @brief Sélectionne la ligne entière du tableau etat de jeu.
     *
     *  @param index
     */
    void on_TbV_JeuxEtat_clicked(const QModelIndex &index);

    /**
     *  @brief Sélectionne la ligne entière du tableau statut de jeu.
     *
     *  @param index
     */
    void on_TbV_JeuxStatut_clicked(const QModelIndex &index);

    /**
     *  @brief Sélectionne la ligne entière du tableau emplacement de jeu.
     *
     *  @param index
     */
    void on_TbV_JeuxEmplacement_clicked(const QModelIndex &index);

    /**
     *  @brief Sélectionne la ligne entière du tableau lieux de info ludo.
     *
     *  @param index
     */
    void on_TbV_InfoLieux_clicked(const QModelIndex &index);

    /**
     *  @brief Permet d'ajouter un titre d'un membre.
     *
     */
    void on_Bt_AjouterMembreTitre_clicked();

    /**
     *  @brief Permet de modifier un titre d'un membre.
     *
     */
    void on_Bt_ModifierMembreTitre_clicked();

    /**
     *  @brief Permet de supprimer un titre d'un membre.
     *
     */
    void on_Bt_SupprimerMembreTitre_clicked();

    /**
     *  @brief Permet d'ajouter un type de membre.
     *
     */
    void on_Bt_AjouterMembreType_clicked();

    /**
     *  @brief Permet de modifier un type de membre.
     *
     */
    void on_Bt_ModifierMembreType_clicked();

    /**
     *  @brief Permet de supprimer un type d'un membre.
     *
     */
    void on_Bt_SupprimerMembreType_clicked();

    /**
     *  @brief Permet de ajouter un paiement de membre.
     *
     */
    void on_Bt_AjouterMembrePaiement_clicked();

    /**
     *  @brief Permet de modifier un paiement de membre.
     *
     */
    void on_Bt_ModifierMembrePaiement_clicked();

    /**
     *  @brief Permet de supprimer un paiement de membre.
     *
     */
    void on_Bt_SupprimerMembrePaiement_clicked();

    /**
     *  @brief Permet d'ajouter un type d'emprunt.
     *
     */
    void on_Bt_AjouterEmpruntType_clicked();

    /**
     *  @brief Permet de modifier un type d'emprunt.
     *
     */
    void on_Bt_ModifierEmpruntType_clicked();

    /**
     *  @brief Permet de supprimer un type d'emprunt.
     *
     */
    void on_Bt_SupprimerEmpruntType_clicked();

    /**
     *  @brief Permet d'ajouter un type de jeu.
     *
     */
    void on_Bt_AjouterJeuxType_clicked();

    /**
     *  @brief Permet de modifier un type de jeu.
     *
     */
    void on_Bt_ModifierJeuxType_clicked();

    /**
     *  @brief Permet de supprimer un type de jeu.
     *
     */
    void on_Bt_SupprimerJeuxType_clicked();

    /**
     *  @brief Permet d'ajouter un etat de jeu.
     *
     */
    void on_Bt_AjouterEtat_clicked();

    /**
     *  @brief Permet de modifier un etat de jeu.
     *
     */
    void on_Bt_ModifierEtat_clicked();

    /**
     *  @brief Permet de supprimer un etat de jeu.
     *
     */
    void on_Bt_SupprimerEtat_clicked();

    /**
     *  @brief Permet d'ajouter un statut de jeu.
     *
     */
    void on_Bt_AjouterStatut_clicked();

    /**
     *  @brief Permet de modifier un statut de jeu.
     *
     */
    void on_Bt_ModifierStatut_clicked();

    /**
     *  @brief Permet de supprimer un statut de jeu.
     *
     */
    void on_Bt_SupprimerStatut_clicked();

    /**
     *  @brief Permet d'ajouter un emplacement de jeu.
     *
     */
    void on_Bt_AjouterEmplacement_clicked();

    /**
     *  @brief Permet de modifier un emplacement de jeu.
     *
     */
    void on_Bt_ModifierEmplacement_clicked();

    /**
     *  @brief Permet de supprimer un emplacement de jeu.
     *
     */
    void on_Bt_SupprimerEmplacement_clicked();

    /**
     *  @brief Permet de définir le chemin des règles des jeux.
     *
     */
    void on_Bt_ParcourirRegle_clicked();

    /**
     *  @brief Permet de definir le chemin des images des jeux.
     *
     */
    void on_Bt_ParcourirImage_clicked();

    /**
     *  @brief Permet d'ajouter un lieu de info ludo.
     *
     */
    void on_Bt_AjouterLieux_clicked();

    /**
     *  @brief Permet de modifier un lieu de info ludo.
     *
     */
    void on_Bt_ModifierLieux_clicked();

    /**
     *  @brief Permet de supprimer un lieu de info ludo.
     *
     */
    void on_Bt_SupprimerLieux_clicked();

    /**
     *  @brief Récupèration de la fonction pour l'affichage du style XX XX XX XX XX.
     *
     *  @param arg1
     */
    void on_LE_NumeroTel_textEdited(const QString &arg1);

    /**
     *  @brief Récupèration de la fonction pour l'affichage du style XX XX XX XX XX.
     *
     *  @param arg1
     */
    void on_LE_NumeroFax_textEdited(const QString &arg1);

    /**
     * @brief Test la nouvelle connection à la nouvelle BDD.
     *
     */
    void on_Bt_Connection_clicked();

    /**
     *  @brief Verifie si le nouveau code administrateur est correct.
     *
     */
    void on_Bt_ValiderCode_clicked();

    /**
     *  @brief Affiche le manuel d'utilisation du programme.
     *
     */
    void on_Bt_Aide_clicked();

private:
    // ATTRIBUTs ----------------------------------------------------------------
    ///< Pointeur de la classe graphique.
    Ui::F_Preferences *ui;

    ///< Pointeur qui contient l'adresse de la fenêtre graphique F_PopUpCLESTTEM.
    F_PopUpCLESTTEM * pTitreMembreAjMod;

    ///< Pointeur qui contient l'adresse de la fenêtre graphique F_PopUpCLESTTEM.
    F_PopUpCLESTTEM * pTypeMembreAjMod;

    ///< Pointeur qui contient l'adresse de la fenêtre graphique F_PopUpCLESTTEM.
    F_PopUpCLESTTEM * pPaiementMembreAjMod;

    ///< Pointeur qui contient l'adresse de la fenêtre graphique F_PopUpCLESTTEM.
    F_PopUpCLESTTEM * pTypeEmpruntAjMod;

    ///< Pointeur qui contient l'adresse de la fenêtre graphique F_PopUpCLESTTEM.
    F_PopUpCLESTTEM * pTypeJeuAjMod;

    ///< Pointeur qui contient l'adresse de la fenêtre graphique F_PopUpCLESTTEM.
    F_PopUpCLESTTEM * pEtatJeuAjMod;

    ///< Pointeur qui contient l'adresse de la fenêtre graphique F_PopUpCLESTTEM.
    F_PopUpCLESTTEM * pStatutJeuAjMod;

    ///< Pointeur qui contient l'adresse de la fenêtre graphique F_PopUpCLESTTEM.
    F_PopUpCLESTTEM * pEmplacementJeuAjMod;

    ///< Pointeur qui contient l'adresse de la fenêtre graphique F_PopUpCLESTTEM.
    F_PopUpCLESTTEM * pLieuxInfoLudoAjMod;

    ///< Creer un modele du tableau pour les types des membres.
    QStandardItemModel * TbMembresType;

    ///< Creer un modele du tableau pour les titres des membres.
    QStandardItemModel * TbMembresTitre;

    ///< Creer un modele du tableau pour les paiements des membres.
    QStandardItemModel * TbMembresPaiement;

    ///< Creer un modele du tableau pour les type d'emprunt.
    QStandardItemModel * TbEmpruntType;

    ///< Creer un modele du tableau pour les types des jeux.
    QStandardItemModel * TbJeuxType;

    ///< Creer un modele du tableau pour les statut des jeux.
    QStandardItemModel * TbJeuxStatut;

    ///< Creer un modele du tableau pour les états des jeux.
    QStandardItemModel * TbJeuxEtat;

    ///< Creer un modele du tableau pour les états des jeux.
    QStandardItemModel * TbJeuxEmplacement;

    ///< Creer un modele du tableau pour les lieux de l'info ludo.
    QStandardItemModel * TbInfoLudoLieux;

    ///< Récupère la valeur du nom du type membre.
    QString sTypeMembre;

    ///< Récupère la valeur du titre membre.
    QString sTitreMembre;

    ///< Récupère la valeur du paiement membre.
    QString sPaiementMembre;

    ///< Récupère la valeur du type emprunt.
    QString sTypeEmprunt;

    ///< Récupère la valeur du type jeux.
    QString sTypeJeux;

    ///< Récupère la valeur de l'état jeux.
    QString sEtatJeux;

    ///< Récupère la valeur du statut jeux.
    QString sStatutJeux;

    ///< Récupère la valeur du statut jeux.
    QString sEmplacementJeux;

    ///< Récupère la valeur du lieu de la ludothèque.
    QString sLieuxInfoLudo;

    // METHODEs -----------------------------------------------------------------
    /**
     *  @brief Le line edit qui récupère cette fonction se met en forme sous le type XX XX XX XX XX.
     *
     *  @param sNumero
     *  @return QString
     */
    QString BlocNumeroTelFax(QString sNumero);

    /**
     *  @brief Consulte la BDD et récupère les informations de celle-ci. Il sont ensuite mis dans les champs correspondant.
     *
     */
    void AfficherAutresInformations();

    /**
     *  @brief Fonction qui permet de mettre à  jour le tableau contenant toutes les informations de la BDD.
     *
     */
    void AfficherTousLesTableaux();
};

//------------------------------------------------------------------------------
#endif // F_PREFERENCES_H
//------------------------------------------------------------------------------
