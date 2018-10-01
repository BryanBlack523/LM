#include "activitylistmodel.h"
#include <QDebug>
#include <QSize>

ActivityListModel::ActivityListModel(QObject *parent) : QAbstractListModel(parent) {}

ActivityListModel::~ActivityListModel()
{
    qDeleteAll(activities);
}

int ActivityListModel::rowCount(const QModelIndex&) const
{
    return activities.count();
}

int ActivityListModel::columnCount(const QModelIndex&) const
{
    return 1;
}

QModelIndex ActivityListModel::parent(const QModelIndex&) const
{
    return QModelIndex();
}

QVariant ActivityListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
        qDebug() << "ActivityListModel::data::\t\tindex is not valid";
        return QVariant();
    }

    switch (role)
    {
        case Name: return activities[index.row()]->getName();
        case BeginDate: return activities[index.row()]->getBeginDate();
        case ElapsedTime: return activities[index.row()]->getElapsedTime();
        case Qt::SizeHintRole: return QSize(70, 70);
    }

    return QModelIndex();
}

bool ActivityListModel::insertRows(int row, int count, QString &name, const QModelIndex &parent = QModelIndex())
{
    (void) parent;

    beginInsertRows(QModelIndex(), row, row + count - 1);
    for (int i = row; i < row + count; ++ i)
    {
        QPointer<Activity> p = new Activity (this, name);
        activities.append(p);
    }
    endInsertRows();

    return true;
}

bool ActivityListModel::appendRow(const QString &name, const QModelIndex &parent = QModelIndex())
{
    (void) parent;

    int row = activities.count();

    beginInsertRows(QModelIndex(), row, row);
    QPointer<Activity> p = new Activity(this, name);
    activities.append(p);

    endInsertRows();

    return true;
}

bool ActivityListModel::removeRows(int row, int count, const QModelIndex &parent = QModelIndex())
{
    (void) parent;

    beginRemoveRows(QModelIndex(), row, row + count - 1);
    while (count--)
        delete activities.takeAt(row);
    endRemoveRows();

    return true;
}

QPointer<Activity> ActivityListModel::find(const QString& name) const
{
    auto it = std::find_if(activities.begin(),
                           activities.end(),
                           [name](QPointer<Activity> p)
                                {
                                    return name == p->getName();
                                }
                           );

    return it == activities.end() ? nullptr : *it;
}

QVariant ActivityListModel::getIdx(const QString& name) const
{
    for (int i = 0; i < activities.count(); ++i)
    {
        if (activities.at(i)->getName() == name)
            return i + 1;//crutch for the outgoing check
    }

    return false;
}

QHash<int, QByteArray> ActivityListModel::roleNames() const
{
    QHash <int,QByteArray> roles;
    roles [Name]="name";
    roles [BeginDate]="beginDate";
    roles [ElapsedTime]="ElapsedTime";
    return roles;
}
