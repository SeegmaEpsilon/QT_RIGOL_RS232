// mainwindow.cpp
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  timerRequest = new QTimer();
  uart = new UART();

  channel1 = new Channel(ui->checkBox_channel_1_active, ui->lineEdit_channel_1_command, this);
  channel2 = new Channel(ui->checkBox_channel_2_active, ui->lineEdit_channel_2_command, this);

  uart->configureAllMenus(ui->menuSettings);

  connect(uart, &UART::signalPortUnavailable, this, [this]() { QMessageBox::warning(this, QString::fromUtf8("Ошибка"), QString::fromUtf8("Выбранный порт недоступен")); });

  connect(uart, &UART::signalDeviceNotOpen, this, [this]() { QMessageBox::warning(this, QString::fromUtf8("Ошибка"), QString::fromUtf8("Нет связи с устройством")); });

  connect(timerRequest, &QTimer::timeout, this, &MainWindow::slotWriteCommand);
  connect(uart, &UART::signalMessageReseived, this, &MainWindow::slotMessageProcess);

  connect(channel1->getCheckBox(), &QCheckBox::toggled, this, [this](bool checked) { on_channel_active_toggled(1, checked); });
  connect(channel2->getCheckBox(), &QCheckBox::toggled, this, [this](bool checked) { on_channel_active_toggled(2, checked); });

  QString date = QDateTime::currentDateTime().toString("dd.MM.yyyy");
  ui->lineEdit_output_file_name->setText(QString("log_%1.txt").arg(date));
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::slotWriteCommand()
{
  QMap<int, QString> eofMap{
      {0, ""}, {1, "\r"}, {2, "\n"}, {3, "\r\n"}, {4, "\n\r"},
  };

  if (channel1->isActive())
  {
    QString command = channel1->getCommand(eofMap[ui->comboBox_EOF->currentIndex()]);
    if (!command.isEmpty())
    {
      uart->send(command);
      qDebug() << "Requested [Channel 1]: " << command;
    }
  }

  if (channel2->isActive())
  {
    QString command = channel2->getCommand(eofMap[ui->comboBox_EOF->currentIndex()]);
    if (!command.isEmpty())
    {
      uart->send(command);
      qDebug() << "Requested [Channel 2]: " << command;
    }
  }
}

void MainWindow::slotMessageProcess(QString message)
{
  // Если оба канала активны
  if (channel1->isActive() && channel2->isActive())
  {
    if (channel1->getMessage().isEmpty())
    {
      channel1->setMessage(message);
    }
    else if (channel2->getMessage().isEmpty())
    {
      channel2->setMessage(message);
    }

    // Проверяем, готовы ли оба сообщения
    if (!channel1->getMessage().isEmpty() && !channel2->getMessage().isEmpty())
    {
      log();  // Запись лога только после получения сообщений от обоих каналов
    }
  }
  else if (channel1->isActive() && !channel2->isActive())  // Только первый канал активен
  {
    channel1->setMessage(message);
    log();  // Запись лога после получения сообщения от первого канала
  }
  else if (channel2->isActive() && !channel1->isActive())  // Только второй канал активен
  {
    channel2->setMessage(message);
    log();  // Запись лога после получения сообщения от второго канала
  }

}

void MainWindow::log()
{
  QString date = QDateTime::currentDateTime().toString("dd.MM.yyyy");
  QString time = QTime::currentTime().toString("HH:mm:ss");

  QString message_to_write = QString("%1 %2,%3,%4\r\n")
                                 .arg(date, time, channel1->getMessage(), channel2->getMessage());

  printConsole(message_to_write);

  QFile fileOut(QCoreApplication::applicationDirPath() + "/" + ui->lineEdit_output_file_name->text());

  if (fileOut.open(QIODevice::WriteOnly | QIODevice::Append))
  {
    fileOut.write(message_to_write.toUtf8());
    fileOut.close();
  }

  qDebug() << "Logged: " << message_to_write;

         // Сбрасываем сообщения только после успешной записи лога
  channel1->reset();
  channel2->reset();
}

void MainWindow::on_pushButton_uart_connect_clicked()
{
  static bool isConnected = false;

  if (!isConnected)
  {
    uart->open();
    ui->pushButton_uart_connect->setText(tr("Отключиться"));
    isConnected = true;
  }
  else
  {
    uart->close();
    ui->pushButton_uart_connect->setText(tr("Подключиться"));
    isConnected = false;
  }
}

void MainWindow::on_channel_active_toggled(int channel, bool checked)
{
  Channel* ch = (channel == 1) ? channel1 : channel2;

  ch->lineEditState(checked);

  if (checked)
  {
    ui->comboBox_EOF->setEnabled(true);
    ui->pushButton_polling->setEnabled(true);
    ui->pushButton_write_command->setEnabled(true);
  }
  else if (!channel1->isActive() && !channel2->isActive())
  {
    ui->comboBox_EOF->setEnabled(false);
    ui->pushButton_polling->setEnabled(false);
    ui->pushButton_write_command->setEnabled(false);
  }
}

void MainWindow::on_pushButton_polling_clicked()
{
  static bool isConnected = false;

  if (!isConnected)
  {
    timerRequest->start(ui->lineEdit_polling_time->text().toInt());
    ui->pushButton_polling->setText(tr("Остановить опрос"));
    isConnected = true;
  }
  else
  {
    timerRequest->stop();
    ui->pushButton_polling->setText(tr("Начать опрос"));
    isConnected = false;
  }
}

void MainWindow::on_pushButton_write_command_clicked() { slotWriteCommand(); }

void MainWindow::printConsole(const QString& string)
{
  QString time = QTime::currentTime().toString("> ");

  ui->console->setTextColor(Qt::blue); // Received message's color is blue.
  ui->console->append(time + string);
  ui->console->moveCursor(QTextCursor::End);
}
