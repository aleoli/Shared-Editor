#pragma once

#include "dependencies.h"

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
  void send(QByteArray msg);

private slots:
  void read();
  void _disconnected();

private:
  void readSize();
  void readData();

  int _id;
  Session* _session = nullptr;

  QTcpSocket *_socket;
  QByteArray _in_buffer;

  uint32_t _in_size = 0;

};
