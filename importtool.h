#ifndef IMPORTTOOL_H
#define IMPORTTOOL_H

#include <QObject>
#include <QFile>
#include <QFileInfo>

class ImportTool : public QObject
{
    Q_OBJECT
public:
    explicit ImportTool(QObject *parent = nullptr, const QString &filePath = QString());
    ~ImportTool();

    void read();
    void parse();

private:
    QFileInfo fileInfo;
    QList<QString> lines;
signals:

public slots:
};

#endif // IMPORTTOOL_H
