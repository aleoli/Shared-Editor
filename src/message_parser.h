#pragma once

#include "Message.h"

#include <memory>

#include <QByteArray>

class MessageParser {
public:
  explicit MessageParser(const QByteArray& msg);

  virtual std::shared_ptr<Message> get();

protected:
  std::shared_ptr<Message> _msg;
};
