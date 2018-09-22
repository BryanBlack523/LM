#ifndef ACTIVITYLISTDELEGATE_H
#define ACTIVITYLISTDELEGATE_H
#include <QItemDelegate>
#include <activitylistmodel.h>

class ActivityListDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit ActivityListDelegate(QObject *parent = nullptr);
protected:
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // LABELDDELEGATE_H
