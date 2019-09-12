#pragma once

#include "../server.h"

#include <iostream>

#include <QApplication>
#include <QThread>
#include <QTimer>

#include "tmp.h"

void prova_server(int argc, char *argv[]) {
  QApplication app(argc, argv);

  Tmp tmp;

  /*auto server = Server::get();
  QThread thread;
  server->moveToThread(&thread);
  QObject::connect(&thread, SIGNAL(started()), server.get(), SLOT(connect()));
  QObject::connect(&thread, SIGNAL(started()), &tmp, SLOT(started()));
  QObject::connect(server.get(), SIGNAL(connection_error()), &thread, SLOT(quit()));
  QObject::connect(server.get(), SIGNAL(connection_error()), &app, SLOT(quit()));
  thread.start();*/

  //QTimer::singleShot(1000, server.get(), SLOT(write()));
  /*QTimer::singleShot(1900, &thread, SLOT(quit()));
  QTimer::singleShot(2000, &app, SLOT(quit()));*/

  app.exec();
}
