#pragma once

#define PKG_TERMINATOR 'a'

#include "dependencies.h"

#include <thread>
#include <atomic>

#include <QTcpSocket>

class Client: public QObject {
  Q_OBJECT
public:
  Client(const Client &c) = delete;
  Client(Client &&c);
  explicit Client(QTcpSocket *s, int id);

  ~Client();

  Client& operator=(const Client &c) = delete;
  void operator()();

signals:
  void dataReady(QByteArray data);
  void disconnected(int id);

public slots:
  void send(QString msg);

private slots:
  void read();
  void _disconnected();

private:
  void readSize();
  void readData();

  int id;
  Session* _session = nullptr;

  QTcpSocket *socket;
  QByteArray in_buffer;

  uint32_t in_size = 0;

};
