#ifndef MENUWINDOW_H
#define MENUWINDOW_H

#include <database.h>
#include <QDialog>

namespace Ui {
class MenuWindow;
}

class MenuWindow : public QDialog
{
    Q_OBJECT

public:
    explicit MenuWindow(QWidget *parent = nullptr, DataBase *database = nullptr);
    ~MenuWindow();

private slots:
    void on_importButton_clicked();

private:
    Ui::MenuWindow *ui;
    DataBase *db;
};

#endif // MENUWINDOW_H
