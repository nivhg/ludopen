#ifndef F_AJOUTERCOTICARTE_H
#define F_AJOUTERCOTICARTE_H

// En-têtes propres l'application nécessaires dans ce fichier en-tête --------
#include <QtWidgets>
#include <f_paiement.h>


// Declaration des TYPEs UTILISATEURs ------------------------------------------

namespace Ui {
    class F_AjouterCotiCarte;
}


/** @brief Permet de gérer gérer les abonnements
 *
 *  Il est possible d'y ajouter et de modifier un membre.
 *  On peut consulter les abonnements en cour ainsi que les emprunts en cours et l'historique des emprunts d'un membre.
 */
class F_AjouterCotiCarte : public QWidget
{
    Q_OBJECT

public:
    // CONSTRUCTEURs et DESTRUCTEURs
    /** Initialisation de la classe. Permet de remplir les combobox avec les abonnements.
     *  @test   Voir la procédure dans le fichier associé.
     */
    explicit F_AjouterCotiCarte(QWidget *parent = 0);
    ~F_AjouterCotiCarte();

    // METHODEs -----------------------------------------------------------------

    //! Permet de modifier un membre.
    void ModifierAbonnement( int nIDAbonnement ) ;

    //! Permet d'ajouter un membre.
    void AjouterAbonnement(int nIDMembre);

    //! Met à jour la liste
    void MaJListeAbonnements() ;

signals:
    //! Signal emit lors de l'ajout ou de la modification d'un abonnment.
    void SignalAjoutCotisationCarte();

private slots:
    //! Valide la modification ou l'ajout
    void on_Bt_Valider_clicked();

    //! Change les informations en fonction du choix dans le combo box
    void on_CBx_ChoixAbonnement_currentIndexChanged(const QString &arg1);

    //! Verrouille le bouton valider si rien n'est sélectionné
    void on_CBx_ChoixAbonnement_currentIndexChanged(int index) ;

    //! Ferme la fenêtre sans rien mofifier
    void on_Bt_Annuler_clicked();

    void on_Bt_Prolonger_clicked();

    void on_CBx_ModePaiement_currentIndexChanged(int index);

private:
    Ui::F_AjouterCotiCarte *ui;

    QString sCombo ; //! Contient la selection du combobox
    int nIDMembre ; //! Contient l'id du membre selectionne
    int nIDAbonnement ; //! Contient l'id de l'abonnement selectionne
    bool bNouvelAbo ; //! Indique s'il s'agit d'un nouvel abonnement
    //! Pointeur vers la fenêtre du paiement
    F_Paiement * pPaiement;
};

#endif // F_AJOUTERCOTICARTE_H
