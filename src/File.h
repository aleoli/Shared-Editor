#pragma once

#include "Symbol.h"

#include <vector>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

class File {
public:
  File();
  File(int id, std::vector<int> user_ids, std::vector<Symbol> _symbols);
  explicit File(QJsonObject &json);
  explicit File(QJsonObject &&json);
  //TODO altri costruttori..

  static File fromJsonObject(QJsonObject &json);
  QJsonObject toJsonObject() const;

  int getId() const;
  std::vector<int> getUserIds() const;
  std::vector<Symbol> getSymbols() const;
  std::string to_string() const;

private:
  void checkAndAssign(QJsonValue idValue, QJsonValue userIdsValue, QJsonValue symbolsValue);

  QJsonArray userIdsToJsonArray() const;
  std::string userIdsToString() const;
  static std::vector<int> jsonArrayToUserIds(QJsonArray &array);
  
  QJsonArray symbolsToJsonArray() const;
  std::string symbolsToString() const;
  std::vector<Symbol> jsonArrayToSymbols(QJsonArray &array);

  int _id;
  std::vector<int> _user_ids;
  std::vector<Symbol> _symbols;
};
