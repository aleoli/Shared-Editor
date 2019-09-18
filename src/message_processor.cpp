#include "message_processor.h"

MessageProcessor::MessageProcessor(const Message &m): _m(m) {
}

MessageProcessor::operator Message() const {
  return this->_res;
}

void MessageProcessor::process_message() {
  switch(this->_m.getType()) {
    case Message::Type::FILE:
      break;
    case Message::Type::USER:
      break;
    case Message::Type::FILESYSTEM:
      break;
  }
  this->dumb();
}

void MessageProcessor::dumb() {
  this->_res = this->_m;
  this->_has_resp = true;
}

bool MessageProcessor::hasResp() const {
  return this->_has_resp;
}
