#include "activitylistmodel.h"

ActivityListModel::ActivityListModel()
{

}

int ActivityListModel::rowCount(const QModelIndex&) const
{
    return activities.size(); // warning
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
        return QVariant();

    switch (role)
    {
//        case 0: return QString::fromStdString(activities[index.row()]->url);
//        case 1: return activities[index.row()]->getName();
//        case 2: return activities[index.row()]->getBeginDate();
//        case 3: return activities[index.row()]->getElapsedTime();
    }

//    return activities[index.row()];
}

bool ActivityListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
    return false;

    (void) value;

    switch (role)
    {
//        case 1: activities[index.row()]->url = value.toString().toStdString(); return true;
//        case 2: activities[index.row()]->hint = value.toString().toStdString(); return true;
    }

    return false;
}

//QPointer<Activity> ActivityListModel::find(const QString& name) const
//{
//    auto it = std::find_if(activities.begin(), activities.end(), [name](QPointer<Activity> s){
//        return name == s.getName();
//    });

//    return it == activities.end() ? nullptr : *it;
//}
