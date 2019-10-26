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
  void disconnect(QString why);

  //handlers  
  void login();
  void newUser();
  void editUser();
  void deleteUser();
  void newFile();
  void getFile();
  void editFile();
  void deleteFile();
  void fileDeleted();
  void getLink();
  void activateLink();
  void remoteInsert();
  void remoteDelete();
  void remoteUpdate();
  void userConnected();
  void userDisconnected();
  void remoteMove();
  void newDir();
  void editDir();
  void deleteDir();
  void getDir();
  void moveFile();

  std::shared_ptr<MessageManager> _manager;
};
