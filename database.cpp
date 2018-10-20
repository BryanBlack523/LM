#include "database.h"

#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QSqlError>
#include <QSqlQuery>

#define DictActivity "DictActivity"
#define HistorySchedule "HistorySchedule"
#define DailySchedule "DailySchedule"

DataBase::DataBase(QObject *parent)
  : QObject(parent)
{}

DataBase::~DataBase(){}

bool DataBase::open(const QString &dbPath)
{
    if(!QFile(dbPath).exists())
    {
        qDebug() << "DataBase::connect::\t\tDB path not found";
        return false;
    }

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbPath);

    if (db.open())
    {
        initActivityMap();
        initFrequency();
        archiveJob();
        return  true;
    }

    qDebug() << "DataBase::open::\t\tcould not open db";
    qDebug() << db.lastError().text();
    return false;
}

void DataBase::archiveJob()
{
    if (!isArchived())
    {
        qDebug() << "DataBase::archiveJob\t\t begin updating " HistorySchedule" ";
        fillSpaces();
        archiveActivities();
        clearDailySchedule();
        initFrequency();
    }
    else
        qDebug() << "DataBase::archiveJob\t\t " HistorySchedule" is up to date";
}

void DataBase::close()
{
    db.close();
}

//------------------------------------getters

int DataBase::setFrequency(int id)
{
//    qDebug() << "DataBase::setFrequency::\t" << id;
    int frequency = 0;

    QSqlQuery query;
    query.prepare("select count(ActivityID) from " HistorySchedule" where ActivityID = :id and date(BeginDate) > date('now', '-31 day')");

    query.bindValue(":id", id);
    query.exec();

    while (query.next())
        frequency = query.value(0).toInt();

    QSqlQuery update;
    update.prepare("update " DictActivity" set Frequency = :frequency where ActivityID = :id");
    update.bindValue(":frequency", frequency);
    update.bindValue(":id", id);
    if (!update.exec())
    {
        qDebug() << "DataBase::setFrequency::\t\tcould not update " << id << " new frequency is " << frequency;
        qDebug() << update.lastError().text();
    }

    return frequency;
}

void DataBase::initFrequency()
{
    qDebug() << "DataBase::initFrequency::\t";
    QSqlQuery activities("select ActivityID from " DictActivity"");

    while(activities.next())
        setFrequency(activities.value(0).toInt());
    qDebug() << "DataBase::initFrequency::\t";
}


void DataBase::getFrequency(QMap<QString, int> &result)
{
    qDebug() << "DataBase::getFrequency::\t";
    QSqlQuery query("select Activity, Frequency, ActivityID from " DictActivity"");

    result.clear();

    while (query.next())
    {
        int frequency;

        if (query.isNull(1))
            frequency = setFrequency(query.value(2).toInt());
        else
            frequency = query.value(1).toInt();

        QString name = query.value(0).toString();

        result.insert(name, frequency);
    }

    result.insert("None", 0);//special case - i do not want "None" activity to be in top... ever

    qDebug() << "DataBase::getFrequency::\t";
}

const QMap<QString, int>& DataBase::getActivityMap()
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



