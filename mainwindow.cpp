#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "activitylistdelegate.h"
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

    fillTable();

    listModel = new QStandardItemModel(this);

    int x = this->height();

    ActivityListDelegate *del = new ActivityListDelegate(this);
    ui->currentActivitiesListView->setItemDelegate(del);

    connect(ui->activitiesTableView, SIGNAL(pressed(const QModelIndex&)), this, SLOT(on_tableActivityClicked(const QModelIndex&)));
    connect(ui->currentActivitiesListView, SIGNAL(pressed(const QModelIndex&)), this, SLOT(on_listActivityClicked(const QModelIndex&)));

}

void MainWindow::on_tableActivityClicked(const QModelIndex &tableIndex)
{
    QString cellText = ui->activitiesTableView->model()->data(tableIndex).toString();

    if (!hasActivity(cellText))//if not pressed - add
    {
        addActivity(cellText);
    }
    else//else delete
    {
        deleteActivity(cellText);
    }
}

void MainWindow::on_listActivityClicked(const QModelIndex &listIndex)
{
    QString cellText = ui->currentActivitiesListView->model()->data(listIndex).toString();

    deleteActivity(cellText);
}



bool MainWindow::hasActivity(const QString &name) const
{
    for (int i = 0; i < activities.size(); ++i)
    {
        if (activities[i].getName() == name)
            return true;
    }
    return false;
}

void MainWindow::deleteActivity(const QString &cellText)
{
    int rowCount = listModel->rowCount();

    QRegularExpression reg("^" + cellText + "$");//exactly the right word

    for (int i = 0; i < rowCount; ++i)
    {
        QModelIndex listIndex = listModel->index(i, 0, QModelIndex());

        QRegularExpressionMatch match = reg.match(listModel->itemData(listIndex).value(0).toString());

//            qDebug() << i << " " << listModel->itemData(listIndex).value(0).toString().contains(cellText) << " Data:" << listModel->itemData(listIndex).value(0);

        if(match.hasMatch())
        {
//                qDebug() << "Deleting value:\t" << cellText << " at x: " << listIndex.row() << " L|C " << listModel->rowCount(QModelIndex()) << "|" << activities.size();
            listModel->removeRows(i, 1, QModelIndex());

            for (int i = 0; i < activities.size(); ++i)
            {
                if (activities[i].getName() == cellText)
                    activities.removeAt(i);
            }
        }
    }
}

void MainWindow::addActivity(const QString &cellText)
{
    listModel->appendRow(new QStandardItem(QString(cellText)));

    activities.append(Activity(cellText));

//        qDebug() << "Added value:\t" << cellText << " at x: " << listIndex.row() << " L|C " << rowCount << "|" << activities.size();

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

    qDebug() << "\nItems in CONTAINER:";
    for (int i =0; i < activities.size(); ++i)
    {
        qDebug() << activities[i].getName();
    }
}
