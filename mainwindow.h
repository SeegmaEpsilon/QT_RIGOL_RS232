// mainwindow.h
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QFile>
#include <QDateTime>

#include "channel.h"
#include "channelmanager.h"
#include "uart.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_uart_connect_clicked();
    void on_pushButton_polling_clicked();
    void on_pushButton_write_command_clicked();
    void printConsole(const QString& string);
    void log();

    void on_pushButton_consoleClear_clicked();

private:
    Ui::MainWindow *ui;
    QTimer* timerRequest;

    UART* uart;
    ChannelManager* channelManager;
};

#endif // MAINWINDOW_H
