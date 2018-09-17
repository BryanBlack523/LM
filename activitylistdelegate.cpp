#include "activitylistdelegate.h"
#include <QLabel>
#include <QApplication>
#include <QStyledItemDelegate>

ActivityListDelegate::ActivityListDelegate(QObject *parent) :QItemDelegate (parent)
{

}

//QWidget *activityListDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
//{
//    (void) index;
//    (void) option;
//    return new QLabel(parent);
//}

void ActivityListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QItemDelegate::paint(painter, option, index);

    QIcon icon(":/monkey_off_32x32.png");
    QRect iconRect(option.rect.right() - option.rect.height(), option.rect.top(), option.rect.height(), option.rect.height());

    icon.paint(painter, iconRect, Qt::AlignRight);

//    QStyleOptionButton button;
//    QRect r = option.rect;//getting the rect of the cell
//    int x,y,w,h;
//    x = r.left() + r.width() - 30;//the X coordinate
//    y = r.top();//the Y coordinate
//    w = 30;//button width
//    h = 30;//button height
//    button.rect = QRect(x,y,w,h);
//    button.text = "=^.^=";
//    button.state = QStyle::State_Enabled;

//    QApplication::style()->drawControl( QStyle::CE_PushButton, &button, painter);

}

//void activityListDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
//{

//}

//void activityListDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
//{

//}

//QSize activityListDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
//{

//}

//void activityListDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
//{

//}
