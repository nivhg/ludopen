#include "f_recherchemembre.h"
#include "ui_f_recherchemembre.h"

F_RechercheMembre::F_RechercheMembre(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::F_RechercheMembre)
{
    ui->setupUi(this);
}

F_RechercheMembre::~F_RechercheMembre()
{
    delete ui;
}
