#pragma once

#include <string>
#include <QJsonObject>

class SymbolId {
public:
  SymbolId();
  SymbolId(int clientId, int charId);
  explicit SymbolId(const QJsonObject &json);
  explicit SymbolId(const QJsonObject &&json);

  static SymbolId fromJsonObject(const QJsonObject &json);
  static SymbolId fromJsonObject(const QJsonObject &&json);
  QJsonObject toJsonObject() const;

  int getClientId() const;
  int getCharId() const;
  std::string to_string() const;

private:
  void checkAndAssign(const QJsonValue &clientIdValue, const QJsonValue &charIdValue);

  int _clientId, _charId;
};
