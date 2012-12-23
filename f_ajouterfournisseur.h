//f_ajouterfournisseur.h	0.1	<Mary Florian>
#ifndef F_AJOUTERFOURNISSEUR_H
#define F_AJOUTERFOURNISSEUR_H

class F_AjouterFournisseur ;

// En-têtes standards necessaires dans ce fichier en-tete seulement ------------
#include <QDialog>

// En-têtes propres à l'application necessaires dans ce fichier en-tete --------
#include "f_ajoutsuppmodifjeux.h"

namespace Ui {
    class F_AjouterFournisseur;
}

/** @brief Classe permettant l'ajout d'un fournisseur
 *
 *  Cette classe est appelé lorsque l'on veut ajouter un fournisseur depuis
 *  le ComboBox dans la fenêtre administration et le widget Jeux.
 *
 */
class F_AjouterFournisseur : public QDialog
{
    Q_OBJECT

public:
    explicit F_AjouterFournisseur(F_AjoutSuppModifJeux * pFenetre, QWidget *parent = 0);
    ~F_AjouterFournisseur();

private slots:
    // METHODEs -----------------------------------------------------------------
    /** @brief Appelé lorsque l'on clique sur le bouton Valider
     *
     */
    void on_Bt_Valider_clicked();
    
    /** @brief Appelé lorsque l'on clique sur le bouton Annuler
     *
     */
    void on_Bt_Annuler_clicked();
    
private:
    Ui::F_AjouterFournisseur *ui;
    
    // ATTRIBUTs ----------------------------------------------------------------
    /** @brief Pointeur qui fait le lien entre cette classe et la classe F_AjoutSuppModifJeux
     *
     */
    F_AjoutSuppModifJeux * pFenetreAjoutJeu ;
};

//------------------------------------------------------------------------------
#endif // F_AJOUTERFOURNISSEUR_H
//------------------------------------------------------------------------------
