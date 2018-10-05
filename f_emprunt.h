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
#include "f_malles.h"
#include "f_membres.h"
#include "searchbox.h"


#define MODE_EMPRUNT 0
#define MODE_MALLES 1

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
    QDateTime DateRetourPrevu;      ///< date du retour
    unsigned int idReservation;   ///< prix de l'emprunt (en crédits)
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
    /** @brief Ajoute un jeu à valider
     */
    void AjouterJeuAValider(int iIdMembre, int iIdJeu, int iIdReservation,int iCodeJeu,
                            int iTypeEmprunt,QDate dDateRetourPrevu,
                            int iPrixEmprunt,QString sNomJeu,bool bJeuSpecial);
    /** @brief Affiche les jeux empruntés par le membre
     */
    static int AfficherJeuxEnEmprunt(QStandardItemModel *ModeleJeuxEmpruntes,QString MembreActif,
                               bool bRetour,QString *NbreJeuxRendre,QString *AmendeAPayer);
    /** @brief Affiche les jeux réservés par le membre
     */
    static void AfficherJeuxReserve(QStandardItemModel *ModeleJeuxEmpruntes,QString MembreActif,
                              bool bRetour);
    /** @brief Affiche les jeux réservés par le membre dans une malle
     */
    void AfficherMalle(int iIdMalle);
    /** @brief Vide le formilaire du jeu
     */
    void ViderJeu();
    /** @brief Actualise les informations du jeu
     */
    void ActualiserJeu();
    /** @brief Actualise les informations du membre
     */
    void ActualiserMembre();
    /** @brief Actualise la liste des jeux
     */
    void ActualiserListeJeux();
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

    //! Actualise le combobox des types d'emprunt
    void ActualiserTypeEmprunt(int iTitreMembre);

    //! Efface le tableau des jeux à valider
    void EffacerJeuAValider();

    //! Affiche l'état de paiement de la malle
    void AfficherEtatPaiement();

    //! Renvoie l'ID de la malle réservée sélectionnée
    int get_MalleReserveeSelectionnee();

    //! Renvoie l'ID de la malle empruntée sélectionnée
    int get_MalleEmprunteeSelectionnee();

    //! Suppression d'une réservation
    static void SuppressionReservation(int iIdReservation);

    //! Suppression de la réservation sélectionnée
    static bool SupprimerReservation(QTreeView *Tv_JeuxReserves,QStandardItemModel *ModeleJeuxReserves,QString MembreActif);

    //! Vérifier si ce jeu est réservé
    bool VerifJeuReserve();

private slots:
    void on_TxE_Remarques_textChanged();

    void on_Bt_ValiderRemarques_clicked();

    void on_Bt_AnnulerRemarques_clicked();

    void on_TxE_RemarquesJeu_textChanged();

    void on_Bt_ValiderRemarquesJeu_clicked();

    void on_Bt_AnnulerRemarquesJeu_clicked();

    void on_CBx_TypeEmprunt_currentIndexChanged(int index);

    void on_Bt_AjouterJeu_clicked();

    void on_Bt_Emprunter_clicked();

    void on_LE_CodeJeu_returnPressed();

    void on_LE_SearchMembre_MembreTrouve();

    void on_LE_SearchJeux_returnPressed();

    void on_LE_SearchJeux_jeuTrouve();

    void on_Tv_JeuxMembres_clicked(const QModelIndex &index);

    void on_Tv_JeuxReserves_clicked(const QModelIndex &index);

    void on_TbV_EmpruntAValider_clicked(const QModelIndex &index);

    void on_Bt_SupprimerReservation_clicked();

    void on_Bt_SupprimerEmpruntAValider_clicked();

    void on_Bt_Reserver_clicked();

    void on_Bt_CalendrierMalles_clicked();

    void on_DtE_Retour_editingFinished();

    void on_Bt_AjoutNonAdherent_clicked();

    void on_rB_Mode_Emprunt_toggled(bool checked);

    void on_DtE_Depart_dateChanged(const QDate &date);

public slots:
    void slot_Clic_Emprunter(int iIdMalle);

    void slot_Non_Adherent_Cree(int iCodeMembre);

signals:
    void Signal_Reservation_Malle(int iIdMalle);
private:
    Ui::F_Emprunt *ui;

//! Code du membre actif sur la fenêtre
    QString MembreActif;
//! Code du jeu actif sur la fenêtre
    QString JeuActif;
//! Id de réservation du jeu actif
    int iIdReservation;
//! Id de la malle active sur la fenêtre
    int iMalleActive;
//! Booléen qui indique si la malle active a été payée ou non
    bool bRegle;
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
//! Pointeur vers la fenêtre du paiement
    F_Paiement * pPaiement;
//! Pointeur vers la fenêtre de calendrier des malles
    F_Malles * pMalles;

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

//! Indique le mode dans lequel se trouve la classe (emprunt ou résa malles)
    bool NonAdherent;

//! Tableau associatif pour stocker la table TypeMalle
    QHash< int, QHash<QString, QVariant> > HashTypeEmprunt;

//! Nombre de jeux en cours d'emprunt hors malles
    int iNbEmpruntEnCours;
    int iNbJeuxEmpruntables;
    int iNbNouveauxEmprunts;
};

#endif // F_EMPRUNT_H
