#pragma once

#include <QAbstractItemModel>
#include <QPointer>

#include <list>
#include <memory>

#include "activity.h"

class ActivityListModel
  : public QAbstractListModel
{
    Q_OBJECT

    using QAbstractListModel::insertRows;//to get rid of warning

public:
    ActivityListModel(QObject *parent = nullptr);
    ~ActivityListModel() override;

    enum activityRoles
    {
        Name=Qt::UserRole+1,
        BeginDate,
        ElapsedTime
    };

    QHash<int, QByteArray> roleNames() const override
    {
        QHash <int,QByteArray> roles;
        roles [Name]="name";
        roles [BeginDate]="beginDate";
        roles [ElapsedTime]="ElapsedTime";
        return roles;
    }

    QModelIndex parent(const QModelIndex& child) const override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    std::shared_ptr<Activity> find(const QString& name) const;
    int getIdx(const QString& name) const;

    bool insertRows(int row, int count, QString& name, const QModelIndex &parent);
    bool appendRow(const QString &name, const QModelIndex &parent);
    bool removeRows(int row, int count, const QModelIndex &parent) override;

private:
    std::vector<std::shared_ptr<Activity>> activities;
};
