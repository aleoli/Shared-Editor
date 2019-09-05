#pragma once

#include <atomic>

#include "../dependencies.h"

class IOThread {
public:
  IOThread(const IOThread &io_t) = delete;
  IOThread(IOThread &&io_t);
  IOThread() = delete;
  explicit IOThread(std::atomic<bool>* _is_running, Socket *s);

  virtual ~IOThread();

  virtual IOThread& operator=(const IOThread &io_t) = delete;
  virtual void operator()();

protected:
  bool is_running();
  virtual void action();

  std::atomic<bool>* _is_running;
  Socket *s;

};
