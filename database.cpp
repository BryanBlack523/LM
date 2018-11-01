#include "database.h"

#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>

#define DictActivity "DictActivity"
#define HistorySchedule "HistorySchedule"
#define DailySchedule "DailySchedule"

DataBase::DataBase(QObject *parent)
  : QObject(parent)
{}

DataBase::~DataBase(){}

QString DataBase::path()
{
    QDir resultPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/data/";

    QFile dataFile(resultPath.filePath("LMtest.db"));
    QFile resFile(":/data/db/LMtest.db");

    QFileInfo dataFileInf(dataFile);
    QFileInfo resFileInf(resFile);

    if (!dataFile.exists() && dataFileInf.lastModified() < resFileInf.lastModified())
    {
        QDir newPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
        newPath.mkdir("data");

        resFile.copy(resultPath.filePath("LMtest.db"));
        QFile::setPermissions(resultPath.filePath("LMtest.db"), QFile::WriteOwner | QFile::ReadOwner);
    }

    qDebug() << resultPath.filePath("LMtest.db");
    return  resultPath.filePath("LMtest.db");
}

bool DataBase::createDB()
{
    QSqlQuery makeDaily("CREATE TABLE \" DailySchedule\" ("
                        " `BeginDate` TEXT NOT NULL,"
                        " `EndDate` TEXT NOT NULL,"
                        " `ActivityID` INTEGER NOT NULL"
                        " )");
    QSqlQuery makeHistory("CREATE TABLE \"HistorySchedule\" ("
                          " `ScheduleID` INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,"
                          " `BeginDate` TEXT NOT NULL,"
                          " `EndDate` TEXT NOT NULL,"
                          " `ActivityID` INTEGER NOT NULL"
                          " )");
    QSqlQuery makeDictionary("CREATE TABLE \"DictActivity\" ("
                             " `ActivityID` INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,"
                             " `Activity` TEXT NOT NULL,"
                             " `Group` TEXT NOT NULL, `GroupID` INTEGER NOT NULL,"
                             " `Weight` INTEGER, `Frequency` INTEGER,"
                             " `Color` TEXT DEFAULT '#000000',"
                             " `Icon` TEXT DEFAULT 'Default'"
                             " )");
    QSqlQuery makeWeights("CREATE TABLE \"Weights\" ("
                          " `ActivityID` INTEGER NOT NULL,"
                          " `LastUpdated` TEXT NOT NULL,"
                          " `Weight` INTEGER NOT NULL,"
                          " PRIMARY KEY(`ActivityID`,`LastUpdated`)"
                          " )");

    QSqlQuery triggerActivityToWeights("CREATE TRIGGER add_new_activity_to_weights "
                                       "AFTER INSERT ON DictActivity "
                                       "BEGIN insert into Weights "
                                       "(ActivityID, LastUpdated, Weight) "
                                       "values (new.ActivityID, datetime('now', 'localtime'), new.Weight); "
                                       "END");

    QSqlQuery triggerUpdateActivityWeight("CREATE TRIGGER update_activity_weight "
                                          "AFTER INSERT ON Weights "
                                          "BEGIN update DictActivity "
                                          "set Weight = new.Weight "
                                          "where DictActivity.ActivityID = new.ActivityID; "
                                          "END");

    if (!makeDaily.exec())
    {
        qDebug() << "Could not create " DailySchedule;
        makeDaily.lastError();
        return false;
    }

    if (!makeHistory.exec())
    {
        qDebug() << "Could not create " HistorySchedule;
        makeHistory.lastError();
        return false;
    }

    if (!makeDictionary.exec())
    {
        qDebug() << "Could not create " DictActivity;
        makeDictionary.lastError();
        return false;
    }

    if (!makeWeights.exec())
    {
        qDebug() << "Could not create Weights";
        makeWeights.lastError();
        return false;
    }

    if (!triggerActivityToWeights.exec())
    {
        qDebug() << "Could not create trigger add_new_activity_to_weights";
        triggerActivityToWeights.lastError();
        return false;
    }

    if (!triggerUpdateActivityWeight.exec())
    {
        qDebug() << "Could not create trigger update_activity_weight";
        triggerUpdateActivityWeight.lastError();
        return false;
    }

    return true;
}

bool DataBase::open()
{
    QString dbPath = path();

    if(!QFile(dbPath).exists())
    {
        qDebug() << "DataBase::open::\t\tDB path not found";
        return false;
    }

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbPath);

    if (db.open())
    {
        initActivityMap();
        updateFrequency();
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
        updateFrequency();
    }
    else
        qDebug() << "DataBase::archiveJob\t\t " HistorySchedule" is up to date";
}

void DataBase::close()
{
    db.close();
}

//------------------------------------frequency

int DataBase::writeFrequency(int id)
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

void DataBase::updateFrequency()
{
    qDebug() << "DataBase::initFrequency::\t";
    QSqlQuery activities("select ActivityID from " DictActivity"");

    while(activities.next())
        writeFrequency(activities.value(0).toInt());
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
            frequency = writeFrequency(query.value(2).toInt());
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

        bool addFlag = true;//if date does not cross any of existing, append as new date

        for (unsigned int i = 0; i < matrix.size(); i++)
        {
            QDateTime checkBegin = matrix[i].at(0);
            QDateTime checkEnd = matrix[i].at(1);

            if (((checkBegin.secsTo(begin) >= 0) && (checkEnd.secsTo(begin) <= 0))// if one of the dates between existing
               || ((checkBegin.secsTo(end) >= 0) && (checkEnd.secsTo(end) <= 0)))
            {
                addFlag = false;

                if (((checkBegin.secsTo(begin) >= 0) && (checkEnd.secsTo(begin) <= 0))// only first one is between
                    && !((checkBegin.secsTo(end) >= 0) && (checkEnd.secsTo(end) <= 0)))
                        matrix[i].at(1) = end;
                else if (!((checkBegin.secsTo(begin) >= 0) && (checkEnd.secsTo(begin) <= 0))// only last one is between
                         && ((checkBegin.secsTo(end) >= 0) && (checkEnd.secsTo(end) <= 0)))
                        matrix[i].at(0) = begin;
            }
        }

        if (addFlag)
        {
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
        insertActivityHistory(&currQuery);
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
