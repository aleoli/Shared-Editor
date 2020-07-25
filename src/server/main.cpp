#include <QCoreApplication>

#include "sys.h"
#include "client_manager.h"
#include "message_manager.h"

#include "exceptions.h"

using namespace se_exceptions;

int main(int argc, char *argv[]) {
  QCoreApplication app(argc, argv);
  SysConf conf = initiate_system(app);
  info("System successfully started");

  auto cm = ClientManager::get(conf.port);
  auto mm = MessageManager::get();

  QObject::connect(cm.get(), SIGNAL(dataReceived(quint64, QByteArray)), mm.get(), SLOT(process_data(quint64, QByteArray)));
  QObject::connect(cm.get(), SIGNAL(closeClient(quint64)), mm.get(), SLOT(clientDisconnected(quint64)));
  QObject::connect(mm.get(), SIGNAL(send_data(quint64, QByteArray)), cm.get(), SLOT(sendData(quint64, QByteArray)));
  QObject::connect(mm.get(), SIGNAL(connection_error(quint64)), cm.get(), SIGNAL(force_close(quint64)));

  return app.exec();
}
