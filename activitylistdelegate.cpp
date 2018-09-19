#include "activitylistdelegate.h"
#include <QDebug>
#include <QPainter>
#include <QTime>

ActivityListDelegate::ActivityListDelegate(QObject *parent) :QItemDelegate (parent)
{
    timer.start();
}

void ActivityListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QIcon icon(":/resources/icons/monkey_off_32x32.png");
    QRect iconRect(option.rect.left(), option.rect.top(), option.rect.width(), option.rect.height());

    icon.paint(painter, iconRect, Qt::AlignLeft);

    QRect labelRect (option.rect.left(), option.rect.top(), option.rect.width(), option.rect.height());
    QRect boundingRect;
    painter->drawText(labelRect, Qt::AlignCenter, index.data().toString(), &boundingRect);

    QString out = QString("%1:%2:%3").arg(timer.elapsed() /  360000          , 2, 10, QChar('0'))
                                     .arg((timer.elapsed() % 360000)/ 60000, 2, 10, QChar('0'))
                                     .arg((timer.elapsed() % 60000) /  1000  , 2, 10, QChar('0'));

    painter->drawText(labelRect, Qt::AlignRight, out, &boundingRect);
}
