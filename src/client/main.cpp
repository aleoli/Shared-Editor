#include <iostream>

#include <QApplication>

#include "sys.h"

#include "prova_message.h"
#include "prova_file.h"
#include "exceptions.h"
#include "prova_textedit.h"
#include "prova_crdt.h"
#include "MainThread.h"

#define PROVA_MESSAGE 0
#define PROVA_FILE 0
#define PROVA_TEXTEDIT 0
#define PROVA_CRDT 0
#define PROVA_SERVER 1
#define MAINTHREAD 0

#if PROVA_SERVER
#include "test/prova_server.h"
#endif

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

  return 0;
}
