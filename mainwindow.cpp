#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLabel>
#include <QDateTime>
#define DB_NAME "citta"
#define DB_PASS "attic"
#define DB_USER "citta"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    /* init global parameters */
    serverIp = "192.168.1.5";
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
        viewModelAdd->setHorizontalHeaderItem(5, new QStandardItem("身份证号"));
        viewModelAdd->setHorizontalHeaderItem(6, new QStandardItem("省"));
        viewModelAdd->setHorizontalHeaderItem(7, new QStandardItem("市"));
        viewModelAdd->setHorizontalHeaderItem(8, new QStandardItem("区/县"));
        viewModelAdd->setHorizontalHeaderItem(9, new QStandardItem("镇/村/小区/"));

        ui->tableViewAdd->setModel(viewModelAdd);
        ui->tableViewAdd->horizontalHeader()->setStretchLastSection(true);

        viewModelSearch = new QStandardItemModel();
        viewModelSearch->setHorizontalHeaderItem(0, new QStandardItem("姓名"));
        viewModelSearch->setHorizontalHeaderItem(1, new QStandardItem("法名"));
        viewModelSearch->setHorizontalHeaderItem(2, new QStandardItem("手机"));
        viewModelSearch->setHorizontalHeaderItem(3, new QStandardItem("收据编号"));
        viewModelSearch->setHorizontalHeaderItem(4, new QStandardItem("皈依证号"));
        viewModelSearch->setHorizontalHeaderItem(5, new QStandardItem("身份证号"));

        viewModelSearch->setHorizontalHeaderItem(6, new QStandardItem("省"));
        viewModelSearch->setHorizontalHeaderItem(7, new QStandardItem("市"));
        viewModelSearch->setHorizontalHeaderItem(8, new QStandardItem("区/县"));
        viewModelSearch->setHorizontalHeaderItem(9, new QStandardItem("镇/村/小区/"));

        ui->tableViewSearch->setModel(viewModelSearch);
        ui->tableViewSearch->horizontalHeader()->setStretchLastSection(true);
    }

    statusLabel = new QLabel(this);
    ui->statusBar->addWidget(statusLabel);

    model = new QSqlQueryModel();
    qmodel = new QSqlQueryModel();
}

MainWindow::~MainWindow()
{
    delete ui;
}

/* slots */
void MainWindow::searchInfo()
{
    QString searchText = lineEditSearch->text();
}

void MainWindow::setServerAddr()
{
    serverIp = lineEditConfig->text().trimmed();
    lineEditConfig->setReadOnly(true);
}

