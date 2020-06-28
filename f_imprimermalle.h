#ifndef F_IMPRIMERMALLE_H
#define F_IMPRIMERMALLE_H

#include <QDialog>
#include <QtPrintSupport/QPrintDialog>
#include <QtPrintSupport/QPrinter>
#include <QPrinterInfo>
#include <QDebug>
#include <QtSql>

#include "fonctions_globale.h"
#include "f_preferences.h"

namespace Ui {
class F_ImprimerMalle;
}

class F_ImprimerMalle : public QDialog
{
    Q_OBJECT

public:
    explicit F_ImprimerMalle(QWidget *parent = 0);
    void ImprimerMalle(int iIdMalle,bool EstCeEmprunt);
    ~F_ImprimerMalle();

private slots:
    void on_Bt_Imprimer_clicked();
    void on_Bt_Quitter_clicked();

private:
    Ui::F_ImprimerMalle *ui;
};

#endif // F_IMPRIMERMALLE_H
