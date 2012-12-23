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
    /** @brief Met le code du jeu actif pass� en param�tre
     */
    void set_JeuActif (QString CodeJeu);
    /** @brief Affiche les informations du membre
     */
    void AfficherMembre(QString CodeMembre);
    /** @brief Affiche les informations du jeu
     */
    void AfficherJeu ();
    /** @brief Affiche les jeux d�ja emprunt�s par le membre
     */
    void AfficherJeuxEnEmprunt();
    /** @brief Affiche les jeux d�ja r�serv�s par le membre
     */
    void AfficherJeuxReserve();
    /** @brief Calcule la date du retour
     */
    void CalculerDateRetour();
    /** @brief Colorie les jours concern� par l'emprunt
     */
    void ColorierJours();
    /** @brief D�colorie les jours d�ja colori�
     */
    void DecolorierJours();
    /** @brief Affiche l'�tat du membre pass� en param�tre
     */
    void ViderMembre();
    /** @brief Affiche l'�tat du membre pass� en param�tre
     */
    void AfficherEtatCotisation(QString CodeMembre);
    /** @brief Affiche le nombre de cr�dits restants
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
//! Code du jeu actif sur la f�nentre
    QString JeuActif;
//! Code du membre actif sur la f�nentre
    QString MembreActif;
//! mod�le du TableView des membres
    QStandardItemModel * ModelMembre;
//! mod�le du TableView des jeux r�serv�s
    QStandardItemModel * ModelJeuReserves;
//! mod�le du TableView des jeux emprunt�s
    QStandardItemModel * ModelJeuEmpruntes;
//! Date de L'emprunt
    QDate DateEmprunt;
//! Date du retour
    QDate DateRetour;

};

#endif // FENETRERESERVATION_H
