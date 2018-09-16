#ifndef ACTIVITY_H
#define ACTIVITY_H

#include <QString>
#include <QDateTime>
#include <QElapsedTimer>

class Activity
{
public:
    Activity(const QString &);
    ~Activity();

    QString getName();
    QDateTime getBeginDate();
    qint64 getElapsedTime();


private:
    bool timeIsValid();

    QString m_name;
    QString beginDate;
    QElapsedTimer timer;

};

#endif // ACTIVITY_H
