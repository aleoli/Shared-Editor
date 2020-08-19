#include <iostream>
#include <csignal>
#include <QApplication>
#include <optional>

#include "sys.h"
#include "utils.h"
#include "declarations.h"
#include "gui_manager.h"

void signalHandler(int signum);

int main(int argc, char **argv) {
  QApplication app(argc, argv);
  SysConf conf = initiate_system(app);
  info("System successfully started");

  std::signal(SIGINT, signalHandler);
  std::signal(SIGTERM, signalHandler);

  registerClasses();

  QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

  auto w = GuiManager::get(std::optional<SysConf>(conf));
  QObject::connect(w.get(), SIGNAL(quit()), &app, SLOT(quit()), Qt::QueuedConnection);
  w->run();

  return QApplication::exec();
}

void signalHandler(int signum) {
  info("Bye bye");
  QApplication::exit(0);
}
