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
  //TODO vedere lo stub per come bisogna agire:
  // per collegare gli opportuni signal/slot serve una classe "Wrapper" che inglobi 
  //tutte le componenti principali, sia grafiche che non
#endif

#if STUB
  GuiWrapper w(conf);
  QObject::connect(&w, SIGNAL(quit()), &app, SLOT(quit()), Qt::QueuedConnection);
  w.run();

  return app.exec();
#endif

  return 0;
}
