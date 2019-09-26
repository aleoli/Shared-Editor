#pragma once

#include "message_processor.h"
#include "message_manager.h"

class ServerMessageProcessor: public MessageProcessor {
public:
  ServerMessageProcessor(const Message &m, MessageManager::Data& data);

  bool shouldSendToAll() const;
  int fileId() const;

protected:
  virtual void process_file();
  virtual void process_user();
  virtual void process_filesystem();

private:
  bool _to_all;
  MessageManager::Data& _data;
};
