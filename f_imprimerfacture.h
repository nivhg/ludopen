#ifndef F_IMPRIMERFACTURE_H
#define F_IMPRIMERFACTURE_H

#include <QDialog>
#include <QtPrintSupport/QPrintDialog>
#include <QtPrintSupport/QPrinter>
#include <QPrinterInfo>
#include <QDebug>
#include <QtSql>

#include "fonctions_globale.h"
#include "f_preferences.h"

namespace Ui {
class F_ImprimerFacture;
}

class F_ImprimerFacture : public QDialog
{
    Q_OBJECT

public:
    explicit F_ImprimerFacture(QWidget *parent = 0);
    void ImprimerFacture(uint nIdPaiement, bool EstCeFacture,QHash<QString, QVariant> HPaiement=QHash<QString, QVariant>());
    ~F_ImprimerFacture();

private slots:
    void on_Bt_Imprimer_clicked();
    void on_Bt_Quitter_clicked();

private:
    bool EstCeFacture;
    Ui::F_ImprimerFacture *ui;
};

#endif // F_ImprimerFacture_H
