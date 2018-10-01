#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>

#define DictActivity "DictActivity"
#define HistorySchedule "HistorySchedule"
#define DailyActivity "DailyActivity"

struct ScheduleData
{
    int id;
    QString beginDate;
    QString endDate;
};

class DataBase : public QObject
{
    Q_OBJECT
public:
    explicit DataBase(QObject *parent = nullptr);
    ~DataBase();

    bool insertActivity(const QVariantList &data);
    bool insertActivity(QSqlQuery *insert, QSqlQuery *currQuery);
    void connect();
    const QMap<QString, int> *getFrequency();
    const QMap<QString, int> getActivityMap();

private:

    QMap<QString, int> activityMap;
    QSqlDatabase db;
//    QString dbPath = "G:/Projects/LM/db/LMtest.db";
    QString dbPath = "D:/Projects/LM/db/LMtest.db";

    void archiveActivityJob();
    bool wasArchived();
    void addEmptyActivity(QDateTime &prevEnd, QDateTime &currBegin);
    QString findName(int id);

    bool open();
    void close();
    void initActivityMap();


signals:
    void opened();

public slots:
    void initDB();
};

#endif // DATABASE_H
