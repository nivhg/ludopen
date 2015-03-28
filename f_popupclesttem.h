// f_clesttem.h	0.1	<06/2012>	<NORMAND Julien>
#ifndef F_POPUPCLESTTEM_H
#define F_POPUPCLESTTEM_H

// En-têtes standards necessaires dans ce fichier en-tête seulement ------------
#include <QtWidgets>
#include <QtSql>

namespace Ui {
class F_PopUpCLESTTEM;
}

/**
 *  @brief F_PopUpCLESTTEM permet d'avoir une pop-up en fonction de du int nCLESTTEM.
 *
 *  Le int nCLESSTEM prend pour valeur un nombre, le nombre fait modifier les labels de la fenêtre donc :
 *      -Si 0, il s'agit d'un Etat Jeu.
 *      -Si 1, il s'agit d'un Statut Jeu.
 *      -Si 2, il s'agit d'un Emplacement Jeu.
 *      -Si 3, il s'agit d'une Classification Jeu.
 *      -Si 4, il s'agit d'un Mot clé Jeu.
 *      -Si 5, il s'agit d'un Type Membre.
 *      -Si 6, il s'agit d'un Titre Membre.
 *      -Si 7, il s'agit d'un Type Emprunt.
 *      -Si 8, il s'agit d'un Lieux Ludothèque.
 *      -Si 9, il s'agit d'un Paiement Membre.
 *      -Si 10, il s'agit d'une activité d'un membre.
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
    void Ajouter(int nCLESTTEM);

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
};

//------------------------------------------------------------------------------
#endif // F_POPUPCLESTTEM_H
//------------------------------------------------------------------------------
