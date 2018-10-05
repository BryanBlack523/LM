#include "database.h"
#include <QFile>
#include <QDebug>
#include <QSqlError>
#include <QDateTime>



DataBase::DataBase(QObject *parent) : QObject(parent) {}

DataBase::~DataBase() {}

//-----------------------opening db
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

void DataBase::initDB()
{
    initActivityMap();

    if (!isArchived())
    {
        qDebug() << "DataBase::initDB\t\t begin updating " HistorySchedule" ";
        fillSpaces();
        archiveActivityJob();
        clearDailySchedule();
    }
    else
    {
        qDebug() << "DataBase::initDB\t\t " HistorySchedule" is up to date";
    }
}

void DataBase::close()
{
    db.close();
}

//------------------------------------getters

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

//----------------------------------------checks


bool DataBase::isArchived()
{
    QSqlQuery currQuery("select BeginDate from " DailySchedule" order by BeginDate limit 1");

    QDateTime date;

    if(!currQuery.size())
    {
        qDebug() << "DataBase::isArchived\t\t " DailySchedule" is empty";
        return false;
    }

    while (currQuery.next())
        date = currQuery.value(0).toDateTime();

    qDebug() << "DataBase::isArchived\t\t first BeginDate is " << date.toString("yyyy-MM-dd HH:mm:ss.zzz");
    if (date.daysTo(QDateTime::currentDateTime()) > 0)
        return false;
    else
        return true;
}

//----------------------------------------inits

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

void DataBase::fillSpaces()
{
    qDebug() << "DataBase::fillSpaces\t\t";
    QList<QVariantList> fillers;

    QSqlQuery dailyQuery("select BeginDate, EndDate from " DailySchedule" order by BeginDate");

    QSqlQuery lastDateQuery("select EndDate from " HistorySchedule" order by ScheduleID desc limit 1");

    QDateTime prevEndTime;

    while (lastDateQuery.next())// just for the sake of it check if last row in History is too far from first in Daily
//        prevEndTime = QDateTime::fromString(lastDateQuery.value(0).toString(), "yyyy-MM-dd HH:mm:ss.zzz");
          prevEndTime = lastDateQuery.value(0).toDateTime();

    qDebug() << prevEndTime.toString("yyyy-MM-dd HH:mm:ss.zzz");

    //first we will store in ram, then insert to avoid locks in db
    while(dailyQuery.next())
    {
        QDateTime currBeginTime = dailyQuery.value(0).toDateTime();

        qDebug() << prevEndTime.toString("yyyy-MM-dd HH:mm:ss.zzz") << " " << (char)((bool)(prevEndTime.secsTo(currBeginTime) > 0) ? '<' : '>') << " " << currBeginTime.toString("yyyy-MM-dd HH:mm:ss.zzz");
        if(prevEndTime.secsTo(currBeginTime) > 0)//find all spaces and record them
        {
//            qDebug() << "\t fill it!";
            QVariantList filler;

            filler.append(0);
            filler.append(prevEndTime);
            filler.append(currBeginTime);

            fillers.append(filler);
        }
        prevEndTime =  dailyQuery.value(1).toDateTime();
    }

    // now insert them in table
    for (int i = 0; i < fillers.size(); i++)
    {
        insertActivity(fillers[i].value(0).toInt(), fillers[i].value(1).toDateTime(), fillers[i].value(2).toDateTime(), DailySchedule);
    }
    qDebug() << "DataBase::fillSpaces\t\t filled";
}

void DataBase::archiveActivityJob()
{
    qDebug() << "DataBase::archiveActivityJob\t\t";
    QSqlQuery currQuery;
    currQuery.prepare("select ActivityId, BeginDate, EndDate from " DailySchedule" where date(BeginDate) < date (:Date) order by BeginDate");

    QDateTime date = QDateTime::currentDateTime();

    qDebug() << date.toString("yyyy-MM-dd");
    currQuery.bindValue(":Date", date.toString("yyyy-MM-dd"));
    currQuery.exec();
    qDebug() << currQuery.lastQuery();


    QSqlQuery insert;
    insert.prepare("insert into " HistorySchedule" (ActivityID, BeginDate, EndDate)"
                  "VALUES (:ActivityID, :BeginDate, :EndDate)");

    while(currQuery.next())
    {
        qDebug() << "inserting " << currQuery.value(0).toInt() << " " << currQuery.value(1).toDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz") << " " << currQuery.value(2).toDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz");
        insertActivity(&insert, &currQuery);
    }
    qDebug() << "DataBase::archiveActivityJob\t\t complete";
}

bool DataBase::clearDailySchedule()
{
    qDebug() << "DataBase::clearDailySchedule\t\t";
    QDateTime date = QDateTime::currentDateTime();

    QSqlQuery truncate;
    truncate.prepare("delete from " DailySchedule" where date(BeginDate) < date (:Date)");

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

//--------------------------------

QString DataBase::findName(int id)
{
    for (auto activityKey : activityMap.keys())
    {
        if (activityMap.value(activityKey) == id)
            return activityKey;
    }

    qDebug() << "DataBase::findName::\t\tcould not find name for id:" << id;
    return "";
}

void DataBase::filled()
{
    QSqlQuery dailyQuery("select BeginDate, EndDate from " DailySchedule" order by BeginDate");

    QList<QList<QDateTime>> matrix;

    while(dailyQuery.next())
    {
        QDateTime begin = dailyQuery.value(0).toDateTime();
        QDateTime end = dailyQuery.value(1).toDateTime();

        for (auto checkList : matrix)
        {
            if ((checkList[0].secsTo(begin) > 0) && (checkList[1].secsTo(end)))
            {

            }
        }

        QList<QDateTime> t_list;

        t_list.append(begin);
        t_list.append(end);

        matrix.append(t_list);
    }
}

bool DataBase::diffDays(const QDateTime &begin,const QDateTime &end)
{
    return begin.daysTo(end) > 0 ? true : false;
}

void DataBase::separateDate(const int id,const QDateTime &begin,const QDateTime &end)
{
    qDebug() << "DataBase::separateDate::\t\tcutting in half " << id << " " << begin.toString("yyyy-MM-dd HH:mm:ss.zzz") << " " << end.toString("yyyy-MM-dd HH:mm:ss.zzz");
    QDateTime newEnd = begin;
    newEnd.setTime(QTime(23, 59, 59, 998));

    QDateTime newBegin = end;
    newBegin.setTime(QTime(00, 00, 00, 001));

    if (begin.daysTo(QDateTime::currentDateTime()) > 0)
        insertActivity(id, begin, newEnd, HistorySchedule);
    else
        insertActivity(id, begin, newEnd, DailySchedule);

    if (end.daysTo(QDateTime::currentDateTime()) > 0)
        insertActivity(id, newBegin, end, HistorySchedule);
    else
        insertActivity(id, newBegin, end, DailySchedule);
}

//-----------------------inserters

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
//    qDebug() << "DataBase::insertActivity::\t\t";
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

bool DataBase::insertActivity(QSqlQuery *insert, const QSqlQuery *currQuery)
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
