#define TEST_USER 0
#define TEST_SOCKET 1
#define TEST_MESSAGE 1

#include <QApplication>

#include <iostream>

#include "persistence_global.h"
#include "sys.h"

#if TEST_USER
#include "test_user.h"
#endif

#if TEST_SOCKET or TEST_MESSAGE
#include <QTcpServer>
#include <QTcpSocket>
#include "client_manager.h"
#endif

#if TEST_MESSAGE
#include "message_manager.h"
#include <memory>
#include <QThread>
#endif

#include "exceptions.h"

using namespace se_exceptions;

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  SysConf conf = initiate_system(app);

#if TEST_USER
  test_user();
#endif

#if TEST_SOCKET or TEST_MESSAGE
  auto cm = ClientManager::get(conf.port);
#endif

#if TEST_MESSAGE
  auto mm = MessageManager::get();
  QThread mm_thread{};
  mm->moveToThread(&mm_thread);
  QObject::connect(cm.get(), SIGNAL(dataReceived(quint64, QByteArray)), mm.get(), SLOT(process_data(quint64, QByteArray)));
  QObject::connect(cm.get(), SIGNAL(closeClient(quint64)), mm.get(), SLOT(client_disconnected(quint64)));
  QObject::connect(mm.get(), SIGNAL(send_data(quint64, QByteArray)), cm.get(), SLOT(sendData(quint64, QByteArray)));
  QObject::connect(mm.get(), SIGNAL(connection_error(quint64)), cm.get(), SIGNAL(force_close(quint64)));
  mm_thread.start();
#endif

  return app.exec();
}
