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

    qDebug() << "here";

    fillFrequencyTable();
    qDebug() << "here";

//    frequencyModel->sort(1, Qt::DescendingOrder);

    fillTable();
    qDebug() << "here";

    listModel = new ActivityListModel(this);

//    int x = this->height();

    ActivityListDelegate *del = new ActivityListDelegate(this);
    ui->currentActivitiesListView->setItemDelegate(del);

    timer_1s = new QTimer(this);
    timer_1h = new QTimer(this);

    connect(ui->activitiesTableView, SIGNAL(pressed(const QModelIndex&)), this, SLOT(on_tableActivityClicked(const QModelIndex&)));
    connect(ui->currentActivitiesListView, SIGNAL(pressed(const QModelIndex&)), this, SLOT(on_listActivityClicked(const QModelIndex&)));
    connect(timer_1s, SIGNAL(timeout()), this, SLOT(updateTime()));//emit dataChange to refresh elapsed time in real time, 1s period

    timer_1s->start(1000);//1s

//    connect(timer_1h, SIGNAL(timeout()), this, SLOT(updateTable()));//refill table after 1h (to keep up with changes)

    timer_1h->start(10000);//1h 360000
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addActivity(const QString &cellText)
{
    listModel->appendRow(cellText, QModelIndex());

    ui->currentActivitiesListView->setModel(listModel);
}

void MainWindow::deleteActivity(const QString &cellText)
{
    QVariant row = listModel->getIdx(cellText);

    if (!row.toBool())
        qDebug() << "Failed to get Idx of " << cellText;
    else
    {
        row = row.toInt() - 1;// why? check ActivityListModel::getIdx()
        listModel->removeRows(row.toInt(), 1, QModelIndex());
    }
}

//-------------------------- initialise models

void MainWindow::fillTable()
{
//    qDebug() << "fillTable";
    int items = frequencyModel->rowCount();
    int collumns = 4;

    int rows = items / collumns + ((items % collumns) ? 1 : 0); // add whole number + 1 if there is something left
    proxyTableModel = new QSortFilterProxyModel();
    qDebug() << "proxy model";
    proxyTableModel->setDynamicSortFilter(true);
    qDebug() << "sort enabled";
    proxyTableModel->setFilterRole(FrequencyModel::frR_ClickCount);
    qDebug() << "filter set";
//    proxyTableModel->setData()

    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < collumns; ++j)
        {
            int filledCount = i * collumns + j;// number of items added

//            qDebug() << i << "/" << j << " filledCount " << filledCount << " item " << frequencyModel->data(frequencyModel->index(filledCount, 0)).toString();
            if (filledCount < items)
            {
                qDebug() << "before name";
                QString name = frequencyModel->data(frequencyModel->index(filledCount, 0)).toString();
                qDebug() << "before setData";
                proxyTableModel->setData(proxyTableModel->index(i, j), name);
            }
//            else
//                proxyTableModel->itemFromIndex(proxyTableModel->index(i, j))->setFlags(Qt::NoItemFlags);//set item not active for user
        }
    }
    qDebug() << "after for";

    ui->activitiesTableView->setModel(proxyTableModel);
}

void MainWindow::fillFrequencyTable()
{    
    frequencyModel->appendRow("cat", 56, QModelIndex());
    frequencyModel->appendRow("dog", 23, QModelIndex());
    frequencyModel->appendRow("par", 41, QModelIndex());
    frequencyModel->appendRow("cat2", 15, QModelIndex());
    frequencyModel->appendRow("dog2", 18, QModelIndex());
    frequencyModel->appendRow("par2", 1, QModelIndex());


    ui->debugListView->setModel(frequencyModel);
}

//--------------------------------SLOTS implementation

void MainWindow::on_tableActivityClicked(const QModelIndex &tableIndex)
{
    QString cellText = ui->activitiesTableView->model()->data(tableIndex).toString();

    if (!cellText.isEmpty())
    {
        if (!listModel->find(cellText))
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
}

void MainWindow::on_listActivityClicked(const QModelIndex &listIndex)
{
    if (!listIndex.isValid())
        qDebug() << "index is not valid";

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

void MainWindow::updateTable()
{
    frequencyModel->sort(1,  Qt::DescendingOrder);

    ui->debugTableView->setModel(frequencyModel);

    fillTable();
}

//---------------------------Push Buttons SLOTS

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
