#include "activitylistdelegate.h"

#include <QDebug>
#include <QPainter>

#include "activitylistmodel.h"

ActivityListDelegate::ActivityListDelegate(QObject *parent)
  : QItemDelegate (parent)
{}

void ActivityListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
//    qDebug() << "paint is called";
    QIcon icon(":/resources/icons/monkey_off_32x32.png");
    QRect iconRect(option.rect.left(), option.rect.top(), option.rect.width(), option.rect.height());

    icon.paint(painter, iconRect, Qt::AlignLeft);

    QRect labelRect (option.rect.left(), option.rect.top(), option.rect.width(), option.rect.height());
    QRect boundingRect;

    QString name = index.data(ActivityListModel::Name).toString();
//    qDebug() << "delegate name of activity " << name;

    painter->drawText(labelRect, Qt::AlignCenter, name, &boundingRect);

    int time = index.data(ActivityListModel::ElapsedTime).toInt();

    painter->drawText(labelRect, Qt::AlignRight, timeToString(time), &boundingRect);
}

QString ActivityListDelegate::timeToString(qint64 time)
{
    return QString("%1:%2:%3").arg(time /  360000          , 2, 10, QChar('0'))
            .arg((time % 360000)/ 60000, 2, 10, QChar('0'))
            .arg((time % 60000) /  1000  , 2, 10, QChar('0'));
}

