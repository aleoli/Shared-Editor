#pragma once

#include "File.h"
#include "FSElement.h"

#include <memory>
#include <vector>

class MessageManager: public QObject {
  Q_OBJECT
public:
  MessageManager(const MessageManager&) = delete;
  MessageManager(MessageManager&&) = delete;
  MessageManager& operator=(const MessageManager&) = delete;
  MessageManager& operator=(MessageManager&&) = delete;

  static std::shared_ptr<MessageManager> get();

  ~MessageManager();

signals:
  void send_data(QByteArray data);
  void connection_error();

  // API per la GUI
  void errorResponse(QString reason);

  void loginResponse(QString token, QString nickname, QString icon); //ATTENZIONE nickname e icon possono essere nullptr!
  void newUserResponse(QString token);
  void editUserResponse();
  void deleteUserResponse();
  void newFileResponse(int fileId);
  void getFileResponse(File file, int charId);
  void editFileResponse();
  void deleteFileResponse();
  void fileDeletedQuery(int fileId);
  void getLinkResponse(QString link);
  void activateLinkResponse(FSElement element, File file);

  void remoteInsertQuery(std::vector<Symbol> symbols);
  void remoteDeleteQuery(std::vector<SymbolId> ids);
  void remoteUpdateQuery(std::vector<Symbol> symbols);
  void userConnectedQuery(int fileId, int clientId, QString username);
  void userDisconnectedQuery(int fileId, int clientId);
  void remoteMoveQuery(int fileId, int clientId, SymbolId symbolId, int cursorPosition);

  void newDirResponse(int dirId);
  void editDirResponse();
  void deleteDirResponse();
  void getDirResponse(std::vector<FSElement> elements);
  void moveFileResponse();

public slots:
  void process_data(QByteArray data);

private:
  static std::shared_ptr<MessageManager> instance;
  explicit MessageManager(QObject *parent = nullptr);

  QString _token;
};
