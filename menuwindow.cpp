#include "menuwindow.h"

#include <QDateTime>
#include <QDebug>
#include <QFileDialog>
#include <QStandardPaths>

#include "database.h"
#include "importtool.h"
#include "ui_menuwindow.h"

MenuWindow::MenuWindow(std::shared_ptr<DataBase> db, QWidget *parent)
  : QDialog(parent)
  , m_ui(new Ui::MenuWindow)
  , m_db(db)
{
    m_ui->setupUi(this);
}

MenuWindow::~MenuWindow()
{
    delete m_ui;
}

void MenuWindow::importButtonClicked()
{
    QFile importFile(QFileDialog::getOpenFileName(this,
                                                  tr("Open file"),
                                                  "",
                                                  tr("Files (*.txt, *csv, *.db)")));
    QFileInfo inf(importFile);

    ImportTool tool(inf.filePath(), this);

//    progressDialog = new ImportProgressDialog(this);
//    progressDialog->show();


    tool.read();
    tool.parse();
    tool.convertDate();

    for (int i = 0; i < tool.size(); ++i)
    {
        QList<QString> entry = tool.getEntry(i);
        int id = m_db->findId(entry[2]);
        m_db->insertActivityDaily(id,
                                  QDateTime::fromString(entry[0], "yyyy-MM-dd HH:mm:ss.zzz"),
                                  QDateTime::fromString(entry[1], "yyyy-MM-dd HH:mm:ss.zzz"));
    }
    qDebug() << "Import complete";

}

void MenuWindow::exportButtonClicked()
{
    QDir resultPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);

    QFile exported(resultPath.path());
    exported.copy(resultPath.filePath("LMtest.db"));
}
