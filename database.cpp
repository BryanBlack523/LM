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
        return  true;
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

const QMap<QString, int>* DataBase::getFrequency()
{
    //because SQLight does not have RIGHT JOIN, hence a crutch:
    //first get all Activity names and shove it in map

    QMap<QString, int>* result = new QMap<QString, int>;

    initActivityMap();

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

void DataBase::initActivityMap()
{
    qDebug() << "here";
    QSqlQuery activities("select " DictActivity".Activity, " DictActivity".ActivityID from " DictActivity"");

//    activityMap = *new QMap<QString, int>;

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

    qDebug() << data[0] << data[1].toDateTime() << data[2].toDateTime();

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



