#ifndef MENUWINDOW_H
#define MENUWINDOW_H

#include <database.h>
#include <importprogressdialog.h>
#include <QDialog>

namespace Ui {
class MenuWindow;
}

class MenuWindow : public QDialog
{
    Q_OBJECT

public:
    explicit MenuWindow(QWidget *parent = nullptr, const QString &dbPath = QString());
    ~MenuWindow();

private slots:
    void on_importButton_clicked();

private:
    Ui::MenuWindow *ui;
    ImportProgressDialog *progressDialog;
    DataBase db;
};

#endif // MENUWINDOW_H
