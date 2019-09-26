#include "message_processor.h"

MessageProcessor::MessageProcessor(const Message &m)
  : _m(m), _has_resp(false) {
}

void MessageProcessor::process_message() {
  switch(this->_m.getType()) {
    case Message::Type::FILE:
      process_file();
      break;
    case Message::Type::USER:
      process_user();
      break;
    case Message::Type::FILESYSTEM:
      process_filesystem();
      break;
  }
  this->dumb();
}

void MessageProcessor::dumb() {
  this->_res = this->_m;
  this->_has_resp = true;
}

bool MessageProcessor::hasResponse() const {
  return this->_has_resp;
}

Message MessageProcessor::getResponse() const {
  return this->_res;
}
