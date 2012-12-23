// f_emprunt.h	0.1	<01/01/1012>	<Philippe Botherel>
#ifndef F_EMPRUNT_H
#define F_EMPRUNT_H

class F_Emprunt;

#include <QStandardItem>
#include <QDate>
#include <QWidget>
#include "f_paiement.h"
//#include "montableview.h"


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
    QDate DateEmprunt;          ///< date de l'emprunt
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
    explicit F_Emprunt(QWidget *parent = 0);
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


private slots:
    void on_Bt_OK_clicked();

    void on_Bt_RechercheOK_clicked();

    void on_TxE_Remarques_textChanged();

    void on_Bt_ValiderRemarques_clicked();

    void on_Bt_AnnulerRemarques_clicked();

    void on_TxE_RemarquesJeu_textChanged();

    void on_Bt_ValiderRemarquesJeu_clicked();

    void on_Bt_AnnulerRemarquesJeu_clicked();

    void on_CBx_TypeEmprunt_currentIndexChanged(int index);

    void on_Bt_Ajouter_clicked();

    void on_LE_RechercheMembre_textChanged(const QString &arg1);

    void on_Bt_ValiderEmprunt_clicked();

    void on_LE_CodeJeu_returnPressed();

    void on_LE_CodeMembre_returnPressed();

    void on_TbV_Recherche_clicked(const QModelIndex &index);

    void on_TbV_JeuxMembres_clicked(const QModelIndex &index);

    void on_TbV_JeuxReserves_clicked(const QModelIndex &index);

    void on_TbV_NewEmprunt_clicked(const QModelIndex &index);

    void on_bt_JeuxReserves_clicked();

    void on_bt_NewEmprunt_clicked();

private:
    Ui::F_Emprunt *ui;

////! Attribut du table view des emprunts
    //MonTableView* TbV_JeuxReserves;
//! Code du membre actif sur la fênentre
    QString MembreActif;
//! Code du jeu actif sur la fênentre
    QString JeuActif;
//! modèle du TableView des membres
    QStandardItemModel * ModelMembre;
//! Vecteur des nouveau emprunts
    QVector <Emprunts> NouveauEmprunts;
//! modèle du TableView des nouveaux emprunts
    QStandardItemModel * ModelNewEmprunts;
//! modèle du TableView des jeux réservés
    QStandardItemModel * ModelJeuReserves;
//! modèle du TableView des jeux empruntés
    QStandardItemModel * ModelJeuEmpruntes;
//! Attibut contenant l'état de la cotisation d'un membre (vrai : à jours/ faut : Expirée)
    bool EtatCotisationMembre ;
//! Nombre de ligne dans le tableau des nouveau emprunts
    int NmbLigneNewEmprunts ;
//! Pointeur qui pointe sur la fenêtre du paiement
    F_Paiement * pPaiement;




};

#endif // F_EMPRUNT_H
