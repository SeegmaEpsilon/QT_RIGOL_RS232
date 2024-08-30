// mainwindow.cpp
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    timerRequest = new QTimer();
    uart = new UART();

    channelManager = new ChannelManager(uart, ui->comboBox_EOF, this);
    channelManager->addChannel(new Channel(ui->lineEdit_channel_1_command, this));
    channelManager->addChannel(new Channel(ui->lineEdit_channel_2_command, this));

    uart->configureAllMenus(ui->menuSettings);

    connect(uart, &UART::signalMessageReseived, channelManager, &ChannelManager::onMessageReceived);
    connect(channelManager, &ChannelManager::allMessagesReceived, this, &MainWindow::log);
    connect(timerRequest, &QTimer::timeout, channelManager, &ChannelManager::process);

    ui->lineEdit_output_file_name->setText(QString("log_%1.csv").arg(QDateTime::currentDateTime().toString("dd.MM.yyyy")));
}

MainWindow::~MainWindow()
{
    delete ui;
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

void MainWindow::on_pushButton_write_command_clicked()
{
    channelManager->process();         // Начинаем с первого канала
}

void MainWindow::log()
{
    QString dateTime = QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss");
    QStringList messages = channelManager->collectMessages();
    QString message_to_write = QString("%1,%2\r\n").arg(dateTime, messages.join(','));

    printConsole(message_to_write);

    QString logFilePath = QCoreApplication::applicationDirPath() + "/" + ui->lineEdit_output_file_name->text();
    QFile fileOut(logFilePath);

    bool fileExists = QFile::exists(logFilePath);
    if (fileOut.open(QIODevice::WriteOnly | QIODevice::Append))
    {
        if (!fileExists || fileOut.size() == 0)
        {
            QString header = "date time,channel1,channel2\r\n";
            fileOut.write(header.toUtf8());
        }
        fileOut.write(message_to_write.toUtf8());
        fileOut.close();
    }

    qDebug() << "Logged: " << message_to_write;
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

void MainWindow::printConsole(const QString& string)
{
    QString time = QTime::currentTime().toString("> ");

    ui->console->setTextColor(Qt::blue);
    ui->console->append(time + string.trimmed());
    ui->console->moveCursor(QTextCursor::End);
}

void MainWindow::on_pushButton_consoleClear_clicked()
{
    ui->console->clear();
}
