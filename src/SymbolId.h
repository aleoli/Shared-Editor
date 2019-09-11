#pragma once

#include <string>
#include <QJsonObject>

class SymbolId {
public:
  SymbolId();
  SymbolId(int clientId, int charId);
  explicit SymbolId(const QJsonObject &json);
  explicit SymbolId(QJsonObject &&json);

  friend bool operator<(const SymbolId& lhs, const SymbolId& rhs);
  friend bool operator==(const SymbolId& lhs, const SymbolId& rhs);

  static SymbolId fromJsonObject(const QJsonObject &json);
  static SymbolId fromJsonObject(QJsonObject &&json);
  QJsonObject toJsonObject() const;

  int getClientId() const;
  int getCharId() const;
  std::string to_string() const;

private:
  void checkAndAssign(const QJsonObject &json);

  int _clientId, _charId;
};
