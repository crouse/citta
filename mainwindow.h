#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QMenu>
#include <QDebug>
#include <QtXlsx>
#include <QtSql>
#include <QTcpSocket>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QTableView>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QLineEdit *lineEditSearch;
    QLineEdit *lineEditConfig;
    QLineEdit *lineEditEditor;
    QStandardItemModel *viewModelAdd;
    QStandardItemModel *viewModelSearch;
    QLabel *statusLabel;

    int lastMaleCode;
    int lastFemaleCode;

    QString statusName;
    QString statusGender;
    QString statusReceipt;
    QString statusCode;
    QString statusPhone;

    bool databaseTest();
    bool connectDatabase();
    bool closeDatabase();
    bool isOk();
    bool isChinese(QString name);
    bool insertRow(QString name, QString phone, QString gender);
    bool updateRow(QString receipt, QString name, QString phone, QString fname);
    int updateReceiptCodeFnameById(QString table, int id, QString receipt, QString code, QString fname);
    bool updateZen();
    void getLastCode(); // Get last biggest Guyi Code
    int appendData(QTableView *tableView, QSqlQueryModel *model, QString qsql);
    void clearLineEditors();
    void hideCwidgets();
    void modifyFields(int colNum);

    QString makeFname(QString name);

    QString serverIp;
    QString g_receipt;

private slots:
    void searchInfo();
    void setServerAddr();
    void afterLineEditorEditorPressed();

    void on_actionDb_triggered();

    void on_actionConfig_triggered();

    void on_actionSave_triggered();

    void on_pushButtonSave_clicked();

    void on_tableViewSearch_customContextMenuRequested(const QPoint &pos);

    void on_actionModifyNameOrPhone_triggered();

    void on_radioButtonMale_clicked();

    void on_radioButtonFemale_clicked();

    void on_tableViewAdd_customContextMenuRequested(const QPoint &pos);

    void on_pushButtonCancel_clicked();

    void on_pushButtonSaveChange_clicked();

    void on_actionQueryWindow_triggered();

private:
    Ui::MainWindow *ui;
    QSqlDatabase db;
    QSqlQueryModel *model;
    QSqlQueryModel *qmodel;
};

#endif // MAINWINDOW_H
