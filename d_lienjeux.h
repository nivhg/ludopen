#ifndef D_LIENJEUX_H
#define D_LIENJEUX_H

#include <QDialog>
#include <QtSql>

#include "searchbox.h"
#include "fonctions_globale.h"

namespace Ui {
class D_LienJeux;
}

class D_LienJeux : public QDialog
{
    Q_OBJECT

public:
    explicit D_LienJeux(QWidget *parent = 0,QString CodeJeu="");
    ~D_LienJeux();

private slots:
    void on_Bt_Annuler_clicked();

    void on_Bt_OK_clicked();

    void on_Cbx_TypeLien_currentIndexChanged(int index);

private:
    Ui::D_LienJeux *ui;
    SearchBox *SearchJeux;
    QString CodeJeu;
};

#endif // D_LIENJEUX_H
