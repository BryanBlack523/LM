#ifndef ACTIVITYLISTDELEGATE_H
#define ACTIVITYLISTDELEGATE_H
#include <QItemDelegate>
#include <QElapsedTimer>

class ActivityListDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit ActivityListDelegate(QObject *parent = 0);
protected:
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QElapsedTimer timer;
};

#endif // LABELDDELEGATE_H
