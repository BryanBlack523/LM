#include "activity.h"
#include <QDebug>

Activity::Activity(QObject *parent, const QString &name) : QObject(parent), m_name(name), beginDate(QDateTime::currentDateTime())
{
    timer.start();
}

Activity::Activity(const Activity &other) : QObject (other.parent()) {}

Activity &Activity::operator =(const Activity &other)
{
    (void) other;

    return *this;
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
