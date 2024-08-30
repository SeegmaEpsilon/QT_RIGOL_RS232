// channelmanager.h
#ifndef CHANNELMANAGER_H
#define CHANNELMANAGER_H

#include "channel.h"
#include <vector>
#include <uart.h>
#include <QComboBox>

class ChannelManager : public QObject
{
    Q_OBJECT

public:
    explicit ChannelManager(UART* uart, QComboBox* eofComboBox, QObject* parent = nullptr);
    void addChannel(Channel* channel);
    Channel* getChannel(size_t channelIndex);
    void sendNextCommand();
    void handleMessage(const QString& message);
    QStringList collectMessages() const;
    void resetChannels();

public slots:
    void onMessageReceived(const QString& message);

    void process();
signals:
    void allMessagesReceived();

private:
    std::vector<Channel*> channels;
    UART* uart;

    QComboBox* eofComboBox;

    size_t currentChannelIndex;
    size_t activeChannelsCount;
};

#endif // CHANNELMANAGER_H
