#include "Identifier.h"

#include <sstream>

#include "exceptions.h"

using namespace se_exceptions;

IdentifierBase::IdentifierBase() : _digit(-1), _userId(-1) {}

IdentifierBase::IdentifierBase(int digit, int userId)
        : _digit(digit), _userId(userId) {}

IdentifierBase::IdentifierBase(const QJsonObject &json) {
  checkAndAssign(json);
}

IdentifierBase::IdentifierBase(QJsonObject &&json) {
  checkAndAssign(json);
}

void IdentifierBase::checkAndAssign(const QJsonObject &json) {
  auto digitValue = json["digit"];
  auto userIdValue = json["userId"];

  if(digitValue.isUndefined() || userIdValue.isUndefined()) {
    throw SymbolIdentifierFromJsonException{"The QJsonObject has some fields missing"};
  }

  auto digit = digitValue.toInt(-1);
  auto userId = userIdValue.toInt(-1);

  if(digit < 0 || userId < 0) {
    throw SymbolIdentifierFromJsonException{"One or more fields are not valid"};
  }

  _digit = digit;
  _userId = userId;
}

IdentifierBase IdentifierBase::fromJsonObject(const QJsonObject &json) {
  return IdentifierBase(json);
}

IdentifierBase IdentifierBase::fromJsonObject(QJsonObject &&json) {
  return IdentifierBase(json);
}

QJsonObject IdentifierBase::toJsonObject() const {
  QJsonObject json;

  json["digit"] = QJsonValue(_digit);
  json["userId"] = QJsonValue(_userId);

  return json;
}

bool operator==(const IdentifierBase& lhs, const IdentifierBase& rhs) {
  return lhs._digit == rhs._digit && lhs._userId == rhs._userId;
}

bool operator<(const IdentifierBase& lhs, const IdentifierBase& rhs) {
  if(lhs._digit == rhs._digit)
    return lhs._userId < rhs._userId;

  return lhs._digit < rhs._digit;
}

int IdentifierBase::getDigit() const {
  return _digit;
}

int IdentifierBase::getUserId() const {
  return _userId;
}

std::string IdentifierBase::to_string() const {
  std::stringstream ss;
  ss << _digit << "-" << _userId;
  return ss.str();
}
