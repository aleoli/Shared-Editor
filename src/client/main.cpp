#include <iostream>

#include <QApplication>

#include "sys.h"
#include "MainThread.h"

#include "prove.h"

int main(int argc, char **argv) {
  QApplication app(argc, argv);
  SysConf conf = initiate_system(app);

#if PROVA_MESSAGE
  prova_message();
#endif

#if PROVA_FILE
  prova_file();
#endif

#if PROVA_TEXTEDIT
  prova_textedit(app);
#endif

#if PROVA_CRDT
  prova_crdt();
#endif

#if PROVA_SERVER
  prova_server(app, conf);
  return app.exec();
#endif

#if MAINTHREAD
  MainThread mthr{app};
  mthr.start();
#endif

#if PROVA_IMAGE
  prova_image(app);
#endif

#if PROVA_FSELEMENT
  prova_fselement();
#endif

  return 0;
}
