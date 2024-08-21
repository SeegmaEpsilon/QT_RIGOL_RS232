#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    timerRequest = new QTimer();
    uart = new UART();

    uart->configureAllMenus(ui->menuSettings);

    connect(uart, &UART::signalPortUnavailable, this, [this]()
    {
        QMessageBox::warning(this, QString::fromUtf8("Ошибка"), QString::fromUtf8("Выбранный порт недоступен"));
    });

    connect(timerRequest, &QTimer::timeout, this, [this]()
    {
        QString request = ":meas:vrms? chan1\n";
        uart->send(request);
        qDebug() << "Requested:     " << request;
    });

    connect(uart, &UART::signalMessageReseived, this, &MainWindow::slotMessageProcess);

    // TODO: реализовать запрос данных от осциллографа RIGOL через RS232
}

MainWindow::~MainWindow()
{
    delete ui;
}

// в .csv формате
void MainWindow::log(const QString& message)
{
  QString time = QTime::currentTime().toString("HH:mm:ss.zzz");
  QString date = QDateTime::currentDateTime().toString("dd.MM.yyyy");
  QString message_to_write = date + "," + time + "," + message + "\r\n";

  QFile fileOut(QCoreApplication::applicationDirPath () + "/" + "log.txt");

  if(fileOut.open(QIODevice::WriteOnly | QIODevice::Append))
  {
    fileOut.write(message_to_write.toUtf8());
    fileOut.close();
  }
}

void MainWindow::slotMessageProcess(QString message)
{
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

void MainWindow::on_pushButton_get_RMS_V_clicked()
{
    static bool isConnected = false;

    if(!isConnected)
    {
        timerRequest->start(1000);
        ui->pushButton_get_RMS_V->setText(tr("Остановить запрос СКЗ напряжения первого канала осциллографа"));
        isConnected = true;
    }
    else
    {
        timerRequest->stop();
        ui->pushButton_get_RMS_V->setText(tr("Начать запрос СКЗ напряжения первого канала осциллографа"));
        isConnected = false;
    }
}
