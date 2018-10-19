#pragma once

#include <QItemDelegate>

class ActivityListDelegate
  : public QItemDelegate
{
    Q_OBJECT

public:
    explicit ActivityListDelegate(QObject *parent = nullptr);

protected:

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    static QString timeToString(qint64 time);
};
