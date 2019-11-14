#include <iostream>

#include <QApplication>

#include "sys.h"
#include "utils.h"

int main(int argc, char **argv) {
  QApplication app(argc, argv);
  SysConf conf = initiate_system(app);
  info("System successfully started");

  //vero main!
  //TODO vedere lo stub per come bisogna agire:
  // per collegare gli opportuni signal/slot serve una classe "Wrapper" che inglobi
  //tutte le componenti principali, sia grafiche che non

  return 0;
}
