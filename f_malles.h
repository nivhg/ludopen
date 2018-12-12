#ifndef F_MALLES_H
#define F_MALLES_H

#include <QDialog>
#include <QWidget>
#include <QDate>
#include <QDebug>
#include <QStandardItemModel>
#include <QMapIterator>

#include "fonctions_globale.h"
#include "f_preferences.h"
#include "tableviewtooltipmodifier.h"

namespace Ui {
class F_Malles;
}

class F_Malles : public QDialog
{
    Q_OBJECT

public:
    explicit F_Malles(QWidget *parent = 0,F_Malles *pCalendrierMalles=0);
    void AfficherCalendrier(int mois=0,int annee=0);
    ~F_Malles();

private slots:
    void on_CBx_Mois_currentIndexChanged(int index);

    void on_CBx_Annee_currentIndexChanged(int index);

    void on_TbV_CalendrierMalles_clicked(const QModelIndex &index);

    void on_Bt_SupprimerMalle_clicked();

    void on_Bt_EmprunterMalle_clicked();

    void slot_actualiserCalendrier();

signals:
    //! Signal indiquant qu'un clic a eu lieu sur le bouton Emprunter
    void Signal_Clic_Emprunter( int iIdMalle);

private:
    Ui::F_Malles *ui;
    //! Tableau associatif pour stocker la table TypeMalle
    QStandardItemModel * ModeleMalle;
    QTableView *TbV_CalendrierMalles;
    int iIdMalleChoisie;
    //! Calendrier des malles pour les adhérents (2° écran)
    F_Malles * pCalendrierMalles;
};

#endif // F_MALLES_H
