#include "SymbolId.h"

#include <sstream>

#include "exceptions.h"

using namespace se_exceptions;

SymbolId::SymbolId() : _userId(-1), _charId(-1) {}

SymbolId::SymbolId(int userId, int charId) :
  _userId(userId), _charId(charId) {}

SymbolId::SymbolId(const QJsonObject &json) {
  checkAndAssign(json);
}

SymbolId::SymbolId(QJsonObject &&json) {
  checkAndAssign(json);
}

bool operator<(const SymbolId& lhs, const SymbolId& rhs) {
  if(lhs._userId == rhs._userId) {
    return lhs._charId < rhs._charId;
  }

  return lhs._userId < rhs._userId;
}

bool operator==(const SymbolId& lhs, const SymbolId& rhs) {
  return lhs._userId == rhs._userId && lhs._charId == rhs._charId;
}

void SymbolId::checkAndAssign(const QJsonObject &json) {
  auto userIdValue = json["userId"];
  auto charIdValue = json["charId"];

  if(userIdValue.isUndefined() || charIdValue.isUndefined()) {
    throw SymbolIdFromJsonException{"The QJsonObject has some fields missing"};
  }

  auto userId = userIdValue.toInt(-1);
  auto charId = charIdValue.toInt(-1);

  if(userId == -1 || charId == -1) {
    throw SymbolIdFromJsonException{"One or more fields are not valid"};
  }

  _userId = userId;
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

  json["userId"] = QJsonValue(_userId);
  json["charId"] = QJsonValue(_charId);

  return json;
}

int SymbolId::getUserId() const {
  return _userId;
}

int SymbolId::getCharId() const {
  return _charId;
}

std::string SymbolId::to_string() const {
  std::stringstream ss;
  ss << _userId << "-" << _charId;
  return ss.str();
}
