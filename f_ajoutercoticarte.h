

#ifndef F_AJOUTERCOTICARTE_H
#define F_AJOUTERCOTICARTE_H

// En-ttes propres  l'application necessaires dans ce fichier en-tete --------
#include <QWidget>


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
    void ModifierMembre( int nIDAbonnement ) ;

    //! Permet d'ajouter un membre.
    void AjouterMembre(int nIDMembre);

    //! Met à jour la liste
    void MaJListeAbonnements() ;

signals:
    //! Signal emit lors de l'ajout ou de la modification d'un abonnment.
    void SignalAjoutCotisationCarte();

private slots:
    //! Valide la modification ou l'ajout
    void on_Bt_Valider_clicked();

    //! Change les informations en fonction du choix dans le combo box
    void on_CBx_Abo_currentIndexChanged(const QString &arg1);

    //! Verrouille le bouton valider si rien n'est sélectionné
    void on_CBx_Abo_currentIndexChanged(int index) ;

    //! Ferme la fenêtre sans rien mofifier
    void on_Bt_Annuler_clicked();

private:
    Ui::F_AjouterCotiCarte *ui;

    QString sCombo ; //! Contient la selection du combobox
    int nIDMembre ; //! Contient l'id du membre selectionne
    int nIDAbonnement ; //! Contient l'id de l'abonnement selectionne
    bool bNouvelAbo ; //! Indique s'il s'agit d'un nouvel abonnement
};

#endif // F_AJOUTERCOTICARTE_H
