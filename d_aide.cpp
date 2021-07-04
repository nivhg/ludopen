#include "d_aide.h"
#include "ui_d_aide.h"

D_aide::D_aide(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::D_aide)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Window);
}

D_aide::~D_aide()
{
    delete ui;
}

void D_aide::ModifierContenu(QString contenu)
{
    ui->Te_Contenu->setText(contenu);
}
