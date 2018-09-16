#include "activity.h"
#include <QDebug>



Activity::Activity(const QString &name)
{
    m_name = name;
    timer.start();
    QDateTime date;
    beginDate = date.currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz");
}

Activity::~Activity()
{
    if (timeIsValid())
        qDebug() << "Activity: " << m_name << " started " << beginDate << " elapsed " << timer.elapsed();
    else
        qDebug() << "Activity is not valid: " << m_name << " started " << beginDate << " elapsed " << timer.elapsed();
}

QString Activity::getName()
{
    return m_name;
}

QDateTime Activity::getBeginDate()
{
    return QDateTime::fromString(beginDate, "yyyy-MM-dd HH:mm:ss.zzz");
}

qint64 Activity::getElapsedTime()
{
    return timer.elapsed();
}

bool Activity::timeIsValid()
{
    if (timer.elapsed() < 30000) //30s
        return false;
    else
        return true;
}
