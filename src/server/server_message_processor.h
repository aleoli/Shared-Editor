#pragma once

#include "message_processor.h"
#include "message_manager.h"

#include <memory>

class ServerMessageProcessor: public MessageProcessor {
public:
  ServerMessageProcessor(const Message &m, quint64 clientId);

  bool shouldSendToAll() const;

protected:
  virtual void process_file();
  virtual void process_user();
  virtual void process_filesystem();

private:
  bool _to_all;
  quint64 _clientId;
  std::shared_ptr<MessageManager> _manager;
};
