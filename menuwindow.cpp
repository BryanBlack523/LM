#include "menuwindow.h"
#include "ui_menuwindow.h"

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
