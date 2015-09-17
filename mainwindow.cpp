#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /* search */
    lineEditSearch = new QLineEdit;
    lineEditSearch->setFixedSize(200, 20);
    lineEditSearch->setStyleSheet("border-radius: 5px;");
    connect(lineEditSearch, SIGNAL(returnPressed()), this, SLOT(searchInfo()));
    ui->mainToolBar->addWidget(lineEditSearch);
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
