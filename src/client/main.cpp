#include <iostream>
#include "prova_message.h"
#include "prova_file.h"
#include "Symbol.h"
#include "File.h"
#include "exceptions.h"
#include <string>
#include <vector>
#include "TextEdit.h"
#include "prova_textedit.h"
#include "prova_crdt.h"

#define PROVA_MESSAGE 0
#define PROVA_FILE 0
#define PROVA_TEXTEDIT 0
#define PROVA_CRDT 0
#define PROVA_SERVER 1

#if PROVA_SERVER
#include "test/prova_server.h"
#endif

int main(int argc, char **argv) {
  std::cout << "hello world!" << std::endl;

#if PROVA_MESSAGE
  prova_message();
#endif

#if PROVA_FILE
  prova_file();
#endif

#if PROVA_TEXTEDIT
  prova_textedit(argc, argv);
#endif

#if PROVA_CRDT
  prova_crdt();
#endif

#if PROVA_SERVER
  prova_server(argc, argv);
#endif

  return 0;
}
