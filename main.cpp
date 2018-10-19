#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QStandardPaths>

#include "mainwindow.h"

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
    QApplication application(argc, argv);

    application.setOrganizationDomain("IB");
    application.setApplicationName("LM");

    createWorkDir();

    MainWindow w;
    w.show();

    return application.exec();
}
