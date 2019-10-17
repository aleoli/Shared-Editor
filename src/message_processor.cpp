#include "message_processor.h"

MessageProcessor::MessageProcessor(const Message &m)
  : _m(m), _has_resp(false) {
}

void MessageProcessor::process_message() {
  switch(this->_m.getType()) {
    case Message::Type::ERROR:
      process_error();
      break;
    case Message::Type::USER:
      process_user();
      break;
    case Message::Type::FILE:
      process_file();
      break;
    case Message::Type::FILE_EDIT:
      process_file_edit();
      break;
    case Message::Type::FILESYSTEM:
      process_filesystem();
      break;
    // non ci possono essere altri casi: l'eccezione sarebbe stata lanciata
    // quando il messaggio è stato deserializzato
  }
}

bool MessageProcessor::hasResponse() const {
  return this->_has_resp;
}

Message MessageProcessor::getResponse() const {
  return this->_res;
}
