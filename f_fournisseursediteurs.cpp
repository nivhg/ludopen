#include "f_fournisseursediteurs.h"
#include "ui_f_fournisseursediteurs.h"

F_FournisseursEditeurs::F_FournisseursEditeurs(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::F_FournisseursEditeurs)
{
    ui->setupUi(this);
}

F_FournisseursEditeurs::~F_FournisseursEditeurs()
{
    delete ui;
}
