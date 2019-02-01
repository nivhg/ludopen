#include "d_aide.h"
#include "ui_d_aide.h"

D_aide::D_aide(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::D_aide)
{
    ui->setupUi(this);
    QMediaPlayer *player = new QMediaPlayer;
    player->setMedia(QUrl::fromLocalFile("/home/cfa-info/Documents/Perso/build-LudOpen-Desktop-Release/Aide/D_Panier.mp4"));

    QVideoWidget *videoWidget = new QVideoWidget;
    player->setVideoOutput(videoWidget);

    videoWidget->show();

    player->setVideoOutput(videoWidget);
    //ui->Vw_Video->show();
    player->play();
}

D_aide::~D_aide()
{
    delete ui;
}
