#include "uart.h"

UART::UART(QObject *parent) : QObject(parent)
{
    serialPort = new QSerialPort(this);

    uartCurrent.name = "";
    uartCurrent.baudRate = QSerialPort::Baud9600;
    uartCurrent.dataBits = QSerialPort::Data8;
    uartCurrent.parity = QSerialPort::NoParity;
    uartCurrent.stopBits = QSerialPort::OneStop;
}

UART::~UART()
{
    delete serialPort;
}

void UART::send(const QString& message)
{
    if(serialPort->write(message.toStdString().c_str()) == -1)
    {
        emit signalDeviceNotOpen();
    }
}

void UART::open()
{
    if (!serialPort->isOpen())
    {
        // Настройка последовательного порта
        uartCurrent.name = uartCurrent.name.split(" ").at(0);
        serialPort->setPortName(uartCurrent.name);
        serialPort->setBaudRate(uartCurrent.baudRate);
        serialPort->setDataBits(uartCurrent.dataBits);
        serialPort->setParity(uartCurrent.parity);
        serialPort->setStopBits(uartCurrent.stopBits);
        serialPort->setFlowControl(QSerialPort::NoFlowControl);

        if (!serialPort->open(QIODevice::ReadWrite))
        {
            emit signalPortUnavailable();
            return;
        }

        connect(serialPort, &QSerialPort::readyRead, [this]()
        {
            QByteArray data = serialPort->readAll();
            serialBuffer.append(data);

            // Обработка полных сообщений в буфере
            while (!serialBuffer.isEmpty())
            {
                int endIndexR = serialBuffer.indexOf('\r'); // Поиск символа возврата каретки CR, 0x0D
                int endIndexN = serialBuffer.indexOf('\n'); // Поиск символа новой строки LN, 0x0A

                int endIndex = -1;
                bool isDoubleEnd = false;

                if (endIndexR != -1 && (endIndexR < endIndexN || endIndexN == -1))
                {
                    endIndex = endIndexR;
                    if (endIndexN == endIndexR + 1) // Проверка на '\r\n'
                    {
                        isDoubleEnd = true;
                    }
                }
                else if (endIndexN != -1 && (endIndexN < endIndexR || endIndexR == -1))
                {
                    endIndex = endIndexN;
                    if (endIndexR == endIndexN + 1) // Проверка на '\n\r'
                    {
                        isDoubleEnd = true;
                    }
                }

                if (endIndex == -1) break; // Если нет полного сообщения, выходим из цикла

                QString message = serialBuffer.left(endIndex); // Извлекаем сообщение
                serialBuffer.remove(0, endIndex + (isDoubleEnd ? 2 : 1)); // Удаляем обработанное сообщение из буфера

                emit signalMessageReseived(message);
            }
        });
    }
}

void UART::close()
{
    if(serialPort->isOpen()) serialPort->close();
}

QStringList UART::portCheckout()
{
    QStringList availablePorts; // Список доступных COM-портов

    foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts())
    {
        QString name = serialPortInfo.portName();
        QString description = serialPortInfo.description();

        availablePorts.append(QString("%1 (%2)").arg(name, description));
    }

   return availablePorts;
}

void UART::configureAllMenus(QMenu* menu_to_config)
{
    configureMenuPorts(menu_to_config);
    configureMenuBaudrate(menu_to_config);
    configureMenuDataBits(menu_to_config);
    configureMenuParity(menu_to_config);
    configureMenuStopBits(menu_to_config);
}

void UART::configureMenuPorts(QMenu* menu_to_config)
{
    QStringList ports = portCheckout();

    QMenu *menuPorts = new QMenu(tr("Порт"));
    QActionGroup *portsGroup = new QActionGroup(this);

    // Добавление новых COM-портов в комбобокс
    foreach (const QString &portName, ports)
    {
        QAction *action = menuPorts->addAction(portName, this, [this, portName]() { uartCurrent.name = portName; });
        portsGroup->addAction(action);
    }

    // Устанавливаем эксклюзивность, чтобы только одно действие могло быть выбрано в один момент времени
    portsGroup->setExclusive(true);

    for (QAction *action : portsGroup->actions()) {
        action->setCheckable(true);
    }

    menu_to_config->addMenu(menuPorts);
}

void UART::configureMenuBaudrate(QMenu* menu_to_config)
{
    QMenu *menuBaudrate = new QMenu(tr("Скорость обмена"));
    QActionGroup *baudrateGroup = new QActionGroup(this);

    // Создаем действия и добавляем их в группу
    QAction *action1200 = menuBaudrate->addAction(tr("1200"), this, [this]() { uartCurrent.baudRate = QSerialPort::Baud1200; });
    QAction *action2400 = menuBaudrate->addAction(tr("2400"), this, [this]() { uartCurrent.baudRate = QSerialPort::Baud2400; });
    QAction *action4800 = menuBaudrate->addAction(tr("4800"), this, [this]() { uartCurrent.baudRate = QSerialPort::Baud4800; });
    QAction *action9600 = menuBaudrate->addAction(tr("9600"), this, [this]() { uartCurrent.baudRate = QSerialPort::Baud9600; });
    QAction *action19200 = menuBaudrate->addAction(tr("19200 (по умолчанию)"), this, [this]() { uartCurrent.baudRate = QSerialPort::Baud19200; });
    QAction *action38400 = menuBaudrate->addAction(tr("38400"), this, [this]() { uartCurrent.baudRate = QSerialPort::Baud38400; });
    QAction *action57600 = menuBaudrate->addAction(tr("57600"), this, [this]() { uartCurrent.baudRate = QSerialPort::Baud57600; });
    QAction *action115200 = menuBaudrate->addAction(tr("115200"), this, [this]() { uartCurrent.baudRate = QSerialPort::Baud115200; });

    // Добавляем действия в группу
    baudrateGroup->addAction(action1200);
    baudrateGroup->addAction(action2400);
    baudrateGroup->addAction(action4800);
    baudrateGroup->addAction(action9600);
    baudrateGroup->addAction(action19200);
    baudrateGroup->addAction(action38400);
    baudrateGroup->addAction(action57600);
    baudrateGroup->addAction(action115200);

    // Устанавливаем эксклюзивность, чтобы только одно действие могло быть выбрано в один момент времени
    baudrateGroup->setExclusive(true);

    // Устанавливаем начальное состояние (например, 19200 по умолчанию)
    action19200->setCheckable(true);
    action19200->setChecked(true);

    for (QAction *action : baudrateGroup->actions())
    {
        action->setCheckable(true);
    }

    menu_to_config->addMenu(menuBaudrate);
}

