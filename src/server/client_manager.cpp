#include "client_manager.h"

#include "client.h"

#include <iostream>

ClientManager::ClientManager(QObject *parent): QObject(parent) {
  connect(&this->_s, SIGNAL(newConnection()), SLOT(newConnection()));
  if(!this->_s.listen(QHostAddress::Any, 1234)) {
    std::cerr << "Non posso fare il bind alla porta 1234" << std::endl;
    exit(1);
  }
}

ClientManager::~ClientManager() {
  for(auto& v: this->_clients) {
    delete v.second;
  }
}

void ClientManager::newConnection() {
  while(this->_s.hasPendingConnections()) {
    auto cl = new Client{this->_s.nextPendingConnection(), this->_next_client_id};
    connect(cl, SIGNAL(disconnected(int)), SLOT(disconnected(int)));
    this->_clients[this->_next_client_id++] = cl;
    cl->send("Bella!!");
  }
}

void ClientManager::disconnected(int id) {
  auto cl = this->_clients[id];
  this->_clients.erase(id);
  delete cl;
}
