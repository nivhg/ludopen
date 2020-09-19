#include "spinboxdelegate.h"
#include <QtGui>

SpinBoxDelegate::SpinBoxDelegate(QList<int> ColumnSB,QList<int> ColumnDefault,int IndexOfColumnMaxValue, int ColumnOfMaxValue,bool ManageCheckBox,
                                 QObject *parent)
    : QItemDelegate(parent)
{
    this->ColumnSB=ColumnSB;
    this->ColumnOfMaxValue=ColumnOfMaxValue;
    this->IndexOfColumnMaxValue=IndexOfColumnMaxValue;
    this->ColumnDefault=ColumnDefault;
    this->ManageCheckBox=ManageCheckBox;
    this->parent=parent;
    connect(this,SIGNAL(editorSignal(QEvent *, QAbstractItemModel *,const QStyleOptionViewItem &, const QModelIndex &)),
            parent,SLOT(on_Tv_Contenu_editorEvent(QEvent *, QAbstractItemModel *,const QStyleOptionViewItem &, const QModelIndex &)));
//    this->installEventFilter(this);
}

QWidget *SpinBoxDelegate::createEditor(QWidget *parent,
    const QStyleOptionViewItem & option ,
    const QModelIndex & index ) const
{
    if(CancelEdition)
    {
        return NULL;
    }
    // Cherche la valeur dans la QList ColumnSB
    if ( ColumnSB.indexOf(index.column()) != -1)
    {
        QSpinBox *editor = new QSpinBox(parent);
        int maxValue=99999999;
        if(IndexOfColumnMaxValue!=-1&&index.column()==IndexOfColumnMaxValue&&ColumnOfMaxValue!=-1)
        {
            maxValue=index.sibling(index.row(),ColumnOfMaxValue).data(Qt::DisplayRole).toInt();
        }
        editor->setRange(0,maxValue);

        return editor;
    }
    else
    if( ColumnDefault.indexOf(index.column()) != -1)
    {
        return QItemDelegate::createEditor(parent, option, index);
    }
    else
    {
        return NULL;
    }
}

void SpinBoxDelegate::setEditorData(QWidget *editor,
                                    const QModelIndex &index) const
{
    if (ColumnSB.indexOf(index.column()) != -1)
    {
        int value = index.model()->data(index, Qt::EditRole).toInt();

        QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
        spinBox->setValue(value);
    }
    else
        QItemDelegate::setEditorData(editor,index);
}

void SpinBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                   const QModelIndex &index) const
{
    if (ColumnSB.indexOf(index.column()) != -1)
    {
        QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
        spinBox->interpretText();
        int value = spinBox->value();

        model->setData(index, value, Qt::EditRole);
    }
    else
        QItemDelegate::setModelData(editor,model,index);
}

void SpinBoxDelegate::updateEditorGeometry(QWidget *editor,
    const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}

bool SpinBoxDelegate::editorEvent(QEvent *event, QAbstractItemModel *model,
    const QStyleOptionViewItem &option, const QModelIndex &index)
{
    CancelEdition=false;
    bool retour=emit(editorSignal(event,model,option,index));
    if(ManageCheckBox)
    {
        return QItemDelegate::editorEvent(event, model,option,index);
    }
    else
    {
        CancelEdition=retour;
        return false;
    }
}


/*void SpinBoxDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
//    QStyledItemDelegate::paint(painter,option,index);
    QSpinBox *editor = new QSpinBox();
    editor->setPalette(option.palette);
    editor->setGeometry(option.rect);
    editor->render(painter);
    painter->restore();
}
*/

