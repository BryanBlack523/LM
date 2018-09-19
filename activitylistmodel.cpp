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
        case MR_URL: return QString::fromStdString(sources[index.row()]->url);
        case MR_HINT: return QString::fromStdString(sources[index.row()]->hint);
        case MR_SOURCE: return QVariant::fromValue(sources[index.row()]);
    }

    return activities[index.row()];
}

bool ActivityListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
    return false;

    switch (role)
    {
        case MR_URL: sources[index.row()]->url = value.toString().toStdString(); return true;
        case MR_HINT: sources[index.row()]->hint = value.toString().toStdString(); return true;
    }

    return false;
}

QPointer<Activity> ActivityListModel::find(const std::string& name) const
{
    auto it = std::find_if(activities.begin(), activities.end(), [name](QPointer<Activity> s){
    return name == s->url;
    });

    return it == activities.end() ? nullptr : *it;
}
