#pragma once

#include "Message.h"

class MessageProcessor {
public:
  explicit MessageProcessor(const Message &m);

  [[nodiscard]] bool hasResponse() const;
  [[nodiscard]] Message getResponse() const;

protected:
  virtual void process_message();
  virtual void process_error() = 0;
  virtual void process_user() = 0;
  virtual void process_file_edit() = 0;
  virtual void process_file() = 0;
  virtual void process_comment() = 0;
  virtual void process_filesystem() = 0;

  const Message& _m;
  Message _res;
  bool _has_resp;

private:
};
