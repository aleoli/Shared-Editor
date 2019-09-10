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
  QJsonObject toJsonObject() const;

  int getId() const;
  std::vector<int> getUserIds() const;
  std::vector<Symbol> getSymbols() const;
  std::string to_string() const;

  void addUserId(int id);
  void addSymbol(const Symbol &sym, int pos);

private:
  void checkAndAssign(const QJsonObject &json);

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
