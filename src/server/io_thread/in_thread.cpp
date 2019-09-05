#include "in_thread.h"

#include <iostream>

InThread::InThread(std::atomic<bool>* _is_running) {
  this->_is_running = _is_running;
}

InThread::InThread(InThread &&in_t) {
  this->_is_running = in_t._is_running;
}

void InThread::action() {
  std::cout << "ciao!! ";
}
