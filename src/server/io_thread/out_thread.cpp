#include "out_thread.h"

#include <iostream>

OutThread::OutThread(std::atomic<bool>* _is_running) {
  this->_is_running = _is_running;
}

OutThread::OutThread(OutThread &&out_t) {
  this->_is_running = out_t._is_running;
}

void OutThread::action() {
  std::cout << "ciao!! ";
}
