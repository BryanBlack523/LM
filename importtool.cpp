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
    qDebug() << "parse";
    for (auto line : lines)
    {
        QStringList temp = line.split(',');

        if (temp[0] != 1)
        {
            QList<QString> entry;

            entry.append(temp[1]);//start dt
            entry.append(temp[3]);//duration
            entry.append(temp[5]);//activity name

            parsedList.append(entry);
        }
    }
}

void ImportTool::convertDate()
{
    for (int i = 0; i < parsedList.size(); ++i)
    {
        QDateTime stamp = QDateTime::fromString(parsedList[i].at(0), "dd MMM yyyy HH:mm");//28 Apr 2018 20:20
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


