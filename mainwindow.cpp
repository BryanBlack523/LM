#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "activitiesdelegate.h"
#include <QStandardItemModel>
#include <QTableView>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->mwStackedWidget->setCurrentIndex(0);

    QStandardItemModel model(4, 2);
    QTableView activitiesTableView;
    activitiesTableView.setModel(&model);

    ActivitiesDelegate delegate;
    activitiesTableView.setItemDelegate(&delegate);

    activitiesTableView.horizontalHeader()->setStretchLastSection(true);
    activitiesTableView.show();
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
