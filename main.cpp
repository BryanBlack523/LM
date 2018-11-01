#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QStandardPaths>
#include <QDateTime>

#include "mainwindow.h"

QScopedPointer<QFile>   m_logFile;
bool logging = true;

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

QString createLogDir()
{
    auto pathLogLocation = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/LM/";
    if (pathLogLocation.isEmpty())
        qFatal("Cannot determine data storage location");

    QDir logDirectory{pathLogLocation};
    if (logDirectory.mkpath(logDirectory.absolutePath()))
    {
        qDebug() << pathLogLocation << " created";
    }

    return pathLogLocation;
}

void enableLog()
{
    if (logging)
    {
        QString logPath = createLogDir() + "LMlog.txt";
        m_logFile.reset(new QFile(logPath));

        m_logFile.data()->open(QFile::Append | QFile::Text);

        qInstallMessageHandler(myMessageOutput);
    }
}

int main(int argc, char *argv[])
{
    enableLog();

    QApplication application(argc, argv);

    application.setOrganizationDomain("IB");
    application.setApplicationName("LM");

    createWorkDir();

    MainWindow w;
    w.show();

    return application.exec();
}
