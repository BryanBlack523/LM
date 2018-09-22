#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "activitylistdelegate.h"
//#include <QStandardItemModel>
#include <QTableView>
#include <QStandardItem>
#include <QString>
#include <QErrorMessage>
#include <QDebug>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
//    qDebug() << "well, we're here";
    ui->setupUi(this);
    ui->mwStackedWidget->setCurrentIndex(0);////open timePage

    fillTable();
//    qDebug() << "table filled";
//    listModel = new QStandardItemModel(this);
    listModel = new ActivityListModel(this);
//    qDebug() << "model created";

//    int x = this->height();

    ActivityListDelegate *del = new ActivityListDelegate(this);
    ui->currentActivitiesListView->setItemDelegate(del);
//    qDebug() << "delegate set";

    timer_1s = new QTimer(this);

    connect(ui->activitiesTableView, SIGNAL(pressed(const QModelIndex&)), this, SLOT(on_tableActivityClicked(const QModelIndex&)));
    connect(ui->currentActivitiesListView, SIGNAL(pressed(const QModelIndex&)), this, SLOT(on_listActivityClicked(const QModelIndex&)));
    connect(timer_1s, SIGNAL(timeout()), this, SLOT(updateTime()));//emit dataChange to refresh elapsed time in real time, 1s period

    qDebug() << "connections set";

    timer_1s->start(1000);

    qDebug() << "timer started";
}

void MainWindow::on_tableActivityClicked(const QModelIndex &tableIndex)
{
    QString cellText = ui->activitiesTableView->model()->data(tableIndex).toString();

    if (!hasActivity(cellText))//if not pressed - add
    {
//        qDebug() << cellText << " found";
        addActivity(cellText);
        qDebug() << cellText << " added";
    }
    else//else delete
    {
//        qDebug() << cellText << " found";
        deleteActivity(cellText);
        qDebug() << cellText << " deleted";
    }
}

void MainWindow::on_listActivityClicked(const QModelIndex &listIndex)
{
    QString cellText = ui->currentActivitiesListView->model()->data(listIndex).toString();

    deleteActivity(cellText);
}

void MainWindow::updateTime()
{
//    qDebug() << "updateTime";
    int rowCount = listModel->rowCount(QModelIndex());
//    qDebug() << "rowCount is fine";
    listModel->dataChanged(listModel->index(0, 0, QModelIndex()), listModel->index(rowCount, 0, QModelIndex()));
//    qDebug() << "datachanged";
}

bool MainWindow::hasActivity(const QString &name) const
{
    return listModel->find(name);
}

void MainWindow::deleteActivity(const QString &cellText)
{
    int rowCount = listModel->rowCount();

    qDebug() << rowCount << " rows in list";

    QVariant row = listModel->getIdx(cellText);

    if (!row.toBool())
        qDebug() << "Failed to get Idx of " << cellText;
    else
    {
        row = row.toInt() - 1;// why? check ActivityListModel::getIdx()
        listModel->removeRows(row.toInt(), 1, QModelIndex());
    }
}

void MainWindow::addActivity(const QString &cellText)
{
    listModel->appendRow(cellText, QModelIndex());

    ui->currentActivitiesListView->setModel(listModel);
}

void MainWindow::fillTable()
{
    tableModel = new QStandardItemModel(2,3,this);

    tableModel->setHorizontalHeaderItem(0, new QStandardItem(QString("Column 1")));
    tableModel->setHorizontalHeaderItem(1, new QStandardItem(QString("Column 2")));
    tableModel->setHorizontalHeaderItem(2, new QStandardItem(QString("Column 3")));

    tableModel->setData (tableModel->index(0,0), "cat");
    tableModel->setData (tableModel->index(0,1), "dog");
    tableModel->setData (tableModel->index(0,2), "par");
    tableModel->setData (tableModel->index(1,0), "cat2");
    tableModel->setData (tableModel->index(1,1), "dog2");
    tableModel->setData (tableModel->index(1,2), "par2");

    ui->activitiesTableView->setModel(tableModel);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_TimeButton_clicked()
{
    ui->mwStackedWidget->setCurrentIndex(0);//open timePage
}

void MainWindow::on_PlansButton_clicked()
{
    ui->mwStackedWidget->setCurrentIndex(1);//open plansPage
}

void MainWindow::on_FinancesButton_clicked()
{
    ui->mwStackedWidget->setCurrentIndex(2);//open financesPage
}

void MainWindow::on_PresentButton_2_clicked()
{
    ui->timeStackedWidget->setCurrentIndex(0);//open timePage.presentPage
}

void MainWindow::on_HistoryButton_2_clicked()
{
    ui->timeStackedWidget->setCurrentIndex(1);//open timePage.historyPage
}

void MainWindow::on_GraphsButton_2_clicked()
{
    ui->timeStackedWidget->setCurrentIndex(2);//open timePage.graphsPage
}

void MainWindow::on_debugButton_clicked()
{
    qDebug() << "Here's what i got:\n";
    qDebug() << "Items in LIST:";
    int rowCount = listModel->rowCount(QModelIndex());

    for (int i = 0; i < rowCount; ++i)
    {
        QModelIndex listIndex = listModel->index(i, 0, QModelIndex());
        qDebug() << "List : " << listIndex.row() << ": " << listModel->itemData(listIndex);
    }
}
