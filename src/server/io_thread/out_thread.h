#pragma once

#include "io_thread.h"

class OutThread: public IOThread {
public:
  OutThread(OutThread &&out_t);
  explicit OutThread(std::atomic<bool>* _is_running);

protected:
  virtual void action();

};
