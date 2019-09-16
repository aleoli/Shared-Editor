#pragma once

#include <QApplication>

typedef struct SysConf {
  QString host;
  int port;
} SysConf;

SysConf initiate_system(const QApplication &app);
