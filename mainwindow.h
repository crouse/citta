#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QDebug>

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

private slots:
    void searchInfo();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
