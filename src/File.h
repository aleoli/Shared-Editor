#pragma once

#include "Symbol.h"

#include <vector>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QString>
#include <unordered_map>
#include "utils.h"

class File {
public:
  typedef struct {
    int userId;
    QString username;
    bool online;
  } UserInfo;

  File();
  File(std::unordered_map<int, File::UserInfo> users, std::vector<Symbol> _symbols);
  explicit File(const QJsonObject &json);
  explicit File(QJsonObject &&json);

  static File fromJsonObject(const QJsonObject &json);
  static File fromJsonObject(QJsonObject &&json);
  static File fromQByteArray(const QByteArray &array);
  static File fromQByteArray(QByteArray &&array);
  QJsonObject toJsonObject() const;
  QByteArray toQByteArray() const;

  friend bool operator==(const File& lhs, const File& rhs);
  friend bool operator!=(const File& lhs, const File& rhs);
  friend bool operator==(const File::UserInfo& lhs, const File::UserInfo& rhs);

  std::unordered_map<int, File::UserInfo> getUsers() const;
  std::vector<Symbol> getSymbols() const;
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

  std::unordered_map<int, UserInfo> _users;
  std::vector<Symbol> _symbols;
};
