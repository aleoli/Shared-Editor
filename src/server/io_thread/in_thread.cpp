#include "in_thread.h"

#include <iostream>

#include "socket.h"

InThread::InThread(std::atomic<bool>* _is_running, Socket *s): IOThread(_is_running, s) {
  // in questo modo leggerà in manira non bloccante finchè non trovarà una 'a'
  this->s->setTerminator('a');
}

InThread::InThread(InThread &&in_t): IOThread(in_t._is_running, nullptr) {
  this->s = in_t.s;
  in_t.s = nullptr;
}

void InThread::action() {
  std::string str;
  *this->s >> str;
  if(str != "") {
    // TODO: in questo punto si inserisce il Messagge::fromJson
    std::cout << std::endl << std::endl << str << std::endl;
  }
}
