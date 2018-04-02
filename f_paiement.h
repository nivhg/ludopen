// f_paiement.h	0.1	<25/05/1012>	<Philippe Botherel>
#ifndef F_PAIEMENT_H
#define F_PAIEMENT_H

#include <QDialog>
#include <QAbstractButton>
#include <QtWidgets>
#include <f_popupclesttem.h>

#define VENTILATION_PRET 1
#define VENTILATION_MALLES 5
#define VENTILATION_ABONNEMENT 6

#define PAIEMENT_CHEQUE 2

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
    void AfficherPaiement(QDateTime DatePaiement, QString CodeMembre,int Somme,int TypeVentilation,QString NomTable,int IdTable,int IdFacture,bool EurosOuCredits);
    void MaJBanques(QComboBox *ComboBoxBanque);
    void CalculerPrix();

private slots:
    void on_Bt_AjouterCartePaiement_clicked();
    void on_Bt_AjouterAutrePaiement_clicked();
    void on_TW_PaiementCarte_clicked(const QModelIndex &index);
    void on_TW_PaiementAutre_clicked(const QModelIndex &index);
    void on_Bt_SupprimerCartePaiement_clicked();
    void on_Bt_SupprimerAutrePaiement_clicked();
    void on_Bt_OK_Annuler_accepted();
    void on_ComboBoxBanque_currentIndexChanged(int index);
    void slot_VerifMontant ();

    void on_CBx_ModePaiement_currentIndexChanged(int index);

    void on_CBx_Cartes_currentIndexChanged(int index);

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
    //! Type de ventilation
    int TypeVentilation;
    //! Id Membre
    int IdMembre;
    //! Nom du Membre
    QString NomMembre;
    //! Nom de la table sur lequel pointe le paiement
    QString NomTable;
    //! Id dans la table sur lequel pointe le paiement
    int IdTable;
    //! Id de la facture concernée par ce paiement
    int IdFacture;
    //! Pointeur sur la classe F_PopUpCLESTTEM avec comme paramètre (13)
    F_PopUpCLESTTEM *pBanqueAjMod;
};

#endif // F_PAIEMENT_H
