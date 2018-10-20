#include "importprogressdialog.h"
#include "ui_importprogressdialog.h"

ImportProgressDialog::ImportProgressDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImportProgressDialog)
{
    ui->setupUi(this);
}

ImportProgressDialog::~ImportProgressDialog()
{
    delete ui;
}
