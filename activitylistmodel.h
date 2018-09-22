#ifndef ACTIVITYLISTMODEL_H
#define ACTIVITYLISTMODEL_H
#include <QAbstractItemModel>
#include <activity.h>
#include <QPointer>

class ActivityListModel : public QAbstractListModel
{
    using QAbstractListModel::insertRows;//to get rid of warning
    Q_OBJECT


public:
    ActivityListModel(QObject *parent = nullptr);
    ~ActivityListModel() override;
    QModelIndex parent(const QModelIndex& child) const override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
//    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

    QPointer<Activity> find(const QString& name) const;
    QVariant getIdx(const QString& name) const;

    bool insertRows(int row, int count, QString& name, const QModelIndex &parent);
    bool appendRow(const QString &name, const QModelIndex &parent);
    bool removeRows(int row, int count, const QModelIndex &parent) override;

    QHash<int, QByteArray> roleNames() const override;

    enum activityRoles
        {
            Name=Qt::UserRole+1,
            BeginDate,
            ElapsedTime
        };

    private:
    QList<QPointer<Activity>> activities;
};

#endif // ACTIVITYLISTMODEL_H
