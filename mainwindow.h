#pragma once

#include "activitylistmodel.h"
#include "activity.h"
#include <QMainWindow>
#include <QStandardItemModel>
#include <QList>
#include <QTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_tableActivityClicked(const QModelIndex &);
    void on_listActivityClicked(const QModelIndex &);

    void updateTime();

    void on_TimeButton_clicked();

    void on_PlansButton_clicked();

    void on_FinancesButton_clicked();

    void on_PresentButton_2_clicked();

    void on_GraphsButton_2_clicked();

    void on_HistoryButton_2_clicked();

    void on_debugButton_clicked();

private:
    Ui::MainWindow *ui;

    QStandardItemModel *tableModel;
    ActivityListModel *listModel;

    QTimer *timer_1s;

    bool hasActivity(const QString&) const;

    void deleteActivity(const QString&);
    void addActivity(const QString&);

    void fillTable();
};
