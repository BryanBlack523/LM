#pragma once

#include <QObject>

class QFileInfo;

class ImportTool : public QObject
{
    Q_OBJECT

public:

    explicit ImportTool(const QString &filePath, QObject *parent = nullptr);
    ~ImportTool();

    QList<QString> getEntry(int row);
    int size();
    void convertDate();
    void parse();
    void read();

private:

    QFileInfo *fileInfo;
    QList<QString> lines;
    QList<QList<QString>> parsedList;

};
