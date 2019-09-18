#pragma once

#include "message_processor.h"
#include "message_manager.h"

class ServerMessageProcessor: public MessageProcessor {
public:
  ServerMessageProcessor(const Message &m, MessageManager::Data& data);

  bool sendToAll() const;
  int fileId() const;

protected:
  virtual void process_message();

private:
  bool _to_all;
  MessageManager::Data& _data;
};
