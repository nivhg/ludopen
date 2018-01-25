#ifndef TABLEVIEWTOOLTIPMODIFIER_H
#define TABLEVIEWTOOLTIPMODIFIER_H

#include <QTableView>
#include <QToolTip>
#include <QEvent>
#include <QHelpEvent>
#include <QDebug>

/*namespace Ui {
class TableViewToolTipModifier;
}*/

class TableViewToolTipModifier : public QTableView
{
    Q_OBJECT
 public:
     TableViewToolTipModifier(QWidget *parent=0);
 protected:
     bool viewportEvent(QEvent *event);
     void mouseMoveEvent(QMouseEvent *);
     void mousePressEvent(QMouseEvent * event);
 signals:
     void mouseClickItem(QModelIndex ModelIndex);
 private:
     QPoint _lastPosition;
     bool _isActive = false;
};

#endif // TABLEVIEWTOOLTIPMODIFIER_H
