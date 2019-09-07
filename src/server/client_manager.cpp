#include "client_manager.h"

#include "client.h"

#include <iostream>

ClientManager::ClientManager(QObject *parent): QObject(parent) {
  connect(&this->s, SIGNAL(newConnection()), SLOT(newConnection()));
  if(!this->s.listen(QHostAddress::Any, 1234)) {
    std::cerr << "Non posso fare il bind alla porta 1234" << std::endl;
    exit(1);
  }
}

ClientManager::~ClientManager() {
  for(auto& v: this->clients) {
    delete v.second;
  }
}

void ClientManager::newConnection() {
  while(this->s.hasPendingConnections()) {
    auto cl = new Client{this->s.nextPendingConnection(), this->next_client_id};
		std::cout << this->thread() << std::endl;
		QThread *t = new QThread(cl);
		cl->moveToThread(t);
    connect(cl, SIGNAL(disconnected(int)), SLOT(disconnected(int)));
    this->clients[this->next_client_id] = cl;
		this->threads[this->next_client_id++] = t;
		t->start();
    cl->send("Bella!!");
  }
}

void ClientManager::disconnected(int id) {
	auto cl = this->clients[id];
  this->clients.erase(id);
	auto t = this->threads[id];
	this->threads.erase(id);
	t->quit();
	t->wait();
  delete cl;		// viene distrutto anche il thread perchè client era il suo parent
}
