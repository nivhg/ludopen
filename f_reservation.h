#ifndef F_RESERVATION_H
#define F_RESERVATION_H

#include <QMainWindow>
#include <QDate>
#include <QStandardItem>

namespace Ui {
    class F_Reservation;
}

class F_Reservation : public QMainWindow
{
    Q_OBJECT

public:
    /** @brief Constructeur
     */
    explicit F_Reservation(QWidget *parent = 0);
    /** @brief Destructeur
     */
    ~F_Reservation();
    /** @brief Met le code du jeu actif passé en paramčtre
     */
    void set_JeuActif (QString CodeJeu);
    /** @brief Affiche les informations du membre
     */
    void AfficherMembre(QString CodeMembre);
    /** @brief Affiche les informations du jeu
     */
    void AfficherJeu ();
    /** @brief Affiche les jeux déja empruntés par le membre
     */
    void AfficherJeuxEnEmprunt();
    /** @brief Affiche les jeux déja réservés par le membre
     */
    void AfficherJeuxReserve();
    /** @brief Calcule la date du retour
     */
    void CalculerDateRetour();
    /** @brief Colorie les jours concerné par l'emprunt
     */
    void ColorierJours();
    /** @brief Décolorie les jours déja colorié
     */
    void DecolorierJours();
    /** @brief Affiche l'état du membre passé en paramčtre
     */
    void ViderMembre();
    /** @brief Affiche l'état du membre passé en paramčtre
     */
    void AfficherEtatCotisation(QString CodeMembre);
    /** @brief Affiche le nombre de crédits restants
     */
    void CalculerCreditsRestants();

private slots:
    void on_Bt_RechercheOK_clicked();

    void on_TxE_Remarque_textChanged();

    void on_Bt_ValiderRemarque_clicked();

    void on_Bt_AnnulerRemarque_clicked();

    void on_Cal_DateEmprunt_clicked(const QDate &Date);

    void on_LE_RechercheMembre_textChanged(const QString &arg1);

    void on_LE_CodeMembre_returnPressed();

    void on_TbV_Recherche_clicked(const QModelIndex &index);

    void on_TbV_JeuxMembres_clicked(const QModelIndex &index);

    void on_TbV_JeuxReserves_clicked(const QModelIndex &index);

    void on_bt_SuppReservation_clicked();

    void on_Bt_ValiderReservation_clicked();

    void on_CBx_TypeEmprunt_currentIndexChanged(int index);

    void on_Cal_DateRetour_clicked(const QDate &date);

    void on_Bt_AnnulerReservation_clicked();

private:
    Ui::F_Reservation *ui;
//! Code du jeu actif sur la fęnentre
    QString JeuActif;
//! Code du membre actif sur la fęnentre
    QString MembreActif;
//! modčle du TableView des membres
    QStandardItemModel * ModelMembre;
//! modčle du TableView des jeux réservés
    QStandardItemModel * ModelJeuReserves;
//! modčle du TableView des jeux empruntés
    QStandardItemModel * ModelJeuEmpruntes;
//! Date de L'emprunt
    QDate DateEmprunt;
//! Date du retour
    QDate DateRetour;

};

#endif // FENETRERESERVATION_H
