#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <uart.h>
#include <QTime>
#include <QDebug>
#include <QFile>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void printConsole(const QString &string);
    void slotMessageProcess(QString message);
    void log(const QString &message);
private slots:
    void on_pushButton_uart_connect_clicked();

private:
    Ui::MainWindow *ui;
    UART *uart;
};
#endif // MAINWINDOW_H
