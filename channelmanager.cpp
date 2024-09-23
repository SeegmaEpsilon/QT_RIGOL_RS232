// channelmanager.cpp
#include "channelmanager.h"

ChannelManager::ChannelManager(UART* uart, QComboBox* eofComboBox, QObject* parent)
    : QObject(parent), uart(uart), eofComboBox(eofComboBox), currentChannelIndex(0) {}

void ChannelManager::addChannel(Channel* channel) { channels.push_back(channel); }
Channel *ChannelManager::getChannel(size_t channelIndex) { return channels[channelIndex]; }

void ChannelManager::process()
{
    resetChannels();
    for (Channel* channel : channels)
    {
        if(!channel->getCommand().isEmpty()) activeChannels.push_back(channel);
    }

    if(!activeChannels.empty()) sendNextCommand();
}

void ChannelManager::sendNextCommand()
{
    if(currentChannelIndex < activeChannels.size())
    {
        QMap<int, QString> eofMap { {0, ""}, {1, "\r"}, {2, "\n"}, {3, "\r\n"}, {4, "\n\r"}, };
        Channel* channel = activeChannels[currentChannelIndex];

        // Отправляем команду текущему каналу
        QString command = channel->getCommand() + eofMap[eofComboBox->currentIndex()];
        uart->send(command);
    }
}

void ChannelManager::handleMessage(const QString& message)
{
    if(currentChannelIndex < activeChannels.size())
    {
        Channel* channel = activeChannels[currentChannelIndex];
        channel->setMessage(message);

        currentChannelIndex++;

        if(currentChannelIndex == activeChannels.size()) emit allMessagesReceived();
        else sendNextCommand();
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
    activeChannels.clear();

    for (Channel* channel : channels)
    {
        channel->reset();
    }
}

