#pragma once

#include "Symbol.h"
#include "Identifier.h"
#include "Paragraph.h"

#include <list>
#include <vector>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QString>
#include <unordered_map>
#include "utils.h"
#include <shared_mutex>
#include <QDateTime>
#include <functional>
#include <QTextBlockFormat>

class File {
public:
  typedef Identifier CommentIdentifier;

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
  File(std::unordered_map<int, File::UserInfo> users, std::list<Symbol> _symbols, std::list<Paragraph> paragraphs, std::map<CommentIdentifier, Comment> comments);
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
  std::list<Symbol> getSymbols() const;
  std::list<Symbol>::iterator getSymbolsStart();
  std::list<Paragraph>::iterator getParagraphsStart();
  std::list<Paragraph>::iterator getParagraphsEnd();
  std::map<CommentIdentifier, Comment> getComments() const;
  Symbol& symbolAt(int pos, std::list<Symbol>::iterator *it = nullptr);
  std::list<Symbol>::iterator iteratorAt(int pos);
  std::pair<int, Symbol&> symbolById(const SymbolId &id, std::list<Symbol>::iterator *it = nullptr);
  std::pair<int, std::list<Symbol>::iterator> iteratorById(const SymbolId &id, std::list<Symbol>::iterator *it = nullptr);
  int getPosition(const SymbolId &id, std::list<Symbol>::iterator *it = nullptr);
  int numSymbols() const;
  std::string to_string() const;
  std::string text() const;
  void clear();
  void forEachSymbol(const std::function<void(const Symbol&) >& lambda);
  void forEachParagraph(const std::function<void(const Paragraph&) >& lambda);

  void addUser(int userId, const QString &username);
  void removeUser(int userId);

  void setOnline(int userId, bool val);
  bool isOnline(int userId);

  QString getUsername(int userId);

  void store(const QString &path);

  // comments
  void remoteAddComment(const Comment& comment);
  void remoteUpdateComment(Comment comment);
  void remoteDeleteComment(const Comment& comment);

  //CRDT
  void localInsert(Symbol &sym, int pos, std::list<Symbol>::iterator *it = nullptr);
  int remoteInsert(const Symbol &sym, std::list<Symbol>::iterator *it = nullptr, int oldPos = 0); // returns the position in which i inserted
  void localDelete(int pos, std::list<Symbol>::iterator *it = nullptr);
  int remoteDelete(const SymbolId &id, std::list<Symbol>::iterator *it = nullptr, int oldPos = 0); // returns the position of the deleted element
  std::optional<std::list<Symbol>::iterator> localUpdate(const QTextCharFormat &fmt, int pos, bool ignoreBackground = false, std::list<Symbol>::iterator *it = nullptr);
  int remoteUpdate(const Symbol &sym, int userId, const QDateTime &timestamp, std::list<Symbol>::iterator *it = nullptr, int oldPos = 0);

  // paragraphs
  void localInsertParagraph(Paragraph &par, int pos, std::list<Paragraph>::iterator *it = nullptr);
  int remoteInsertParagraph(const Paragraph &par, std::list<Paragraph>::iterator *it = nullptr, int oldPos = -1);
  void localDeleteParagraph(int pos, std::list<Paragraph>::iterator *it = nullptr);
  int remoteDeleteParagraph(const ParagraphId &id, std::list<Paragraph>::iterator *it = nullptr, int oldPos = -1);
  std::pair<std::list<Paragraph>::iterator, bool> localUpdateParagraph(const QTextBlockFormat &fmt, int pos, std::list<Paragraph>::iterator *it = nullptr);
  int remoteUpdateParagraph(const Paragraph &par, int userId, const QDateTime &timestamp, std::list<Paragraph>::iterator *it = nullptr, int oldPos = -1);
  std::list<Paragraph>::iterator paragraphAt(int pos);
  std::pair<int, std::list<Paragraph>::iterator> paragraphById(const ParagraphId &id, std::list<Paragraph>::iterator *it = nullptr);

private:
  Symbol& _symbolAt(int pos, std::list<Symbol>::iterator *it = nullptr);
  std::pair<int, Symbol&> _symbolById(const SymbolId &id, std::list<Symbol>::iterator *it = nullptr);
  std::pair<int, std::list<Symbol>::iterator> _iteratorById(const SymbolId &id, std::list<Symbol>::iterator *it = nullptr);
  int _getPosition(const SymbolId &id, std::list<Symbol>::iterator *it = nullptr);
  void checkAndAssign(const QJsonObject &json);

  static void findPosition(int userId, std::vector<Identifier> &v1,
    std::vector<Identifier> &v2, std::vector<Identifier> &position,
    int level = 0);
  static int generateDigit(int digit1, int digit2);

  QJsonArray usersToJsonArray() const;
  static std::unordered_map<int, File::UserInfo> jsonArrayToUsers(const QJsonArray &array);

  QJsonArray commentsToJsonArray() const;
  static std::map<CommentIdentifier, Comment> jsonArrayToComments(const QJsonArray &array);

  std::unordered_map<int, UserInfo> _users;
  std::list<Symbol> _symbols;
  std::list<Paragraph> _paragraphs;
  std::map<CommentIdentifier, Comment> _comments;

  bool dirty = false;
  mutable std::shared_mutex _mutex;
};
