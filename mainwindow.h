#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QDebug>
#include <QtXlsx>
#include <QtSql>
#include <QTcpSocket>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QSqlDatabase>

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
    bool databaseTest();
    bool connectDatabase();

    QString serverIp;

private slots:
    void searchInfo();
    void setServerAddr();

    void on_actionDb_triggered();

    void on_actionConfig_triggered();

private:
    Ui::MainWindow *ui;
    QSqlDatabase db;
};

#endif // MAINWINDOW_H
