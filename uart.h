#ifndef UART_H
#define UART_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QMenu>
#include <QDebug>

typedef struct
{
    QString name;
    QSerialPort::DataBits dataBits;
    QSerialPort::BaudRate baudRate;
    QSerialPort::Parity parity;
    QSerialPort::StopBits stopBits;
} uartParameters;

class UART : public QObject
{
    Q_OBJECT
public:
    explicit UART(QObject *parent = nullptr);
    ~UART();

public slots:
  QStringList portCheckout();

  void open();
  void close();
  void send(const QString &message);

  void configureAllMenus(QMenu *menu_to_config);
  void configureMenuBaudrate(QMenu *menu_to_config);
  void configureMenuDataBits(QMenu *menu_to_config);
  void configureMenuParity(QMenu *menu_to_config);
  void configureMenuStopBits(QMenu *menu_to_config);
  void configureMenuPorts(QMenu *menu_to_config);
signals:
  void signalPortUnavailable();
  void signalDeviceNotOpen();

  void signalMessageReseived(const QString& message);

private:
    QSerialPort::BaudRate baudRate_;
    QSerialPort::DataBits dataBits_;
    QSerialPort::Parity parityControl_;
    QSerialPort::StopBits stopBits_;
    QSerialPort::FlowControl flowControl_;

    QSerialPort *serialPort;
    QString serialBuffer;
    uartParameters uartCurrent;
};

#endif // UART_H
