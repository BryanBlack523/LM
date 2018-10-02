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
    clearDailySchedule();
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

bool DataBase::archiveActivityJob()
{
    if (!wasArchived())
    {
        QSqlQuery currQuery("select ActivityId, BeginDate, EndDate from " DailySchedule" order by BeginDate");

        if(!currQuery.size())
            return false;

        QSqlQuery lastDateQuery("select BeginDate from " HistorySchedule" order by ScheduleID desc limit 1");

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
                addEmptyActivity(prevEndTime, currBeginTime);

            insertActivity(&insert, &currQuery);

            prevEndTime =  QDateTime::fromString(currQuery.value(2).toString(), "yyyy-MM-dd HH:mm:ss.zzz");
        }
    }
    else
    {
        qDebug() << "DataBase::archiveActivityJob\t\t " HistorySchedule" is up to date";
        return false;
    }

    return false;
}

bool DataBase::wasArchived()
{
    QSqlQuery historyQuery("select EndDate from " HistorySchedule" order by ScheduleID desc limit 1");

    QDateTime histEnd;

    while (historyQuery.next())
        histEnd = historyQuery.value(0).toDateTime();//QDateTime::fromString(historyQuery.value(0).toString(), "yyyy-MM-dd HH:mm:ss.zzz");

    QSqlQuery currQuery("select BeginDate from " DailySchedule" order by BeginDate limit 1");


    QDateTime currEnd;

    while (currQuery.next())
    {
        currEnd = currQuery.value(0).toDateTime();//QDateTime::fromString(currQuery.value(0).toString(), "yyyy-MM-dd HH:mm:ss.zzz");
    }

    if (histEnd.daysTo(currEnd) > 0)
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

bool DataBase::diffDays(const QDateTime &begin,const QDateTime &end)
{
    return begin.daysTo(end) > 0 ? true : false;
}

void DataBase::separateDate(const int id,const QDateTime &begin,const QDateTime &end)
{
    QDateTime newEnd = begin;
    newEnd.setTime(QTime(23, 59, 59, 998));

    QDateTime newBegin = end;
    newBegin.setTime(QTime(00, 00, 00, 001));

    insertActivity(id, begin, newEnd);
    insertActivity(id, newBegin, end, DailySchedule);
}

bool DataBase::clearDailySchedule()
{
    QDateTime date = QDateTime::currentDateTime();

    QSqlQuery truncate("delete from " DailySchedule" where date(BeginDate) < date (':Date')");

    truncate.bindValue(":Date", date.toString("yyyy-MM-dd"));

    if (!truncate.exec())
    {
        qDebug() << "DataBase::clearDaily::\t\tcould not delete";
        qDebug() << truncate.lastError().text();
        return false;
    }
    else
        return true;

    return false;
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
    if (diffDays(data[1].toDateTime(), data[2].toDateTime()))
        separateDate(activityMap[data[0].toString()], data[1].toDateTime(), data[2].toDateTime());

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

bool DataBase::insertActivity(int id, const QDateTime &begin, const QDateTime &end)
{
    if (diffDays(begin, end))
        separateDate(id, begin, end);

    QSqlQuery query;

    query.prepare("insert into " HistorySchedule" (BeginDate, EndDate, ActivityID)"
                  "VALUES (:BeginDate, :EndDate, :ActivityID)");

    query.bindValue(":ActivityID", id);
    query.bindValue(":BeginDate", begin.toString("yyyy-MM-dd HH:mm:ss.zzz"));
    query.bindValue(":EndDate", end.toString("yyyy-MM-dd HH:mm:ss.zzz"));

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

bool DataBase::insertActivity(int id, const QDateTime &begin, const QDateTime &end, QString table)
{
    if (diffDays(begin, end))
        separateDate(id, begin, end);

    QSqlQuery query;

    query.prepare("insert into " + table + " (BeginDate, EndDate, ActivityID)"
                  "VALUES (:BeginDate, :EndDate, :ActivityID)");

    query.bindValue(":ActivityID", id);
    query.bindValue(":BeginDate", begin.toString("yyyy-MM-dd HH:mm:ss.zzz"));
    query.bindValue(":EndDate", end.toString("yyyy-MM-dd HH:mm:ss.zzz"));

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
    if (diffDays(currQuery->value(1).toDateTime(), currQuery->value(2).toDateTime()))
        separateDate(currQuery->value(0).toInt(), currQuery->value(1).toDateTime(), currQuery->value(2).toDateTime());

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