std::vector<std::vector<QDateTime> > DataBase::getFilledTime()
{// apprehend all used dates in a single timeline
    qDebug() << "DataBase::getFilledTime::\t\t";

    QSqlQuery dailyQuery("select BeginDate, EndDate from " DailySchedule" where date(BeginDate) < date ('now') order by BeginDate");


    QSqlQuery lastDateQuery("select BeginDate, EndDate from " HistorySchedule" order by ScheduleID desc limit 1");

    std::vector<std::vector<QDateTime>> matrix;

    while (lastDateQuery.next())
    {//initialise with last row from history table
        QDateTime begin = lastDateQuery.value(0).toDateTime();
        QDateTime end = lastDateQuery.value(1).toDateTime();

        std::vector<QDateTime> newItem;

        newItem.push_back(begin);
        newItem.push_back(end);

        matrix.push_back(newItem);
    }

    while(dailyQuery.next())
    {//now check, if any of the data is between the dates that already were written
        QDateTime begin = dailyQuery.value(0).toDateTime();
        QDateTime end = dailyQuery.value(1).toDateTime();

//        qDebug() << matrix.size() << " " << begin.toString("yyyy-MM-dd HH:mm:ss.zzz") << " " << end.toString("yyyy-MM-dd HH:mm:ss.zzz");

        bool addFlag = true;//if date does not cross any of existing, append as new date

        for (unsigned int i = 0; i < matrix.size(); i++)
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
                    matrix[i].at(1) = end;
//                    matrix[i].replace(1, end);// expand borders of existing
                }
                else if (!((checkBegin.secsTo(begin) >= 0) && (checkEnd.secsTo(begin) <= 0))// only last one is between
                         && ((checkBegin.secsTo(end) >= 0) && (checkEnd.secsTo(end) <= 0)))
                {
//                    qDebug() << checkBegin.toString("yyyy-MM-dd HH:mm:ss.zzz") << " < " << end.toString("yyyy-MM-dd HH:mm:ss.zzz") << " < " << checkEnd.toString("yyyy-MM-dd HH:mm:ss.zzz");
//                    qDebug() << "expanding\t" << checkBegin.toString("yyyy-MM-dd HH:mm:ss.zzz") << " to " << begin.toString("yyyy-MM-dd HH:mm:ss.zzz");
                    matrix[i].at(0) = begin;
//                    matrix[i].replace(0, begin);// expand borders of existing
                }
            }
        }

        if (addFlag)
        {
//            qDebug() << "create new";

            std::vector<QDateTime> newItem;

            newItem.push_back(begin);
            newItem.push_back(end);

            matrix.push_back(newItem);
        }
    }

    qDebug() << "DataBase::getFilledTime::\tcomplete";
    return matrix;
}

void DataBase::fillSpaces()
{// get the already filled times and fill in the gaps
    qDebug() << "DataBase::fillSpaces\t\t";

    std::vector<std::vector<QDateTime>> matrix = getFilledTime();

    for (unsigned int i =0; i < matrix.size() - 1; ++i)
        if (matrix[i].at(1).secsTo(matrix[i + 1].at(0)) > 0)//not null
            insertActivityDaily(1, matrix[i].at(1), matrix[i + 1].at(0));// 1 - "None" Activity

    qDebug() << "DataBase::fillSpaces\t\t filled";
}

void DataBase::archiveActivities()
{
    qDebug() << "DataBase::archiveActivities\t\t";
    QSqlQuery currQuery("select ActivityId, BeginDate, EndDate from " DailySchedule" where date(BeginDate) < date ('now') order by BeginDate");

    while(currQuery.next())
    {
//        qDebug() << "inserting " << currQuery.value(0).toInt() << " " << currQuery.value(1).toDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz") << " " << currQuery.value(2).toDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz");
        insertActivityHistory(&currQuery);
    }
    qDebug() << "DataBase::archiveActivities\t\t complete";
}

bool DataBase::clearDailySchedule()
{
    qDebug() << "DataBase::clearDailySchedule\t\t";

    QSqlQuery truncate("delete from " DailySchedule" where date(BeginDate) < date ('now')");

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

int DataBase::findId(QString &name)
{
    for (auto activityKey : activityMap.keys())
    {
        if (activityKey == name)
            return activityMap.value(activityKey);
    }

    qDebug() << "DataBase::findId::\t\tcould not find id for name:" << name;
    return 0;
}

//--------------------------------------

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

    begin.daysTo(QDateTime::currentDateTime()) > 0
            ? insertActivityHistory(id, begin, newEnd)
            : insertActivityDaily(id, begin, newEnd);

    end.daysTo(QDateTime::currentDateTime()) > 0
            ? insertActivityHistory(id, newBegin, end)
            : insertActivityDaily(id, newBegin, end);
}

//-----------------------inserters

bool DataBase::insertActivityHistory(int id, const QDateTime &begin, const QDateTime &end)
{
    return insertActivityTable(id, begin, end, HistorySchedule);
}

bool DataBase::insertActivityDaily(int id, const QDateTime &begin, const QDateTime &end)
{
    return insertActivityTable(id, begin, end, DailySchedule);
}

bool DataBase::insertActivityHistory(const QSqlQuery *currQuery)
{// i really don't know why i made this function... but why the fuck not?

    int id = currQuery->value(0).toInt();
    QDateTime begin = currQuery->value(1).toDateTime();
    QDateTime end = currQuery->value(2).toDateTime();

    return insertActivityHistory(id, begin, end);
}

bool DataBase::insertActivityTable(int id, const QDateTime &begin, const QDateTime &end, QString table)
{
    if (diffDays(begin, end))
    {
        separateDate(id, begin, end);
        return true;
    }

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
