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
  void dataReceived(QByteArray);

public slots:
  void newConnection();
  void disconnected(int id);

private:
  QTcpServer _s;
  std::map<int, Client *> _clients;
	std::map<int, QThread *> _threads;
  int _next_client_id = 0;
};
