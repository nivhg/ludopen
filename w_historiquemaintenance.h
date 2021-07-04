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
    int Obtenir_Nb_Maintenance();

signals:
    void Signal_ActualiserContenu();
public slots:
    void ActualiserHistoriqueMaintenance();

private slots:
    void on_Tv_Contenu_editorEvent(QEvent *, QAbstractItemModel *,const QStyleOptionViewItem &, const QModelIndex &);
    void on_Bt_SupprimerEvenement_clicked();
    void on_Tw_HistoriqueMaintenance_itemChanged(QTableWidgetItem *item);

private:

    Ui::W_HistoriqueMaintenance *ui;
    SpinBoxDelegate *Delegate;
    QString CodeJeu;
};

#endif // W_HISTORIQUEMAINTENANCE_H
