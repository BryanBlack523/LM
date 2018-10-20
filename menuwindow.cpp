#include "menuwindow.h"
#include "ui_menuwindow.h"
#include "importtool.h"
#include <QFileDialog>
#include <QStandardPaths>
#include <QDebug>
#include <QDateTime>

MenuWindow::MenuWindow(QWidget *parent, const QString &dbPath) :
    QDialog(parent),
    ui(new Ui::MenuWindow)
{
    ui->setupUi(this);

    db.connect(dbPath);
    db.initDB();
}

MenuWindow::~MenuWindow()
{
    delete ui;
}

void MenuWindow::on_importButton_clicked()
{
    QFile importFile(QFileDialog::getOpenFileName(this,
                                                  tr("Open file"),
                                                  "",
                                                  tr("Files (*.txt, *csv)")));
    QFileInfo inf(importFile);
//    qDebug() << inf.filePath();

    ImportTool tool(this, inf.filePath());

//    progressDialog = new ImportProgressDialog(this);
//    progressDialog->show();


    tool.read();
    tool.parse();
    tool.convertDate();

    for (int i = 0; i < tool.getSize(); ++i)
    {
        QList<QString> entry = tool.getEntry(i);
        int id = db.findId(entry[2]);
//        qDebug() << i << " raw: " << entry[2] << " " << ">>" << id << entry[0] << " " << entry[1];
        db.insertActivity(id, QDateTime::fromString(entry[0], "yyyy-MM-dd HH:mm:ss.zzz"), QDateTime::fromString(entry[1], "yyyy-MM-dd HH:mm:ss.zzz"), DailySchedule);
    }
    qDebug() << "Import complete";

}
