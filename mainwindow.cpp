#include "mainwindow.h"

#include <QDebug>

#include <QKeyEvent>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QGestureEvent>
#include <QString>
#include <QTableView>
#include <QTimer>

#include "activity.h"
#include "activitylistdelegate.h"
#include "activitylistmodel.h"
#include "database.h"
#include "menuwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
  , m_ui(new Ui::MainWindow)
  , m_db(new DataBase())
{
    m_ui->setupUi(this);

    setWindowTitle("LM");
    grabGesture(Qt::SwipeGesture);

    m_ui->mwStackedWidget->setCurrentIndex(0);////open timePage

    m_db->open();

    initTable();

    m_listModel = new ActivityListModel(this);

    ActivityListDelegate *del = new ActivityListDelegate(this);
    m_ui->currentActivitiesListView->setItemDelegate(del);
    m_ui->currentActivitiesListView->setModel(m_listModel);

    connect(m_listModel, SIGNAL(rowsAboutToBeRemoved(const QModelIndex&, int, int)), this, SLOT(saveActivity(const QModelIndex &, int, int)));
    connect(m_ui->activitiesTableView, SIGNAL(pressed(const QModelIndex&)), this, SLOT(tableActivityClicked(const QModelIndex&)));
    connect(m_ui->currentActivitiesListView, SIGNAL(pressed(const QModelIndex&)), this, SLOT(listActivityClicked(const QModelIndex&)));

    m_timerOneS = new QTimer(this);
    m_timerOneHour = new QTimer(this);
    m_timerOneDay = new QTimer(this);

    connect(m_timerOneS, SIGNAL(timeout()), this, SLOT(updateTime()));//emit dataChange to refresh elapsed time in real time, 1s period
    m_timerOneS->start(1000);//1s

    connect(m_timerOneHour, SIGNAL(timeout()), this, SLOT(initTable()));//refill table after 1h (to keep up with changes)
    m_timerOneHour->start(36000000);//1h

    connect(m_timerOneDay, SIGNAL(timeout()), this, SLOT(archive()));
    m_timerOneDay->start(QTime::currentTime().msecsTo(QTime(23, 59, 59, 999)));

}

MainWindow::~MainWindow()
{
    delete m_ui;
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    m_ui->activitiesTableView->horizontalHeader()->resizeSections(QHeaderView::Stretch);
    m_ui->activitiesTableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_ui->activitiesTableView->verticalHeader()->setDefaultSectionSize(m_ui->activitiesTableView->height()/6);

    QMainWindow::resizeEvent(event);
}

void MainWindow::addActivity(const QString &cellText)
{
    m_listModel->appendRow(cellText, QModelIndex());

    m_ui->currentActivitiesListView->setModel(m_listModel);
}

void MainWindow::deleteActivity(const QString &cellText)
{
    int row = m_listModel->indexOf(cellText);

    if (row == -1)
        qDebug() << "MainWindow::deleteActivity\t\tFailed to get indexOf " << cellText;
    else
        m_listModel->removeRows(row, 1, QModelIndex());
}

void MainWindow::keyPressEvent(QKeyEvent* key)
{//open menu window on windows
    if ((key->key()== Qt::Key_F1))
    {
        m_menu = new MenuWindow(m_db, this);
        m_menu->show();
    }
}


bool MainWindow::event(QEvent *event)
{
    if (event->type() == QEvent::Gesture)
        return gestureEvent(static_cast<QGestureEvent*>(event));
    return QWidget::event(event);
}

bool MainWindow::gestureEvent(QGestureEvent *event)
{//open menu window on android
    if (QGesture *swipe = event->gesture(Qt::SwipeGesture))
    {
        m_menu = new MenuWindow(m_db, this);
        m_menu->show();
    }
}

