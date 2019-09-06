#include "out_thread.h"

#include <iostream>
#include <chrono>
#include <thread>

#include "socket.h"

OutThread::OutThread(std::atomic<bool>* _is_running, Socket *s, NoBlockQueue<std::string> *queue): IOThread(_is_running, s) {
  this->queue = queue;
}

OutThread::OutThread(OutThread &&out_t): IOThread(out_t._is_running, nullptr) {
  this->s = out_t.s;
  out_t.s = nullptr;
  this->queue = out_t.queue;
}

void OutThread::action() {
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  std::string str = this->queue->pop();
  if(str != "") {
    *this->s << str;
  }
}
