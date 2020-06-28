//f_panier.h 0.1	<Vincent VICTORIN>
#ifndef F_PANIER_H
#define F_PANIER_H

// En-têtes standards necessaires dans ce fichier en-tete seulement ------------
#include <QtWidgets>
#include <QSqlQuery>
#include <QSqlError>
#include "fonctions_globale.h"
#include "f_popupclesttem.h"
#include "f_imprimerfacture.h"
#include "d_ajoutelementpanier.h"
#include "d_aide.h"

#define VENTILATION_PRET 1
#define VENTILATION_RETARD 3
#define VENTILATION_MALLES 5
#define VENTILATION_ABONNEMENT 6
#define VENTILATION_AMENDE 7
#define VENTILATION_ANIMATION 8
#define VENTILATION_DEPLACEMENT 9

#define COLITEM_DESCRIPTION 0
#define COLITEM_PRIX 1

#define COLPAIE_REMARQUE 5

#define PAIEMENT_CHEQUE 2
#define PAIEMENT_VIREMENT 3

#define FILTRE_LES_DEUX 0
#define FILTRE_FAMILLES 1
#define FILTRE_COLLECTIVITES 2

struct infosPaiements {
    int IdVentilation;
    QString Table;
    QList<QSqlQuery *> *Requetes;
};

namespace Ui {
    class F_Panier;
}

/** @brief Classe qui permet de gérer le panier
 *
 * Cette classe permet de gérer le panier
 *
 */
class F_Panier : public QWidget
{
    Q_OBJECT
public:
    explicit F_Panier(QWidget *parent = nullptr);
    ~F_Panier();
    QStandardItemModel ModelePanier;
signals:
    //! Signal pour mettre à jour l'onglet Panier avec le nombre d'item dans le panier
    void SignalMiseAJourNbItemsPanier(uint);
    //! Signal pour mette à jour le calendrier des malles
    void Signal_Nouvelle_Malle();

public slots:
    /** @brief Permet de rajouter un nouveau élément au panier
     *
     */
    bool slot_AjouterAuPanier(QString Titre=NULL,uint nIDMembre=NULL,double Prix=0,int IdVentilation=0,QString Table=NULL,QList<QSqlQuery *> *requete=NULL);
    bool slot_VerifMembrePanier(uint IdDuMembre);

private slots:
    void on_Bt_OK_clicked();

    void on_CBx_ModePaiement_currentIndexChanged(int index);

    void on_CBx_Banque_currentIndexChanged(int index);

    void on_Bt_SupprimerItemPanier_clicked();

    void on_TbV_Panier_clicked(const QModelIndex &index);

    void on_rB_Les_deux_clicked(bool checked);

    void on_rB_Collectivites_clicked(bool checked);

    void on_rB_Familles_clicked(bool checked);

    void on_Bt_Effacer_clicked();

    void on_CB_PaiementDiffere_toggled(bool checked);

    void on_Bt_ImprimerDevis_clicked();

    void on_Bt_ImprimerFacture_clicked();

    void on_CBx_Mois_currentIndexChanged(int index);

    void on_Bt_AjoutAnimation_clicked();

    void on_TW_Paiements_itemClicked(QTreeWidgetItem *item, int column);

    void on_TW_Paiements_itemChanged(QTreeWidgetItem *item, int column);

    void on_CBx_Annee_currentIndexChanged(int index);

    void on_Bt_Aide_clicked();

private:
    // ATTRIBUTs ----------------------------------------------------------------
    ///< Pointeur de la classe graphique.
    Ui::F_Panier *ui;
    QList<infosPaiements> lInfosPaiements;
    uint nIDPaiement;
    F_PopUpCLESTTEM *pBanqueAjMod;
    // Tableau associatif des infos du membre
    QHash<QString, QVariant> HInfosMembre;

    void ControlePanierVide(bool etat);
    void MaJBanques();
    void CalculerPrix();
    void ActualiserPaiements();
    void ControleChequeVisible(bool etat);
};

#endif // F_PANIER_H
