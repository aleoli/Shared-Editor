#include <iostream>
#include <csignal>
#include <QApplication>

#include "sys.h"
#include "utils.h"

void signalHandler(int signum);

int main(int argc, char **argv) {
  QApplication app(argc, argv);
  SysConf conf = initiate_system(app);
  info("System successfully started");

  std::signal(SIGINT, signalHandler);
  std::signal(SIGTERM, signalHandler);

  //vero main
  //TODO vedere lo stub per come bisogna agire:
  // per collegare gli opportuni signal/slot serve una classe "Wrapper" che inglobi
  //tutte le componenti principali, sia grafiche che non

  return QApplication::exec();
}

void signalHandler(int signum) {
  info("Bye bye");
  QApplication::exit(0);
}
