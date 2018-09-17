#pragma once

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

    void on_tableActivityClicked(const QModelIndex &);
    void on_listActivityClicked(const QModelIndex &);

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
    QStandardItemModel *listModel;

    QList<Activity> activities;

    bool hasActivity(const QString&) const;

    QModelIndex getIdxActivityFromContainer(const QString&,const QModelIndex&);

    void deleteActivity(const QString&);
    void addActivity(const QString&);

    void fillTable();
};
