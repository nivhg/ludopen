#include "spinboxdelegate.h"
#include <QtGui>

SpinBoxDelegate::SpinBoxDelegate(int ColumnSB,int ColumnMaxValue, bool LectureSeule,QObject *parent)
    : QItemDelegate(parent)
{
    this->ColumnSB=ColumnSB;
    this->ColumnMaxValue=ColumnMaxValue;
    this->LectureSeule=LectureSeule;
    this->parent=parent;
//    this->editor=0;
    connect(this,SIGNAL(editorSignal(QEvent *, QAbstractItemModel *,const QStyleOptionViewItem &, const QModelIndex &)),
            parent,SLOT(on_Tv_Contenu_editorEvent(QEvent *, QAbstractItemModel *,const QStyleOptionViewItem &, const QModelIndex &)));
}

QWidget *SpinBoxDelegate::createEditor(QWidget *parent,
    const QStyleOptionViewItem & option ,
    const QModelIndex & index ) const
{
    if (index.column() == ColumnSB)
       {
            QSpinBox *editor = new QSpinBox(parent);
            int maxValue=99999999;
            if(index.column()==ColumnSB && ColumnMaxValue!=-1)
            {
                maxValue=index.sibling(index.row(),ColumnMaxValue).data(Qt::DisplayRole).toInt();

            }
            editor->setRange(0,maxValue);
//            this->editor=editor;

            return editor;
       }
       else // it's just a common column. Live it in default way
            if(!LectureSeule) QItemDelegate::createEditor(parent, option, index);
}

void SpinBoxDelegate::setEditorData(QWidget *editor,
                                    const QModelIndex &index) const
{
    if (index.column() == ColumnSB)
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
    if (index.column() == ColumnSB)
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
    bool retour=emit(editorSignal(event,model,option,index));
    return retour;
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

DetectDelegate::DetectDelegate(QObject *parent)
    : QItemDelegate(parent)
{
    this->parent=parent;
}

QWidget *DetectDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    return QItemDelegate::createEditor(parent, option,index);
}

bool DetectDelegate::editorEvent(QEvent *event, QAbstractItemModel *model,
    const QStyleOptionViewItem &option, const QModelIndex &index)
{
    emit(editingStartedSignal());
    return QItemDelegate::editorEvent(event, model,option,index);
}
