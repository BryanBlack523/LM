#pragma once

#include <QMap>
#include <QObject>
#include <QSqlDatabase>

class QSqlDatabase;
class QSqlQuery;

class DataBase
  : public QObject
{
    Q_OBJECT

public:

    explicit DataBase(QObject *parent = nullptr);
    ~DataBase();

    bool open();
    bool insertActivityDaily(int id, const QDateTime &begin, const QDateTime &end);

    QString findName(int id);
    const QMap<QString, int>& getActivityMap();
    int findId(QString &name);
    void archiveJob();
    void getFrequency(QMap<QString, int>& frequency);
    QString path();

private:

    bool insertActivityHistory(const QSqlQuery *currQuery);
    bool insertActivityHistory(int id, const QDateTime &begin, const QDateTime &end);
    bool insertActivityTable(int id, const QDateTime &begin, const QDateTime &end, QString table);

    std::vector<std::vector<QDateTime> > getFilledTime();
    bool clearDailySchedule();
    bool diffDays(const QDateTime &begin, const QDateTime &end);
    bool isArchived();
    void archiveActivities();
    void fillSpaces();
    void separateDate(const int id, const QDateTime &begin, const QDateTime &end);
    bool createDB();

    int writeFrequency(int id);
    void close();
    void initActivityMap();
    void updateFrequency();

    QMap<QString, int> activityMap;
    QSqlDatabase db;
};
