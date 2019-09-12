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
		QThread *t = new QThread(cl);
		cl->moveToThread(t);
		connect(cl, SIGNAL(dataReady(int, QByteArray)), this, SIGNAL(dataReceived(int, QByteArray)));
    connect(cl, SIGNAL(disconnected(int)), SLOT(disconnected(int)));
		connect(this, SIGNAL(closeClient(int)), cl, SLOT(disconnect(int)));
    this->_clients[this->_next_client_id] = cl;
		this->_threads[this->_next_client_id++] = t;
		t->start();
    // TODO: è una prova
    cl->send("Bella!!");
  }
}

void ClientManager::disconnected(int id) {
	auto cl = this->_clients[id];
  this->_clients.erase(id);
	auto t = this->_threads[id];
	this->_threads.erase(id);
	t->quit();
	t->wait();
  delete cl;		// viene distrutto anche il thread perchè client era il suo parent
}

void ClientManager::onCloseClient(int id) {
	emit this->closeClient(id);
}
