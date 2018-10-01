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

    this->setWindowTitle("LM");

    ui->mwStackedWidget->setCurrentIndex(0);////open timePage

    connect(&db, SIGNAL(opened()), &db, SLOT(initDB()));
    db.connect();

    initTable();

    listModel = new ActivityListModel(this);

    ActivityListDelegate *del = new ActivityListDelegate(this);
    ui->currentActivitiesListView->setItemDelegate(del);
    ui->currentActivitiesListView->setModel(listModel);

    timer_1s = new QTimer(this);
    timer_1h = new QTimer(this);

    connect(listModel, SIGNAL(rowsAboutToBeRemoved(const QModelIndex&, int, int)), this, SLOT(saveActivity(const QModelIndex &, int, int)));
    connect(ui->activitiesTableView, SIGNAL(pressed(const QModelIndex&)), this, SLOT(on_tableActivityClicked(const QModelIndex&)));
    connect(ui->currentActivitiesListView, SIGNAL(pressed(const QModelIndex&)), this, SLOT(on_listActivityClicked(const QModelIndex&)));
    connect(timer_1s, SIGNAL(timeout()), this, SLOT(updateTime()));//emit dataChange to refresh elapsed time in real time, 1s period

    timer_1s->start(1000);//1s

    connect(timer_1h, SIGNAL(timeout()), this, SLOT(initTable()));//refill table after 1h (to keep up with changes)

    timer_1h->start(360000);//1h

}

MainWindow::~MainWindow()
{
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
        qDebug() << "MainWindow::deleteActivity\t\tFailed to get Idx of " << cellText;
    else
    {
        row = row.toInt() - 1;// why? check ActivityListModel::getIdx()
        listModel->removeRows(row.toInt(), 1, QModelIndex());
    }
}

void MainWindow::keyPressEvent(QKeyEvent* key)
{
    if ( (key->key()== Qt::Key_F1))
    {
        menu = new MenuWindow(this);
        menu->show();
    }
}

void MainWindow::saveActivity(const QModelIndex &index, int first, int count)
{
    for(int i = 0; i <= count; ++i)
    {
        QModelIndex idx = listModel->index(first + i, 0, index);

        if(listModel->data(idx, ActivityListModel::ElapsedTime) >= 30000 || listModel->data(idx, ActivityListModel::Name) == "None")
        {
            QVariantList data;

            data.append(listModel->data(idx, ActivityListModel::Name));

            QDateTime beginDate = listModel->data(idx, ActivityListModel::BeginDate).toDateTime();
            QDateTime endDate = beginDate.addMSecs(listModel->data(idx, ActivityListModel::ElapsedTime).toLongLong());

            data.append(beginDate);
            data.append(endDate);

            db.insertActivity(data);
        }
    }

}

//-------------------------- fill models

void MainWindow::fillTable(int rows, int items)
{
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
                tableModel->itemFromIndex(tableModel->index(i, j))->setFlags(Qt::NoItemFlags);//set rest of the cells not active for user
        }
}

void MainWindow::fillFrequencyTable(const QMap<QString, int> *frequencyMap)
{
    frequencyModel = new QStandardItemModel(frequencyMap->size(),2,this);

    int row = 0;

    for (auto key : frequencyMap->keys())
    {
        frequencyModel->setData (frequencyModel->index(row,0), key);
        frequencyModel->setData (frequencyModel->index(row,1), frequencyMap->value(key));

        ++row;
    }
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
            qDebug() << "MainWindow::on_tableActivityClicked\t\t" << cellText << " added";
        }
        else//else delete
        {
            deleteActivity(cellText);
            qDebug() << "MainWindow::on_tableActivityClicked\t\t" << cellText << " deleted";
        }
    }
}

void MainWindow::on_listActivityClicked(const QModelIndex &listIndex)
{
    if (!listIndex.isValid())
        qDebug() << "MainWindow::on_listActivityClicked\t\tindex is not valid";

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
        qDebug() << "MainWindow::updateTime\t\tcalled when not on present time page";
}

void MainWindow::initTable()
{
    fillFrequencyTable(db.getFrequency());

    frequencyModel->sort(1,  Qt::DescendingOrder);

    ui->debugTableView->setModel(frequencyModel);

    int items = frequencyModel->rowCount();

    int rows = items / m_tableCollumnsCount + ((items % m_tableCollumnsCount) ? 1 : 0); // add whole number + 1 if there is something left

    tableModel = new QStandardItemModel(rows, m_tableCollumnsCount, this);

    fillTable(rows, items);

    ui->activitiesTableView->setModel(tableModel);

    //resize right after getting values
    QResizeEvent* resizeEvent = new QResizeEvent(this->size(), this->size());
    QCoreApplication::postEvent(this, resizeEvent);
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