void MainWindow::saveActivity(const QModelIndex &index, int first, int count)
{
    for(int i = 0; i <= count; ++i)
    {
        QModelIndex idx = m_listModel->index(first + i, 0, index);

        if(m_listModel->data(idx, ActivityListModel::ElapsedTime) >= 30000)
        {
            int id = m_db->getActivityMap()[m_listModel->data(idx, ActivityListModel::Name).toString()];
            QDateTime beginDate = m_listModel->data(idx, ActivityListModel::BeginDate).toDateTime();
            QDateTime endDate = beginDate.addMSecs(m_listModel->data(idx, ActivityListModel::ElapsedTime).toLongLong());

            m_db->insertActivityDaily(id, beginDate, endDate);
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

            // make cells not selectable and not editable
            m_tableModel->itemFromIndex(m_tableModel->index(i, j))->setFlags(Qt::NoItemFlags);
//            qDebug() << i << "/" << j << " filledCount " << filledCount << " item " << frequencyModel->data(frequencyModel->index(filledCount, 0)).toString();
            if (filledCount < items)
            {
                QString name = m_frequencyModel->data(m_frequencyModel->index(filledCount, 0)).toString();

                m_tableModel->setData(m_tableModel->index(i, j), name);

                // only this items will be pressable
                m_tableModel->itemFromIndex(m_tableModel->index(i, j))->setFlags(Qt::ItemIsEnabled);
            }
        }
}

void MainWindow::fillFrequencyTable(const QMap<QString, int>& frequencyMap)
{
    m_frequencyModel = new QStandardItemModel(frequencyMap.size(),2,this);

    int row = 0;

    for (auto key : frequencyMap.keys())
    {
        m_frequencyModel->setData (m_frequencyModel->index(row,0), key);
        m_frequencyModel->setData (m_frequencyModel->index(row,1), frequencyMap.value(key));

        ++row;
    }
}

//--------------------------------SLOTS implementation

void MainWindow::tableActivityClicked(const QModelIndex &tableIndex)
{
    QString cellText = m_ui->activitiesTableView->model()->data(tableIndex).toString();

    if (!cellText.isEmpty())
    {
        if (!m_listModel->find(cellText))
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

void MainWindow::listActivityClicked(const QModelIndex &listIndex)
{
    if (!listIndex.isValid())
        qDebug() << "MainWindow::on_listActivityClicked\t\tindex is not valid";

    QString cellText = m_listModel->data(listIndex, ActivityListModel::Name).toString();

    deleteActivity(cellText);
}

void MainWindow::updateTime()
{
    if (m_ui->mwStackedWidget->currentIndex() == 0 && m_ui->timeStackedWidget->currentIndex() == 0)
    {
        int rowCount = m_listModel->rowCount(QModelIndex());

        m_listModel->dataChanged(m_listModel->index(0, 0, QModelIndex()), m_listModel->index(rowCount, 0, QModelIndex()));
    }
    else
        qDebug() << "MainWindow::updateTime\t\tcalled when not on present time page";
}

void MainWindow::initTable()
{
    qDebug() << "MainWindow::initTable\t";

    QMap<QString, int> frequency;
    m_db->getFrequency(frequency);
    fillFrequencyTable(frequency);

    m_frequencyModel->sort(1,  Qt::DescendingOrder);

    m_ui->debugTableView->setModel(m_frequencyModel);

    int items = m_frequencyModel->rowCount();

    int rows = items / m_tableCollumnsCount + ((items % m_tableCollumnsCount) ? 1 : 0); // add whole number + 1 if there is something left

    m_tableModel = new QStandardItemModel(rows, m_tableCollumnsCount, this);

    fillTable(rows, items);

    m_ui->activitiesTableView->setModel(m_tableModel);

    //resize right after getting values
    QResizeEvent* resizeEvent = new QResizeEvent(this->size(), this->size());
    QCoreApplication::postEvent(this, resizeEvent);
}

void MainWindow::archive()
{
    m_db->archiveJob();

    m_timerOneDay->start(QTime::currentTime().msecsTo(QTime(23, 59, 59, 999)));
}

//---------------------------Push Buttons SLOTS

void MainWindow::timeButtonClicked()
{
    m_ui->mwStackedWidget->setCurrentIndex(0);//open timePage
}


void MainWindow::plansButtonClicked()
{
    m_ui->mwStackedWidget->setCurrentIndex(1);//open plansPage
}

void MainWindow::financesButtonClicked()
{
    m_ui->mwStackedWidget->setCurrentIndex(2);//open financesPage
}

void MainWindow::presentButtonClicked()
{
    m_ui->timeStackedWidget->setCurrentIndex(0);//open timePage.presentPage
}

void MainWindow::historyButtonClicked()
{
    m_ui->timeStackedWidget->setCurrentIndex(1);//open timePage.historyPage
}

void MainWindow::graphsButtonClicked()
{
    m_ui->timeStackedWidget->setCurrentIndex(2);//open timePage.graphsPage
}
