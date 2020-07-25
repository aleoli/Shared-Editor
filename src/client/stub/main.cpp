#include <iostream>
#include <QApplication>
#include <csignal>

#include "sys.h"
#include "utils.h"
#include "guiwrapper.h"
#include "declarations.h"

void signalHandler(int signum);

int main(int argc, char **argv) {
  QApplication app(argc, argv);
  SysConf conf = initiate_system(app);
  info("System successfully started");

  std::signal(SIGINT, signalHandler);
  std::signal(SIGTERM, signalHandler);

  registerClasses();

  GuiWrapper w(conf);
  QObject::connect(&w, SIGNAL(quit()), &app, SLOT(quit()), Qt::QueuedConnection);
  w.run();

  return QApplication::exec();
}

void signalHandler(int signum) {
  info("Bye bye");
  QApplication::exit(0);
}
