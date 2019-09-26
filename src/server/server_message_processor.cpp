#include "server_message_processor.h"

//TODO rimuovi
#include <iostream>

ServerMessageProcessor::ServerMessageProcessor(const Message &m, MessageManager::Data& data)
  : MessageProcessor(m), _data(data), _to_all(false) {
  this->process_message();
}

int ServerMessageProcessor::fileId() const {
  return this->_data.file.get() != nullptr ? this->_data.file->getId() : -1;
}

bool ServerMessageProcessor::shouldSendToAll() const {
  return this->_to_all;
}

void ServerMessageProcessor::process_file() {
  //TODO switch statement
  std::cout << "ciao server" << std::endl;
}

void ServerMessageProcessor::process_user() {
  //TODO switch statement
  std::cout << "ciao server" << std::endl;
}

void ServerMessageProcessor::process_filesystem() {
  //TODO switch statement
  std::cout << "ciao server" << std::endl;
}
