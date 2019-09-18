#pragma once

#include "Message.h"

class MessageProcessor {
public:
  MessageProcessor(const Message &m);

  virtual operator Message() const;

protected:
  virtual void process_message();

  const Message& _m;
  Message _res;

private:
  void dumb();
};
