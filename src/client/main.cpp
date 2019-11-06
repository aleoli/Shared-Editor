#include <iostream>

#include <QApplication>

#include "sys.h"
#include "utils.h"

#define TEST 0
#define STUB 1

#if TEST
#include "prove.h"
#endif

#if STUB
#include "guiwrapper.h"
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

#if STUB
  GuiWrapper w(conf);
  QObject::connect(&w, SIGNAL(quit()), &app, SLOT(quit()));
  w.run();

  return app.exec();
#endif

  return 0;
}
