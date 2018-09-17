#include "activity.h"
#include <QDebug>



Activity::Activity(const QString &name)
{
    m_name = name;
    timer.start();
    beginDate = QDateTime::currentDateTime();
}

Activity::~Activity()
{
    if (timeIsValid())
        qDebug() << "Activity: " << m_name << " started " << beginDate.toString("yyyy-MM-dd HH:mm:ss.zzz") << " elapsed " << timer.elapsed();
    else
        qDebug() << "Activity is not valid: " << m_name << " started " << beginDate.toString("yyyy-MM-dd HH:mm:ss.zzz") << " elapsed " << timer.elapsed();
}

QString Activity::getName() const
{
    return m_name;
}

QDateTime Activity::getBeginDate() const
{
    return beginDate;
}

qint64 Activity::getElapsedTime() const
{
    return timer.elapsed();
}

bool Activity::timeIsValid() const { return timer.elapsed() >= 30000; } //30s
