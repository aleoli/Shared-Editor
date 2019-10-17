#pragma once

#include <QApplication>

#include "prova_message.h"
#include "prova_file.h"
#include "exceptions.h"
#include "prova_textedit.h"
#include "prova_crdt.h"
#include "prova_image.h"
#include "prova_fselement.h"
#include "prova_server.h"

#define PROVA_MESSAGE 0
#define PROVA_FILE 0
#define PROVA_TEXTEDIT 0
#define PROVA_CRDT 0
#define PROVA_SERVER 0
#define MAINTHREAD 0
#define PROVA_IMAGE 0
#define PROVA_FSELEMENT 0

void prova(QApplication &app) {
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
}
