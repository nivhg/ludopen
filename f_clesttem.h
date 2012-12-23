// f_clesttem.h	0.1	<07/2012>	<NORMAND Julien>
#ifndef F_CLESTTEM_H
#define F_CLESTTEM_H

// En-têtes standards necessaires dans ce fichier en-tête seulement ------------
#include <QWidget>
#include <QtGui>

// En-têtes propres à l'application necessaires dans ce fichier en-tête --------
#include "f_popupclesttem.h"

namespace Ui {
class F_CLESTTEM;
}

/**
 *  @brief F_CLESTTEM est une classe qui regroupe tous les ajouts, les modifications, les suppressions dans la BDD.
 *
 *  Les options disponibles dans cette classe sont:
 *      - Jeux avec :
 *          - Etat qui dispose le numéro 0 pour les options.
 *          - Statut qui dispose le numéro 1 pour les options.
 *          - Emplacement qui dispose le numéro 2 pour les options.
 *          - Type qui dispose le numéro 3 pour les options.
 *      - Membres avec :
 *          - Type qui dispose le numéro 5 pour les options.
 *          - Titre qui dispose le numéro 6 pour les options.
 *      - Emprunt avec :
 *          - Type qui dispose le numéro 7 pour les options.
 *      - Informations de la ludothèque avec :
 *          - Lieux qui dispose le numéro 8 pour les options.
 *
 */
class F_CLESTTEM : public QWidget
{
    Q_OBJECT
    
public:
    // METHODEs -----------------------------------------------------------------
    // CONSTRUCTEURs et DESTRUCTEURs
    /**
     *  @brief Constructeur de la classe.
     *
     *  @param parent
     */
    explicit F_CLESTTEM(QWidget *parent = 0);

    /**
     *  @brief Destructeur de la classe.
     *
     */
    ~F_CLESTTEM();

public slots:
    // METHODEs ----------------------------------------------------------------
    /**
     *  @brief Slot qui récupère un signal pour la classe F_Preferences.
     *
     */
    void on_Valider();

private slots:
    // METHODEs ----------------------------------------------------------------
    /**
     *  @brief Sélectionne la ligne entière du tableau titre de membres.
     *
     *  @param index
     */
    void on_TbV_TitreMembre_clicked(const QModelIndex &index);

    /**
     *  @brief Permet d'ajouter un titre d'un membre.
     *
     */
    void on_Bt_AjouterTitreMembre_clicked();

    /**
     *  @brief Permet de modifier un titre d'un membre.
     *
     */
    void on_Bt_ModifierTitreMembre_clicked();

    /**
     *  @brief Permet de supprimer un titre d'un membre.
     *
     */
    void on_Bt_SupprimerTitreMembre_clicked();

    /**
     *  @brief Sélectionne la ligne entière du tableau type de membres.
     *
     *  @param index
     */
    void on_TbV_TypeMembre_clicked(const QModelIndex &index);

    /**
     *  @brief Permet d'ajouter un type d'un membre.
     *
     */
    void on_Bt_AjouterTypeMembre_clicked();

    /**
     *  @brief Permet de modifier un type d'un membre.
     *
     */
    void on_Bt_ModifierTypeMembre_clicked();

    /**
     *  @brief Permet de supprimer un type d'un membre.
     *
     */
    void on_Bt_SupprimerTypeMembre_clicked();

    /**
     *  @brief Sélectionne la ligne entière du tableau mode de paiement d'un membre.
     *
     *  @param index
     */
    void on_TbV_PaiementMembre_clicked(const QModelIndex &index);

    /**
     *  @brief Permet d'ajouter un mode de paiement d'un membre.
     *
     */
    void on_Bt_AjouterPaiementMembre_clicked();

    /**
     *  @brief Permet de modifier un mode de paiement d'un membre.
     *
     */
    void on_Bt_ModifierPaiementMembre_clicked();

