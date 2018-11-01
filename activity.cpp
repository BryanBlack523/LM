#include "activity.h"

#include <QDebug>

Activity::Activity(const QString &name)
  : name(name)
  , beginDate(QDateTime::currentDateTime())
{}

Activity::~Activity()
{
    if (timeIsValid())
        qDebug() << "Activity: " << name << " started " << beginDate.toString("yyyy-MM-dd HH:mm:ss.zzz") << " elapsed " << elapsedTime();
    else
        qDebug() << "Activity is not valid: " << name << " started " << beginDate.toString("yyyy-MM-dd HH:mm:ss.zzz") << " elapsed " << elapsedTime();
}

qint64 Activity::elapsedTime() const
{
    return beginDate.msecsTo(QDateTime::currentDateTime());
}

bool Activity::timeIsValid() const
{
    return elapsedTime() >= 30000;  //30s
}
