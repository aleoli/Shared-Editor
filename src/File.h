#pragma once

#include "Symbol.h"

#include <vector>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QString>
#include <unordered_map>
#include "utils.h"
#include <shared_mutex>
#include <QDateTime>

class File {
public:
  typedef IdentifierBase CommentIdentifier;

  typedef struct {
    int userId;
    QString username;
    bool online;
  } UserInfo;

  typedef struct Comment {
    CommentIdentifier identifier;
    QString text;
    QDateTime creationDate;
  } Comment;

  File();
  File(const File &file);
  File(File &&file) noexcept;
  File(std::unordered_map<int, File::UserInfo> users, std::vector<Symbol> _symbols, std::map<CommentIdentifier, Comment> comments);
  explicit File(const QJsonObject &json);
  explicit File(QJsonObject &&json);

  File& operator=(const File& file);
  File& operator=(File&& file) noexcept;

  static File fromJsonObject(const QJsonObject &json);
  static File fromJsonObject(QJsonObject &&json);
  static File fromQByteArray(const QByteArray &array);
  static File fromQByteArray(QByteArray &&array);
  QJsonObject toJsonObject() const;
  QByteArray toQByteArray() const;

  static Comment commentFromJsonObject(const QJsonObject &obj);
  static QJsonObject commentToJsonObject(const Comment &comment);

  friend bool operator==(const File& lhs, const File& rhs);
  friend bool operator!=(const File& lhs, const File& rhs);
  friend bool operator==(const File::Comment& lhs, const File::Comment& rhs);
  friend bool operator==(const File::UserInfo& lhs, const File::UserInfo& rhs);

  std::unordered_map<int, File::UserInfo> getUsers() const;
  std::vector<Symbol> getSymbols() const;
  std::map<CommentIdentifier, Comment> getComments() const;
  Symbol& symbolAt(int pos);
  std::pair<int, Symbol&> symbolById(SymbolId id);
  int getPosition(SymbolId id);
  int numSymbols() const;
  std::string to_string() const;
  std::string text() const;
  void clear();

  void addUser(int userId, QString username);
  void removeUser(int userId);

  void setOnline(int userId, bool val);
  bool isOnline(int userId);

  QString getUsername(int userId);

  void store(const QString &path);

  // comments
  void remoteAddComment(const Comment& comment);
  void remoteUpdateComment(const Comment& comment);
  void remoteDeleteComment(const Comment& comment);

  //CRDT
  void localInsert(Symbol &sym, int pos);
  int remoteInsert(const Symbol &sym); // returns the position in which i inserted
  void localDelete(int pos);
  int remoteDelete(SymbolId id); // returns the position of the deleted element
  int remoteUpdate(const Symbol &sym);

private:
  void checkAndAssign(const QJsonObject &json);

  static void findPosition(int userId, std::vector<Symbol::Identifier> v1,
    std::vector<Symbol::Identifier> v2, std::vector<Symbol::Identifier> &position,
    int level = 0);

  QJsonArray usersToJsonArray() const;
  std::string usersToString() const; //TODO vedi se rimuovere
  static std::unordered_map<int, File::UserInfo> jsonArrayToUsers(const QJsonArray &array);

  std::string symbolsToString() const; //TODO vedi se rimuovere

  QJsonArray commentsToJsonArray() const;
  static std::map<CommentIdentifier, Comment> jsonArrayToComments(const QJsonArray &array);

  std::unordered_map<int, UserInfo> _users;
  std::vector<Symbol> _symbols;
  std::map<CommentIdentifier, Comment> _comments;

  bool dirty = false;
  mutable std::shared_mutex _mutex;
};
