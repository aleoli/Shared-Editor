#include "SymbolId.h"

#include <sstream>

#include "exceptions.h"

using namespace se_exceptions;

SymbolId::SymbolId() : _clientId(-1), _charId(-1) {}

SymbolId::SymbolId(int clientId, int charId) :
  _clientId(clientId), _charId(charId) {}

SymbolId::SymbolId(const QJsonObject &json) {
  checkAndAssign(json);
}

SymbolId::SymbolId(QJsonObject &&json) {
  checkAndAssign(json);
}

bool operator<(const SymbolId& lhs, const SymbolId& rhs) {
  if(lhs._clientId == rhs._clientId) {
    return lhs._charId < rhs._charId;
  }

  return lhs._clientId < rhs._clientId;
}

void SymbolId::checkAndAssign(const QJsonObject &json) {
  auto clientIdValue = json["clientId"];
  auto charIdValue = json["charId"];

  if(clientIdValue.isUndefined() || charIdValue.isUndefined()) {
    throw SymbolIdFromJsonException{"The QJsonObject has some fields missing"};
  }

  auto clientId = clientIdValue.toInt(-1);
  auto charId = charIdValue.toInt(-1);

  if(clientId == -1 || charId == -1) {
    throw SymbolIdFromJsonException{"One or more fields are not valid"};
  }

  _clientId = clientId;
  _charId = charId;
}


SymbolId SymbolId::fromJsonObject(const QJsonObject &json) {
  return SymbolId(json);
}

SymbolId SymbolId::fromJsonObject(QJsonObject &&json) {
  return SymbolId(json);
}

QJsonObject SymbolId::toJsonObject() const {
  QJsonObject json;

  json["clientId"] = QJsonValue(_clientId);
  json["charId"] = QJsonValue(_charId);

  return json;
}

int SymbolId::getClientId() const {
  return _clientId;
}

int SymbolId::getCharId() const {
  return _charId;
}

std::string SymbolId::to_string() const {
  std::stringstream ss;
  ss << _clientId << "-" << _charId;
  return ss.str();
}
