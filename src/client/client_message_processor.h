#pragma once

#include "message_processor.h"
#include "message_manager.h"

class ClientMessageProcessor: public MessageProcessor {
public:
  explicit ClientMessageProcessor(const Message &m);

protected:
  void process_error() override;
  void process_user() override;
  void process_file_edit() override;
  void process_file() override;
  void process_comment() override;
  void process_filesystem() override;

private:
  void disconnect(const QString& why);

  //handlers
  void login();
  void newUser();
  void editUser();
  void deleteUser();
  void getUserIcon();
  void newFile();
  void getFile();
  void editFile();
  void deleteFile();
  void fileDeleted();
  void getLink();
  void activateLink();
  void getFileInfo();
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
  void getPath();
  void getAllDirs();
  void newComment();
  void updateComment();
  void deleteComment();

  std::shared_ptr<MessageManager> _manager;
};
