#include "d_ajoutsuppmodiffournisseursediteurs.h"
#include "ui_d_ajoutsuppmodiffournisseursediteurs.h"
#include "f_ajoutsuppmodiffournisseursediteurs.h"

D_AjoutSuppModifFournisseursEditeurs::D_AjoutSuppModifFournisseursEditeurs(F_AjoutSuppModifJeux *parent, char Mode) :
    QDialog(parent),
    ui(new Ui::D_AjoutSuppModifFournisseursEditeurs)
{
    ui->setupUi(this);

    F_AjoutSuppModifFournisseursEditeurs * pAjoutSuppModifFournisseursEditeurs = new F_AjoutSuppModifFournisseursEditeurs(this, Mode);
    ui->gridLayout_2->addWidget(pAjoutSuppModifFournisseursEditeurs);
    pAjoutSuppModifFournisseursEditeurs->AjoutFournisseursEditeursSeulement();
    pAjoutSuppModifFournisseursEditeurs->show();
    this->setWindowTitle("Ajouter un fournisseur");
    this->Parent = parent;
}

D_AjoutSuppModifFournisseursEditeurs::~D_AjoutSuppModifFournisseursEditeurs()
{
    delete ui;
}
