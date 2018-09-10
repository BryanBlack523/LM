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

    if (!checkActivityExist(cellText,tableIndex))//if not pressed - add
    {
        listModel->appendRow(new QStandardItem(QString(cellText)));

        int rowCount = listModel->rowCount(QModelIndex());

        QModelIndex listIndex = listModel->index(rowCount - 1, 0, QModelIndex());


        qDebug() << "Added value: " << cellText << " at x: " << listIndex.row() << " y: " << listIndex.column();

        activities.append(Activity(cellText,tableIndex,listIndex));

        ui->currentActivitiesListView->setModel(listModel);
    }
    else//else delete
    {
        QModelIndex m_list = getIdxActivityFromContainer(cellText,tableIndex);

        qDebug() << "Deleting value: " << cellText << " at x: " << m_list.row() << " y: " << m_list.column();

        listModel->removeRow(m_list.row());

        for (int i = 0; i < activities.size(); ++i)
        {
            if (activities[i].getName() == cellText && activities[i].getTableIndex() == tableIndex)
                activities.removeAt(i);
        }
    }
}

bool MainWindow::checkActivityExist(const QString &name, const QModelIndex &tableIdx)
{
    for (int i = 0; i < activities.size(); ++i)
    {
        if (activities[i].getName() == name && activities[i].getTableIndex() == tableIdx)
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

    tableModel->setData (tableModel->index(0,0), "1 row, 1 col");
    tableModel->setData (tableModel->index(0,1), "1 row, 2 col");
    tableModel->setData (tableModel->index(0,2), "1 row, 3 col");
    tableModel->setData (tableModel->index(1,0), "2 row, 1 col");
    tableModel->setData (tableModel->index(1,1), "2 row, 2 col");
    tableModel->setData (tableModel->index(1,2), "2 row, 3 col");

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
