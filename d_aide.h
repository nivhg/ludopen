#ifndef D_AIDE_H
#define D_AIDE_H

#include <QDialog>
#include <QtMultimedia/qmediaplayer.h>
#include <QMediaPlaylist>

namespace Ui {
class D_aide;
}

class D_aide : public QDialog
{
    Q_OBJECT

public:
    explicit D_aide(QWidget *parent = 0);
    ~D_aide();

private:
    Ui::D_aide *ui;
};

#endif // D_AIDE_H
