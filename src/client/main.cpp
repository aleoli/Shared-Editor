#include <iostream>
#include "prova_message.h"
#include "prova_file.h"
#include "Symbol.h"
#include "File.h"
#include "exceptions.h"
#include <string>
#include <vector>

#define PROVA_MESSAGE 1
#define PROVA_FILE 1

int main(int argc, char **argv) {
  std::cout << "hello world!" << std::endl;

#if PROVA_MESSAGE
  prova_message();
#endif

#if PROVA_FILE
  prova_file();
#endif

  return 0;
}
