#include "server_message_processor.h"

ServerMessageProcessor::ServerMessageProcessor(const Message &m, MessageManager::Data& data): MessageProcessor(m), _data(data) {
  this->process_message();
}

void ServerMessageProcessor::process_message() {
  MessageProcessor::process_message();
  this->_to_all = false;
}

int ServerMessageProcessor::fileId() const {
  return this->_data.file.get() != nullptr ? this->_data.file->getId() : -1;
}

bool ServerMessageProcessor::sendToAll() const {
  return this->_to_all;
}
