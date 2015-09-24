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
    serverIp = "192.168.1.5";
    lastMaleCode = 12933; // tbd, should read from database
    lastFemaleCode = 20000;
    g_receipt = "";
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
        lineEditConfig->setPlaceholderText("服务器地址");
        lineEditConfig->setText(serverIp);
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
        hideCwidgets();
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
    serverIp = lineEditConfig->text().trimmed();
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
    qDebug() << "Server IP: " << serverIp;
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

    lineEditConfig->setReadOnly(true);
    lineEditEditor->setReadOnly(true);
    qDebug() << "Db connect status: " << db.open();

    return true;
}

int MainWindow::appendData(QTableView *tableView, QString qsql)
{
    model = new QSqlQueryModel();
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
   serverIp = lineEditConfig->text().trimmed();
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
        lineEditConfig->setEnabled(true);
        lineEditConfig->setReadOnly(false);
        lineEditConfig->clear();
        qDebug() << "on_actionConfig_triggered()";
        return;
    }

    qDebug() << "lineEditConfig->isEnabled() " << lineEditConfig->isEnabled();
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

    bool checked = ui->radioButtonFemale->isChecked() || ui->radioButtonMale->isChecked();

    if (!checked) {
        QMessageBox::information(this, "", "请选择性别");
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

    int order = name.length() - 1;
    return QString("贤%1").arg(name[order]);
}

void MainWindow::clearLineEditors()
{
    ui->lineEditName->clear();
    ui->lineEditPhone->clear();
    if (ui->radioButtonFemale->isChecked()) {
        ui->radioButtonFemale->setChecked(false);
        qDebug() << "Female clear";
    }
    if (ui->radioButtonMale->isChecked()) {
        ui->radioButtonMale->setChecked(false);
        qDebug() << "Male clear";
    }
}

void MainWindow::on_actionSave_triggered()
{
    if(!isOk()) return;


    // end
    updateZen();
    clearLineEditors();
    ui->lineEditName->setFocus();
}

void MainWindow::on_pushButtonSave_clicked()
{
    if(!isOk()) return;

    //end
    updateZen();
    clearLineEditors();
    ui->lineEditName->setFocus();
}

bool MainWindow::updateRow(QString receipt, QString name, QString phone, QString fname)
{
    QString table;
    if (receipt.at(0) == QChar('A'))
        table = QString("zen_male");
    else
        table = QString("zen_female");

    QString upsql = QString("UPDATE %1 SET `name` = '%2', `phone_num` = '%3', `fname` = '%4' "
                            " WHERE `receipt` = '%5' ")
            .arg(table).arg(name).arg(phone).arg(fname).arg(receipt);
    qDebug() << "update sql " << upsql;
    QSqlQuery query;
    query.exec(upsql);
    qDebug() << query.lastError().text();
    g_receipt = "";
    return true;
}

bool MainWindow::insertRow(QString name, QString phone, QString gender)
{
    QString table;
    QString codeHeader;
    QString editor;
    char receiptHeader;
    int lastCode;

    editor = lineEditEditor->text().trimmed();

    if (gender == "男") {
        qDebug() << gender;
        table = QString("zen_male");
        codeHeader = QString("A00000");
        receiptHeader = 'A';
        lastCode = lastMaleCode;
    } else {
        table = QString("zen_female");
        codeHeader = QString("B00000");
        receiptHeader = 'B';
        lastCode = lastFemaleCode;
    }

    QSqlQuery query;
    QString sql = QString(" INSERT INTO `%1` (`name`, `phone_num`, `gender`, `editor`) "
                          " VALUES ('%2', '%3', '%4', '%5') "
                ).arg(table, name, phone, gender, editor);

    query.exec(sql);

    int lastInsertId = query.lastInsertId().toInt();
    QString code = QString("%1%2").arg(codeHeader).arg(lastInsertId + lastCode);

    QString receipt;
    receipt.sprintf("%c%03d", receiptHeader, lastInsertId);

    QString fname = makeFname(name);

    QString qsql = QString(" SELECT `name`, `fname`, `phone_num`, `receipt`, `code` "
                           " FROM `zen_male` WHERE editor = '%1' "
                           " UNION  SELECT `name`, `fname`, `phone_num`, `receipt`, `code` "
                           " FROM `zen_female` WHERE editor = '%1' "
                           ).arg(editor);
    qDebug() << qsql;

    updateReceiptCodeFnameById(table, lastInsertId, receipt, code, fname);
    appendData(ui->tableViewAdd, qsql);

    qDebug() << code << receipt << fname;
    return true; // tbd
}

int MainWindow::updateReceiptCodeFnameById(QString table, int id, QString receipt, QString code, QString fname)
{
    QSqlQuery query;
    QString sql = QString(" UPDATE `%1` SET `receipt` = '%2', `code` = '%3', `fname` = '%4' WHERE `id` = '%5' ")
            .arg(table).arg(receipt).arg(code).arg(fname).arg(id);

    query.exec(sql);
    return query.lastInsertId().toInt();
}

