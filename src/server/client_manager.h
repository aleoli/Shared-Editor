#pragma once

#include "dependencies.h"

#include "def.h"

#include <QTcpServer>
#include <QThread>

#include <map>
#include <memory>
#include <list>

class ClientManager: public QObject {
  Q_OBJECT
public:
  ClientManager(const ClientManager&) = delete;
  ClientManager(ClientManager&&) = delete;
  ClientManager& operator=(const ClientManager&) = delete;
  ClientManager(QObject *parent = nullptr) = delete;
  ClientManager& operator=(ClientManager&&) = delete;

  static std::shared_ptr<ClientManager> get(int port = DEF_PORT);

  ~ClientManager();

signals:
  void dataReceived(quint64 client_id, QByteArray msg);
	void closeClient(quint64 id);
  void send_data(quint64 client_id, QByteArray data);

public slots:
  void newConnection();
  void disconnected(quint64 id);
	void onCloseClient(quint64 id);
  void sendData(quint64 client_id, QByteArray data);
  void sendData(std::list<quint64> client_list, QByteArray data);

private:
  static std::shared_ptr<ClientManager> instance;
  explicit ClientManager(int port, QObject *parent = nullptr);

  QTcpServer _s;
  std::map<quint64, Client *> _clients;
	std::map<quint64, QThread *> _threads;
  quint64 _next_client_id = 0;
};
