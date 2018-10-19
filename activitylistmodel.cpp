#include "activitylistmodel.h"

#include <QDebug>
#include <QSize>

ActivityListModel::ActivityListModel(QObject *parent)
  : QAbstractListModel(parent)
{}

ActivityListModel::~ActivityListModel()
{
    activities.clear();
}

int ActivityListModel::rowCount(const QModelIndex&) const
{
    return activities.size();
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
        case Name: return activities[index.row()]->name;
        case BeginDate: return activities[index.row()]->beginDate;
        case ElapsedTime: return activities[index.row()]->elapsedTime();
        case Qt::SizeHintRole: return QSize(70, 70);
    }

    return QModelIndex();
}

bool ActivityListModel::insertRows(int row, int count, QString &name, const QModelIndex &)
{
    beginInsertRows(QModelIndex(), row, row + count - 1);

    for (int i = row; i < row + count; ++ i)
        activities.push_back(std::make_shared<Activity>(name));

    endInsertRows();

    return true;
}

bool ActivityListModel::appendRow(const QString &name, const QModelIndex&)
{
    int row = activities.size();

    beginInsertRows(QModelIndex(), row, row);

    activities.push_back(std::make_shared<Activity>(name));

    endInsertRows();

    return true;
}

bool ActivityListModel::removeRows(int row, int count, const QModelIndex&)
{
    beginRemoveRows(QModelIndex(), row, row + count - 1);

    activities.erase(activities.begin() + row, activities.begin() + row + count); // TODO: check that

    endRemoveRows();

    return true;
}

std::shared_ptr<Activity> ActivityListModel::find(const QString& name) const
{
    auto it = std::find_if(activities.begin(),
                           activities.end(),
                           [name](const std::shared_ptr<Activity>& p)
                                {
                                    return name == p->name;
                                }
                           );

    return it == activities.end() ? nullptr : *it;
}

int ActivityListModel::getIdx(const QString& name) const
{
    auto it = std::find_if(activities.begin(),
                           activities.end(),
                           [name](const std::shared_ptr<Activity>& p)
                                {
                                    return name == p->name;
                                }
                           );

    return it == activities.end() ? -1 : it - activities.begin();
}

