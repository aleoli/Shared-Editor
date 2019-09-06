#pragma once

#define PKG_TERMINATOR 'a'

#include "dependencies.h"
#include "socket.h"
#include "NoBlockQueue.h"

#include <thread>
#include <atomic>

class Client {
public:
  Client(const Client &c) = delete;
  Client(Client &&c);
  explicit Client(Socket s);

  ~Client();

  Client& operator=(const Client &c) = delete;
  void operator()();

  void send(std::string msg);

private:
  int id;
  Session* _session = nullptr;
  std::thread* in_thread = nullptr;
  std::atomic<bool> _in_running;
  std::thread* out_thread = nullptr;
  std::atomic<bool> _out_running;

  Socket socket;
  NoBlockQueue<std::string> out_queue;

};
