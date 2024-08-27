#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <uart.h>
#include <QTime>
#include <QDebug>
#include <QFile>
#include <QTimer>
#include "channel.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private slots:
  void slotWriteCommand();
  void slotMessageProcess(QString message);
  void on_pushButton_uart_connect_clicked();
  void on_pushButton_polling_clicked();
  void on_pushButton_write_command_clicked();
  void on_channel_active_toggled(int channel, bool checked);

  void printConsole(const QString& string);

private:
  Ui::MainWindow *ui;
  QTimer* timerRequest;
  UART* uart;

  Channel* channel1;
  Channel* channel2;

  void log();
};

#endif // MAINWINDOW_H
