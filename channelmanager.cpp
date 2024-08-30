// channelmanager.cpp
#include "channelmanager.h"

ChannelManager::ChannelManager(UART* uart, QComboBox* eofComboBox, QObject* parent)
    : QObject(parent), uart(uart), eofComboBox(eofComboBox), currentChannelIndex(0), activeChannelsCount(0) {}

void ChannelManager::addChannel(Channel* channel) { channels.push_back(channel); }
Channel *ChannelManager::getChannel(size_t channelIndex) { return channels[channelIndex]; }

void ChannelManager::process()
{
    resetChannels();
    for (Channel* channel : channels)
    {
        if(!channel->getCommand().isEmpty()) activeChannelsCount++;
    }
    sendNextCommand();
}

void ChannelManager::sendNextCommand()
{
    if(currentChannelIndex < activeChannelsCount)
    {
        QMap<int, QString> eofMap { {0, ""}, {1, "\r"}, {2, "\n"}, {3, "\r\n"}, {4, "\n\r"}, };
        Channel* channel = channels[currentChannelIndex];

        // TODO: исправить передачу данных 2-му каналу
        if(channel->getCommand().isEmpty()) return;

        QString command = channel->getCommand() + eofMap[eofComboBox->currentIndex()];
        uart->send(command);
    }
}

void ChannelManager::handleMessage(const QString& message)
{
    if(currentChannelIndex < activeChannelsCount)
    {
        Channel* channel = channels[currentChannelIndex];
        channel->setMessage(message);
        currentChannelIndex++;

        if (currentChannelIndex < activeChannelsCount)
        {
            sendNextCommand(); // Отправляем следующую команду, если есть еще каналы
        }
        else
        {
            emit allMessagesReceived(); // Сообщаем, что все сообщения получены
        }
    }
}

QStringList ChannelManager::collectMessages() const
{
    QStringList messages;
    for (Channel* channel : channels)
    {
        messages.append(channel->getMessage());
    }
    return messages;
}

void ChannelManager::resetChannels()
{
    currentChannelIndex = 0;
    activeChannelsCount = 0;

    for (Channel* channel : channels)
    {
        channel->reset();
    }
}

void ChannelManager::onMessageReceived(const QString& message)
{
    handleMessage(message);
}
