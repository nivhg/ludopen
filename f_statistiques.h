// f_statistiques.h	1.0	<01/2012>	<NORMAND Julien>
#ifndef F_STATISTIQUES_H
#define F_STATISTIQUES_H

// En-têtes standards necessaires dans ce fichier en-tête seulement ------------
#include <QtWidgets>
#include <QtWidgets>

// En-têtes propres à  l'application necessaires dans ce fichier en-tête --------
#include "f_plusdestatistiques.h"

namespace Ui {
class F_Statistiques;
}

/**
 *  @brief F_Statistiques permet de consulter les statistiques dans la partie "Administration".
 *
 *  Les statistiques disponibles sont:
 *      - Total d'adhérents inscrit.
 *      - Adhérents ayant emprunté sur une période.
 *      - Répartition des adhérents par commune.
 *      - Classement des adhérents empruntant le plus.
 *      - Classement des jeux par popularités
 *      - Nombre de jeux empruntés sur une période.
 *
 */
class F_Statistiques : public QWidget
{
    Q_OBJECT

public:
    // METHODEs ----------------------------------------------------------------
    // CONSTRUCTEURs et DESTRUCTEURs
    /**
     * @brief Constructeur de la classe.
     *
     * @param parent
     */

    explicit F_Statistiques(QWidget *parent = 0);
    /**
     * @brief Destructeur de la classe.
     *
     */
    ~F_Statistiques();

private slots:
    // METHODEs ----------------------------------------------------------------
    /**
     *  @brief Permet d'afficher la classe graphique F_PlusDeStatistiques.
     *
     */
    void on_Bt_PlusStat_clicked();

    /**
     *  @brief Permet de récupérer l'entier du combo box.
     *
     */
    void on_CBx_Stat_currentIndexChanged(int index);

    /**
     *  @brief Permet d'afficher les DateTime Edit sous forme "dd/MM/yyyy".
     *
     */
    void on_Rbt_Jour_clicked();

    /**
     *  @brief Permet d'afficher les DateTime Edit sous forme "MM/yyyy".
     *
     */
    void on_RBt_Mois_clicked();

    /**
     *  @brief Permet d'afficher les DateTime Edit sous forme "yyyy".
     *
     */
    void on_RBt_Annee_clicked();

    /**
     *  @brief Permet d'afficher la statistique sélectionnée avec les options voulues.
     *
     */
    void on_Bt_AfficherStatistique_clicked();

    /**
     *  @brief Permet d'exporter les tableaux dans un fichier .csv.
     *
     */
    void on_Bt_Exporter_clicked();

private:
    // ATTRIBUTs ----------------------------------------------------------------
    ///< Pointeur de la classe graphique.
    Ui::F_Statistiques *ui;

    ///< Pointeur qui contient l'adresse de la fenêtre graphique F_PlusDeStatistiques.
    F_PlusDeStatistiques * pPlusDeStatistiques;

    ///< Creer un modele du tableau de gauche.
    QStandardItemModel * TbStatModele;

    ///< Creer un modele du tableau de droite.
    QStandardItemModel * TbStatPlusModele;

    ///< Entier qui permet de voir quelles statistiques a été sélectionnées dans la combo box.
    int nChampsCombo;

    ///< Entier qui permet de voir quels radios boutons a été sélectionnées.
    int nPeriode;

    ///< Entier qui récupére le jour de la DtE_DateDebut.
    int nJourDebut;

    ///< Entier qui récupére le jour de la DtE_DateFin.
    int nJourFin;

    ///< Entier qui récupére le mois de la DtE_DateDebut.
    int nMoisDebut;

    ///< Entier qui récupére le mois de la DtE_DateFin.
    int nMoisFin;

    ///< Entier qui récupére l'année de la DtE_DateDebut.
    int nAnneeDebut;

    ///< Entier qui récupére l'année de la DtE_DateFin.
    int nAnneeFin;

    // METHODEs -----------------------------------------------------------------
    /**
     *  @brief Fonction qui exécute la requête SQL "SELECT" qui montre tous les adhérents inscrits.
     *
     */
    void EffectuerRequeteAdherentInscrit();

    /**
     *  @brief Fonction qui exécute la requête SQL "SELECT".
     *
     *  La requête permet de voir les adhérents inscrits en fonction des jours, du mois et de l'année dans le tableau de droite.
     */
    void EffectuerRequeteAdherentInscritJour();

    /**
     *  @brief Fonction qui exécute la requête SQL "SELECT".
     *
     *  La requête permet de voir les adhérents inscrits en fonction du mois et de l'année dans le tableau de droite.
     */
    void EffectuerRequeteAdherentInscritMois();

    /**
     *  @brief Fonction qui exécute la requête SQL "SELECT".
     *
     *  La requête permet de voir les adhérents inscrits en fonction de l'année dans le tableau de droite.
     */
    void EffectuerRequeteAdherentInscritAnnee();

    /**
     *  @brief Fonction qui exécute la requête SQL "SELECT" qui montre tous les adhérents ayant empruntés.
     *
     */
    void EffectuerRequeteAdherentEmprunt();

    /**
     *  @brief Fonction qui exécute la requête SQL "SELECT".
     *
     *  La requête permet de voir le nombre d'adhérents empruntant en fonction des jours, du mois et de l'année dans le tableau de droite.
     */
    void EffectuerRequeteAdherentEmpruntJour();

    /**
     *  @brief Fonction qui exécute la requête SQL "SELECT".
     *
     *  La requête permet de voir le nombre d'adhérents empruntant en fonction du mois et de l'année dans le tableau de droite.
     */
    void EffectuerRequeteAdherentEmpruntMois();

    /**
     *  @brief Fonction qui exécute la requête SQL "SELECT".
     *
     *  La requête permet de voir le nombre d'adhérents empruntant en fonction de l'année dans le tableau de droite.
     */
    void EffectuerRequeteAdherentEmpruntAnnee();

    /**
     *  @brief Fonction qui exécute la requête SQL "SELECT" qui montrent le nombre d'adhérent vivant dans les villes.
     *
     */
    void EffectuerRequeteAdherentCommune();

    /**
     *  @brief Fonction qui exécute la requête SQL "SELECT" qui montre le nombre de fois que les adhérents ont emprunté.
     *
     */
    void EffectuerRequeteAdherentPlus();

    /**
     *  @brief Fonction qui exécute la requête SQL "SELECT" qui montre le nombre de fois que les jeux ont été empruntés.
     *
     */
    void EffectuerRequeteJeuxPopularite();

    /**
     *  @brief Fonction qui exécute la requête SQL "SELECT" qui montre tous les jeux empruntés.
     *
     */
    void EffectuerRequeteJeuxEmprunt();

    /**
     *  @brief Fonction qui exécute la requête SQL "SELECT".
     *
     *  La requête permet de voir les jeux empruntés en fonction des jours, du mois et de l'année dans le tableau de droite.
     */
    void EffectuerRequeteJeuxEmpruntJour();

    /**
     *  @brief Fonction qui exécute la requête SQL "SELECT".
     *
     *  La requête permet de voir les jeux empruntés en fonction du mois et de l'année dans le tableau de droite.
     */
    void EffectuerRequeteJeuxEmpruntMois();

    /**
     *  @brief Fonction qui exécute la requête SQL "SELECT".
     *
     *  La requête permet de voir les jeux empruntés en fonction de l'année dans le tableau de droite.
     */
    void EffectuerRequeteJeuxEmpruntAnnee();
};

//------------------------------------------------------------------------------
#endif // STATISTIQUES_H
//------------------------------------------------------------------------------
