#include "database.h"
#include <QFile>
#include <QDebug>
#include <QSqlError>
#include <QDateTime>



DataBase::DataBase(QObject *parent) : QObject(parent) {}

DataBase::~DataBase() {}

void DataBase::connect()
{
    if(!QFile(dbPath).exists())//":/db/db/LMtest.db"
        qDebug() << "DataBase::connect::\t\tDB path not found";
    else
        this->open();
}

bool DataBase::open()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
//    db.setHostName("LMtest");
    db.setDatabaseName(dbPath);

    if (db.open())
    {
        emit opened();
        return  true;
    }
    else
    {
        qDebug() << "DataBase::open::\t\tcould not open db";
        qDebug() << db.lastError().text();
        return false;
    }
}

void DataBase::close()
{
    db.close();
}

void DataBase::initDB()
{
    initActivityMap();
    archiveActivityJob();
}

const QMap<QString, int>* DataBase::getFrequency()
{
    //because SQLight does not have RIGHT JOIN, hence a crutch:
    //first get all Activity names and shove it in map

    QMap<QString, int>* result = new QMap<QString, int>;

    for (auto it : activityMap.keys())
        result->insert(it, 0);

    // then get frequency data from here
    QSqlQuery query("select " DictActivity".Activity, count(" HistorySchedule".ActivityID) as Frequency"
                    " from " HistorySchedule""
                    " join " DictActivity" on " DictActivity".ActivityID = " HistorySchedule".ActivityID"
                    " group by " HistorySchedule".ActivityID");

    while (query.next())
    {
        QString name = query.value(0).toString();
        int frequency = query.value(1).toInt();

        result->insert(name, frequency);
    }

    return  result;
}

const QMap<QString, int> DataBase::getActivityMap()
{
    return activityMap;
}

void DataBase::archiveActivityJob()
{
    if (!wasArchived())
    {
        QSqlQuery currQuery("select " DailyActivity".ActivityId, " DailyActivity".BeginDate, " DailyActivity".EndDate from " DailyActivity"");
        QSqlQuery lastDateQuery("select " HistorySchedule".EndDate from " HistorySchedule" order by ID desc limit 1");

        QDateTime prevEndTime;

        while (lastDateQuery.next())
            prevEndTime = QDateTime::fromString(lastDateQuery.value(0).toString(), "yyyy-MM-dd HH:mm:ss.zzz");

        QSqlQuery insert;

        insert.prepare("insert into " HistorySchedule" (ActivityID, BeginDate, EndDate)"
                      "VALUES (:ActivityID, :BeginDate, :EndDate)");

        while(currQuery.next())
        {
            QDateTime currBeginTime = QDateTime::fromString(currQuery.value(1).toString(), "yyyy-MM-dd HH:mm:ss.zzz");

            if(prevEndTime.secsTo(currBeginTime) > 0)
            {
                addEmptyActivity(prevEndTime, currBeginTime);
            }
            insertActivity(&insert, &currQuery);

            prevEndTime =  QDateTime::fromString(currQuery.value(2).toString(), "yyyy-MM-dd HH:mm:ss.zzz");
        }
    }
    else
        qDebug() << "DataBase::archiveActivityJob\t\t " HistorySchedule" is up to date";
}

bool DataBase::wasArchived()
{
    QSqlQuery historyQuery("select " HistorySchedule".EndDate from " HistorySchedule" order by ID desc limit 1");

    QDateTime histEnd;

    while (historyQuery.next())
        histEnd = QDateTime::fromString(historyQuery.value(0).toString(), "yyyy-MM-dd") ;

    QSqlQuery currQuery("select " DailyActivity".EndDate from " DailyActivity" order by ID limit 1");

    QDateTime currEnd;

    while (currQuery.next())
    {
        currEnd = QDateTime::fromString(currQuery.value(0).toString(), "yyyy-MM-dd");
    }

    if (histEnd.secsTo(currEnd) > 0)
        return false;
    else
        return true;
}

void DataBase::addEmptyActivity(QDateTime &prevEnd, QDateTime &currBegin)
{
    QVariantList data;

    data.append(0);
    data.append(prevEnd);
    data.append(currBegin);

    insertActivity(data);
}

QString DataBase::findName(int id)
{
    for (auto it : activityMap.keys())
    {
        if (activityMap.value(it) == id)
            return it;
    }

    return "";
}

void DataBase::initActivityMap()
{
    QSqlQuery activities("select " DictActivity".Activity, " DictActivity".ActivityID from " DictActivity"");

    while (activities.next())
    {
        QString name = activities.value(0).toString();
        int id = activities.value(1).toInt();

        activityMap.insert(name, id);
    }
}

bool DataBase::insertActivity(const QVariantList &data)
{
    QSqlQuery query;

    query.prepare("insert into " HistorySchedule" (BeginDate, EndDate, ActivityID)"
                  "VALUES (:BeginDate, :EndDate, :ActivityID)");

    int id = activityMap[data[0].toString()];

    query.bindValue(":ActivityID", id);
    query.bindValue(":BeginDate", data[1].toDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz"));
    query.bindValue(":EndDate", data[2].toDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz"));

    if (!query.exec())
    {
        qDebug() << "DataBase::insertActivity::\t\tcould not insert";
        qDebug() << query.lastError().text();
        return false;
    }
    else
        return true;

    return false;
}

bool DataBase::insertActivity(QSqlQuery *insert, QSqlQuery *currQuery)
{
    insert->bindValue(":ActivityID", currQuery->value(0).toInt());
    insert->bindValue(":BeginDate", currQuery->value(1).toString());
    insert->bindValue(":EndDate", currQuery->value(2).toString());

    if (!insert->exec())
    {
        qDebug() << "DataBase::insertActivity::\t\tcould not insert";
        qDebug() << insert->lastError().text();
        return false;
    }
    else
        return true;

    return false;
}



