#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "activitylistdelegate.h"
#include <QTableView>
#include <QStandardItem>
#include <QString>
#include <QErrorMessage>
#include <QDebug>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->mwStackedWidget->setCurrentIndex(0);////open timePage

    fillFrequencyTable();

    frequencyModel->sort(1, Qt::DescendingOrder);

    fillTable();

    listModel = new ActivityListModel(this);

//    int x = this->height();

    ActivityListDelegate *del = new ActivityListDelegate(this);
    ui->currentActivitiesListView->setItemDelegate(del);

    timer_1s = new QTimer(this);

    connect(ui->activitiesTableView, SIGNAL(pressed(const QModelIndex&)), this, SLOT(on_tableActivityClicked(const QModelIndex&)));
    connect(ui->currentActivitiesListView, SIGNAL(pressed(const QModelIndex&)), this, SLOT(on_listActivityClicked(const QModelIndex&)));
    connect(timer_1s, SIGNAL(timeout()), this, SLOT(updateTime()));//emit dataChange to refresh elapsed time in real time, 1s period

    timer_1s->start(1000);


}

void MainWindow::on_tableActivityClicked(const QModelIndex &tableIndex)
{
    QString cellText = ui->activitiesTableView->model()->data(tableIndex).toString();

    if (!hasActivity(cellText))//if not pressed - add
    {
        addActivity(cellText);
        qDebug() << cellText << " added";
    }
    else//else delete
    {
        deleteActivity(cellText);
        qDebug() << cellText << " deleted";
    }
}

void MainWindow::on_listActivityClicked(const QModelIndex &listIndex)
{
    if (!listIndex.isValid())
    {
        qDebug() << "index is not valid";
    }

    QString cellText = listModel->data(listIndex, ActivityListModel::Name).toString();

    deleteActivity(cellText);
}

void MainWindow::updateTime()
{
    if (ui->mwStackedWidget->currentIndex() == 0 && ui->timeStackedWidget->currentIndex() == 0)
    {
        int rowCount = listModel->rowCount(QModelIndex());

        listModel->dataChanged(listModel->index(0, 0, QModelIndex()), listModel->index(rowCount, 0, QModelIndex()));
    }
    else
        qDebug() << "mainwindow::updatetime called when not on present time page";
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
    int items = frequencyModel->rowCount();
    int collumns = 4;

    int rows = items / collumns + ((items % collumns) ? 1 : 0);

    tableModel = new QStandardItemModel(rows, collumns, this);

    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < collumns; ++j)
        {
            int filledCount = i * 4 + j;

            if (filledCount < items)
            {
                QString name = frequencyModel->data(frequencyModel->index(filledCount, 0)).toString();

                tableModel->setData(tableModel->index(i, j), name);
            }
        }
    }

    ui->activitiesTableView->setModel(tableModel);
}

void MainWindow::fillFrequencyTable()
{
    frequencyModel = new QStandardItemModel(6,2,this);

    frequencyModel->setData (frequencyModel->index(0,0), "cat");
    frequencyModel->setData (frequencyModel->index(0,1), "56");

    frequencyModel->setData (frequencyModel->index(1,0), "dog");
    frequencyModel->setData (frequencyModel->index(1,1), "23");

    frequencyModel->setData (frequencyModel->index(2,0), "par");
    frequencyModel->setData (frequencyModel->index(2,1), "41");

    frequencyModel->setData (frequencyModel->index(3,0), "cat2");
    frequencyModel->setData (frequencyModel->index(3,1), "15");

    frequencyModel->setData (frequencyModel->index(4,0), "dog2");
    frequencyModel->setData (frequencyModel->index(4,1), "18");

    frequencyModel->setData (frequencyModel->index(5,0), "par2");
    frequencyModel->setData (frequencyModel->index(5,1), "1");

    ui->debugTableView->setModel(frequencyModel);
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
    qDebug() << "Items in LIST:";
    int rowCount = listModel->rowCount(QModelIndex());

    for (int i = 0; i < rowCount; ++i)
    {
        QModelIndex listIndex = listModel->index(i, 0, QModelIndex());
        qDebug() << "Activity started : " << listIndex.row() << ": " << listModel->data(listIndex, ActivityListModel::BeginDate)
                                                             << " " << listModel->data(listIndex, ActivityListModel::Name)
                                                             << " " << listModel->data(listIndex, ActivityListModel::ElapsedTime);
    }
}
