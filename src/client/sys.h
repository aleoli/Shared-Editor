#pragma once

#include <QApplication>

#define CONNECT_TIME_LIMIT 2000
#define AUTO_CONNECT 1

typedef struct SysConf {
  QString host;
  int port;
} SysConf;

SysConf initiate_system(const QApplication &app);
