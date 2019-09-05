#include "in_thread.h"

#include <iostream>

#include "socket.h"

InThread::InThread(std::atomic<bool>* _is_running, Socket *s): IOThread(_is_running, s) {
}

InThread::InThread(InThread &&in_t): IOThread(in_t._is_running, nullptr) {
  this->s = in_t.s;
  in_t.s = nullptr;
}

void InThread::action() {
  bool has_res = false;
  auto str = this->s->_recv(has_res);
  std::cout << str;
}
