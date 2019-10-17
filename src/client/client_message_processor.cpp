#include "client_message_processor.h"

//TODO rimuovi
#include <iostream>

ClientMessageProcessor::ClientMessageProcessor(const Message &m): MessageProcessor(m) {
  _manager = MessageManager::get();
  this->process_message();
}

void ClientMessageProcessor::process_error() {
  //TODO fai qualcosa
  std::cout << "ciao client" << std::endl;
}

void ClientMessageProcessor::process_user() {
  //TODO switch statement
  std::cout << "ciao client" << std::endl;
}

void ClientMessageProcessor::process_file() {
  //TODO switch statement
  std::cout << "ciao client" << std::endl;
}

void ClientMessageProcessor::process_file_edit() {
  //TODO switch statement
  std::cout << "ciao client" << std::endl;
}

void ClientMessageProcessor::process_filesystem() {
  //TODO switch statement
  std::cout << "ciao client" << std::endl;
}
