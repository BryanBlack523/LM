#include "labelddelegate.h"
#include <QLabel>
#include <QApplication>

LabelDDelegate::LabelDDelegate(QObject *parent) :QItemDelegate (parent)
{

}

QWidget *LabelDDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    (void) index;
    (void) option;
    return new QLabel(parent);
}

void LabelDDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionButton button;
    QRect r = option.rect;//getting the rect of the cell
    int x,y,w,h;
    x = r.left() + r.width() - 30;//the X coordinate
    y = r.top();//the Y coordinate
    w = 30;//button width
    h = 30;//button height
    button.rect = QRect(x,y,w,h);
    button.text = "=^.^=";
    button.state = QStyle::State_Enabled;

    QApplication::style()->drawControl( QStyle::CE_PushButton, &button, painter);

}

void LabelDDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{

}

void LabelDDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{

}

QSize LabelDDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{

}

void LabelDDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{

}
