#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <uart.h>
#include <QTime>
#include <QDebug>
#include <QFile>
#include <QTimer>
#include "channel.h"

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
  void slotWriteCommand();
  void slotMessageProcess(QString message);
  void on_pushButton_uart_connect_clicked();
  void on_channel_active_toggled(int channelIndex, bool checked);
  void on_pushButton_polling_clicked();
  void on_pushButton_write_command_clicked();
  void printConsole(const QString& string);

private:
  Ui::MainWindow *ui;
  QTimer* timerRequest;
  UART* uart;

         // Заменяем отдельные каналы на вектор
  std::vector<Channel*> channels;

  void log();
};

#endif // MAINWINDOW_H
