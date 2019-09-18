#pragma once

#include "message_processor.h"

class ClientMessageProcessor: public MessageProcessor {
public:
  ClientMessageProcessor(const Message &m);

protected:
  virtual void process_message();
};
