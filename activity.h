#ifndef ACTIVITY_H
#define ACTIVITY_H

#include <QString>
#include <QModelIndex>

class Activity
{
public:
    Activity(const QString &,const QModelIndex &,const QModelIndex &listIndex);
    ~Activity();

    QString getName();
    QModelIndex getTableIndex();
    QModelIndex getListIndex();

private:
    QString m_name;
    QModelIndex m_tableIdx;
    QModelIndex m_listIdx;

};

#endif // ACTIVITY_H
