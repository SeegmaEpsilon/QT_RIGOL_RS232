// channel.h
#ifndef CHANNEL_H
#define CHANNEL_H

#include <QObject>
#include <QCheckBox>
#include <QLineEdit>
#include <QString>
#include <QMap>
#include <QDebug>

class Channel : public QObject
{
  Q_OBJECT

public:
  explicit Channel(QCheckBox* activeCheckBox, QLineEdit* commandLineEdit, QObject* parent = nullptr);

  bool isActive() const;
  QString getCommand(QString terminalSymbol) const;
  void setMessage(const QString& message);
  QString getMessage() const;
  void reset();
  void setLineEditState(bool state);

  QLineEdit* getLineEdit();
  QCheckBox* getCheckBox();

private:
  QCheckBox* activeCheckBox;
  QLineEdit* commandLineEdit;
  QString message;
  bool messageReady;
};

#endif // CHANNEL_H
