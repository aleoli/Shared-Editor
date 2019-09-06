#include "io_thread.h"

#include <iostream>

IOThread::IOThread(std::atomic<bool>* _is_running, Socket *s): _is_running(_is_running), s(s) {
}

IOThread::IOThread(IOThread &&io_t): _is_running(io_t._is_running) {
  this->s = io_t.s;
  io_t.s = nullptr;
}

IOThread::~IOThread() {
}

void IOThread::operator()() {
  this->_is_running->store(true);

  while(this->is_running()) {
    this->action();
  }
}

bool IOThread::is_running() {
  return this->_is_running->load();
}

void IOThread::action() {
}
