//f_detailsjeux.h	0.1	<Mary Florian>
#ifndef F_DETAILSJEUX_H
#define F_DETAILSJEUX_H

// En-têtes standards necessaires dans ce fichier en-tete seulement ------------
#include <QWidget>
#include <QStandardItemModel>

namespace Ui {
    class F_DetailsJeux;
}

/** @brief Classe qui permet de consulter plus de détails sur un jeu
 *
 * Cette classe permet de consulter les détails sur un jeu,
 * soit le prix de location, d'achat, l'historique des interventions, des emprunts ...
 *
 */
class F_DetailsJeux : public QWidget
{
    Q_OBJECT

public:
    explicit F_DetailsJeux(QWidget *parent = 0);
    ~F_DetailsJeux();

    // METHODEs -----------------------------------------------------------------
    /** @brief Permet de définir le jeu en consultation et récupère en argument le code de ce jeu
     *
     */
    void set_JeuEnConsultation(QString CodeJeu) ;

    /** @brief Permet d'afficher les détails sur le jeu
     *
     */
    void AfficherDetailJeu() ;
    
private slots:
    // METHODEs -----------------------------------------------------------------
    /** @brief Appeler lorsque l'on clic sur le bouton Quitter
     *
     */
    void on_Bt_Quitter_clicked();

private:
    // ATTRIBUTs ----------------------------------------------------------------
    Ui::F_DetailsJeux *ui;

    //! @brief Contient le code du jeu en consultation
    QString JeuEnConsultation ;
    
    //! Model du TableView de l'historique des interventions
    QStandardItemModel * ModelHistoriqueInterventions;

    //! Model du TableView de l'historique des emprunts
    QStandardItemModel * ModelHistoriqueEmprunts ;
};

//------------------------------------------------------------------------------
#endif // F_DETAILSJEUX_H
//------------------------------------------------------------------------------
