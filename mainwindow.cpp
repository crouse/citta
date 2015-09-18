#include "mainwindow.h"
#include "ui_mainwindow.h"
#define DB_NAME "citta"
#define DB_PASS "123456"
#define DB_USER "citta"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    /* init global parameters */
    serverIp = "192.168.31.5";

    /* search lineEdit */
    {
        lineEditSearch = new QLineEdit;
        lineEditSearch->setFixedSize(200, 20);
        lineEditSearch->setStyleSheet("border-radius: 5px;");
        lineEditSearch->setPlaceholderText(" 查询 <ENTER>");
        connect(lineEditSearch, SIGNAL(returnPressed()), this, SLOT(searchInfo()));
        ui->mainToolBar->addWidget(lineEditSearch);
    }


    /* config lineEdit */
    {
        lineEditConfig = new QLineEdit;
        lineEditConfig->setFixedSize(100, 20);
        lineEditConfig->setStyleSheet("border-radius: 5px;");
        lineEditConfig->setPlaceholderText(serverIp);
        lineEditConfig->setReadOnly(true);
        connect(lineEditConfig, SIGNAL(returnPressed()), this, SLOT(setServerAddr()));
        ui->mainToolBar->addWidget(lineEditConfig);
    }


    /* editor lineEdit */
    {
        lineEditEditor = new QLineEdit;
        lineEditEditor->setFixedSize(100, 20);
        lineEditEditor->setStyleSheet("border-radius: 5px; background: yellow");
        lineEditEditor->setPlaceholderText(" 编辑人必填");
        connect(lineEditEditor, SIGNAL(returnPressed()), this, SLOT(afterLineEditorEditorPressed()));
        ui->mainToolBar->addWidget(lineEditEditor);
    }

    /* hide widgets */
    {
        ui->tableViewSearch->hide();
    }

    /* table view setting */
    {
        viewModelAdd = new QStandardItemModel();
        viewModelAdd->setHorizontalHeaderItem(0, new QStandardItem("姓名"));
        viewModelAdd->setHorizontalHeaderItem(1, new QStandardItem("法名"));
        viewModelAdd->setHorizontalHeaderItem(2, new QStandardItem("手机"));
        viewModelAdd->setHorizontalHeaderItem(3, new QStandardItem("收据编号"));
        viewModelAdd->setHorizontalHeaderItem(4, new QStandardItem("皈依证号"));
        ui->tableViewAdd->setModel(viewModelAdd);
        ui->tableViewAdd->horizontalHeader()->setStretchLastSection(true);

        viewModelSearch = new QStandardItemModel();
        viewModelSearch->setHorizontalHeaderItem(0, new QStandardItem("姓名"));
        viewModelSearch->setHorizontalHeaderItem(1, new QStandardItem("法名"));
        viewModelSearch->setHorizontalHeaderItem(2, new QStandardItem("手机"));
        viewModelSearch->setHorizontalHeaderItem(3, new QStandardItem("收据编号"));
        viewModelSearch->setHorizontalHeaderItem(4, new QStandardItem("皈依证号"));
        ui->tableViewSearch->setModel(viewModelSearch);
        ui->tableViewSearch->horizontalHeader()->setStretchLastSection(true);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

/* slots */
void MainWindow::searchInfo()
{
    QString searchText = lineEditSearch->text();
    qDebug() << "search text: " + searchText;
}

void MainWindow::setServerAddr()
{
    serverIp = lineEditConfig->text();
    qDebug() << "serverIp: " + serverIp;
    lineEditConfig->setReadOnly(true);
}

void MainWindow::afterLineEditorEditorPressed()
{
    QString qsql = QString(" SELECT `name`, `fname`, `phone_num`, `receipt`, `code` "
                           " FROM `zen_male` WHERE editor = '%1' "
                           " UNION  SELECT `name`, `fname`, `phone_num`, `receipt`, `code` "
                           " FROM `zen_female` WHERE editor = '%1' "
                           ).arg(lineEditEditor->text());

    appendData(ui->tableViewAdd, qsql);

}

bool MainWindow::databaseTest()
{
    bool ret;
    QTcpSocket tsock;
    tsock.connectToHost(serverIp, 3306);
    ret = tsock.waitForConnected(1000);
    if (ret) tsock.close();
    return ret;
}

bool MainWindow::connectDatabase()
{
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName(serverIp);
    db.setDatabaseName(DB_NAME);
    db.setUserName(DB_USER);
    db.setPassword(DB_PASS);

    if(!db.open()) {
        QMessageBox::critical(this, "数据库错误", db.lastError().text());
        return false;
    }

    qDebug() << QString("db status: %1 ").arg(db.open());

    return true;
}

void MainWindow::appendData(QTableView *tableView, QString qsql)
{
    model = new QSqlQueryModel;
    model->setQuery(qsql);
    model->setHeaderData(0, Qt::Horizontal, "姓名");
    model->setHeaderData(1, Qt::Horizontal, "法名");
    model->setHeaderData(2, Qt::Horizontal, "手机");
    model->setHeaderData(3, Qt::Horizontal, "收据编号");
    model->setHeaderData(4, Qt::Horizontal, "皈依证号");
    tableView->setModel(model);
    tableView->show();
}

void MainWindow::on_actionDb_triggered()
{
   if (lineEditEditor->text().isEmpty()) {
       QMessageBox::information(this, "编辑人不能为空", "请输入编辑人姓名");
       return;
   }

   bool conStatus = databaseTest();
   if (conStatus == false) {
       qDebug() << "Can not connect to mysql server";
       QMessageBox::critical(this, "无法连接数据库", "请设置正确的数据库地址以及端口.");
       return;
   }
   ui->actionDb->setDisabled(true);
   ui->actionConfig->setDisabled(true);
   connectDatabase();

   QString qsql = QString(" SELECT `name`, `fname`, `phone_num`, `receipt`, `code` "
                          " FROM `zen_male` WHERE editor = '%1' "
                          " UNION  SELECT `name`, `fname`, `phone_num`, `receipt`, `code` "
                          " FROM `zen_female` WHERE editor = '%1' "
                          ).arg(lineEditEditor->text());

   appendData(ui->tableViewAdd, qsql);
}

void MainWindow::on_actionConfig_triggered()
{
    if (lineEditConfig->isReadOnly()) {
        lineEditConfig->setReadOnly(false);
        lineEditConfig->setEnabled(true);
        qDebug() << "on_actionConfig_triggered()";
    }

    lineEditConfig->setFocus();
}

bool MainWindow::isOk()
{
    if (!db.isOpen() || !databaseTest()) {
       QMessageBox::critical(this, "数据库未连接", "请连接数据库");
       if (!ui->actionDb->isEnabled()) {
           ui->actionDb->setEnabled(true);
           ui->actionConfig->setEnabled(true);
       }

       return false;
    }

    if (lineEditEditor->text().isEmpty()) {
        QMessageBox::information(this, "编辑人不能为空", "请设置编辑人");
        return false;
    }

    if (ui->lineEditName->text().isEmpty()) {
        QMessageBox::information(this, "姓名不能为空", "请写入姓名");
        return false;
    }


    if (ui->lineEditGender->text().isEmpty()) {
        QMessageBox::information(this, "姓别不能为空", "请写入性别：男 或 女");
        return false;
    }

    return true;

}

void MainWindow::clearLineEditors()
{
    ui->lineEditGender->clear();
    ui->lineEditName->clear();
    ui->lineEditPhone->clear();
}

void MainWindow::on_actionSave_triggered()
{
    if(!isOk()) return;


    // end
    clearLineEditors();
}

void MainWindow::on_pushButtonSave_clicked()
{
    if(!isOk()) return;
    qDebug() << "save record";

    //end
    clearLineEditors();
}