void UART::configureMenuDataBits(QMenu* menu_to_config)
{
    QMenu *menuDataBits = new QMenu(tr("Количество битов данных"));
    QActionGroup *dataBitsGroup = new QActionGroup(this);

    // Создаем действия и добавляем их в группу
    QAction *action5 = menuDataBits->addAction(tr("5"), this, [this]() { uartCurrent.dataBits = QSerialPort::Data5; });
    QAction *action6 = menuDataBits->addAction(tr("6"), this, [this]() { uartCurrent.dataBits = QSerialPort::Data6; });
    QAction *action7 = menuDataBits->addAction(tr("7"), this, [this]() { uartCurrent.dataBits = QSerialPort::Data7; });
    QAction *action8 = menuDataBits->addAction(tr("8 (по умолчанию)"), this, [this]() { uartCurrent.dataBits = QSerialPort::Data8; });

    // Добавляем действия в группу
    dataBitsGroup->addAction(action5);
    dataBitsGroup->addAction(action6);
    dataBitsGroup->addAction(action7);
    dataBitsGroup->addAction(action8);

    // Устанавливаем эксклюзивность, чтобы только одно действие могло быть выбрано в один момент времени
    dataBitsGroup->setExclusive(true);

    // Устанавливаем начальное состояние (например, 115200 по умолчанию)
    action8->setCheckable(true);
    action8->setChecked(true);

    for (QAction *action : dataBitsGroup->actions()) {
        action->setCheckable(true);
    }

    menu_to_config->addMenu(menuDataBits);
}

void UART::configureMenuParity(QMenu* menu_to_config)
{
    QMenu *menuParity = new QMenu(tr("Контроль четности"));
    QActionGroup *parityGroup = new QActionGroup(this);

    // Создаем действия и добавляем их в группу
    QAction *actionNoParity = menuParity->addAction(tr("Без контроля (по умолчанию)"), this, [this]() { uartCurrent.parity = QSerialPort::NoParity; });
    QAction *actionEvenParity = menuParity->addAction(tr("Четность"), this, [this]() { uartCurrent.parity = QSerialPort::EvenParity; });
    QAction *actionOddParity = menuParity->addAction(tr("Нечетность"), this, [this]() { uartCurrent.parity = QSerialPort::OddParity; });

    // Добавляем действия в группу
    parityGroup->addAction(actionNoParity);
    parityGroup->addAction(actionEvenParity);
    parityGroup->addAction(actionOddParity);

    // Устанавливаем эксклюзивность, чтобы только одно действие могло быть выбрано в один момент времени
    parityGroup->setExclusive(true);

    // Устанавливаем начальное состояние (например, 115200 по умолчанию)
    actionNoParity->setCheckable(true);
    actionNoParity->setChecked(true);

    for (QAction *action : parityGroup->actions()) {
        action->setCheckable(true);
    }

    menu_to_config->addMenu(menuParity);
}

void UART::configureMenuStopBits(QMenu* menu_to_config)
{
    QMenu *menuStopBits = new QMenu(tr("Количество стоп битов"));
    QActionGroup *stopBitsGroup = new QActionGroup(this);

    // Создаем действия и добавляем их в группу
    QAction *action1 = menuStopBits->addAction(tr("1 (по умолчанию)"), this, [this]() { uartCurrent.stopBits = QSerialPort::OneStop; });
    QAction *action1_5 = menuStopBits->addAction(tr("1.5"), this, [this]() { uartCurrent.stopBits = QSerialPort::OneAndHalfStop; });
    QAction *action2 = menuStopBits->addAction(tr("2"), this, [this]() { uartCurrent.stopBits = QSerialPort::TwoStop; });

    // Добавляем действия в группу
    stopBitsGroup->addAction(action1);
    stopBitsGroup->addAction(action1_5);
    stopBitsGroup->addAction(action2);

    // Устанавливаем эксклюзивность, чтобы только одно действие могло быть выбрано в один момент времени
    stopBitsGroup->setExclusive(true);

    // Устанавливаем начальное состояние (например, 115200 по умолчанию)
    action1->setCheckable(true);
    action1->setChecked(true);

    for (QAction *action : stopBitsGroup->actions()) {
        action->setCheckable(true);
    }

    menu_to_config->addMenu(menuStopBits);
}

