#include "importtool.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QFileInfo>

ImportTool::ImportTool(QObject *parent, const QString &filePath) : QObject(parent)
{
    qDebug() << "ImportTool::ImportTool";
    fileInfo.setFile(filePath);
    qDebug() << fileInfo.path();
}

ImportTool::~ImportTool()
{

}

void ImportTool::read()
{
    QFile importFile(fileInfo.path());

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
    QList<QList<QString>> parsedList;
    for (auto line : lines)
    {
        QStringList temp = line.split(',');

        for (auto out : temp)
            qDebug() << out;
    }
}


