#include "message_manager.h"

#include <iostream>

std::shared_ptr<MessageManager> MessageManager::instance = nullptr;

MessageManager::MessageManager(QObject *parent): QObject(parent) {
}

MessageManager::~MessageManager() {
}

std::shared_ptr<MessageManager> MessageManager::get() {
  if(instance == nullptr) {
    instance.reset(new MessageManager{});
  }
  return instance;
}

void MessageManager::process_data(quint64 client_id, QByteArray data) {
  auto it = this->_data_env.find(client_id);
  if(it == this->_data_env.end()) {
    Data data;
    data.client_id = client_id;
    this->_data_env[client_id] = data;
  }
  std::cout << "MM: " << data.data() << std::endl;
  // TODO: process message
  QByteArray out{"Ciao!!"};
  emit this->send_data(client_id, out);
}

void MessageManager::open_file(quint64 client_id, std::shared_ptr<File> file) {
  auto it = this->_data_env.find(client_id);
  if(it == this->_data_env.end()) {
    Data data;
    data.client_id = client_id;
    data.file = file;
    this->_data_env[client_id] = data;
  } else {
    it->second.file = file;
  }
  auto it2 = this->_open_files.find(file->getId());
  if(it2 == this->_open_files.end()) {
    std::list<quint64> l;
    l.push_back(client_id);
    this->_open_files[file->getId()] = l;
  } else {
    it2->second.push_back(client_id);
  }
}

void MessageManager::close_file(quint64 client_id, std::shared_ptr<File> file) {
  auto it = this->_open_files.find(file->getId());
  if(it != this->_open_files.end()) {
    it->second.remove_if([client_id](quint64 n) {
      return n == client_id;
    });
    if(it->second.empty()) {
      this->_open_files.erase(it);
    }
  }
}

void MessageManager::client_disconnected(quint64 client_id) {
  this->_data_env.erase(client_id);
  for(auto& it: this->_open_files) {
    it.second.remove_if([client_id](quint64 n) {
      return n == client_id;
    });
    if(it.second.empty()) {
      this->_open_files.erase(it.first);
    }
  }
}

void MessageManager::send_all(quint64 client_id, int file_id, QByteArray data) {
  auto it = this->_open_files.find(file_id);
  if(it != this->_open_files.end()) {
    std::list<quint64> l;
    for(auto& i: it->second) {
      if(client_id != i) {
        l.push_back(i);
      }
    }
    if(!l.empty()) {
      emit this->send_data(l, data);
    }
  }
}
