#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_TimeButton_clicked();

    void on_PlansButton_clicked();

    void on_FinancesButton_clicked();

    void on_PresentButton_2_clicked();

    void on_GraphsButton_2_clicked();

    void on_HistoryButton_2_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
