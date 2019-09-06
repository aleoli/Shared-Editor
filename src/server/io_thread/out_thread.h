#pragma once

#include "io_thread.h"
#include "NoBlockQueue.h"

class OutThread: public IOThread {
public:
  OutThread(OutThread &&out_t);
  explicit OutThread(std::atomic<bool>* _is_running, Socket *s, NoBlockQueue<std::string> *queue);

protected:
  virtual void action();

  NoBlockQueue<std::string> *queue;

};
