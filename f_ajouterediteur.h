//f_ajouterediteur.h	0.1	<Mary Florian>
#ifndef F_AJOUTEREDITEUR_H
#define F_AJOUTEREDITEUR_H

// Déclaration de la classe utile dans les appels récursifs des fichiers .h
class F_AjouterEditeur ;

// En-têtes standards necessaires dans ce fichier en-tete seulement ------------
#include <QDialog>

// En-têtes propres à l'application necessaires dans ce fichier en-tete --------
#include "f_ajoutsuppmodifjeux.h"

namespace Ui {
    class F_AjouterEditeur;
}

/** @brief Classe permettant l'ajout d'un éditeur
 *
 *  Cette classe est appelé lorsque l'on veut ajouter un éditeur depuis
 *  le ComboBox dans la fenêtre administration et le widget Jeux.
 *
 */
class F_AjouterEditeur : public QDialog
{
    Q_OBJECT

public:
    explicit F_AjouterEditeur(F_AjoutSuppModifJeux * pFenetreAdmin, QWidget *parent = 0);
    ~F_AjouterEditeur();

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
    Ui::F_AjouterEditeur *ui;

    // ATTRIBUTs ----------------------------------------------------------------
    /** @brief Pointeur qui fait le lien entre cette classe et la classe F_AjoutSuppModifJeux
     *
     */

    F_AjoutSuppModifJeux * pFenetreAdminJeu ;
};

//------------------------------------------------------------------------------
#endif // F_AJOUTEREDITEUR_H
//------------------------------------------------------------------------------
