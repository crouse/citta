#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /* search lineEdit */
    {
    lineEditSearch = new QLineEdit;
    lineEditSearch->setFixedSize(200, 20);
    lineEditSearch->setStyleSheet("border-radius: 5px;");
    lineEditSearch->setPlaceholderText(" 查询 <ENTER>");
    connect(lineEditSearch, SIGNAL(returnPressed()), this, SLOT(searchInfo()));
    ui->mainToolBar->addWidget(lineEditSearch);
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

void MainWindow::searchInfo()
{
    QString searchText = lineEditSearch->text();
    qDebug() << "search text: " + searchText;
}

MainWindow::~MainWindow()
{
    delete ui;
}
