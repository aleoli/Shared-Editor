#include "dependencies.h"

#include <QtCore>
#include <QtNetwork>

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
  QTcpServer s;
  std::map<int, Client *> clients;
  int next_client_id = 0;
};
