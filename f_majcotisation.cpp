#include "f_majcotisation.h"
#include "ui_f_majcotisation.h"

F_MajCotisation::F_MajCotisation(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::F_MajCotisation)
{
    ui->setupUi(this);
}

F_MajCotisation::~F_MajCotisation()
{
    delete ui;
}
