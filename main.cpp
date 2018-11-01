#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QStandardPaths>
#include <QDateTime>

#include "mainwindow.h"

QScopedPointer<QFile>   m_logFile;

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QTextStream out(m_logFile.data());

    out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz ");

    switch (type)
    {
    case QtInfoMsg:     out << "INF "; break;
    case QtDebugMsg:    out << "DBG "; break;
    case QtWarningMsg:  out << "WRN "; break;
    case QtCriticalMsg: out << "CRT "; break;
    case QtFatalMsg:    out << "FTL "; break;
    }
    // Write to the output category of the message and the message itself
    out << context.category << ": "
        << msg << endl;
    out.flush();    // Clear the buffered data
}

void createWorkDir()
{
    auto pathDataLocation = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    if (pathDataLocation.isEmpty())
        qFatal("Cannot determine data storage location");

    QDir workDirectory{pathDataLocation};
    if (workDirectory.mkpath(workDirectory.absolutePath()))
    {
        qDebug() << pathDataLocation << " created";
    }
}

int main(int argc, char *argv[])
{
    m_logFile.reset(new QFile("C:/example/logFile.txt"));
    // Open the file logging
    m_logFile.data()->open(QFile::Append | QFile::Text);

    qInstallMessageHandler(myMessageOutput);

    QApplication application(argc, argv);

    application.setOrganizationDomain("IB");
    application.setApplicationName("LM");

    createWorkDir();

    MainWindow w;
    w.show();

    return application.exec();
}
