#pragma once

#include <string>
#include <QJsonObject>

class SymbolId {
public:
  SymbolId();
  SymbolId(int clientId, int charId);
  explicit SymbolId(QJsonObject &json);

  static SymbolId fromJsonObject(QJsonObject &json);
  QJsonObject toJsonObject();

  int getClientId() const;
  int getCharId() const;
  std::string to_string() const;

private:
  int _clientId, _charId;
};
