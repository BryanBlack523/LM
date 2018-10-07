#include "database.h"
#include <QFile>
#include <QDebug>
#include <QSqlError>
#include <QDateTime>



DataBase::DataBase(QObject *parent) : QObject(parent) {}

DataBase::~DataBase() {}

//-----------------------opening db
void DataBase::connect(const QString &dbPath)
{
    if(!QFile(dbPath).exists())//":/db/db/LMtest.db"
        qDebug() << "DataBase::connect::\t\tDB path not found";
    else
        this->open(dbPath);
}

bool DataBase::open(const QString &dbPath)
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
{//list of functions that will launch with app
    initActivityMap();

    archiveJob();
}

void DataBase::archiveJob()
{
    if (!isArchived())
    {
        qDebug() << "DataBase::archiveJob\t\t begin updating " HistorySchedule" ";
        fillSpaces();
        archiveActivities();
        clearDailySchedule();
    }
    else
        qDebug() << "DataBase::archiveJob\t\t " HistorySchedule" is up to date";
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

    result->insert("None", 0);//special case - i do not want "None" activity to be in top... ever

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

    while (currQuery.next())
        date = currQuery.value(0).toDateTime();

    if ((date.daysTo(QDateTime::currentDateTime()) > 0) || (date.isValid()))
    {
        qDebug() << "DataBase::isArchived\t\t first BeginDate is " << date.toString("yyyy-MM-dd HH:mm:ss.zzz");
        return false;
    }
    else
    {
        qDebug() << "DataBase::isArchived\t\t " DailySchedule" is empty";
        return true;
    }
}

//----------------------------------------inits

void DataBase::initActivityMap()
{//get all from DictionaryActivity
    QSqlQuery activities("select " DictActivity".Activity, " DictActivity".ActivityID from " DictActivity"");

    while (activities.next())
    {
        QString name = activities.value(0).toString();
        int id = activities.value(1).toInt();

        activityMap.insert(name, id);
    }
}

