#pragma once

#include "menuwindow.h"
#include "activitylistmodel.h"
#include "activity.h"
#include "database.h"
#include <QMainWindow>
#include <QStandardItemModel>
#include <QList>
#include <QTimer>
#include <QKeyEvent>
#include <QSqlQueryModel>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void resizeEvent(QResizeEvent *event);

private slots:

    void on_tableActivityClicked(const QModelIndex &);
    void on_listActivityClicked(const QModelIndex &);

    void updateTime();
    void initTable();
    void archive();
    void keyPressEvent(QKeyEvent *key);
    void saveActivity(const QModelIndex &index, int, int);

    void on_TimeButton_clicked();

    void on_PlansButton_clicked();

    void on_FinancesButton_clicked();

    void on_PresentButton_2_clicked();

    void on_GraphsButton_2_clicked();

    void on_HistoryButton_2_clicked();

private:
    Ui::MainWindow *ui;
    MenuWindow *menu;

    QStandardItemModel *tableModel;
    QStandardItemModel *frequencyModel;
    ActivityListModel *listModel;

    QTimer *timer_1s;
    QTimer *timer_1h;
    QTimer *timer_1d;

    QTime midnightTime;

    void deleteActivity(const QString&);
    void addActivity(const QString&);

    void fillTable(int rows, int items);
    void fillFrequencyTable(const QMap<QString, int>*);

    int m_tableCollumnsCount = 4;
    DataBase db;
};
