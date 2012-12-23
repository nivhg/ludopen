// f_abonnements.h	0.1	<01/2012>	<NORMAND Julien>
#ifndef F_ABONNEMENTS_H
#define F_ABONNEMENTS_H

// En-têtes standards necessaires dans ce fichier en-tête seulement ------------
#include <QWidget>
#include <QtGui>

namespace Ui {
class F_Abonnements;
}

/**
 *  @brief F_Abonnements permet la gestion des abonnements dans la partie "Administration".
 *
 *  Les options disponibles dans cette classe sont:
 *      - l'ajout d'un abonnement.
 *      - la suppression d'un abonnement.
 *      - la modification d'un abonnement.
 *      - la consultaion d'un abonnement.
 */
class F_Abonnements : public QWidget
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
    explicit F_Abonnements(QWidget *parent = 0);

    /**
     *  @brief Destructeur de la classe.
     *
     */
    ~F_Abonnements();

private slots:
    // METHODEs ----------------------------------------------------------------- 
    /**
     *  @brief Permet de faire une recherche depuis la BDD.
     *
     */
    void on_Bt_Recherche_clicked();

    /**
     *  @brief Permet d'enregister dans la BDD, les actions effectuées après le clic sur le bouton "Ajouter" ou "Modifier".
     *
     */
    void on_Bt_Valider_clicked();

    /**
     *  @brief Permet de modifier un abonnement.
     *
     */
    void on_Bt_Modifier_clicked();

    /**
     *  @brief Permet d'ajouter un abonnement.
     *
     */
    void on_Bt_Ajouter_clicked();

    /**
     *  @brief Permet de bloquer le line edit "Credit Disponible".
     *
     */
    void on_RBt_Prestation_clicked();

    /**
     *  @brief Permet de débloquer le line edit "Credit Disponible".
     *
     */
    void on_RBt_CartePrepayee_clicked();

    /**
     *  @brief Permet de supprimer un abonnement.
     *
     */
    void on_Bt_Supprimer_clicked();

    /**
     *  @brief Permet d'annuler toutes les actions effectuées après le clic sur le bouton "Ajouter" ou "Modifier".
     *
     */
    void on_Bt_Annuler_clicked();

    /**
     *  @brief Sélectionne la ligne entière du tableau.
     *
     */
    void on_TbV_Recherche_clicked(const QModelIndex &index);

private:
    // ATTRIBUTs ----------------------------------------------------------------
    ///< Pointeur de la classe graphique.
    Ui::F_Abonnements *ui;

    ///< Récupère le nom de la prestation du line edit pour l'exécution de la requête UPDATE.
    QString sNomPrestationPrecedent;

    ///< Récupère le nom de la carte prépayée du line edit pour l'exécution de la requête UPDATE.
    QString sNomCartePrecedent;

    ///< Creer un modele du tableau.
    QStandardItemModel * TbRechercheModele;

    ///< Permet de savoir s'il s'agit d'un nouvel abonnement pour les éxécutions des requêtes.
    bool bEstUnNouvelAbo;

    // METHODEs -----------------------------------------------------------------
    /**
     *  @brief Fonction qui permet de mettre à  jour le tableau contenant toutes les informations de la BDD.
     *
     */
    void MettreAJourBDD();

    /**
     *  @brief Fonction qui permet de mettre les informations des abonnements dans les lines edits.
     *
     *  @param sTexte
     */
    void AfficherDetailAbonnement(QString sTexte);

    /**
     *  @brief Fonction qui exécute la requête SQL "UPDATE pour une prestation.
     *
     *  @param sTexte
     *  @param nDuree
     *  @param fPrix
     */
    void MettreAJourPrestation(QString sTexte, unsigned int nDuree, float fPrix);

    /**
     *  @brief Fonction qui exécute la requête SQL "UPDATE" pour une carte prépayée.
     *
     *  @param sTexte
     *  @param nDuree
     *  @param fPrix
     *  @param nCredit
     */
    void MettreAJourCartePrepayee(QString sTexte, unsigned int nDuree, float fPrix, unsigned int nCredit);

    /**
     *  @brief Fonction qui exécute la requête SQL "INSERT INTO" pour une prestation.
     *
     *  @param sTexte
     *  @param nDuree
     *  @param fPrix
     */
    void CreerPrestation(QString sTexte, unsigned int nDuree, float fPrix);

    /**
     *  @brief Fonction qui exécute la requête SQL "INSERT INTO" pour une carte prépayée.
     *
     *  @param sTexte
     *  @param nDuree
     *  @param fPrix
     *  @param nCredit
     */
    void CreerCartePrepayee(QString sTexte, unsigned int nDuree, float fPrix, unsigned int nCredit);

    /**
     *  @brief Fonction qui exécute la requête SQL "DELETE".
     *
     *  @param sTexte
     *  @param nCredit
     */
    void SupprimerAbonnement(QString sTexte, int nCredit);
};

//------------------------------------------------------------------------------
#endif // F_ABONNEMENTS_H
//------------------------------------------------------------------------------
