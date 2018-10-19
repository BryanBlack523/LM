#pragma once

#include <QDateTime>
#include <QElapsedTimer>

class Activity
{
public:
    explicit Activity(const QString &name);
    ~Activity();

    const QString name;
    const QDateTime beginDate;
    qint64 elapsedTime() const;

private:
    bool timeIsValid() const;

    QElapsedTimer timer;

};
