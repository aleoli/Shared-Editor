#pragma once

#include "message_parser.h"

class ServerMessageParser: public MessageParser {
public:
  explicit ServerMessageParser(const QByteArray& msg);
};
