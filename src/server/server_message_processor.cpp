#include "server_message_processor.h"

//TODO rimuovi
#include <iostream>

ServerMessageProcessor::ServerMessageProcessor(const Message &m, quint64 clientId)
    : MessageProcessor(m), _clientId(clientId), _to_all(false) {
  _manager = MessageManager::get();
  this->process_message();
}

bool ServerMessageProcessor::shouldSendToAll() const {
  return this->_to_all;
}

void ServerMessageProcessor::process_error() {
  //TODO fai qualcosa
  std::cout << "ciao server" << std::endl;
}

void ServerMessageProcessor::process_user() {
  //TODO switch statement
  std::cout << "ciao server" << std::endl;
}

void ServerMessageProcessor::process_file() {
  //TODO switch statement
  std::cout << "ciao server" << std::endl;
}

void ServerMessageProcessor::process_file_edit() {
  //TODO switch statement
  std::cout << "ciao server" << std::endl;
}

void ServerMessageProcessor::process_filesystem() {
  //TODO switch statement
  std::cout << "ciao server" << std::endl;
}
