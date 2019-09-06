#include "out_thread.h"

#include <iostream>

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
  {
    std::unique_lock<std::mutex> ul(this->queue->getMutex());
    if(this->queue->empty_safe() && this->is_running()) {
      this->queue->getCv().wait(ul);      // esco da qua se c'è qualcosa da inviare o se il thread deve essere chiuso
                                          // altre eventuali uscite si bloccano di nuovo al prossimo richiamo di action()
    }
  }

  std::string str = this->queue->pop();
  if(str != "") {
    *this->s << str;
  }
}
