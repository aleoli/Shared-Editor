#pragma once

#include "io_thread.h"

class InThread: public IOThread {
public:
  InThread(InThread &&in_t);
  explicit InThread(std::atomic<bool>* _is_running);

protected:
  virtual void action();

};
