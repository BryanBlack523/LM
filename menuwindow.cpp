#include "menuwindow.h"
#include "ui_menuwindow.h"
#include "importtool.h"
#include <QFileDialog>
#include <QStandardPaths>
#include <QDebug>
#include <QDateTime>

MenuWindow::MenuWindow(QWidget *parent, DataBase *database) :
    QDialog(parent),
    ui(new Ui::MenuWindow)
{
    ui->setupUi(this);

    db = database;
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
    qDebug() << inf.filePath();

    ImportTool tool(this, inf.filePath());
    tool.read();
    tool.parse();
    tool.convertDate();

    for (int i = 0; i < tool.getSize(); ++i)
    {
        QList<QString> entry = tool.getEntry(i);
        int id = db->findId(entry[2]);
        db->insertActivity(id, QDateTime::fromString(entry[0], "yyyy-MM-dd HH:mm:ss.zzz"), QDateTime::fromString(entry[1], "yyyy-MM-dd HH:mm:ss.zzz"), DailySchedule);
    }

}
