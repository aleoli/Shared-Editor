#pragma once

#include "File.h"
#include "FSElement.h"
#include "Message.h"

#include <memory>
#include <vector>
#include <list>
#include <optional>

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
  void send_data(const QByteArray &data);
  void connection_error();

  // messaggi Server -> Client. La GUI deve collegarci gli slot
  void errorResponse(const QString &reason);

  void loginResponse(const QString &token, int userId, const std::optional<QString> &icon);
  void newUserResponse(const QString &token, int userId);
  void editUserResponse();
  void deleteUserResponse();
  void newFileResponse(int fileId);
  void getFileResponse(const File &file, int charId, int commentId);
  void editFileResponse();
  void deleteFileResponse();
  void fileDeletedQuery(int fileId);
  void getLinkResponse(const QString &link);
  void activateLinkResponse(const FSElement &element, const File &file);
  void getIconResponse(int userId, const QString &icon);
  void fileInfoResponse(const FSElement::FileInfo& fileInfo);

  void remoteInsertQuery(int fileId, int userId, const std::list<Symbol> &symbols, const std::list<Paragraph> &paragraphs);
  void remoteDeleteQuery(int fileId, int userId, const std::list<Identifier> &ids, const std::list<Identifier> &paragraphs);
  void remoteUpdateQuery(int fileId, int userId, const std::list<Symbol> &symbols, const std::list<Paragraph> &paragraphs);
  void userConnectedQuery(int fileId, int userId, const QString &username);
  void userDisconnectedQuery(int fileId, int userId);
  void remoteMoveQuery(int fileId, int userId, const SymbolId &symbolId, int cursorPosition);

  void newDirResponse(int dirId);
  void editDirResponse();
  void deleteDirResponse();
  void getDirResponse(const std::vector<FSElement> &elements, QString name, int parentId);
  void moveFileResponse();
  void getPathResponse(const std::vector<FSElement> &elements);
  void getAllDirsResponse(const std::list<std::pair<QString, int>> &items);

  void commentRemoteInsertQuery(int fileId, int userId, const File::Comment &comment);
  void commentRemoteUpdateQuery(int fileId, int userId, const File::Comment &comment);
  void commentRemoteDeleteQuery(int fileId, int userId, const File::Comment &comment);

public slots:
  void process_data(const QByteArray &data);

  // Messaggi Client -> Server. La GUI deve collegarci dei segnali
  void loginQuery(const QString &username, const QString &password);
  void logoutQuery(const QString &token);
  void newUserQuery(const QString &username, const QString &password, const QString &pswRepeat, const std::optional<QString> &icon);
  void editUserQuery(const QString &token, const std::optional<QString> &oldPassword, const std::optional<QString> &password, const std::optional<QString> &pswRepeat, const std::optional<QString> &icon);
  void deleteUserQuery(const QString &token);
  void getUserIconQuery(int userId);

  // newFileQuery: se il file è creato nella root, il dirId non è necessario
  void newFileQuery(const QString &token, const QString &name, const std::optional<int> &dirId = std::nullopt);
  void getFileQuery(const QString &token, int fileId);
  void closeFileQuery(const QString &token, int fileId);
  void editFileQuery(const QString &token, int fileId, const std::optional<QString> &name);
  void deleteFileQuery(const QString &token, int fileId);
  void getLinkQuery(const QString &token, int fileId);
  void activateLinkQuery(const QString &token, const QString &link);
  void getFileInfoQuery(const QString &token, int fileId);

  void localInsertQuery(const QString &token, int fileId, const std::list<Symbol> &symbols, const std::list<Paragraph> &paragraphs);
  void localDeleteQuery(const QString &token, int fileId, const std::list<Identifier> &ids, const std::list<Identifier> &paragraphs);
  void localUpdateQuery(const QString &token, int fileId, const std::list<Symbol> &symbols, const std::list<Paragraph> &paragraphs);
  void localMoveQuery(const QString &token, int fileId, const SymbolId &symbolId, int cursorPosition);

  // newDirQuery: se la dir è creata nella root, il parentId non è necessario
  void newDirQuery(const QString &token, const QString &name, const std::optional<int> &parentId = std::nullopt);
  void editDirQuery(const QString &token, int dirId, const std::optional<QString> &name, const std::optional<int> &parentId);
  void deleteDirQuery(const QString &token, int dirId);
  void getDirQuery(const QString &token, const std::optional<int> &dirId = std::nullopt);
  void moveFileQuery(const QString &token, int fileId, int dirId);
  void getPathQuery(const QString &token, int elementId);
  void getAllDirsQuery(const QString &token);

  void commentLocalInsertQuery(const QString &token, int fileId, const File::Comment &comment);
  void commentLocalUpdateQuery(const QString &token, int fileId, const File::Comment &comment);
  void commentLocalDeleteQuery(const QString &token, int fileId, const File::Comment &comment);

private:
  static std::shared_ptr<MessageManager> instance;
  explicit MessageManager(QObject *parent = nullptr);

  QString _token;
};
