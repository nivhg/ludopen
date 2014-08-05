//-------------------------------------------------------------------------------
/** @file         d_ajoutsuppmodiffournisseursediteurs.cpp
 *  @brief        Permet l'affichage de l'interface d'admin des fournisseurs et des éditeurs
 *
 *  @author       Vincent VICTORIN
 *  @since        01/08/2014
 *  @version      0.1
 *
 *  Permet l'affichage de l'interface d'admin des fournisseurs-éditeurs,
 *  d'ajouter, de supprimer, ou de modifier une fiche de fournisseur ou éditeur.
 *
 *  Fabrication   QtCreator
 *
 *
 */
//-------------------------------------------------------------------------------
// En-têtes standards  ----------------------------------------------------------

// En-tête propre à l'application -----------------------------------------------
#include "d_ajoutsuppmodiffournisseursediteurs.h"
#include "ui_d_ajoutsuppmodiffournisseursediteurs.h"
#include "f_ajoutsuppmodiffournisseursediteurs.h"

/**
 * @brief Constructeur
 *
 * @param F_AjoutSuppModifJeux
 * @param char Mode : Mode de fonctionnement : editeur ou fournisseur
 */
D_AjoutSuppModifFournisseursEditeurs::D_AjoutSuppModifFournisseursEditeurs(F_AjoutSuppModifJeux *parent, char Mode) :
    QDialog(parent),
    ui(new Ui::D_AjoutSuppModifFournisseursEditeurs)
{
    ui->setupUi(this);

    F_AjoutSuppModifFournisseursEditeurs * pAjoutSuppModifFournisseursEditeurs = new F_AjoutSuppModifFournisseursEditeurs(this, Mode);
    // Ajoute dans le layout la fenêtre F_AjoutSuppModifFournisseursEditeurs
    ui->gridLayout_2->addWidget(pAjoutSuppModifFournisseursEditeurs);
    // N'affiche pas la partie recherche
    pAjoutSuppModifFournisseursEditeurs->AjoutFournisseursEditeursSeulement();
    pAjoutSuppModifFournisseursEditeurs->show();
    QString title;
    switch(Mode)
    {
        case MODE_INDEFINI: {
                title = "Ajouter un éditeur ou un fournisseur";
                break;
        }
        case MODE_FOURNISSEUR: {
                title = "Ajouter un fournisseur";
                break;
        }
        case MODE_EDITEUR: {
                title = "Ajouter un éditeur";
                break;
        }
        case MODE_EDITEUR_FOURNISSEUR: {
                title = "Ajouter un éditeur/fournisseur";
                break;
        }
    }

    this->setWindowTitle(title);
    this->Parent = parent;
}

D_AjoutSuppModifFournisseursEditeurs::~D_AjoutSuppModifFournisseursEditeurs()
{
    delete ui;
}
