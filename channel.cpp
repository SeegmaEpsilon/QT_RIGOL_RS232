// channel.cpp
#include "channel.h"

Channel::Channel(QLineEdit* commandLineEdit, QObject* parent)
    : QObject(parent), commandLineEdit(commandLineEdit) {}

void Channel::setLineEditState(bool state) { this->commandLineEdit->setEnabled(state); }
QLineEdit* Channel::getLineEdit() { return this->commandLineEdit; }

QString Channel::getCommand() const { return commandLineEdit->text(); }
QString Channel::getMessage() const { return message; }

void Channel::setMessage(const QString& message) { this->message = message; }

void Channel::reset() { message.clear(); }
