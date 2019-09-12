#pragma once

#include <memory>

#include <QObject>
#include <QString>
#include <QTcpSocket>

class Server: public QObject {
  Q_OBJECT
public:
  Server(const Server&) = delete;
  Server& operator=(const Server&) = delete;

  static std::shared_ptr<Server> get();

public slots:
  void connect();
  void write(QByteArray data);
  void disconnect();

private slots:
  void read();

signals:
  void connection_error();
  void dataReady(QByteArray data);
  void disconnected();

private:
  static std::shared_ptr<Server> instance;
  Server(QString host, int port = 1234);

  void readSize();
  void readData();

  QString _host;
  int _port;
  QTcpSocket *_socket;
  QByteArray _in_buffer;
  quint32 _in_size = 0;
};