QList<QList<QDateTime>> DataBase::getFilledTime()
{// apprehend all used dates in a single timeline
    qDebug() << "DataBase::getFilledTime::\t\t";

    QSqlQuery dailyQuery;
    dailyQuery.prepare("select BeginDate, EndDate from " DailySchedule" where date(BeginDate) < date (:Date) order by BeginDate");
    QDateTime date = QDateTime::currentDateTime();

    dailyQuery.bindValue(":Date", date.toString("yyyy-MM-dd"));
    dailyQuery.exec();

    QSqlQuery lastDateQuery("select BeginDate, EndDate from " HistorySchedule" order by ScheduleID desc limit 1");

    QList<QList<QDateTime>> matrix;

    while (lastDateQuery.next())
    {//initialise with last row from history table
        QDateTime begin = lastDateQuery.value(0).toDateTime();
        QDateTime end = lastDateQuery.value(1).toDateTime();

        QList<QDateTime> newItem;

        newItem.append(begin);
        newItem.append(end);

        matrix.append(newItem);
    }

    while(dailyQuery.next())
    {//now check, if any of the data is between the dates that already were written
        QDateTime begin = dailyQuery.value(0).toDateTime();
        QDateTime end = dailyQuery.value(1).toDateTime();

//        qDebug() << matrix.size() << " " << begin.toString("yyyy-MM-dd HH:mm:ss.zzz") << " " << end.toString("yyyy-MM-dd HH:mm:ss.zzz");

        bool addFlag = true;//if date does not cross any of existing, append as new date

        for (int i = 0; i < matrix.size(); i++)
        {
            QDateTime checkBegin = matrix[i].at(0);
            QDateTime checkEnd = matrix[i].at(1);

//            qDebug() << "check borders\t" << checkBegin.toString("yyyy-MM-dd HH:mm:ss.zzz") << " " << checkEnd.toString("yyyy-MM-dd HH:mm:ss.zzz");
//            qDebug() << (checkBegin.secsTo(begin) > 0) << " && " << (checkEnd.secsTo(begin) < 0) << " || " << (checkBegin.secsTo(end) > 0) << " && " << (checkEnd.secsTo(end) < 0);
            if (((checkBegin.secsTo(begin) >= 0) && (checkEnd.secsTo(begin) <= 0))// if one of the dates between existing
               || ((checkBegin.secsTo(end) >= 0) && (checkEnd.secsTo(end) <= 0)))
            {
                addFlag = false;
//                qDebug() << "gotcha";

                if (((checkBegin.secsTo(begin) >= 0) && (checkEnd.secsTo(begin) <= 0))// only first one is between
                    && !((checkBegin.secsTo(end) >= 0) && (checkEnd.secsTo(end) <= 0)))
                {
//                    qDebug() << checkBegin.toString("yyyy-MM-dd HH:mm:ss.zzz") << " < " << begin.toString("yyyy-MM-dd HH:mm:ss.zzz") << " < " << checkEnd.toString("yyyy-MM-dd HH:mm:ss.zzz");
//                    qDebug() << "expanding\t" << checkEnd.toString("yyyy-MM-dd HH:mm:ss.zzz") << " to " << end.toString("yyyy-MM-dd HH:mm:ss.zzz");
                    matrix[i].replace(1, end);// expand borders of existing
                }
                else if (!((checkBegin.secsTo(begin) >= 0) && (checkEnd.secsTo(begin) <= 0))// only last one is between
                         && ((checkBegin.secsTo(end) >= 0) && (checkEnd.secsTo(end) <= 0)))
                {
//                    qDebug() << checkBegin.toString("yyyy-MM-dd HH:mm:ss.zzz") << " < " << end.toString("yyyy-MM-dd HH:mm:ss.zzz") << " < " << checkEnd.toString("yyyy-MM-dd HH:mm:ss.zzz");
//                    qDebug() << "expanding\t" << checkBegin.toString("yyyy-MM-dd HH:mm:ss.zzz") << " to " << begin.toString("yyyy-MM-dd HH:mm:ss.zzz");
                    matrix[i].replace(0, begin);// expand borders of existing
                }
            }
        }

        if (addFlag)
        {
//            qDebug() << "create new";

            QList<QDateTime> newItem;

            newItem.append(begin);
            newItem.append(end);

            matrix.append(newItem);
        }
    }

    qDebug() << "DataBase::getFilledTime::\tcomplete";
    return matrix;
}

void DataBase::fillSpaces()
{// get the already filled times and fill in the gaps
    qDebug() << "DataBase::fillSpaces\t\t";

    QList<QList<QDateTime>> matrix = getFilledTime();

    for (int i =0; i < matrix.size() - 1; ++i)
        if (matrix[i].value(1).secsTo(matrix[i + 1].value(0)) > 0)//not null
            insertActivity(1, matrix[i].value(1), matrix[i + 1].value(0), DailySchedule);// 1 - "None" Activity

    qDebug() << "DataBase::fillSpaces\t\t filled";
}

void DataBase::archiveActivities()
{
    qDebug() << "DataBase::archiveActivities\t\t";
    QSqlQuery currQuery;
    currQuery.prepare("select ActivityId, BeginDate, EndDate from " DailySchedule" where date(BeginDate) < date (:Date) order by BeginDate");

    QDateTime date = QDateTime::currentDateTime();

//    qDebug() << date.toString("yyyy-MM-dd");
    currQuery.bindValue(":Date", date.toString("yyyy-MM-dd"));
    currQuery.exec();
//    qDebug() << currQuery.lastQuery();

    QSqlQuery insert;
    insert.prepare("insert into " HistorySchedule" (ActivityID, BeginDate, EndDate)"
                  "VALUES (:ActivityID, :BeginDate, :EndDate)");

    while(currQuery.next())
    {
//        qDebug() << "inserting " << currQuery.value(0).toInt() << " " << currQuery.value(1).toDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz") << " " << currQuery.value(2).toDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz");
        insertActivity(&insert, &currQuery);
    }
    qDebug() << "DataBase::archiveActivities\t\t complete";
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
{// i really don't know why i made this function... but why the fuck not?
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
