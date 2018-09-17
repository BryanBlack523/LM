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

    QString getName() const;
    QDateTime getBeginDate() const;
    qint64 getElapsedTime() const;


private:
    bool timeIsValid() const;

    QString m_name;
    QDateTime beginDate;
    QElapsedTimer timer;

};

#endif // ACTIVITY_H
