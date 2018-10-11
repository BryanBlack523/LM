#include "menuwindow.h"
#include "ui_menuwindow.h"
#include "importtool.h"
#include <QFileDialog>
#include <QStandardPaths>
#include <QDebug>

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
    qDebug() << inf.path();

    ImportTool tool(this, inf.path());
    tool.read();
    tool.parse();

}
