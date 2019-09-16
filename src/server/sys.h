#pragma once

#include <QApplication>

typedef struct SysConf {
  int port;
  bool mem_only;
} SysConf;

SysConf initiate_system(const QApplication &app);
