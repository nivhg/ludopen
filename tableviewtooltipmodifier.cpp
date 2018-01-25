#include "tableviewtooltipmodifier.h"

TableViewToolTipModifier::TableViewToolTipModifier(QWidget* parent)
    : QTableView(parent)
{
    this->setMouseTracking(true);
}

void TableViewToolTipModifier::mousePressEvent(QMouseEvent * event)
{
    QTableView::mousePressEvent(event);
    QModelIndex ModelIndex = this->selectionModel()->currentIndex();
    emit mouseClickItem(ModelIndex);
}

void TableViewToolTipModifier::mouseMoveEvent(QMouseEvent * event)
{
    _isActive = false;
     QToolTip::hideText();

     QTableView::mouseMoveEvent(event);
}

bool TableViewToolTipModifier::viewportEvent(QEvent *event)
{
    if (event->type() == QEvent::ToolTip)
    {
        QHelpEvent* Help= static_cast<QHelpEvent*>(event);
        _lastPosition=Help->pos();
        _isActive = true;
        if (QTableView::underMouse())
        {
            const QModelIndex index = this->indexAt(_lastPosition);
            if (index.isValid())
            {
                QString toolTip = index.data(Qt::ToolTipRole).toString();
                QToolTip::showText(Help->globalPos(), toolTip, this, QRect());
            }
        }
        return true;
    }
    return QTableView::viewportEvent(event);
}
