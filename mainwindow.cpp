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

void MainWindow::on_actionDb_triggered()
{
   bool conStatus = databaseTest();
   if (conStatus == false) {
       qDebug() << "Can not connect to mysql server";
       QMessageBox::critical(this, "无法连接数据库", "请设置正确的数据库地址以及端口.");
       return;
   }
   ui->actionDb->setDisabled(true);
   ui->actionConfig->setDisabled(true);
   connectDatabase();
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
