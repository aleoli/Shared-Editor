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

  QObject::connect(&tmp, SIGNAL(quit()), &app, SLOT(quit()));

  app.exec();
}
