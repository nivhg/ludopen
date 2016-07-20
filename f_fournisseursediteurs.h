#ifndef F_FOURNISSEURSEDITEURS_H
#define F_FOURNISSEURSEDITEURS_H

#include <QWidget>

namespace Ui {
    class F_FournisseursEditeurs;
}

class F_FournisseursEditeurs : public QWidget
{
    Q_OBJECT

public:
    explicit F_FournisseursEditeurs(QWidget *parent = 0);
    ~F_FournisseursEditeurs();

private:
    Ui::F_FournisseursEditeurs *ui;
};

#endif // F_FOURNISSEURSEDITEURS_H
