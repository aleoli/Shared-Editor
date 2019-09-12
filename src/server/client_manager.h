#include "dependencies.h"

#include <QTcpServer>
#include <QThread>

#include <map>

class ClientManager: public QObject {
  Q_OBJECT
public:
  explicit ClientManager(QObject *parent = nullptr);

  ~ClientManager();

signals:
  void dataReceived(int client_id, QByteArray msg);
	void closeClient(int id);

public slots:
  void newConnection();
  void disconnected(int id);
	void onCloseClient(int id);

private:
  QTcpServer _s;
  std::map<quint64, Client *> _clients;
	std::map<quint64, QThread *> _threads;
  quint64 _next_client_id = 0;
};
