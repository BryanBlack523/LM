#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>

#define DictActivity "DictActivity"
#define HistorySchedule "HistorySchedule"
#define DailySchedule "DailySchedule"

class DataBase : public QObject
{
    Q_OBJECT
public:
    explicit DataBase(QObject *parent = nullptr);
    ~DataBase();

    bool insertActivity(const QVariantList &data);
    bool insertActivity(QSqlQuery *insert, const QSqlQuery *currQuery);
    bool insertActivity(int id, const QDateTime &begin, const QDateTime &end);
    bool insertActivity(int id, const QDateTime &begin, const QDateTime &end, QString table);
    void connect(const QString &dbPath);
    const QMap<QString, int> *getFrequency();
    const QMap<QString, int> getActivityMap();
    void archiveJob();
    QString findName(int id);
    int findId(QString &name);

private:

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

    bool open(const QString &dbPath);
    void close();
    void initActivityMap();
    int setFrequency(int id);
    void initFrequency();


signals:
    void opened();

public slots:
    void initDB();
};

#endif // DATABASE_H
