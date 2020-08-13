#pragma once

#include "message_processor.h"
#include "message_manager.h"

#include <memory>

class ServerMessageProcessor: public MessageProcessor {
public:
  ServerMessageProcessor(const Message &m, quint64 clientId);

  [[nodiscard]] bool shouldSendToAll() const;

protected:
  void process_error() override;
  void process_user() override;
  void process_file_edit() override;
  void process_file() override;
  void process_comment() override;
  void process_filesystem() override;

private:
  bool _to_all = false;
  quint64 _clientId;
  std::shared_ptr<MessageManager> _manager;

  static void delete_lambda(int link_id, int owner_id);

  static void disconnect(const QString& why);

  void sendErrorMsg(const QString& reason);

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

  void newComment();
  void updateComment();
  void deleteComment();

  void newDir();
  void editDir();
  void deleteDir();
  void getDir();
  void moveFile();
};
