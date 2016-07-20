#include "f_malles.h"
#include "ui_f_malles.h"

F_Malles::F_Malles(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::F_Malles)
{
    ui->setupUi(this);
}

F_Malles::~F_Malles()
{
    delete ui;
}
