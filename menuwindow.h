#pragma once

#include <QDialog>

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
    void on_importButton_clicked();

private:
    Ui::MenuWindow *m_ui;
    std::shared_ptr<DataBase> m_db;
};

