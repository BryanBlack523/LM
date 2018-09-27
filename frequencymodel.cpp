#include "frequencymodel.h"
#include <QDebug>
#include <QSize>

FrequencyModel::FrequencyModel(QObject *parent) : QAbstractListModel(parent) {}

FrequencyModel::~FrequencyModel()
{
    qDeleteAll(activities);
}

int FrequencyModel::rowCount(const QModelIndex&) const
{
    return activities.count();
}

int FrequencyModel::columnCount(const QModelIndex&) const
{
    return 1;
}

QModelIndex FrequencyModel::parent(const QModelIndex&) const
{
    return QModelIndex();
}

QVariant FrequencyModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
        qDebug() << "index is not valid";
        return QVariant();
    }

    switch (role)
    {
        case Qt::DisplayRole: return activities[index.row()]->name;
        case frR_ClickCount: return activities[index.row()]->clickCount;
//        case Qt::SizeHintRole: return QSize(70, 70);
    }

    return QModelIndex();
}

bool FrequencyModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    (void) value;

    if (role == frR_ClickCount)
        activities[index.row()]->clickCount = value.toInt(); return true;

    return false;
}

bool FrequencyModel::insertRows(int row, int count, QString &name, int clickCount, const QModelIndex &parent = QModelIndex())
{
    (void) parent;

    beginInsertRows(QModelIndex(), row, row + count - 1);
    for (int i = row; i < row + count; ++ i)
    {
        activity *p = new activity;
        p->name = name;
        p->clickCount = clickCount;

        activities.append(p);
    }
    endInsertRows();

    return true;
}

bool FrequencyModel::appendRow(const QString &name, int clickCount, const QModelIndex &parent = QModelIndex())
{
    (void) parent;

    qDebug() << name << " " << clickCount;
    int row = activities.count();
    qDebug() << row;
    beginInsertRows(QModelIndex(), row, row);
    activity *p = new activity;
    qDebug() << "new activity";
    p->name = name;
    p->clickCount = clickCount;

    activities.append(p);
    qDebug() << "appended";
    endInsertRows();

    return true;
}

bool FrequencyModel::removeRows(int row, int count, const QModelIndex &parent = QModelIndex())
{
    (void) parent;

    beginRemoveRows(QModelIndex(), row, row + count - 1);
    while (count--)
        delete activities.takeAt(row);
    endRemoveRows();

    return true;
}

activity* FrequencyModel::find(const QString& name) const
{
    auto it = std::find_if(activities.begin(),
                           activities.end(),
                           [name](activity* p)
                                {
                                    return name == p->name;
                                }
                           );

    return it == activities.end() ? nullptr : *it;
}

QVariant FrequencyModel::getIdx(const QString& name) const
{
    for (int i = 0; i < activities.count(); ++i)
    {
        qDebug() << i << ": " << activities.at(i)->name;
        if (activities.at(i)->name == name)
            return i + 1;//crutch for the outgoing check
    }

    return false;
}