bool MainWindow::updateZen()
{
    QString name = ui->lineEditName->text();
    QString phone = ui->lineEditPhone->text();

    QString gender;
    ui->radioButtonMale->isChecked()? gender = QString("男"): gender = QString("女");
    ui->radioButtonFemale->isChecked()? gender = QString("女"): gender = QString("男");

    qDebug() << gender;

    QString qsql = QString(" SELECT `name`, `fname`, `phone_num`, `receipt`, `code` "
                           " FROM `zen_male` "
                           " WHERE `name` = '%1' AND `phone_num` = '%2' AND `gender` = '%3' "
                           ).arg(name, phone, gender);

    int rowCount = appendData(ui->tableViewSearch, qsql);

    qDebug() << rowCount;

    if (rowCount) {
        ui->tableViewSearch->show();
        QMessageBox::information(this, "已在库中", "修改未实现");
        //ui->tableViewSearch->hide(); // will remove tbd
        return true;
    }

    insertRow(name, phone, gender); // will refresh tableViewAdd
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

void MainWindow::on_radioButtonMale_clicked()
{
    ui->radioButtonFemale->setChecked(false);
}

void MainWindow::on_radioButtonFemale_clicked()
{
    ui->radioButtonMale->setChecked(false);
}

void MainWindow::on_tableViewAdd_customContextMenuRequested(const QPoint &pos)
{
    int rowNum = ui->tableViewAdd->verticalHeader()->logicalIndexAt(pos); // Get line order
    int colNum = ui->tableViewAdd->horizontalHeader()->logicalIndexAt(pos);
    if (rowNum < 0 || colNum < 0) return;
    if (colNum > 2) return;

    QMenu *popMenu = new QMenu(this);
    QString name = model->index(rowNum, 0).data().toString();
    QString fname = model->index(rowNum, 1).data().toString();
    QString phone = model->index(rowNum, 2).data().toString();
    QString receipt = model->index(rowNum, 3).data().toString();

    // set a global var
    g_receipt = receipt;

    qDebug() << name << phone << receipt;
    qDebug() << rowNum << colNum;
    popMenu->addAction(ui->actionModifyNameOrPhone);
    popMenu->exec(QCursor::pos());

    ui->pushButtonSaveChange->show();
    ui->pushButtonCancel->show();
    ui->lineEditCname->setText(name);
    ui->lineEditCfname->setText(fname);
    ui->lineEditCPhone->setText(phone);

    switch(colNum) {
    case 0: // name
        ui->lineEditCname->show();
        qDebug() << "name " << name;
        ui->lineEditCname->setFocus();
        if (!ui->lineEditCname->isHidden()) ui->lineEditCfname->hide();
        if (!ui->lineEditCPhone->isHidden()) ui->lineEditCPhone->hide();
        break;
    case 1: // fname
        ui->lineEditCfname->show();
        ui->lineEditCfname->setFocus();
        if (!ui->lineEditCname->isHidden()) ui->lineEditCname->hide();
        if (!ui->lineEditCPhone->isHidden()) ui->lineEditCPhone->hide();
        break;
    case 2: // phone
        ui->lineEditCPhone->show();
        ui->lineEditCPhone->setFocus();
        if (!ui->lineEditCname->isHidden()) ui->lineEditCname->hide();
        if (!ui->lineEditCfname->isHidden()) ui->lineEditCfname->hide();
        break;
    }
}

void MainWindow::hideCwidgets()
{
    ui->lineEditCname->clear();
    ui->lineEditCfname->clear();
    ui->lineEditCPhone->clear();

    ui->lineEditCname->hide();
    ui->lineEditCfname->hide();
    ui->lineEditCPhone->hide();
    ui->pushButtonCancel->hide();
    ui->pushButtonSaveChange->hide();
}

void MainWindow::on_pushButtonCancel_clicked()
{
    hideCwidgets();
}


void MainWindow::on_pushButtonSaveChange_clicked()
{
    if (g_receipt == "") return;
    // hide at end
    //QString receipt, QString name, QString phone, QString fname
    updateRow(g_receipt,
              ui->lineEditCname->text().trimmed(),
              ui->lineEditCPhone->text().trimmed(),
              ui->lineEditCfname->text().trimmed()
              );
    QString qsql = QString(" SELECT `name`, `fname`, `phone_num`, `receipt`, `code` "
                           " FROM `zen_male` WHERE editor = '%1' "
                           " UNION  SELECT `name`, `fname`, `phone_num`, `receipt`, `code` "
                           " FROM `zen_female` WHERE editor = '%1' "
                           ).arg(lineEditEditor->text().trimmed());
    qDebug() << qsql;

    appendData(ui->tableViewAdd, qsql);

    hideCwidgets();
    g_receipt = "";
}

void MainWindow::on_actionQueryWindow_triggered()
{
    if (ui->tableViewSearch->isHidden()) ui->tableViewSearch->show();
    else ui->tableViewSearch->hide();
}
