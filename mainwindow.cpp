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
//    QTableView activitiesTableView;
    tableModel->setHorizontalHeaderItem(0, new QStandardItem(QString("Column 1")));
    tableModel->setHorizontalHeaderItem(1, new QStandardItem(QString("Column 2")));
    tableModel->setHorizontalHeaderItem(2, new QStandardItem(QString("Column 3")));
    tableModel->setData (tableModel->index(0,0), 523);

    ui->activitiesTableView->setModel(tableModel);
    ui->activitiesTableView->show();

    listModel = new QStandardItemModel(this);

    connect(ui->activitiesTableView, SIGNAL(pressed(const QmodelIndex &)), ui->currentActivitiesListView, SLOT(on_activityClicked(const QModelIndex tableModel.index)));

}

void MainWindow::on_activityClicked(const QModelIndex &index)
{
//    ui->activitiesTableView->setGridStyle(Qt::DashDotLine);

    QString cellText = ui->activitiesTableView->model()->data(index).toString();

    listModel->appendRow(new QStandardItem(QString(cellText)));
    ui->currentActivitiesListView->setModel(listModel);
    ui->currentActivitiesListView->show();
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

void MainWindow::on_activitiesTableView_clicked(const QModelIndex &index)
{

}