void MainWindow::afterLineEditorEditorPressed()
{
    QString qsql = QString(" SELECT `name`, `fname`, `phone_num`, `receipt`, `code`, `personnel_id`, `province`, `city`, `district`, `address`  "
                           " FROM `zen_male` WHERE editor = '%1' "
                           " UNION  SELECT `name`, `fname`, `phone_num`, `receipt`, `code`, `personnel_id`,  `province`, `city`, `district`, `address`  "
                           " FROM `zen_female` WHERE editor = '%1' "
                           ).arg(lineEditEditor->text().trimmed());

    appendData(ui->tableViewAdd, model, qsql);

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

bool MainWindow::closeDatabase()
{
   db.close();
   lineEditConfig->setReadOnly(false);
   ui->actionDb->setEnabled(true);
   ui->actionConfig->setEnabled(true);
   return true;
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

    return true;
}

int MainWindow::appendData(QTableView *tableView, QSqlQueryModel *model, QString qsql)
{
    model->setQuery(qsql);
    model->setHeaderData(0, Qt::Horizontal, "姓名");
    model->setHeaderData(1, Qt::Horizontal, "法名");
    model->setHeaderData(2, Qt::Horizontal, "手机");
    model->setHeaderData(3, Qt::Horizontal, "收据编号");
    model->setHeaderData(4, Qt::Horizontal, "皈依证号");
    model->setHeaderData(5, Qt::Horizontal, "身份证号");
    model->setHeaderData(6, Qt::Horizontal, "省");
    model->setHeaderData(7, Qt::Horizontal, "市");
    model->setHeaderData(8, Qt::Horizontal, "区/县");
    model->setHeaderData(9, Qt::Horizontal, "镇/村/小区");

    int rowCount = model->rowCount();
    if (rowCount == 0) return 0;

    tableView->setModel(model);
    tableView->show();
    return rowCount;
}

void MainWindow::on_actionDb_triggered()
{
   serverIp = lineEditConfig->text().trimmed();
   if (lineEditEditor->text().trimmed().isEmpty()) {
       QMessageBox::information(this, "编辑人不能为空", "请输入编辑人姓名");
       return;
   }

   bool conStatus = databaseTest();
   if (conStatus == false) {
       QMessageBox::critical(this, "无法连接数据库", "请设置正确的数据库地址以及端口.");
       return;
   }
   ui->actionDb->setDisabled(true);
   ui->actionConfig->setDisabled(true);
   connectDatabase();

   getLastCode();

   QString qsql = QString(" SELECT `name`, `fname`, `phone_num`, `receipt`, `code`, `personnel_id`, `province`, `city`, `district`, `address`"
                          " FROM `zen_male` WHERE editor = '%1' "
                          " UNION  SELECT `name`, `fname`, `phone_num`, `receipt`, `code`, `personnel_id`, `province`, `city`, `district`, `address` "
                          " FROM `zen_female` WHERE editor = '%1' "
                          ).arg(lineEditEditor->text().trimmed());

   appendData(ui->tableViewAdd, model, qsql);
}

void MainWindow::on_actionConfig_triggered()
{
    if (lineEditConfig->isReadOnly()) {
        lineEditConfig->setEnabled(true);
        lineEditConfig->setReadOnly(false);
        lineEditConfig->clear();
        return;
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

    if (lineEditEditor->text().trimmed().isEmpty()) {
        QMessageBox::information(this, "编辑人不能为空", "请设置编辑人");
        return false;
    }

    if (ui->lineEditName->text().trimmed().isEmpty()) {
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
    ui->lineEditID->clear();
/* Do not clear gender now
    if (ui->radioButtonFemale->isChecked()) {
        ui->radioButtonFemale->setChecked(false);
    }
    if (ui->radioButtonMale->isChecked()) {
        ui->radioButtonMale->setChecked(false);
    }
*/
}

void MainWindow::on_actionSave_triggered()
{
    if(!isOk()) return;

    updateZen();
    clearLineEditors();
    ui->lineEditName->setFocus();
}

void MainWindow::on_pushButtonSave_clicked()
{
    if(!isOk()) return;

    updateZen();
    QString info = QString("  %1, %2, %3, %4, %5")
            .arg(statusName, statusGender, statusReceipt, statusCode, statusPhone);
    statusLabel->setText(info);

    statusName = "";
    statusGender = "";
    statusReceipt = "";
    statusCode = "";
    statusPhone = "";

    clearLineEditors();
    ui->lineEditName->setFocus();
}

bool MainWindow::updateRow(QString receipt, QString name, QString phone, QString fname, QString personnel_id)
{
    QString table;
    if (receipt.at(0) == QChar('A'))
        table = QString("zen_male");
    else
        table = QString("zen_female");

    QString upsql = QString("UPDATE %1 SET `name` = '%2', `phone_num` = '%3', `fname` = '%4', `personnel_id` = '%5' "
                            " WHERE `receipt` = '%6' ")
            .arg(table).arg(name).arg(phone).arg(fname).arg(personnel_id).arg(receipt);

    QSqlQuery query;
    query.exec(upsql);
    return true;
}

bool MainWindow::insertRow(QString name, QString phone, QString gender, QString personnel_id)
{
    QString table;
    QString editor;
    char header;
    int lastCode;

    editor = lineEditEditor->text().trimmed();

    if (gender == "男") {
        qDebug() << gender;
        table = QString("zen_male");
        header = 'A';
        lastCode = lastMaleCode;
    } else {
        table = QString("zen_female");
        header = 'B';
        lastCode = lastFemaleCode;
    }

    QSqlQuery query;
    QString sql = QString(" INSERT INTO `%1` (`name`, `phone_num`, `gender`, `editor`, `personnel_id`, `fahui_name`) "
                          " VALUES ('%2', '%3', '%4', '%5', '%6', '%7') "
                ).arg(table, name, phone, gender, editor, personnel_id, fahui_name);

    query.exec(sql);

    int lastInsertId = query.lastInsertId().toInt();

    QString code;
    code.sprintf("%c%010d", header, lastInsertId + lastCode);

    QString receipt;
    receipt.sprintf("%c%04d", header, lastInsertId);

    QString fname = makeFname(name);

    QString qsql = QString(" SELECT `name`, `fname`, `phone_num`, `receipt`, `code`, `personnel_id`, `province`, `city`, `district`, `address` "
                           " FROM `zen_male` WHERE editor = '%1' "
                           " UNION  SELECT `name`, `fname`, `phone_num`, `receipt`, `code`, `personnel_id`, `province`, `city`, `district`, `address` "
                           " FROM `zen_female` WHERE editor = '%1' "
                           ).arg(editor);

    updateReceiptCodeFnameById(table, lastInsertId, receipt, code, fname);

    // to show in status bar
    statusName = name;
    statusGender = gender;
    statusReceipt = receipt;
    statusCode = code;
    statusPhone = phone;

    appendData(ui->tableViewAdd, model, qsql);

    return true;
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
    QString name = ui->lineEditName->text().trimmed();
    QString phone = ui->lineEditPhone->text().trimmed();
    QString personnel_id = ui->lineEditID->text().trimmed();

    QString gender;
    ui->radioButtonMale->isChecked()? gender = QString("男"): gender = QString("女");
    ui->radioButtonFemale->isChecked()? gender = QString("女"): gender = QString("男");

    qDebug() << gender;

    QString qsql = QString(" SELECT `name`, `fname`, `phone_num`, `receipt`, `code`, `personnel_id` "
                           " FROM `zen_male` "
                           " WHERE `name` = '%1' AND `phone_num` = '%2' AND `gender` = '%3' "
                           " UNION "
                           " SELECT `name`, `fname`, `phone_num`, `receipt`, `code`, `personnel_id` "
                           " FROM `zen_female` "
                           " WHERE `name` = '%1' AND `phone_num` = '%2' AND `gender` = '%3' "

                           ).arg(name, phone, gender);

    int rowCount = appendData(ui->tableViewSearch, qmodel, qsql);

    qDebug() << rowCount;

    if (rowCount) {
        ui->tableViewSearch->show();
        QMessageBox::information(this, "", "数据已在库中, 如果修改请在下面表格右键单击修改项");
        return true;
    }

    insertRow(name, phone, gender, personnel_id); // will refresh tableViewAdd
    return true;
}

void MainWindow::on_actionModifyNameOrPhone_triggered()
{
    qDebug() << "on_actionModifyNameOrPhone_triggered()";
}

void MainWindow::on_radioButtonMale_clicked()
{
    ui->radioButtonFemale->setChecked(false);
}

void MainWindow::on_radioButtonFemale_clicked()
{
    ui->radioButtonMale->setChecked(false);
}

void MainWindow::modifyFields(int colNum)
{
    switch(colNum) {
    case 0: // name
        ui->lineEditCname->show();
        ui->lineEditCname->setFocus();
        if (!ui->lineEditCname->isHidden()) ui->lineEditCfname->hide();
        if (!ui->lineEditCPhone->isHidden()) ui->lineEditCPhone->hide();
        if (!ui->lineEditCID->isHidden()) ui->lineEditCID->hide();
        break;
    case 1: // fname
        ui->lineEditCfname->show();
        ui->lineEditCfname->setFocus();
        if (!ui->lineEditCname->isHidden()) ui->lineEditCname->hide();
        if (!ui->lineEditCPhone->isHidden()) ui->lineEditCPhone->hide();
        if (!ui->lineEditCID->isHidden()) ui->lineEditCID->hide();
        break;
    case 2: // phone
        ui->lineEditCPhone->show();
        ui->lineEditCPhone->setFocus();
        if (!ui->lineEditCname->isHidden()) ui->lineEditCname->hide();
        if (!ui->lineEditCfname->isHidden()) ui->lineEditCfname->hide();
        if (!ui->lineEditCID->isHidden()) ui->lineEditCID->hide();
        break;

    case 5: // PID
        qDebug() << "5";
        ui->lineEditCID->show();
        ui->lineEditCID->setFocus();
        if (!ui->lineEditCname->isHidden()) ui->lineEditCname->hide();
        if (!ui->lineEditCname->isHidden()) ui->lineEditCname->hide();
        if (!ui->lineEditCfname->isHidden()) ui->lineEditCfname->hide();
        break;

    }

}

void MainWindow::on_tableViewSearch_customContextMenuRequested(const QPoint &pos)
{
    int rowNum = ui->tableViewSearch->verticalHeader()->logicalIndexAt(pos);
    int colNum = ui->tableViewSearch->horizontalHeader()->logicalIndexAt(pos);

    if (rowNum < 0 || colNum < 0)  return;

    if (colNum == 3 || colNum == 4) return;
    if (colNum > 5) return;

    QString name = qmodel->index(rowNum, 0).data().toString();
    QString fname = qmodel->index(rowNum, 1).data().toString();
    QString phone = qmodel->index(rowNum, 2).data().toString();
    QString receipt = qmodel->index(rowNum, 3).data().toString();
    QString personnel_id = qmodel->index(rowNum, 5).data().toString();
    g_receipt = receipt;

    QMenu *popMenu = new QMenu(this);
    popMenu->addAction(ui->actionModifyNameOrPhone);
    popMenu->exec(QCursor::pos());

    ui->pushButtonSaveChange->show();
    ui->pushButtonCancel->show();
    ui->lineEditCname->setText(name);
    ui->lineEditCfname->setText(fname);
    ui->lineEditCPhone->setText(phone);
    qDebug() << personnel_id;
    ui->lineEditCID->setText(personnel_id);

    modifyFields(colNum);

    delete popMenu;
}

void MainWindow::on_tableViewAdd_customContextMenuRequested(const QPoint &pos)
{
    int rowNum = ui->tableViewAdd->verticalHeader()->logicalIndexAt(pos); // Get line order
    int colNum = ui->tableViewAdd->horizontalHeader()->logicalIndexAt(pos);

    if (rowNum < 0 || colNum < 0)  return;

    if (colNum == 3 || colNum == 4) return;
    if (colNum > 5) return;

    QString name = model->index(rowNum, 0).data().toString();
    QString fname = model->index(rowNum, 1).data().toString();
    QString phone = model->index(rowNum, 2).data().toString();
    QString receipt = model->index(rowNum, 3).data().toString();
    QString personnel_id = model->index(rowNum, 5).data().toString();
    g_receipt = receipt;

    QMenu *popMenu = new QMenu(this);
    popMenu->addAction(ui->actionModifyNameOrPhone);
    popMenu->exec(QCursor::pos());

    ui->pushButtonSaveChange->show();
    ui->pushButtonCancel->show();
    ui->lineEditCname->setText(name);
    ui->lineEditCfname->setText(fname);
    ui->lineEditCPhone->setText(phone);
    ui->lineEditCID->setText(personnel_id);

    modifyFields(colNum);
    delete popMenu;
}

void MainWindow::hideCwidgets()
{
    ui->lineEditCname->clear();
    ui->lineEditCfname->clear();
    ui->lineEditCPhone->clear();
    ui->lineEditCID->clear();

    ui->lineEditCname->hide();
    ui->lineEditCfname->hide();
    ui->lineEditCPhone->hide();
    ui->lineEditCID->hide();
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

    updateRow(g_receipt,
              ui->lineEditCname->text().trimmed(),
              ui->lineEditCPhone->text().trimmed(),
              ui->lineEditCfname->text().trimmed(),
              ui->lineEditCID->text().trimmed()
              );

    QString sql = QString(" SELECT `name`, `fname`, `phone_num`, `receipt`, `code`, `personnel_id`, `province`, `city`, `district`, `address` "
                           " FROM `zen_male` WHERE editor = '%1' "
                           " UNION  SELECT `name`, `fname`, `phone_num`, `receipt`, `code`, `personnel_id`, `province`, `city`, `district`, `address` "
                           " FROM `zen_female` WHERE `editor` = '%1' "
                           ).arg(lineEditEditor->text().trimmed());

    appendData(ui->tableViewAdd, model, sql);

    QString qsql = QString(" SELECT `name`, `fname`, `phone_num`, `receipt`, `code`, `personnel_id`, `province`, `city`, `district`, `address`  "
                           " FROM `zen_male` WHERE `receipt` = '%1' "
                           " UNION  SELECT `name`, `fname`, `phone_num`, `receipt`, `code`, `personnel_id`, `province`, `city`, `district`, `address` "
                           " FROM `zen_female` WHERE `receipt` = '%1' "
                           ).arg(g_receipt);

    appendData(ui->tableViewSearch, qmodel, qsql);


    hideCwidgets();
    g_receipt = "";
}

void MainWindow::on_actionQueryWindow_triggered()
{
    if (ui->tableViewSearch->isHidden()) ui->tableViewSearch->show();
    else ui->tableViewSearch->hide();
}

void MainWindow::getLastCode()
{
    lastMaleCode = -1;
    QString fahuiName;
    QDateTime ndt;
    QString dt = ndt.currentDateTime().date().toString("yyyy-MM-dd");
    qDebug() << dt;

    QString sql = QString(" SELECT `last_male_code`, `last_female_code`, `fahui_name` "
                          " FROM `zen_config` "
                          " WHERE `date` = '%1' ").arg(dt);
    qDebug() << sql;
    QSqlQuery query;
    query.exec(sql);
    qDebug() << query.lastError().text();
    while(query.next()) {
        lastMaleCode = query.value(0).toInt();
        lastFemaleCode = query.value(1).toInt();
        fahuiName = query.value(2).toString();
    }

    if (lastMaleCode == -1) {
        QMessageBox::information(this, "", "未设置皈依配置信息，请联系管理员，设置成功后重新打开本程序");
        closeDatabase();
    }

    fahui_name = fahuiName;

    statusLabel->setText(QString("    <请核对清楚：法会名称: %1 男众起始皈依号: %2 女众起始皈依号 %3>")
                         .arg(fahuiName).arg(lastMaleCode).arg(lastFemaleCode));
}

