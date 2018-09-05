#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStandardItemModel>
#include <QTableView>
#include <QStandardItem>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->mwStackedWidget->setCurrentIndex(0);

    QStandardItemModel *model = new QStandardItemModel(2,3,this);
//    QTableView activitiesTableView;
    model->setHorizontalHeaderItem(0, new QStandardItem(QString("Column 1")));
    model->setHorizontalHeaderItem(1, new QStandardItem(QString("Column 2")));
    model->setHorizontalHeaderItem(2, new QStandardItem(QString("Column 3")));

    ui->activitiesTableView->setModel(model);
    ui->activitiesTableView->show();

    connect(ui->activitiesTableView, SIGNAL(cellClicked(int, int)), this, SLOT(activityClicked(int, int)));

}

void MainWindow::activityClicked(int row, int col)
{
    ui->activitiesTableView->setGridStyle(Qt::DashDotLine);

    ui->actionNameLabel->setText("AAAAAAAAAAAAAAA");

//    QStandardItem item;
//    item.setData(QColor(Qt::green), Qt::BackgroundRole);
//    item.setData(QColor(Qt::red), Qt::FontRole);
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
