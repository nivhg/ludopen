// f_emprunt.h	0.1	<01/01/1012>	<Philippe Botherel>
#ifndef F_EMPRUNT_H
#define F_EMPRUNT_H

class F_Emprunt;

#include <QStandardItem>
#include <QDate>
#include <QtWidgets>
#include <QVector>
#include <QHash>

#include "f_paiement.h"
#include "f_membres.h"
#include "searchbox.h"


#define MODE_EMPRUNT 0
#define MODE_RESAMALLES 1

/** @struct Emprunt
 *  @brief Structure d'un emprunt
 *
 *  Contient toute les information nécessaire pour enregistrer l'emprut dans la base de données
 */
struct Emprunts
{
    unsigned int idJeu ;        ///< Id du jeu emprunté
    unsigned int idMembre;      ///< Id du membre
    unsigned int idTypeEmprunt; ///< Id du type de l'emprunt
    unsigned int PrixCaution;   ///< prix de la caution
    unsigned int PrixEmprunt;   ///< prix de l'emprunt (en crédits)
    QDateTime DateEmprunt;          ///< date de l'emprunt
    QDate DateRetourPrevu;      ///< date du retour

};

namespace Ui {
    class F_Emprunt;
}

class F_Emprunt : public QWidget
{
    Q_OBJECT

public:
    //! Constructeur
    explicit F_Emprunt(int iMode=0, QWidget *parent = 0);
    //! Destructeur
    ~F_Emprunt();
    /** @brief affiche les informations du membre
     */
    void AfficherMembre(QString CodeMembre);
    /** @briefcalcule la date du retour
     */
    void CalculerDateRetour();
    /** @brief Affiche les jeux empruntés par le membre
     */
    void AfficherJeuxEnEmprunt();
    /** @brief Affiche les jeux réservés par le membre
     */
    void AfficherJeuxReserve();
    /** @brief Vide le formilaire du jeu
     */
    void ViderJeu();
    /** @brief Actualise les informations du jeu
     */
    void ActualiserJeu();
    /** @brief Actualise les informations du membre
     */
    void ActualiserMembre();
    /** @brief Affiche l'état de la cotisation d'un membre
     */
    bool AfficherEtatCotisation(QString CodeMembre);
    /** @brief Emprunte les jeux
     */
    void EmprunterJeux();
    /** @brief Calcule les crédits restants
     */
    void CalculerCreditsRestants();
    /** @brief Met à jour l'affichage du nombre de jeu en cours d'emprunt aujourd'hui
     */
    void AfficherNbEmpruntsEnCours() ;

    //! Mise à jour de la liste des membres
    bool MaJListeMembres (bool AfficherContact=false);

    //! Mise à jour de la liste des jeux
    QVector<QVector <QString> > MaJListeJeux(QString filtre="");

    //! Affiche ou cache les composants liés aux malles
    void RendreVisibleMalle(bool booleen);

    //! Actualise le combobox des types de malles
    void ActualiserTypeMalle(int iTitreMembre);

private slots:
    void on_TxE_Remarques_textChanged();

    void on_Bt_ValiderRemarques_clicked();

    void on_Bt_AnnulerRemarques_clicked();

    void on_TxE_RemarquesJeu_textChanged();

    void on_Bt_ValiderRemarquesJeu_clicked();

    void on_Bt_AnnulerRemarquesJeu_clicked();

    void on_CBx_TypeEmprunt_currentIndexChanged(int index);

    void on_Bt_Ajouter_clicked();

    void on_Bt_ValiderEmprunt_clicked();

    void on_LE_CodeJeu_returnPressed();

    void on_LE_SearchMembre_MembreTrouve();

    void on_LE_SearchJeux_returnPressed();

    void on_LE_SearchJeux_jeuTrouve();

    void on_TbV_JeuxMembres_clicked(const QModelIndex &index);

    void on_Tv_JeuxReserves_clicked(const QModelIndex &index);

    void on_TbV_EmpruntAValider_clicked(const QModelIndex &index);

    void on_Bt_SupprimerReservation_clicked();

    void on_Bt_SupprimerEmpruntAValider_clicked();

    void on_CBx_TypeMalle_currentIndexChanged(int index);

private:
    Ui::F_Emprunt *ui;

//! Code du membre actif sur la fenêtre
    QString MembreActif;
//! Code du jeu actif sur la fenêtre
    QString JeuActif;
//! Vecteur des nouveau emprunts
    QVector <Emprunts> NouveauEmprunts;
//! modèle du TableView des nouveaux emprunts
    QStandardItemModel * ModeleEmpruntsAValider;
//! modèle du TableView des jeux réservés
    QStandardItemModel * ModeleJeuxReserves;
//! modèle du TableView des jeux empruntés
    QStandardItemModel * ModeleJeuxEmpruntes;
//! Attibut contenant l'état de la cotisation d'un membre (vrai : à jours/ faut : Expirée)
    bool EtatCotisationMembre ;
//! Nombre de ligne dans le tableau des nouveau emprunts
    int NbLignesEmpruntsAValider ;
//! Pointeur qui pointe sur la fenêtre du paiement
    F_Paiement * pPaiement;

//! Liste des membres
    QVector<QVector <QString> > VecteurMembres;

//! Liste des jeux
    QVector<QVector <QString> > VecteurJeux;

//! Prix de la caution du jeu
    int PrixCaution;

//! LineEdit SearchBox pour la recherche de membre
    SearchBox *SearchMembre;

//! LineEdit SearchBox pour la recherche de jeux
    SearchBox *SearchJeux;

//! Indique le mode dans lequel se trouve la classe (emprunt ou résa malles)
    int iMode;

//! Tableau associatif pour stocker la table TypeMalle
    QHash< int, QHash<QString, int> > HashTypeMalle;
};

#endif // F_EMPRUNT_H
