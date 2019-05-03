#ifndef D_RELEVE_H
#define D_RELEVE_H

#include <QDialog>
#include <QDebug>
#include <QtSql>
#include <QMessageBox>
#include <QKeyEvent>
#include "f_preferences.h"
#include "fonctions_globale.h"
class F_MainWindow;

namespace Ui {
class D_Releve;
}

class D_Releve : public QDialog
{
    Q_OBJECT

public:
    explicit D_Releve(QWidget *parent);
    ~D_Releve();
    void ChangementModeSaisie(bool DebutFin);

private slots:
    void on_SBx_UnCent_valueChanged(int arg1);

    void on_SBx_DeuxCents_valueChanged(int arg1);

    void on_SBx_CinqCents_valueChanged(const QString &arg1);

    void on_SBx_DixCents_valueChanged(int arg1);

    void on_SBx_VingtCents_valueChanged(int arg1);

    void on_SBx_CinquanteCents_valueChanged(int arg1);

    void on_SBx_UnEuro_valueChanged(int arg1);

    void on_SBx_DeuxEuros_valueChanged(int arg1);

    void on_SBx_CinqEuros_valueChanged(int arg1);

    void on_SBx_DixEuros_valueChanged(int arg1);

    void on_SBx_VingtEuros_valueChanged(int arg1);

    void on_SBx_CinquanteEuros_valueChanged(int arg1);

    void on_SBx_CentEuros_valueChanged(int arg1);

    void on_SBx_Total_valueChanged(double arg1);

    void on_SBx_Difference_valueChanged(double arg1);

    void on_Bt_Valider_clicked();

    void TbV_Releves_ItemChanged(QStandardItem *item);

    void on_Bt_PlusTard_clicked();

    void on_CBx_Mois_currentIndexChanged(int index);

    void on_CBx_Annee_currentIndexChanged(int index);

private:
    void VerifChampsSaisis();
    void CalculTotal();
    Ui::D_Releve *ui;
    QStandardItemModel ModeleReleves;
    void MajReleves();
    bool DebutFin;
    void keyPressEvent(QKeyEvent *e);
    F_MainWindow *main;

signals:
    void SignalPlusTard();
    void SignalReleveFini();
};

#endif // D_RELEVE_H
