#include "dependencies.h"

#include "def.h"

#include <QTcpServer>
#include <QThread>

#include <map>
#include <memory>

class ClientManager: public QObject {
  Q_OBJECT
public:
  ClientManager(const ClientManager&) = delete;
  ClientManager& operator=(const ClientManager&) = delete;
  ClientManager(QObject *parent = nullptr) = delete;

  static std::shared_ptr<ClientManager> get(int port = DEF_PORT);

  ~ClientManager();

signals:
  void dataReceived(int client_id, QByteArray msg);
	void closeClient(int id);

public slots:
  void newConnection();
  void disconnected(int id);
	void onCloseClient(int id);

private:
  static std::shared_ptr<ClientManager> instance;
  explicit ClientManager(int port, QObject *parent = nullptr);

  QTcpServer _s;
  std::map<quint64, Client *> _clients;
	std::map<quint64, QThread *> _threads;
  quint64 _next_client_id = 0;
};
