#include "out_thread.h"

#include <iostream>

#include "socket.h"

OutThread::OutThread(std::atomic<bool>* _is_running, Socket *s): IOThread(_is_running, s) {
}

OutThread::OutThread(OutThread &&out_t): IOThread(out_t._is_running, nullptr) {
  this->s = out_t.s;
  out_t.s = nullptr;
}

void OutThread::action() {
  // va fatta un lista non bloccante da cui legge ogni tot e se c'è qualcosa invia
  std::cout << "";
}
