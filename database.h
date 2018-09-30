#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>

#define DictActivity    "DictActivity"
#define HistorySchedule    "HistorySchedule"

class DataBase : public QObject
{
    Q_OBJECT
public:
    explicit DataBase(QObject *parent = nullptr);
    ~DataBase();

    bool saveActivity(const QVariantList &data);
    void connect();
    const QMap<QString, int> *getFrequency();

private:
    QSqlDatabase db;
    QString dbPath = "G:/Projects/LM/db/LMtest.db";

    bool open();
    void close();


signals:

public slots:
};

#endif // DATABASE_H