    /**
     *  @brief Permet de supprimer un mode de paiement d'un membre.
     *
     */
    void on_Bt_SupprimerPaiementMembre_clicked();

    /**
     *  @brief Sélectionne la ligne entière du tableau emplacement des jeux.
     *
     *  @param index
     */
    void on_TbV_EmplacementJeux_clicked(const QModelIndex &index);

    /**
     *  @brief Permet d'ajouter un emplacement de jeu.
     *
     */
    void on_Bt_AjouterEmplacementJeux_clicked();

    /**
     *  @brief Permet de modifier un emplacement de jeu.
     *
     */
    void on_Bt_ModifierEmplacementJeux_clicked();

    /**
     *  @brief Permet de supprimer un emplacement de jeu.
     *
     */
    void on_Bt_SupprimerEmplacementJeux_clicked();

    /**
     *  @brief Sélectionne la ligne entière du tableau état des jeux.
     *
     *  @param index
     */
    void on_TbV_EtatJeux_clicked(const QModelIndex &index);

    /**
     *  @brief Permet d'ajouter un état de jeu.
     *
     */
    void on_Bt_AjouterEtatJeux_clicked();

    /**
     *  @brief Permet de modifier un état de jeu.
     *
     */
    void on_Bt_ModifierEtatJeux_clicked();

    /**
     *  @brief Permet de supprimer un état de jeu.
     *
     */
    void on_Bt_SupprimerEtatJeux_clicked();

    /**
     *  @brief Sélectionne la ligne entière du tableau statut des jeux.
     *
     *  @param index
     */
    void on_TbV_StatutJeux_clicked(const QModelIndex &index);

    /**
     *  @brief Permet d'ajouter un statut de jeu.
     *
     */
    void on_Bt_AjouterStatutJeux_clicked();

    /**
     *  @brief Permet de modifier un statut de jeu.
     *
     */
    void on_Bt_ModifierStatutJeux_clicked();

    /**
     *  @brief Permet de supprimer un statut de jeu.
     *
     */
    void on_Bt_SupprimerStatutJeux_clicked();

    /**
     *  @brief Sélectionne la ligne entière du tableau type des jeux.
     *
     *  @param index
     */
    void on_TbV_TypeJeux_clicked(const QModelIndex &index);

    /**
     *  @brief Permet d'ajouter un type de jeu.
     *
     */
    void on_Bt_AjouterTypeJeux_clicked();

    /**
     *  @brief Permet de modifier un type de jeu.
     *
     */
    void on_Bt_ModifierTypeJeux_clicked();

    /**
     *  @brief Permet de supprimer un type de jeu.
     *
     */
    void on_Bt_SupprimerTypeJeux_clicked();

    /**
     *  @brief Sélectionne la ligne entière du tableau type d'emprunt.
     *
     *  @param index
     */
    void on_TbV_TypeEmprunt_clicked(const QModelIndex &index);

    /**
     *  @brief Permet d'ajouter un type d'emprunt.
     *
     */
    void on_Bt_AjouterTypeEmprunt_clicked();

    /**
     *  @brief Permet de modifier un type d'emprunt.
     *
     */
    void on_Bt_ModifierTypeEmprunt_clicked();

    /**
     *  @brief Permet de supprimer un type d'emprunt.
     *
     */
    void on_Bt_SupprimerTypeEmprunt_clicked();

    /**
     *  @brief Sélectionne la ligne entière du tableau lieu de la ludothèque.
     *
     *  @param index
     */
    void on_TbV_LieuxLudo_clicked(const QModelIndex &index);

    /**
     *  @brief Permet d'ajouter un lieu de la ludothèque.
     *
     */
    void on_Bt_AjouterLieuxLudo_clicked();

    /**
     *  @brief Permet de modifier un lieu de la ludothèque.
     *
     */
    void on_Bt_ModifierLieuxLudo_clicked();

