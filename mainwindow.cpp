#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStandardItemModel>
#include <QTableView>
#include <QStandardItem>
#include <QString>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->mwStackedWidget->setCurrentIndex(0);

    tableModel = new QStandardItemModel(2,3,this);

    tableModel->setHorizontalHeaderItem(0, new QStandardItem(QString("Column 1")));
    tableModel->setHorizontalHeaderItem(1, new QStandardItem(QString("Column 2")));
    tableModel->setHorizontalHeaderItem(2, new QStandardItem(QString("Column 3")));
    tableModel->setData (tableModel->index(0,0), 523);

    ui->activitiesTableView->setModel(tableModel);

    listModel = new QStandardItemModel(this);

    connect(ui->activitiesTableView, SIGNAL(pressed(const QModelIndex&)), this, SLOT(on_activityClicked(const QModelIndex&)));

}

void MainWindow::on_activityClicked(const QModelIndex &index)
{
    QString cellText = ui->activitiesTableView->model()->data(index).toString();

    if (checkActivityExist(cellText,index))
    {

    }
    else
    {
        listModel->appendRow(new QStandardItem(QString(cellText)));
        QModelIndex listIndex = listModel->indexFromItem(new QStandardItem(QString(cellText)));

        activities.append(Activity(cellText,index,listIndex));

        ui->currentActivitiesListView->setModel(listModel);
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

QModelIndex MainWindow::getIdxActivityFromVector(const QString &name,const QModelIndex &tableIdx)
{
    for (int i = 0; i < activities.size(); ++i)
    {
        if (activities[i].getName() == name && activities[i].getTableIndex() == tableIdx)
            return activities[i].getListIndex();
        else
            if (i = activities.size())

    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_TimeButton_clicked()
{
    ui->mwStackedWidget->setCurrentIndex(0);
}

void MainWindow::on_PlansButton_clicked()
{
    ui->mwStackedWidget->setCurrentIndex(1);
}

void MainWindow::on_FinancesButton_clicked()
{
    ui->mwStackedWidget->setCurrentIndex(2);
}

void MainWindow::on_PresentButton_2_clicked()
{
    ui->timeStackedWidget->setCurrentIndex(0);
}

void MainWindow::on_HistoryButton_2_clicked()
{
    ui->timeStackedWidget->setCurrentIndex(1);
}

void MainWindow::on_GraphsButton_2_clicked()
{
    ui->timeStackedWidget->setCurrentIndex(2);
}
