#ifndef FREQUENCYMODEL_H
#define FREQUENCYMODEL_H

#include <QObject>
#include <QAbstractListModel>

struct activity
{
    int clickCount;
    QString name;
};

class FrequencyModel : public QAbstractListModel
{    using QAbstractListModel::insertRows;//to get rid of warning
    Q_OBJECT
public:
    FrequencyModel(QObject *parent = nullptr);
    ~FrequencyModel() override;
    QModelIndex parent(const QModelIndex& child) const override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    activity* find(const QString& name) const;
    QVariant getIdx(const QString& name) const;

    bool insertRows(int row, int count, QString& name, int clickCount, const QModelIndex &parent);
    bool appendRow(const QString &name, int clickCount, const QModelIndex &parent);
    bool removeRows(int row, int count, const QModelIndex &parent) override;

    enum frequencyRoles
        {
            frR_ClickCount=Qt::UserRole+1
        };
private:
    QList<activity*> activities;
};

#endif // FREQUENCYMODEL_H
