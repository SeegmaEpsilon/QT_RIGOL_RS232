#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    timerRequest = new QTimer();
    uart = new UART();
    expectedMessagesCounter = 0;

    uart->configureAllMenus(ui->menuSettings);

    connect(uart, &UART::signalPortUnavailable, this, [this]()
    {
        QMessageBox::warning(this, QString::fromUtf8("Ошибка"), QString::fromUtf8("Выбранный порт недоступен"));
    });

    connect(uart, &UART::signalDeviceNotOpen, this, [this]()
    {
        QMessageBox::warning(this, QString::fromUtf8("Ошибка"), QString::fromUtf8("Нет связи с устройством"));
    });

    connect(timerRequest, &QTimer::timeout, this, &MainWindow::slotWriteCommand);

    connect(uart, &UART::signalMessageReseived, this, &MainWindow::slotMessageProcess);

    connect(ui->checkBox_channel_1_active, &QCheckBox::toggled, this, [=](bool checked){ on_channel_active_toggled(1, checked); });
    connect(ui->checkBox_channel_2_active, &QCheckBox::toggled, this, [=](bool checked){ on_channel_active_toggled(2, checked); });

    QString date = QDateTime::currentDateTime().toString("dd.MM.yyyy");

    ui->lineEdit_output_file_name->setText(QString("log_%1.txt").arg(date));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slotWriteCommand()
{
    QMap<int, QString> eofMap
    {
        {0, ""},
        {1, "\r"},
        {2, "\n"},
        {3, "\r\n"},
        {4, "\n\r"},
    };

    if(ui->checkBox_channel_1_active->isChecked())
    {
        if(ui->lineEdit_channel_1_command->text().isEmpty())
        {
            QMessageBox::warning(this, QString::fromUtf8("Ошибка"), QString::fromUtf8("Введите команду"));
            return;
        }
    }

    if(ui->checkBox_channel_2_active->isChecked())
    {
        if(ui->lineEdit_channel_2_command->text().isEmpty())
        {
            QMessageBox::warning(this, QString::fromUtf8("Ошибка"), QString::fromUtf8("Введите команду"));
            return;
        }
    }

    if(ui->checkBox_channel_1_active->isChecked() && ui->checkBox_channel_2_active->isChecked())
    {
        if(ui->lineEdit_channel_1_command->text().isEmpty() || ui->lineEdit_channel_2_command->text().isEmpty())
        {
            QMessageBox::warning(this, QString::fromUtf8("Ошибка"), QString::fromUtf8("Введите команду"));
            return;
        }
    }

    QString request_channel_1 = ui->lineEdit_channel_1_command->text() + eofMap[ui->comboBox_EOF->currentIndex()];
    QString request_channel_2 = ui->lineEdit_channel_2_command->text() + eofMap[ui->comboBox_EOF->currentIndex()];

    if(ui->checkBox_channel_1_active->isChecked()) uart->send(request_channel_1);
    if(ui->checkBox_channel_2_active->isChecked()) uart->send(request_channel_2);

    qDebug() << "Requested [channel 1]:     " << request_channel_1;
    qDebug() << "Requested [channel 2]:     " << request_channel_2;
}

// в .csv формате
void MainWindow::log(QString message)
{
  static uint8_t innerCounter = 0;
  static QList<QString> messages({"", ""});

  messages[innerCounter] = message;
  innerCounter++;

  if(innerCounter < expectedMessagesCounter) return;
  else
  {
    innerCounter = 0;
  }

  QString date = QDateTime::currentDateTime().toString("dd.MM.yyyy");
  QString time = QTime::currentTime().toString("HH:mm:ss");

  QString message_channel_1;
  QString message_channel_2;

  if (expectedMessagesCounter == 2)
  {
    message_channel_1 = messages.at(0);
    message_channel_2 = messages.at(1);
  }
  else if (ui->checkBox_channel_1_active->isChecked())
  {
    message_channel_1 = messages.at(0);
    message_channel_2 = "";  // Второй канал неактивен
  }
  else if (ui->checkBox_channel_2_active->isChecked())
  {
    message_channel_1 = "";  // Первый канал неактивен
    message_channel_2 = messages.at(0);
  }

  QString message_to_write = QString("%1 %2,%3,%4\r\n").arg(date, time, message_channel_1, message_channel_2);

  messages[0].clear();
  messages[1].clear();

  QFile fileOut(QCoreApplication::applicationDirPath () + "/" + ui->lineEdit_output_file_name->text());

  if(fileOut.open(QIODevice::WriteOnly | QIODevice::Append))
  {
    fileOut.write(message_to_write.toUtf8());
    fileOut.close();
  }

  qDebug() << "Logged: " << message_to_write;
}

void MainWindow::slotMessageProcess(QString message)
{
  if(ui->checkBox_channel_1_active->isChecked() && ui->checkBox_channel_2_active->isChecked()) expectedMessagesCounter = 2;
  else expectedMessagesCounter = 1;

  printConsole(message);
  log(message);
}

void MainWindow::printConsole(const QString& string)
{
    QString time = QTime::currentTime().toString("HH:mm:ss.zzz > ");

    ui->console->setTextColor(Qt::blue); // Received message's color is blue.
    ui->console->append(time + string);
    ui->console->moveCursor(QTextCursor::End);
}

void MainWindow::on_pushButton_uart_connect_clicked()
{
    static bool isConnected = false;

    if(!isConnected)
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
  if(channel == 1)
  {
    ui->lineEdit_channel_1_command->setEnabled(checked);
  }
  else if(channel == 2)
  {
    ui->lineEdit_channel_2_command->setEnabled(checked);
  }

  if(checked)
  {
    ui->comboBox_EOF->setEnabled(true);
    ui->pushButton_polling->setEnabled(true);
    ui->pushButton_write_command->setEnabled(true);
  }
  else
  {
    if(!ui->checkBox_channel_1_active->isChecked() && !ui->checkBox_channel_2_active->isChecked())
    {
      ui->comboBox_EOF->setEnabled(false);
      ui->pushButton_polling->setEnabled(false);
      ui->pushButton_write_command->setEnabled(false);
    }
  }
}

void MainWindow::on_pushButton_polling_clicked()
{
    static bool isConnected = false;

    if(!isConnected)
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

void MainWindow::on_pushButton_write_command_clicked()
{
    slotWriteCommand();
}
