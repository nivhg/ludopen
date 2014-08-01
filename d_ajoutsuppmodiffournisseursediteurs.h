#ifndef D_AJOUTSUPPMODIFFOURNISSEURSEDITEURS_H
#define D_AJOUTSUPPMODIFFOURNISSEURSEDITEURS_H

// En-têtes propres à l'application necessaires dans ce fichier en-tete --------
#include "f_ajoutsuppmodiffournisseursediteurs.h"
#include "f_ajoutsuppmodifjeux.h"

#include <QDialog>

// Déclaration de la classe utile dans les appels récursifs des fichiers .h
class D_AjoutSuppModifFournisseursEditeurs;

namespace Ui {
class D_AjoutSuppModifFournisseursEditeurs;
}

class D_AjoutSuppModifFournisseursEditeurs : public QDialog
{
    Q_OBJECT

public:
    explicit D_AjoutSuppModifFournisseursEditeurs( F_AjoutSuppModifJeux *parent = 0, char Mode=MODE_INDEFINI);
    ~D_AjoutSuppModifFournisseursEditeurs();
    F_AjoutSuppModifJeux *Parent;

private:
    Ui::D_AjoutSuppModifFournisseursEditeurs *ui;
    // ATTRIBUTs ----------------------------------------------------------------
};

#endif // D_AJOUTSUPPMODIFFOURNISSEURSEDITEURS_H
