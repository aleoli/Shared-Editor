#include "client_message_processor.h"

ClientMessageProcessor::ClientMessageProcessor(const Message &m): MessageProcessor(m) {
  this->process_message();
}

void ClientMessageProcessor::process_message() {
  MessageProcessor::process_message();
  this->_has_resp = false;
}
