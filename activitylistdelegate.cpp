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

//    qDebug() << timeToString(50) << " 00:00:00.50";
//    qDebug() << timeToString(50) << " 00:00:01.50";
//    qDebug() << timeToString(999) << " 00:00:00.999";
//    qDebug() << timeToString(1499) << " 00:00:01.499";
//    qDebug() << timeToString(1501) << " 00:00:01.501";
//    qDebug() << timeToString(20002) << " 00:00:20.02";
//    qDebug() << timeToString(59000) << " 00:00:59.0";
//    qDebug() << timeToString(61000) << " 00:01:01.0";
//    qDebug() << timeToString(1200000) << " 00:20:00.0";
//    qDebug() << timeToString(3540000) << " 00:59:00.0";
//    qDebug() << timeToString(3600000) << " 01:00:00.0";
//    qDebug() << timeToString(43200000) << " 12:00:00.0";
//    qDebug() << timeToString(12537343) << " 03:28:57.343";
//    qDebug() << timeToString(82800000) << " 23:00:00.0";
}

QString ActivityListDelegate::timeToString(qint64 time)
{
    return QString("%1:%2:%3").arg(time /  3600000          , 2, 10, QChar('0'))
                              .arg((time % 3600000)/ 60000, 2, 10, QChar('0'))
                              .arg((time % 60000) /  1000  , 2, 10, QChar('0'));
}

