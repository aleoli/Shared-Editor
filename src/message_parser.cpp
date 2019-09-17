#include "message_parser.h"

#include <QJsonDocument>

MessageParser::MessageParser(const QByteArray& msg) {
#if SAVE_BINARY
  auto doc = QJsonDocument::fromBinaryData(msg);
#else
  auto doc = QJsonDocument::fromJson(msg);
#endif
  this->_msg = std::make_shared<Message>(doc.object());
}

std::shared_ptr<Message> MessageParser::get() {
  return this->_msg;
}
