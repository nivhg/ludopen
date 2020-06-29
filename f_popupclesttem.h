// f_clesttem.h	0.1	<06/2012>	<NORMAND Julien>
#ifndef F_POPUPCLESTTEM_H
#define F_POPUPCLESTTEM_H

// En-têtes standards necessaires dans ce fichier en-tête seulement ------------
#include <QtWidgets>
#include <QtSql>
#include <fonctions_globale.h>
class W_ContenuJeu;
//#include "w_contenujeu.h"

#define POPUP_ETAT_JEU 0
#define POPUP_STATUT 1
#define POPUP_EMPLACEMENT 2
#define POPUP_CLASSIFICATION 3
#define POPUP_MOT_CLE 4
#define POPUP_TYPE_MEMBRE 5
#define POPUP_TITRE_MEMBRE 6
#define POPUP_TYPE_EMPRUNT 7
#define POPUP_LIEU 8
#define POPUP_PAIEMENT 9
#define POPUP_ACTIVITE 10
#define POPUP_AUTEUR 11
#define POPUP_DOMAINE_EMAIL 12
#define POPUP_BANQUE 13
#define POPUP_CONTENUJEU 14

namespace Ui {
class F_PopUpCLESTTEM;
}

/**
 *  @brief F_PopUpCLESTTEM permet d'avoir une pop-up en fonction de du int nCLESTTEM.
 *
 *  Le int nCLESSTEM prend pour valeur un nombre, le nombre fait modifier les labels de la fenêtre donc :


 *
 */
class F_PopUpCLESTTEM : public QDialog
{
    Q_OBJECT
    
public:
    // METHODEs ----------------------------------------------------------------
    // CONSTRUCTEURs et DESTRUCTEURs
    /**
     *  @brief Constructeur de la classe.
     *
     *  @param nCLESTTEM
     *  @param parent
     */
    explicit F_PopUpCLESTTEM(QWidget *parent = 0);

    /**
     *  @brief Destructeur de la classe.
     *
     */
    ~F_PopUpCLESTTEM();

    /**
     *  @brief Ouvrir la fenêtre en mode "ajouter".
     *
     */
    int Ajouter(int nCLESTTEM);

    /**
     *  @brief Ouvrir la fenêtre en mode "modifier".
     *
     *  @param sCLESTTEM
     */
    int Modifier(QString sCLESTTEM,int nCLESTTEM, QTableView * TbV_CLESTTEM=0);

signals:
    // METHODEs ----------------------------------------------------------------
    /**
     *  @brief Signal lors de la fermeture d'une fenêtre F_PopUpCLESTTEM.
     *
     */
    void SignalValider();

private slots:
    // METHODEs ----------------------------------------------------------------
    /**
     *  @brief Permet de valider l'ajout ou la modification en fonction de nCLESTTEM.
     *
     */
    void on_Bt_Valider_clicked();

    /**
     *  @brief Ferme la pop-up.
     *
     */
    void on_Bt_Annuler_clicked();

    /**
     *  @brief Verrouille ou Déverouille le bouton "Valider" quand le champs du TextEdit LE_CLESTTEM n'est pas vide.
     *
     *  @param arg1
     */
    void on_LE_CLESTTEM_textEdited(const QString &arg1);

    /**
     *  @brief Verrouille ou Déverouille le bouton "Valider" quand le champs du TextEdit LE_Autres n'est pas vide.
     *
     *  @param arg1
     */
    void on_LE_Autres_textEdited(const QString &arg1);

    void toUpper(const QString &text);

    void on_TE_CLESTTEM_textChanged();

private:
    // ATTRIBUTs ----------------------------------------------------------------
    ///< Pointeur de la classe graphique.
    Ui::F_PopUpCLESTTEM *ui;

    ///< Permet de savoir comment les labels vont réagir selon les modes et les types.
    int nCLESTTEM;

    ///< Permet de savoir s'il s'agit d'un ajout ou d'une modification.
    bool bCLESTTEM;

    ///< Récupère la valeur du champs du tableau pour le mode "modifier".
    QString sCLESTTEM;

    ///< QTableView
    QTableView * TbV_CLESTTEM;

    W_ContenuJeu * pContenuJeu;
};

//------------------------------------------------------------------------------
#endif // F_POPUPCLESTTEM_H
//------------------------------------------------------------------------------
