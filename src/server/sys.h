#pragma once

#include <QApplication>

typedef struct SysConf {
  int port;
} SysConf;

SysConf initiate_system(const QApplication &app);
