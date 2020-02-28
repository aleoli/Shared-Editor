#pragma once

#include <QCoreApplication>

typedef struct SysConf {
  int port;
  bool mem_only;
} SysConf;

SysConf initiate_system(const QCoreApplication &app);
