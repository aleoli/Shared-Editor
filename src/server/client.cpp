#include "client.h"

#include "session.h"
#include "io_thread/in_thread.h"
#include "io_thread/out_thread.h"
#include "socket.h"

Client::Client(Socket s) {
  this->id = 0;
  this->socket = std::move(s);
  this->socket.setTerminator(PKG_TERMINATOR);
}

Client::Client(Client &&c) {
  this->id = c.id;
  this->_session = c._session;
  c._session = nullptr;
  this->socket = std::move(c.socket);
}

Client::~Client() {
  if(this->_session != nullptr) {
    delete this->_session;
    this->_session = nullptr;
  }
  if(this->in_thread != nullptr) {
    this->_in_running.store(false);
    this->in_thread->join();
    delete this->in_thread;
    this->in_thread = nullptr;
  }
  if(this->out_thread != nullptr) {
    this->_out_running.store(false);
    this->out_thread->join();
    delete this->out_thread;
    this->out_thread = nullptr;
  }
}

void Client::operator()() {
  this->_in_running.store(true);
  InThread in_t{&this->_in_running, &this->socket};
  this->in_thread = new std::thread(std::move(in_t));

  this->_out_running.store(true);
  OutThread out_t{&this->_out_running, &this->socket, &this->out_queue};
  this->out_thread = new std::thread(std::move(out_t));
}

void Client::send(std::string msg) {
  this->out_queue.push(msg + PKG_TERMINATOR);
}
