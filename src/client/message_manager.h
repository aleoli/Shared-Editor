#pragma once

#include "File.h"
#include "FSElement.h"
#include "Message.h"

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

  // messaggi Server -> Client. La GUI deve collegarci gli slot
  void errorResponse(QString reason);

  void loginResponse(QString token, int userId, QString nickname, QString icon); //ATTENZIONE nickname e icon possono essere nullptr!
  void newUserResponse(QString token, int userId);
  void editUserResponse();
  void deleteUserResponse();
  void newFileResponse(int fileId);
  void getFileResponse(File file, int charId);
  void editFileResponse();
  void deleteFileResponse();
  void fileDeletedQuery(int fileId);
  void getLinkResponse(QString link);
  void activateLinkResponse(FSElement element, File file);

  void remoteInsertQuery(int fileId, std::vector<Symbol> symbols);
  void remoteDeleteQuery(int fileId, std::vector<SymbolId> ids);
  void remoteUpdateQuery(int fileId, std::vector<Symbol> symbols);
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

  // Messaggi Client -> Server. La GUI deve collegarci dei segnali
  void loginQuery(QString username, QString password);
  void logoutQuery(QString token);
  void newUserQuery(QString username, QString password, QString pswRepeat);
  // editUserQuery: impostare a nullptr i campi che non si vogliono modificare
  void editUserQuery(QString token, QString nickname, QString oldPassword,
    QString password, QString pswRepeat, QString icon);
  void deleteUserQuery(QString token);

  // newFileQuery: se il file è creato nella root, impostare dirId = 0 oppure non settare proprio
  void newFileQuery(QString token, QString name, int dirId = 0);
  void getFileQuery(QString token, int fileId);
  void closeFileQuery(QString token, int fileId);
  // editFileQuery: impostare a nullptr i campi che non si vogliono modificare
  void editFileQuery(QString token, int fileId, QString name);
  void deleteFileQuery(QString token, int fileId);
  void getLinkQuery(QString token, int fileId);
  void activateLinkQuery(QString token, QString link);

  void localInsertQuery(QString token, int fileId, std::vector<Symbol> symbols);
  void localDeleteQuery(QString token, int fileId, std::vector<SymbolId> ids);
  void localUpdateQuery(QString token, int fileId, std::vector<Symbol> symbols);
  void localMoveQuery(QString token, int fileId, SymbolId symbolId, int cursorPosition);

  // newDirQuery: se la dir è creata nella root, impostare parentId = 0 oppure non settare proprio
  void newDirQuery(QString token, QString name, int parentId = 0);
  // editDirQuery: se non modifico il nome -> name = nullptr, se non modifico la cartella -> parentId = -1
  void editDirQuery(QString token, int dirId, QString name, int parentId);
  void deleteDirQuery(QString token, int dirId);
  void getDirQuery(QString token, int dirId);
  void moveFileQuery(QString token, int fileId, int dirId);

private:
  static std::shared_ptr<MessageManager> instance;
  explicit MessageManager(QObject *parent = nullptr);

  QString _token;
};
