#pragma once

#include "dependencies.h"

#include <QTcpSocket>

class Client: public QObject {
  Q_OBJECT
public:
  Client(const Client &c) = delete;
  Client(Client &&c) noexcept;
  explicit Client(QTcpSocket *s, quint64 id);

  ~Client() override;

  Client& operator=(const Client &c) = delete;

signals:
  void dataReady(quint64 client_id, QByteArray data);
  void disconnected(quint64 id);

public slots:
  void send(quint64 client_id, const QByteArray& msg);
  void disconnect(quint64 id);
  void quit();

private slots:
  void read();
  void _disconnected();

private:
  void readSize();
  void readData();

  quint64 _id;

  QTcpSocket *_socket;
  QByteArray _in_buffer;

  quint32 _in_size = 0;

};
