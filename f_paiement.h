// f_paiement.h	0.1	<25/05/1012>	<Philippe Botherel>
#ifndef F_PAIEMENT_H
#define F_PAIEMENT_H

#include <QDialog>
#include <QAbstractButton>
#include <QtWidgets>
#include <f_popupclesttem.h>
#include "f_panier.h"

#define COL_CARTE 0
#define COL_CREDITS 1
#define COL_IDCARTE 2

#define COLAUTRE_MODEPAIEMENT 0
#define COLAUTRE_NUMCHEQUE 1
#define COLAUTRE_BANQUE 2
#define COLAUTRE_EMETEUR 3
#define COLAUTRE_DATE 4
#define COLAUTRE_MONTANT 5
#define COLAUTRE_SOMME 6

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
    void AfficherPaiement(QDateTime DatePaiement, QString CodeMembre,int *Somme,int *retour,bool PanierIndispo=false);
    void MaJBanques(QComboBox *ComboBoxBanque);

private slots:
    void on_Bt_AjouterCartePaiement_clicked();
    void on_TW_PaiementCarte_clicked(const QModelIndex &index);
    void on_Bt_SupprimerCartePaiement_clicked();
    void CalculerPrix();

    void on_Bt_Annuler_clicked();

    void on_Bt_OK_clicked();

    void on_Bt_AjoutPanier_clicked();

private:
    Ui::F_Paiement *ui;
    //! Nombre de lignes dans le TableWidget des catres
    unsigned int NombreLignePaiement;
    //! Prix à payer en crédits
    int Prix;
    //! Reste à payer en crédits
    int Reste;
    //! Pointeur vers le reste à payer
    int *RetourReste;
    //! Code du membre qui emprunte
    QString MembreActif;
    //! Prix unitaire du crédit
    int PrixCredit ;
    //! Id Membre
    int IdMembre;
    //! Nom du Membre
    QString NomMembre;
    //! Pointeur sur la classe F_PopUpCLESTTEM avec comme paramètre (13)
    F_PopUpCLESTTEM *pBanqueAjMod;
    //! Pointeur sur la classe F_PopUpCLESTTEM avec comme paramètre (14)
    F_PopUpCLESTTEM *pRetardsAmendesAjMod;
    int *Retour;
};

#endif // F_PAIEMENT_H
