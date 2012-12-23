#include "f_plusdestatistiques.h"
#include "ui_f_plusdestatistiques.h"

F_PlusDeStatistiques::F_PlusDeStatistiques(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::F_PlusDeStatistiques)
{
    ui->setupUi(this);
}

F_PlusDeStatistiques::~F_PlusDeStatistiques()
{
    delete ui;
}
