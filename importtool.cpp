#include "importtool.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QFileInfo>
#include <QDateTime>

ImportTool::ImportTool(QObject *parent, const QString &filePath) : QObject(parent)
{
    qDebug() << "ImportTool::ImportTool";
    fileInfo.setFile(filePath);
    qDebug() << fileInfo.filePath();
}

ImportTool::~ImportTool()
{

}

void ImportTool::read()
{
    QFile importFile(fileInfo.filePath());

    if (importFile.open(QIODevice::ReadOnly))
    {
       QTextStream in(&importFile);
       while (!in.atEnd())
       {
           lines.append(in.readLine());
       }
       importFile.close();
    }
    else
        qDebug() << "could not open file";
}

void ImportTool::parse()
{
    for (auto line : lines)
    {
        QStringList temp = line.split(',');

        if (temp[0] != "RECORD ID")
        {
            QList<QString> entry;

            entry.append(temp[1]);//start dt
            entry.append(temp[3]);//duration
            entry.append(temp[5]);//activity name

//            qDebug() << ":" << entry.at(0) << " " << entry.at(1) << " " << entry.at(2);

            parsedList.append(entry);
        }
    }
}

void ImportTool::convertDate()
{
    for (int i = 0; i < parsedList.size(); ++i)
    {
        QString sDate = parsedList[i].at(0).left(11);
        QString sTime = parsedList[i].at(0).right(5);

        QLocale curLocale(QLocale("en_EN"));
        QLocale::setDefault(curLocale);

        QDate Date = QLocale().toDate(sDate, "dd MMM yyyy");//QDate::fromString(sDate, "dd MMM yyyy");
        QTime Time = QTime::fromString(sTime, "HH:mm");
//        qDebug() << parsedList[i].at(0) << sDate << " " << sTime << " vs " << Date << " " << Time;

        QDateTime stamp;
        stamp.setDate(Date);
        stamp.setTime(Time);

        QTime duration = QTime::fromString(parsedList[i].at(1), "HH:mm:ss");

        qint64 secs = (duration.hour()*60 + duration.minute())*60 + duration.second();

        parsedList[i].replace(0, stamp.toString("yyyy-MM-dd HH:mm:ss.zzz"));
        parsedList[i].replace(1, stamp.addSecs(secs).toString("yyyy-MM-dd HH:mm:ss.zzz"));
    }
}

QList<QString> ImportTool::getEntry(int row)
{
    return parsedList[row];
}

int ImportTool::getSize()
{
    return parsedList.size();
}


