#ifndef ACTIVITIESDELEGATE_H
#define ACTIVITIESDELEGATE_H

#include <QItemDelegate>
#include <QModelIndex>
#include <QObject>
#include <QSize>

class ActivitiesDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    ActivitiesDelegate(QObject *parent = 0);
    ~ActivitiesDelegate();

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);
};

#endif // ACTIVITIESDELEGATE_H
