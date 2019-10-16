#include <iostream>

#include <QApplication>

#include "sys.h"
#include "prove.h"

int main(int argc, char **argv) {
  QApplication app(argc, argv);
  SysConf conf = initiate_system(app);

  prova(app);

  return 0;
}
