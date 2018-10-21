#pragma once

#include <database.h>

#include <memory>
#include <QDialog>

#include <importprogressdialog.h>


namespace Ui {
class MenuWindow;
}

class DataBase;

class MenuWindow
  : public QDialog
{
    Q_OBJECT

public:
    explicit MenuWindow(std::shared_ptr<DataBase> m_db, QWidget *parent = nullptr);
    ~MenuWindow();

private slots:
    void importButtonClicked();

    void exportButtonClicked();

private:

    Ui::MenuWindow *m_ui;
    ImportProgressDialog *progressDialog;
    std::shared_ptr<DataBase> m_db;
};

