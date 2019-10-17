#pragma once

#include "message_processor.h"
#include "message_manager.h"

class ClientMessageProcessor: public MessageProcessor {
public:
  ClientMessageProcessor(const Message &m);

protected:
  virtual void process_error();
  virtual void process_user();
  virtual void process_file_edit();
  virtual void process_file();
  virtual void process_filesystem();

private:
  std::shared_ptr<MessageManager> _manager;
};