    /**
     *  @brief Permet de supprimer un lieu de la ludothèque.
     *
     */
    void on_Bt_SpupprimerLieuxLudo_clicked();

private:
    // ATTRIBUTs ----------------------------------------------------------------
    ///< Pointeur de la classe graphique.
    Ui::F_CLESTTEM *ui;

    ///< Pointeur qui contient l'adresse de la fenêtre graphique F_PopUpCLESTTEM.
    F_PopUpCLESTTEM * pTitreMembreAjMod;

    ///< Pointeur qui contient l'adresse de la fenêtre graphique F_PopUpCLESTTEM.
    F_PopUpCLESTTEM * pTypeMembreAjMod;

    ///< Pointeur qui contient l'adresse de la fenêtre graphique F_PopUpCLESTTEM.
    F_PopUpCLESTTEM * pPaiementMembreAjMod;

    ///< Pointeur qui contient l'adresse de la fenêtre graphique F_PopUpCLESTTEM.
    F_PopUpCLESTTEM * pEmplacementJeuAjMod;

    ///< Pointeur qui contient l'adresse de la fenêtre graphique F_PopUpCLESTTEM.
    F_PopUpCLESTTEM * pEtatJeuAjMod;

    ///< Pointeur qui contient l'adresse de la fenêtre graphique F_PopUpCLESTTEM.
    F_PopUpCLESTTEM * pStatutJeuAjMod;

    ///< Pointeur qui contient l'adresse de la fenêtre graphique F_PopUpCLESTTEM.
    F_PopUpCLESTTEM * pTypeJeuAjMod;

    ///< Pointeur qui contient l'adresse de la fenêtre graphique F_PopUpCLESTTEM.
    F_PopUpCLESTTEM * pTypeEmpruntAjMod;

    ///< Pointeur qui contient l'adresse de la fenêtre graphique F_PopUpCLESTTEM.
    F_PopUpCLESTTEM * pLieuxInfoLudoAjMod;

    ///< Creer un modele du tableau pour les types des membres.
    QStandardItemModel * TbTypeMembre;

    ///< Creer un modele du tableau pour les titres des membres.
    QStandardItemModel * TbTitreMembre;

    ///< Creer un modele du tableau pour les titres des membres.
    QStandardItemModel * TbPaiementMembre;

    ///< Creer un modele du tableau pour les états des jeux.
    QStandardItemModel * TbEmplacementJeux;

    ///< Creer un modele du tableau pour les états des jeux.
    QStandardItemModel * TbEtatJeux;

    ///< Creer un modele du tableau pour les statut des jeux.
    QStandardItemModel * TbStatutJeux;

    ///< Creer un modele du tableau pour les types des jeux.
    QStandardItemModel * TbTypeJeux;

    ///< Creer un modele du tableau pour les types des emprunts.
    QStandardItemModel * TbTypeEmprunt;

    ///< Creer un modele du tableau pour les lieux de l'info ludo.
    QStandardItemModel * TbLieuxLudo;

    ///< Récupère la valeur du nom du type membre.
    QString sTypeMembre;

    ///< Récupère la valeur du titre membre.
    QString sTitreMembre;

    ///< Récupère la valeur du titre membre.
    QString sPaiementMembre;

    ///< Récupère la valeur du statut jeux.
    QString sEmplacementJeux;

    ///< Récupère la valeur de l'état jeux.
    QString sEtatJeux;

    ///< Récupère la valeur du statut jeux.
    QString sStatutJeux;

    ///< Récupère la valeur du type jeux.
    QString sTypeJeux;

    ///< Récupère la valeur du type emprunts.
    QString sTypeEmprunt;

    ///< Récupère la valeur du lieu de la ludothèque.
    QString sLieuxLudo;

    // METHODEs -----------------------------------------------------------------
    /**
     *  @brief Fonction qui permet de mettre à  jour le tableau contenant toutes les informations de la BDD.
     *
     */
    void MettreAJourBDD();
};

//------------------------------------------------------------------------------
#endif // F_CLESTTEM_H
//------------------------------------------------------------------------------
