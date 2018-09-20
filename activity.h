#ifndef ACTIVITY_H
#define ACTIVITY_H

#include <QObject>
#include <QDateTime>
#include <QElapsedTimer>

class Activity : public QObject
{
    Q_OBJECT
public:
    explicit Activity(QObject *parent = nullptr,  const QString &name = QString());
    Activity(const Activity &other);
    Activity& operator =(const Activity &other);
    ~Activity();

    QString getName() const;
    QDateTime getBeginDate() const;
    qint64 getElapsedTime() const;


private:
    bool timeIsValid() const;

    QString m_name;
    QDateTime beginDate;
    QElapsedTimer timer;



signals:

public slots:
};

    Q_DECLARE_METATYPE(Activity)

#endif // ACTIVITY_H
