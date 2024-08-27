// channel.cpp
#include "channel.h"

Channel::Channel(QCheckBox* activeCheckBox, QLineEdit* commandLineEdit, QObject* parent) : QObject(parent), activeCheckBox(activeCheckBox), commandLineEdit(commandLineEdit), messageReady(false) {}

bool Channel::isActive() const { return activeCheckBox->isChecked(); }

void Channel::lineEditState(bool state) { this->commandLineEdit->setEnabled(state); }

QLineEdit* Channel::getLineEdit() { return this->commandLineEdit; }

QCheckBox* Channel::getCheckBox() { return this->activeCheckBox; }

QString Channel::getCommand(QString terminalSymbol) const
{
  if (commandLineEdit->text().isEmpty())
  {
    qDebug() << "Command is empty";
    return QString();
  }
  return commandLineEdit->text() + terminalSymbol;
}

void Channel::setMessage(const QString& message)
{
  this->message = message;
  this->messageReady = true;
}

QString Channel::getMessage() const { return messageReady ? message : QString(); }

void Channel::reset()
{
  message.clear();
  messageReady = false;
}
