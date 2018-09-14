#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "activity.h"
#include <QMainWindow>
#include <QStandardItemModel>
#include <QList>

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

    void on_activityClicked(const QModelIndex &);

    void on_TimeButton_clicked();

    void on_PlansButton_clicked();

    void on_FinancesButton_clicked();

    void on_PresentButton_2_clicked();

    void on_GraphsButton_2_clicked();

    void on_HistoryButton_2_clicked();

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;

    QStandardItemModel *tableModel;
    QStandardItemModel *listModel;

    QList<Activity> activities;

    bool checkActivityExist(const QString&);

    QModelIndex getIdxActivityFromContainer(const QString&,const QModelIndex&);

    void deleteActivity (const QString&);

    void mockTable();
};

#endif // MAINWINDOW_H
