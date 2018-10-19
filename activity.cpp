#include "activity.h"

#include <QDebug>

Activity::Activity(const QString &name)
  : name(name)
  , beginDate(QDateTime::currentDateTime())
{
    timer.start();
}

Activity::~Activity()
{
    if (timeIsValid())
        qDebug() << "Activity: " << name << " started " << beginDate.toString("yyyy-MM-dd HH:mm:ss.zzz") << " elapsed " << timer.elapsed();
    else
        qDebug() << "Activity is not valid: " << name << " started " << beginDate.toString("yyyy-MM-dd HH:mm:ss.zzz") << " elapsed " << timer.elapsed();
}

qint64 Activity::elapsedTime() const
{
    return timer.elapsed();
}

bool Activity::timeIsValid() const
{
    return timer.elapsed() >= 30000;  //30s
}
