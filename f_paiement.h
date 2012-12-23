// f_paiement.h	0.1	<25/05/1012>	<Philippe Botherel>
#ifndef F_PAIEMENT_H
#define F_PAIEMENT_H

#include <QDialog>
#include <QAbstractButton>
#include <QtGui>


namespace Ui {
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
    void on_Bt_Ajouter_clicked();
    void on_Bt_Supprimer_clicked();
    void on_TW_Paiement_clicked(const QModelIndex &index);
    void on_CalculePrix ();
    void on_Bt_AjouterAutres_clicked();
    void on_TW_PaiementAutre_clicked(const QModelIndex &index);
    void on_Bt_SupprimerAutres_clicked();
    void on_buttonBox_clicked(QAbstractButton *button);

private:
    Ui::F_Paiement *ui;
    //! Nombre de ligne dans le TableWidget des catres
    int NombreLignePaiement;
    //! Nombre de ligne dans le TableWidget des autres paiemeents
    int NombreLignePaiementAutre;
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
