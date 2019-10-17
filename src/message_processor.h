#pragma once

#include "Message.h"

class MessageProcessor {
public:
  MessageProcessor(const Message &m);

  bool hasResponse() const;
  Message getResponse() const;

protected:
  virtual void process_message();
  virtual void process_error() = 0;
  virtual void process_user() = 0;
  virtual void process_file_edit() = 0;
  virtual void process_file() = 0;
  virtual void process_filesystem() = 0;

  const Message& _m;
  Message _res;
  bool _has_resp;

private:
};
