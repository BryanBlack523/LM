#pragma once

#include <memory>

#include <QMainWindow>

class QTimer;
class QStandardItemModel;

namespace Ui {
  class MainWindow;
}

class MenuWindow;
class ActivityListModel;
class DataBase;

class MainWindow
  : public QMainWindow
{
    Q_OBJECT

public:

    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void resizeEvent(QResizeEvent *event);

private slots:

    void tableActivityClicked(const QModelIndex &);
    void listActivityClicked(const QModelIndex &);

    void archive();
    void initTable();
    void keyPressEvent(QKeyEvent *key);
    void saveActivity(const QModelIndex &index, int, int);
    void updateTime();

    void financesButtonClicked();
    void graphsButtonClicked();
    void historyButtonClicked();
    void plansButtonClicked();
    void presentButtonClicked();
    void timeButtonClicked();

private:

    void deleteActivity(const QString&);
    void addActivity(const QString&);

    QString dbPath();

    void fillTable(int rows, int items);
    void fillFrequencyTable(const QMap<QString, int>&);

    Ui::MainWindow *m_ui;
    MenuWindow *m_menu;

    QStandardItemModel *m_tableModel;
    QStandardItemModel *m_frequencyModel;
    ActivityListModel *m_listModel;

    QTimer *m_timerOneS;
    QTimer *m_timerOneHour;
    QTimer *m_timerOneDay;

    int m_tableCollumnsCount = 4;
    std::shared_ptr<DataBase> m_db;
};
