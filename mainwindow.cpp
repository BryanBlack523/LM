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

    m_tableCollumnsCount = 4;
    ui->activitiesTableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    ui->mwStackedWidget->setCurrentIndex(0);////open timePage

    db.connect();

    fillFrequencyTable(db.getFrequency());

    frequencyModel->sort(1, Qt::DescendingOrder);

    fillTable();

    listModel = new ActivityListModel(this);

    ActivityListDelegate *del = new ActivityListDelegate(this);
    ui->currentActivitiesListView->setItemDelegate(del);

    timer_1s = new QTimer(this);
    timer_1h = new QTimer(this);

    connect(ui->activitiesTableView, SIGNAL(pressed(const QModelIndex&)), this, SLOT(on_tableActivityClicked(const QModelIndex&)));
    connect(ui->currentActivitiesListView, SIGNAL(pressed(const QModelIndex&)), this, SLOT(on_listActivityClicked(const QModelIndex&)));
    connect(timer_1s, SIGNAL(timeout()), this, SLOT(updateTime()));//emit dataChange to refresh elapsed time in real time, 1s period

    timer_1s->start(1000);//1s

    connect(timer_1h, SIGNAL(timeout()), this, SLOT(updateTable()));//refill table after 1h (to keep up with changes)

    timer_1h->start(10000);//1h 360000
}

MainWindow::~MainWindow()
{
    int rows = listModel->rowCount();

    for (int i = 0; i < rows; ++i)
    {
        QVariantList list;

//        db.saveActivity()
    }

    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    ui->activitiesTableView->horizontalHeader()->resizeSections(QHeaderView::Stretch);
    ui->activitiesTableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QMainWindow::resizeEvent(event);
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

void MainWindow::keyPressEvent(QKeyEvent* key)
{
    qDebug() << "here";
    if ( (key->key()== Qt::Key_F1))
    {
        menu = new MenuWindow(this);
        menu->show();
    }
}

//-------------------------- initialise models

void MainWindow::fillTable()
{
//    qDebug() << "fillTable";
    int items = frequencyModel->rowCount();

    int rows = items / m_tableCollumnsCount + ((items % m_tableCollumnsCount) ? 1 : 0); // add whole number + 1 if there is something left

    tableModel = new QStandardItemModel(rows, m_tableCollumnsCount, this);

    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < m_tableCollumnsCount; ++j)
        {
            int filledCount = i * m_tableCollumnsCount + j;// number of items added

//            qDebug() << i << "/" << j << " filledCount " << filledCount << " item " << frequencyModel->data(frequencyModel->index(filledCount, 0)).toString();
            if (filledCount < items)
            {
                QString name = frequencyModel->data(frequencyModel->index(filledCount, 0)).toString();

                tableModel->setData(tableModel->index(i, j), name);
            }
            else
                tableModel->itemFromIndex(tableModel->index(i, j))->setFlags(Qt::NoItemFlags);//set item not active for user
        }

    ui->activitiesTableView->setModel(tableModel);
}

void MainWindow::fillFrequencyTable(const QMap<QString, int> *frequencyMap)
{
    frequencyModel = new QStandardItemModel(frequencyMap->size(),2,this);

    int iterator = 0;

    for (auto j : frequencyMap->keys())
    {
        frequencyModel->setData (frequencyModel->index(iterator,0), j);
        frequencyModel->setData (frequencyModel->index(iterator,1), frequencyMap->value(j));

        ++iterator;
    }

    ui->debugTableView->setModel(frequencyModel);
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
