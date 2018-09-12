#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStandardItemModel>
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

    mockTable();

    listModel = new QStandardItemModel(this);

    connect(ui->activitiesTableView, SIGNAL(pressed(const QModelIndex&)), this, SLOT(on_activityClicked(const QModelIndex&)));

}

void MainWindow::on_activityClicked(const QModelIndex &tableIndex)
{
    QString cellText = ui->activitiesTableView->model()->data(tableIndex).toString();

    qDebug() << cellText;
    if (!checkActivityExist(cellText))//if not pressed - add
    {
        listModel->appendRow(new QStandardItem(QString(cellText)));

        int rowCount = listModel->rowCount(QModelIndex());

        QModelIndex listIndex = listModel->index(rowCount - 1, 0, QModelIndex());

        activities.append(Activity(cellText));

        qDebug() << "Added value:\t" << cellText << " at x: " << listIndex.row() << " L|C " << rowCount << "|" << activities.size();

        ui->currentActivitiesListView->setModel(listModel);
    }
    else//else delete
    {
        int rowCount = listModel->rowCount(QModelIndex());
        qDebug() << "rows " << rowCount;
        for (int i = 0; i < rowCount; ++i)
        {
            QModelIndex listIndex = listModel->index(i, 0, QModelIndex());

            qDebug() << i << " " << listModel->itemData(listIndex).value(0).toString().contains(cellText) << " Data:" << listModel->itemData(listIndex).value(0);

            if(listModel->itemData(listIndex).value(0).toString().contains(cellText))
            {
                qDebug() << "Deleting value:\t" << cellText << " at x: " << listIndex.row() << " L|C " << listModel->rowCount(QModelIndex()) << "|" << activities.size();
                listModel->removeRows(i, 1, QModelIndex());

                for (int i = 0; i < activities.size(); ++i)
                {
                    if (activities[i].getName() == cellText)
                        activities.removeAt(i);
                }
            }
        }
    }
}

bool MainWindow::checkActivityExist(const QString &name)
{
    for (int i = 0; i < activities.size(); ++i)
    {
        if (activities[i].getName() == name)
            return true;
    }
    return false;
}

QModelIndex MainWindow::getIdxActivityFromContainer(const QString &name,const QModelIndex &tableIdx)
{
    for (int i = 0; i < activities.size(); ++i)
    {
        if (activities[i].getName() == name && activities[i].getTableIndex() == tableIdx)
            return activities[i].getListIndex();// if exist return one
        else                                    //else throw error message
            if (i == activities.size() - 1)
            {
                QErrorMessage msg(this);

                msg.showMessage("Could not find Activity " + name + " in CurrentActivitiesList");
            }

    }
}

void MainWindow::mockTable()
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

void MainWindow::on_pushButton_clicked()
{
    qDebug() << "Here's what i got:\n";
    qDebug() << "Items in LIST:";
    int rowCount = listModel->rowCount(QModelIndex());

    for (int i = 0; i < rowCount; ++i)
    {
        QModelIndex listIndex = listModel->index(i, 0, QModelIndex());
        qDebug() << "List : " << listIndex.row() << ": " << listModel->itemData(listIndex);
    }

    qDebug() << "\nItems in CONTAINER:";
    for (int i =0; i < activities.size(); ++i)
    {
        qDebug() << activities[i].getName() << " list: " << activities[i].getListIndex().row() << " list col: " << activities[i].getListIndex().column() << " table x:" << activities[i].getTableIndex().row() << " y:" << activities[i].getTableIndex().column();
    }
}
