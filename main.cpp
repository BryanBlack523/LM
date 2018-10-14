#include "mainwindow.h"
#include <QApplication>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setOrganizationDomain("IB");
    a.setApplicationName("LM");

    auto path = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    if (path.isEmpty())
        qFatal("Cannot determine data storage location");

    QDir d{path};
    if (d.mkpath(d.absolutePath()))
    {
      qDebug() << path << " created";
//      QFile f{"settings.txt"};
//      if (f.open(QIODevice::WriteOnly | QIODevice::Truncate))
//        f.write("Hello, World");
    }

    MainWindow w;
    w.show();

    return a.exec();
}
