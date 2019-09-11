#define TEST_USER 0
#define TEST_SOCKET 1

#include <QApplication>

#include <iostream>

#include "persistence_global.h"
#include "sys.h"

#if TEST_USER
#include "test_user.h"
#endif

#if TEST_SOCKET
#include <QTcpServer>
#include <QTcpSocket>
#include "client_manager.h"
#endif

#include "exceptions.h"

using namespace se_exceptions;

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  initiate_system();

#if TEST_USER
  test_user();
#endif

#if TEST_SOCKET
  ClientManager cm;
#endif

  return app.exec();
}
