#include "database.h"
#include <QFile>
#include <QDebug>
#include <QSqlError>



DataBase::DataBase(QObject *parent) : QObject(parent) {}

DataBase::~DataBase() {}

void DataBase::connect()
{
    if(!QFile(dbPath).exists())//":/db/db/LMtest.db"
        qDebug() << "DB path not found";
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
        qDebug() << "could not open db";
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
    QSqlQuery names("select " DictActivity".Activity from " DictActivity"");

    QMap<QString, int>* result = new QMap<QString, int>;

    while (names.next())
    {
        QString name = names.value(0).toString();

        result->insert(name, 0);
    }

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

bool DataBase::saveActivity(const QVariantList &data)
{
    QSqlQuery query;

    query.prepare("ÏNSERT INTO " DictActivity" (BeginDate, EndDate, ActivityID)"
                  "VALUES (:BeginDate, :EndDate, :ActivityID)");

    query.bindValue(":BeginDate", data[0].toString());
    query.bindValue(":EndDate", data[1].toString());
    query.bindValue(":ActivityID", data[1].toString());

    if (!query.exec())
    {
        qDebug() << "could not insert";
        qDebug() << query.lastError().text();
        return false;
    }
    else
        return true;

    return false;
}



