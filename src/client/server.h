#pragma once

#include "def.h"

#include <memory>

#include <QObject>
#include <QString>
#include <QTcpSocket>

class Server: public QObject {
  Q_OBJECT
public:
  Server(const Server&) = delete;
  Server& operator=(const Server&) = delete;
  Server(QObject *parent) = delete;

  static std::shared_ptr<Server> get();

public slots:
  void connect(const QString &host, int port);
  void abort();
  void write(const QByteArray &data);
  void disconnect();

private slots:
  void read();

signals:
  void connection_error();
  void dataReady(const QByteArray &data);
  void connected();
  void disconnected();

private:
  static std::shared_ptr<Server> instance;
  Server();

  void readSize();
  void readData();

  QTcpSocket *_socket;
  QByteArray _in_buffer;
  quint32 _in_size = 0;
};
