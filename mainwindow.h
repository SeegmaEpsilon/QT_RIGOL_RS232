#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <uart.h>
#include <QTime>
#include <QDebug>
#include <QFile>
#include <QTimer>

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
    void log(QString message);
    void slotWriteCommand();
  private slots:
    void on_pushButton_uart_connect_clicked();
    void on_channel_active_toggled(int channel, bool checked);

    void on_pushButton_polling_clicked();

    void on_pushButton_write_command_clicked();

private:
    Ui::MainWindow *ui;
    UART *uart;
    QTimer* timerRequest;
    uint8_t expectedMessagesCounter;
};
#endif // MAINWINDOW_H
