#include <iostream>
#include "prova_message.h"

#define PROVA_MESSAGE 1

int main(int argc, char **argv) {
  std::cout << "hello world!" << std::endl;

#if PROVA_MESSAGE
  prova_message();
#endif

  return 0;
}
