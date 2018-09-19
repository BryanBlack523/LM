#ifndef ACTIVITYLISTMODEL_H
#define ACTIVITYLISTMODEL_H
#include <QAbstractItemModel>
#include <bactivity.h>

class ActivityListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    ActivityListModel();
    QModelIndex parent(const QModelIndex& child) const override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

    QPointer<BActivity> find(const QString& name) const;
    private:
    QList<BActivity> activities;
};

#endif // ACTIVITYLISTMODEL_H
