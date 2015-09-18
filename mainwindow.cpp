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

int MainWindow::appendData(QTableView *tableView, QString qsql)
{
    model = new QSqlQueryModel;
    model->setQuery(qsql);
    model->setHeaderData(0, Qt::Horizontal, "姓名");
    model->setHeaderData(1, Qt::Horizontal, "法名");
    model->setHeaderData(2, Qt::Horizontal, "手机");
    model->setHeaderData(3, Qt::Horizontal, "收据编号");
    model->setHeaderData(4, Qt::Horizontal, "皈依证号");

    int rowCount = model->rowCount();
    if (rowCount == 0) return 0;

    tableView->setModel(model);
    tableView->show();
    return rowCount;
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

bool MainWindow::isChinese(QString name)
{
    int count = name.count();
    for (int i = 0; i < count; i++) {
        QChar ch = name.at(i);
        ushort unicode = ch.unicode();
        if (unicode >= 0x4E00 && unicode <= 0x9FA5)
            return true;
    }

    return false;
}

QString MainWindow::makeFname(QString name)
{
    if (!isChinese(name)) {
        return QString("贤善");
    }

    return QString("贤%1").arg(name[1]);
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
    updateZen();
    clearLineEditors();
}

bool MainWindow::insertRow(QString name, QString phone, QString gender)
{
    QString table;
    if (gender == "男")
        table = "zen_male";
    else
        table = "zen_female";

    QSqlQuery query;
    query.prepare("INSERT INTO `zen_male` (`name`, `phone_num`, `gender`) "
                  "VALUES (:name, :phone, :gender)"
                  );
    query.bindValue(":table", "zen_male");
    query.bindValue(":name", name);
    query.bindValue(":phone", phone);
    query.bindValue(":gender", gender);
    query.exec();
    qDebug() << query.lastQuery();
    qDebug() << query.lastError().text();
    qDebug() << "last insert id";
    qDebug() << query.lastInsertId().toInt();
    return true;
}

bool MainWindow::updateZen()
{
    QString name = ui->lineEditName->text();
    QString phone = ui->lineEditPhone->text();
    QString gender = ui->lineEditGender->text();

    qDebug() << "here";

    QString qsql = QString(" SELECT `name`, `fname`, `phone_num`, `receipt`, `code` "
                           " FROM `zen_male` "
                           " WHERE `name` = '%1' AND `phone_num` = '%2' AND `gender` = '%3' "
                           ).arg(name, phone, gender);

    int rowCount = appendData(ui->tableViewSearch, qsql);

    qDebug() << rowCount;

    if (rowCount) {
        ui->tableViewSearch->show();
        QMessageBox::information(this, "已在库中", "修改未实现");
        ui->tableViewSearch->hide(); // will remove tbd
        return true;
    }

    insertRow(name, phone, gender);
    return true;
}

void MainWindow::on_tableViewSearch_customContextMenuRequested(const QPoint &pos)
{
    qDebug() << "begining of right";
    int rowNum = ui->tableViewSearch->verticalHeader()->logicalIndexAt(pos);
    if (rowNum < 0) {
        qDebug() << "exit 298";
        return;
    }

    QMenu *popMenu = new QMenu(this);
    QString name = viewModelSearch->index(rowNum, 0).data().toString();
    QString phone = viewModelSearch->index(rowNum, 2).data().toString();
    qDebug() << name << phone;
    popMenu->addAction(ui->actionModifyNameOrPhone);
    popMenu->exec(QCursor::pos());
    // tbd
}

void MainWindow::on_actionModifyNameOrPhone_triggered()
{
    qDebug() << "on_actionModifyNameOrPhone_triggered()";
    //tbd
}
