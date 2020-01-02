#pragma once

#include "message_processor.h"
#include "message_manager.h"

#include <memory>

class ServerMessageProcessor: public MessageProcessor {
public:
  ServerMessageProcessor(const Message &m, quint64 clientId);

  bool shouldSendToAll() const;

protected:
  virtual void process_error();
  virtual void process_user();
  virtual void process_file_edit();
  virtual void process_file();
  virtual void process_filesystem();

private:
  bool _to_all = false;
  quint64 _clientId;
  std::shared_ptr<MessageManager> _manager;

  void disconnect(QString why);

  // handlers

  void login();
  void logout();
  void newUser();
  void editUser();
  void deleteUser();

  void newFile();
  void getFile();
  void closeFile();
  void editFile();
  void deleteFile();
  void getLink();
  void activateLink();

  void localInsert();
  void localDelete();
  void localUpdate();
  void localMove();

  void newDir();
  void editDir();
  void deleteDir();
  void getDir();
  void moveFile();
};
