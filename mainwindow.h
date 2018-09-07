#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "activity.h"
#include <QMainWindow>
#include <QStandardItemModel>
#include <QVector>

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

private:
    Ui::MainWindow *ui;

    QStandardItemModel *tableModel;
    QStandardItemModel *listModel;

    QVector<Activity> activities;

    bool checkActivityExist(const QString&,const QModelIndex&);

    QModelIndex getIdxActivityFromVector(const QString&,const QModelIndex&);
};

#endif // MAINWINDOW_H
