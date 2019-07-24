#ifndef W_HISTORIQUEMAINTENANCE_H
#define W_HISTORIQUEMAINTENANCE_H

#include <QWidget>
#include <QSqlQuery>
#include <QSqlError>
#include <QTableWidgetItem>
#include <QDebug>

#include "fonctions_globale.h"
#include "spinboxdelegate.h"

namespace Ui {
class W_HistoriqueMaintenance;
}

class W_HistoriqueMaintenance : public QWidget
{
    Q_OBJECT

public:
    explicit W_HistoriqueMaintenance(QWidget *parent = 0);
    ~W_HistoriqueMaintenance();
    void Vider();
    void Definir_CodeJeu(QString CodeJeu);

signals:
    void Signal_ActualiserContenu();
public slots:
    void ActualiserHistoriqueMaintenance();

private slots:
    void editingStartedHistorique();
    void on_Bt_SupprimerEvenement_clicked();
    void on_Tw_HistoriqueMaintenance_itemChanged(QTableWidgetItem *item);

private:

    Ui::W_HistoriqueMaintenance *ui;
    DetectDelegate *DelegateDetect;
    QString CodeJeu;
};

#endif // W_HISTORIQUEMAINTENANCE_H
