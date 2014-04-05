// f_paiement.h	0.1	<25/05/1012>	<Philippe Botherel>
#ifndef F_PAIEMENT_H
#define F_PAIEMENT_H

#include <QtWidgets\QDialog>
#include <QtWidgets\QAbstractButton>
#include <QtWidgets>

namespace Ui
{
    class F_Paiement;
}

class F_Paiement : public QDialog
{
    Q_OBJECT

public:
    //! Constructeur
    explicit F_Paiement(QWidget *parent = 0);
    //! Destructeur
    ~F_Paiement();
    /** @brief affiche les informations du paiement
     */
    void AfficherPaiement(unsigned int Somme,QString CodeMembre);

private slots:
    void on_Bt_AjouterCartePaiement_clicked();
    void on_Bt_AjouterAutrePaiement_clicked();
    void on_TW_PaiementCarte_clicked(const QModelIndex &index);
    void on_TW_PaiementAutre_clicked(const QModelIndex &index);
    void on_Bt_SupprimerCartePaiement_clicked();
    void on_Bt_SupprimerAutrePaiement_clicked();
    void on_Bt_OK_Annuler_accepted();
    void slot_CalculerPrix ();

private:
    Ui::F_Paiement *ui;
    //! Nombre de lignes dans le TableWidget des catres
    unsigned int NombreLignePaiement;
    //! Nombre de lignes dans le TableWidget des autres paiemeents
    unsigned int NombreLignePaiementAutre;
    //! Prix à payer en crédits
    int Prix;
    //! Reste à payer en crédits
    int Reste;
    //! Code du membre qui emprunte
    QString MembreActif;
    //! Prix unitaire du crédit
    int PrixCredit ;
};

#endif // F_PAIEMENT_H
