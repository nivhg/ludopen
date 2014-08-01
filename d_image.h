//d_image.h	0.1	<Vincent VICTORIN>
#ifndef D_IMAGE_H
#define D_IMAGE_H

// En-têtes standards necessaires dans ce fichier en-tete seulement ------------
#include <QDialog>

// En-têtes propres à l'application necessaires dans ce fichier en-tete --------
#include "lb_image.h"

namespace Ui {
class d_image;
}

class d_image : public QDialog
{
    Q_OBJECT

public:
    //! Constructeur
    explicit d_image(QWidget *parent = 0, lb_image* Lb_Image=0);
    //! Destructeur
    ~d_image();

private slots:
    void on_Lb_Image_clicked();

private:
    Ui::d_image *ui;

};

#endif // D_IMAGE_H
