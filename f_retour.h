/** @file         f_Retour.h
 *  @brief        Classe graphique permettant le retours à la ludothèque des jeux
 *
 *  @author       Botherel Philippe
 *  @author       STS IRIS, Lycée Nicolas APPERT, ORVAULT (FRANCE)
 *  @since        21/01/2012
 *  @version      0.1
 *  @date         02/09/2012 William
 *
 *  Cette classe permet de rendre les jeux qu'un membre a emprunté,
 *  on peut aussi supprimer une réservation de jeu entre autre
 */
#ifndef F_RETOUR_H
#define F_RETOUR_H

#include <QtWidgets>
#include <QStandardItem>
#include "f_paiement.h"
#include "searchbox.h"

namespace Ui {
    class F_Retour;
}

class F_Retour : public QWidget
{
    Q_OBJECT

public:
    //! Constructeur
    explicit F_Retour(QWidget *parent = 0);
    //! Destructeur
    ~F_Retour();
    /** @brief Permet d'afficher les information du membre dans le volet central
     */
    void AfficherMembre();
    /** @brief Permet d'afficher les informations du jeu dans le volet de droite
     */
    void AfficherDetailDuJeu();
    /** @brief Affiche les jeux empruntés par le membre
     */
    void AfficherJeuxEnEmprunt();
    /** @brief Affiche les jeux réservés par le membre
     */
    void AfficherJeuxEnReservation();
    /** @brief Vide les informations du jeu
     */
    void ViderJeu();
    /** @brief Actualise les informations du jeu
     */
    void ActualiserJeu();
    /** @brief Actualise les informations du membre
     */
    void ActualiserMembre();
    /** @brief Actualise la liste des membres ayant des emprunts
     *  Donne la liste de tous les membres ayant un retour à faire.
     */
    void ActualiserListeEmprunteurs();
    /** @brief Affiche l'état du membre passé en paramètre
     */
    void AfficherEtatCotisation(QString CodeMembre);
    /** @brief Emprunte les jeux
     */
    void EmprunterJeux();
    /** @brief Affiche le nombre de crédits restants
     */
    void CalculerCreditsRestants();

    //! Mise à jour de la liste des membres
    bool MaJListeMembres (bool AfficherContact=false);

    //! Mise à jour de la liste des jeux
    bool MaJListeJeux();

private slots:
    void on_LE_SearchMembre_returnPressed();

    void on_LE_SearchJeux_returnPressed();

    void on_Bt_ValiderRemarqueMembre_clicked();

    void on_Txe_RemarqueMembre_textChanged();

    void on_Bt_ValiderRemarqueJeu_clicked();

    void on_Bt_AnnulerRemarqueJeu_clicked();

    void on_Bt_AnnulerRemarqueMembre_clicked();

    void on_TxE_RemarquesJeu_textChanged();

    void on_LE_CodeMembre_returnPressed();

    void on_LE_CodeJeu_returnPressed();

    void on_Bt_RendreJeu_clicked();

    void on_Tv_JeuxEmprunte_clicked(const QModelIndex &index);

    void on_TbV_JeuxReserve_clicked(const QModelIndex &index);

    void on_bt_SuppReservation_clicked();

    void on_CBx_TypeProlongation_currentIndexChanged(int index);

    void on_Bt_Prolonger_clicked();

    void on_Bt_ToutSelectionner_clicked();

private:
    Ui::F_Retour *ui;

    //! Code du membre actif sur la fenêtre
    QString MembreActif;
    //! Code du jeu actif sur la fenêtre
    QString JeuActif;
    //! modèle du TableView des jeux réservés
    QStandardItemModel * ModelJeuReserves;
    //! modèle du TableView des jeux empruntés
    QStandardItemModel * ModelJeuEmpruntes;
    //! Amende calculée pour jours de retard
    float Amende ;
    //! LineEdit SearchBox pour la recherche de membre
    SearchBox *SearchMembre;

    //! LineEdit SearchBox pour la recherche de jeux
    SearchBox *SearchJeux;

    //! Liste des membres
        QVector<QVector <QString> > VecteurMembres;

    //! Liste des jeux
        QVector<QVector <QString> > VecteurJeux;

};

#endif // F_RETOUR_H
