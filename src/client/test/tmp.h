#pragma once

#include "../server.h"

#include <QObject>
#include <QByteArray>
#include <QThread>

#include <memory>

class Tmp: public QObject {
  Q_OBJECT
public:
  Tmp();

public slots:
  void started();
  void write();

signals:
  void writeData(QByteArray data);

private:
  std::shared_ptr<Server> _server;
  QThread* _thread;
};
