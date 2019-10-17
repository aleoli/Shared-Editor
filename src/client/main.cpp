#include <iostream>

#include <QApplication>

#include "sys.h"
#include "utils.h"

#define TEST 1

#if TEST
#include "prove.h"
#endif

int main(int argc, char **argv) {
  QApplication app(argc, argv);
  SysConf conf = initiate_system(app);
  info("System successfully started");

  #if TEST
  prova(app);
  #else
  //vero main
  #endif

  return 0;
}
