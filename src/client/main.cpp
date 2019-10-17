#include <iostream>

#include <QApplication>

#include "sys.h"

#define TEST 1

#if TEST
#include "prove.h"
#endif

int main(int argc, char **argv) {
  QApplication app(argc, argv);
  SysConf conf = initiate_system(app);

  #if TEST
  prova(app);
  #else
  //vero main
  #endif

  return 0;
}
