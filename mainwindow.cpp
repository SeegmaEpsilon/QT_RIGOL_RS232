// mainwindow.cpp
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  timerRequest = new QTimer();
  uart = new UART();

  channels.push_back(new Channel(ui->checkBox_channel_1_active, ui->lineEdit_channel_1_command, this));
  channels.push_back(new Channel(ui->checkBox_channel_2_active, ui->lineEdit_channel_2_command, this));

  uart->configureAllMenus(ui->menuSettings);

  connect(uart, &UART::signalPortUnavailable, this, [this]() { QMessageBox::warning(this, QString::fromUtf8("Ошибка"), QString::fromUtf8("Выбранный порт недоступен")); });

  connect(uart, &UART::signalDeviceNotOpen, this, [this]() { QMessageBox::warning(this, QString::fromUtf8("Ошибка"), QString::fromUtf8("Нет связи с устройством")); });

  connect(timerRequest, &QTimer::timeout, this, &MainWindow::slotWriteCommand);
  connect(uart, &UART::signalMessageReseived, this, &MainWindow::slotMessageProcess);

  // Подключаем сигналы для всех каналов
  for (uint8_t i = 0; i < channels.size(); ++i)
  {
    connect(channels[i]->getCheckBox(), &QCheckBox::toggled, this, [this, i](bool checked) { on_channel_active_toggled(i, checked); });
  }

  QString date = QDateTime::currentDateTime().toString("dd.MM.yyyy");
  ui->lineEdit_output_file_name->setText(QString("log_%1.csv").arg(date));
}

MainWindow::~MainWindow()
{
  delete ui;
  // Не забываем освободить память, выделенную для каналов
  for (Channel* channel : channels)
  {
    delete channel;
  }
}

void MainWindow::slotWriteCommand()
{
  QMap<int, QString> eofMap{
      {0, ""}, {1, "\r"}, {2, "\n"}, {3, "\r\n"}, {4, "\n\r"},
  };

  for (Channel* channel : channels)
  {
    if (channel->isActive())
    {
      QString command = channel->getCommand(eofMap[ui->comboBox_EOF->currentIndex()]);
      if (!command.isEmpty())
      {
        uart->send(command);
        qDebug() << "Requested [Channel]: " << command;
      }
    }
  }
}

void MainWindow::slotMessageProcess(QString message)
{
  bool allMessagesReady = true;

  for (Channel* channel : channels)
  {
    if (channel->isActive() && channel->getMessage().isEmpty())
    {
      channel->setMessage(message);
      break; // Выходим, так как сообщение назначено только одному каналу
    }
  }

  // Проверяем, готовы ли сообщения на всех активных каналах
  for (Channel* channel : channels)
  {
    if (channel->isActive() && channel->getMessage().isEmpty())
    {
      allMessagesReady = false;
      break;
    }
  }

  if (allMessagesReady)
  {
    log(); // Записываем лог, когда все сообщения готовы
  }
}

void MainWindow::log()
{
  QString date = QDateTime::currentDateTime().toString("dd.MM.yyyy");
  QString time = QTime::currentTime().toString("HH:mm:ss");

  QString message_to_write = QString("%1 %2,%3,%4\r\n")
                                 .arg(date, time, channels[0]->getMessage(), channels[1]->getMessage());

  printConsole(message_to_write);

  QString logFilePath = QCoreApplication::applicationDirPath() + "/" + ui->lineEdit_output_file_name->text();
  QFile fileOut(logFilePath);

         // Проверяем, существует ли файл и его размер
  bool fileExists = QFile::exists(logFilePath);
  if (fileOut.open(QIODevice::WriteOnly | QIODevice::Append)) {
    // Если файл не существовал или был пустым, добавляем заголовок
    if (!fileExists || fileOut.size() == 0) {
      QString header = "date time,channel1,channel2\r\n";
      fileOut.write(header.toUtf8());
    }

    // Записываем строку данных
    fileOut.write(message_to_write.toUtf8());
    fileOut.close();
  }

  qDebug() << "Logged: " << message_to_write;

         // Сбрасываем сообщения только после успешной записи лога
  for (Channel* channel : channels) {
    channel->reset();
  }
}

void MainWindow::on_channel_active_toggled(int channelIndex, bool checked)
{
  channels[channelIndex]->setLineEditState(checked);

  if (checked)
  {
    ui->comboBox_EOF->setEnabled(true);
    ui->pushButton_polling->setEnabled(true);
    ui->pushButton_write_command->setEnabled(true);
  }
  else if (!channels[0]->isActive() && !channels[1]->isActive())
  {
    ui->comboBox_EOF->setEnabled(false);
    ui->pushButton_polling->setEnabled(false);
    ui->pushButton_write_command->setEnabled(false);
  }
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
