#pragma once

#include "Symbol.h"

#include <vector>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

class File {
public:
  File();
  explicit File(int id);
  File(int id, std::vector<int> user_ids, std::vector<Symbol> _symbols);
  explicit File(const QJsonObject &json);
  explicit File(QJsonObject &&json);

  static File fromJsonObject(const QJsonObject &json);
  static File fromJsonObject(QJsonObject &&json);
  static File fromQByteArray(const QByteArray &array);
  static File fromQByteArray(QByteArray &&array);
  QJsonObject toJsonObject() const;
  QByteArray toQByteArray() const;

  int getId() const;
  std::vector<int> getUserIds() const;
  std::vector<Symbol> getSymbols() const;
  Symbol& symbolAt(int pos);
  Symbol& symbolById(SymbolId id);
  int numSymbols() const;
  std::string to_string() const;
  std::string text() const;

  void addUserId(int id);

  //CRDT
  void localInsert(Symbol &sym, int pos);
  void remoteInsert(const Symbol &sym);
  void localDelete(int pos);
  void remoteDelete(SymbolId id);
  //TODO per aggiornare simboli?? (es mettere il grassetto..)

private:
  void checkAndAssign(const QJsonObject &json);

  static void findPosition(int clientId, std::vector<Symbol::Identifier> v1,
    std::vector<Symbol::Identifier> v2, std::vector<Symbol::Identifier> &position,
    int level = 0);

  QJsonArray userIdsToJsonArray() const;
  std::string userIdsToString() const; //TODO vedi se rimuovere
  static std::vector<int> jsonArrayToUserIds(const QJsonArray &array);

  QJsonArray symbolsToJsonArray() const;
  std::string symbolsToString() const; //TODO vedi se rimuovere
  static std::vector<Symbol> jsonArrayToSymbols(const QJsonArray &array);

  int _id;
  std::vector<int> _userIds;
  std::vector<Symbol> _symbols;
};
