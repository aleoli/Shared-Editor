#include "client_manager.h"

#include "client.h"

#include <iostream>
#include <utility>
#include <utils.h>

std::shared_ptr<ClientManager> ClientManager::instance = nullptr;

ClientManager::ClientManager(int port, QObject *parent): QObject(parent) {
  connect(&this->_s, SIGNAL(newConnection()), SLOT(newConnection()));
  if(!this->_s.listen(QHostAddress::Any, port)) {
    std::cerr << "Non posso fare il bind alla porta " << port << std::endl;
    exit(1);
  }
}

ClientManager::~ClientManager() {
  for(auto& v: this->_clients) {
    delete v.second;
  }
}

std::shared_ptr<ClientManager> ClientManager::get(int port) {
  if(instance == nullptr) {
    instance.reset(new ClientManager{port});
  }
  return instance;
}

void ClientManager::newConnection() {
  while(this->_s.hasPendingConnections()) {
    auto cl = new Client{this->_s.nextPendingConnection(), this->_next_client_id};
    auto *t = new QThread(cl);
    cl->moveToThread(t);
    connect(cl, SIGNAL(dataReady(quint64, QByteArray)), this, SIGNAL(dataReceived(quint64, QByteArray)));
    connect(cl, SIGNAL(disconnected(quint64)), SLOT(disconnected(quint64)));
    connect(this, SIGNAL(closeClient(quint64)), cl, SLOT(disconnect(quint64)));
    connect(cl, SIGNAL(disconnected(quint64)), this, SIGNAL(closeClient(quint64)));
    connect(this, SIGNAL(force_close(quint64)), cl, SLOT(disconnect(quint64)));
    connect(this, SIGNAL(send_data(quint64, QByteArray)), cl, SLOT(send(quint64, QByteArray)));
    connect(this, &ClientManager::quit, cl, &Client::quit, Qt::QueuedConnection);
    this->_clients[this->_next_client_id] = cl;
    this->_threads[this->_next_client_id++] = t;
    t->start();
  }
}

void ClientManager::disconnected(quint64 id) {
  auto cl = this->_clients[id];
  this->_clients.erase(id);
  auto t = this->_threads[id];
  this->_threads.erase(id);
  t->quit();
  t->wait();
  delete cl;		// viene distrutto anche il thread perchè client era il suo parent
}

void ClientManager::quitSlot() {
  emit quit();
  for(auto& th: this->_threads) {
    th.second->quit();
    th.second->wait();
  }
}

void ClientManager::onCloseClient(quint64 id) {
  emit this->closeClient(id);
}

void ClientManager::sendData(quint64 client_id, QByteArray data) {
  emit this->send_data(client_id, std::move(data));
}

void ClientManager::sendData(const std::list<quint64>& client_list, const QByteArray& data) {
  for(auto& client_id: client_list) {
    emit this->send_data(client_id, data);
  }
}
