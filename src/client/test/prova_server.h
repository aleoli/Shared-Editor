#pragma once

#include "../server.h"

#include <iostream>

#include <QThread>
#include <QTimer>

#include "../sys.h"
#include "tmp.h"

void prova_server(QApplication &app, const SysConf& conf) {
  Tmp tmp{conf};

  QObject::connect(&tmp, SIGNAL(quit()), &app, SLOT(quit()));

  exit(app.exec());
}
