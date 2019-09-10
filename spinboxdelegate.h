#ifndef SPINBOXDELEGATE_H
#define SPINBOXDELEGATE_H

#include <QItemDelegate>
#include <QModelIndex>
#include <QObject>
#include <QSize>
#include <QSpinBox>
#include <QApplication>

class SpinBoxDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    SpinBoxDelegate(QList<int> ColumnSB,QList<int> ColumnDefault,int IndexOfColumnMaxValue=-1, int ColumnOfMaxValue=-1,bool ManageCheckBox=false,
                    QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor,
        const QStyleOptionViewItem &option, const QModelIndex &index) const;

    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);

    //void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

signals:
    bool editorSignal(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);

private:
    int ColumnOfMaxValue,IndexOfColumnMaxValue;
    QObject *parent;
    QList<int> ColumnDefault;
    QList<int> ColumnSB;
    bool ManageCheckBox;
};

#endif // SPINBOXDELEGATE_H
