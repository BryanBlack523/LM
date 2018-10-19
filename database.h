#pragma once

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>

class DataBase
  : public QObject
{
    Q_OBJECT

public:
    explicit DataBase(QObject *parent = nullptr);
    ~DataBase();

    bool open(const QString &dbPath);

    bool insertActivity(const QVariantList &data);
    bool insertActivity(QSqlQuery *insert, const QSqlQuery *currQuery);
    bool insertActivity(int id, const QDateTime &begin, const QDateTime &end);
    bool insertActivityDaily(int id, const QDateTime &begin, const QDateTime &end);

    void getFrequency(QMap<QString, int>& frequency);
    const QMap<QString, int>& getActivityMap();
    void archiveJob();
    QString findName(int id);
    int findId(QString &name);

private:

    bool insertActivity(int id, const QDateTime &begin, const QDateTime &end, QString table);

    QMap<QString, int> activityMap;
    QSqlDatabase db;
//    QString dbPath = "G:/Projects/LM/db/LMtest.db";
//    QString dbPath = "D:/Projects/LM/db/LMtest.db";

    void archiveActivities();
    bool isArchived();
    bool diffDays(const QDateTime &begin, const QDateTime &end);
    void separateDate(const int id, const QDateTime &begin, const QDateTime &end);
    QList<QList<QDateTime> > getFilledTime();
    void fillSpaces();
    bool clearDailySchedule();

    void close();
    void initActivityMap();
    int setFrequency(int id);
    void initFrequency();
};
