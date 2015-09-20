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

    int lastMaleCode;
    int lastFemaleCode;

    bool databaseTest();
    bool connectDatabase();
    bool isOk();
    bool isChinese(QString name);
    bool insertRow(QString name, QString phone, QString gender);
    int updateReceiptCodeFnameById(QString table, int id, QString receipt, QString code, QString fname);
    bool updateZen();
    int appendData(QTableView *tableView, QString qsql);
    void clearLineEditors();

    QString makeFname(QString name);

    QString serverIp;

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

private:
    Ui::MainWindow *ui;
    QSqlDatabase db;
    QSqlQueryModel *model;
};

#endif // MAINWINDOW_H
