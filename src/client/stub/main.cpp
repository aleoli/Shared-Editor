#include <iostream>

#include <QApplication>

#include "sys.h"
#include "utils.h"
#include "guiwrapper.h"
#include "declarations.h"

int main(int argc, char **argv) {
  QApplication app(argc, argv);
  SysConf conf = initiate_system(app);
  info("System successfully started");

  registerClasses();

  GuiWrapper w(conf);
  QObject::connect(&w, SIGNAL(quit()), &app, SLOT(quit()), Qt::QueuedConnection);
  w.run();

  return app.exec();

  return 0;
}
