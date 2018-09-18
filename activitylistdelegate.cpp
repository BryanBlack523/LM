#include "activitylistdelegate.h"
#include <QDebug>
#include <QPainter>
#include <QTime>

ActivityListDelegate::ActivityListDelegate(QObject *parent) :QItemDelegate (parent)
{
    timer.start();
}

//QWidget *activityListDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
//{
//    (void) index;
//    (void) option;
//    return new QLabel(parent);
//}

void ActivityListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
//    QItemDelegate::paint(painter, option, index);

//    QFile file (":/monkey_off_32x32.png");
//    qDebug() << file.exists();

    QIcon icon("D:/Projects/LM/icons/monkey_off_32x32.png");
//    QRect iconRect(option.rect.right() - option.rect.height(), option.rect.top(), option.rect.height(), option.rect.height());
    QRect iconRect(option.rect.left(), option.rect.top(), option.rect.width(), option.rect.height());

    icon.paint(painter, iconRect, Qt::AlignLeft);


//    QFont font = painter->font();
//    font.setPixelSize(48);
//    painter->setFont(font);

    QRect labelRect (option.rect.left(), option.rect.top(), option.rect.width(), option.rect.height());
    QRect boundingRect;
    painter->drawText(labelRect, Qt::AlignCenter, index.data().toString(), &boundingRect);

    QString out = QString("%1:%2:%3").arg(timer.elapsed() / 360000        , 2, 10, QChar('0'))
                                     .arg(timer.elapsed() / 60000         , 2, 10, QChar('0'))
                                     .arg((timer.elapsed() % 60000) / 1000, 2, 10, QChar('0'));

    painter->drawText(labelRect, Qt::AlignRight, out, &boundingRect);
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
