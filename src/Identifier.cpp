#include "Identifier.h"

#include <sstream>

#include "exceptions.h"

using namespace se_exceptions;

Identifier::Identifier() : _first(-1), _second(-1) {}

Identifier::Identifier(int first, int second)
        : _first(first), _second(second) {}

Identifier::Identifier(const QJsonObject &json) {
  checkAndAssign(json);
}

Identifier::Identifier(QJsonObject &&json) {
  checkAndAssign(json);
}

void Identifier::checkAndAssign(const QJsonObject &json) {
  auto firstValue = json["1"];
  auto secondValue = json["2"];

  if(firstValue.isUndefined() || secondValue.isUndefined()) {
    throw IdentifierFromJsonException{"The QJsonObject has some fields missing"};
  }

  auto first = firstValue.toInt(-10);
  auto second = secondValue.toInt(-10);

  if(first == -10 || second == -10) {
    throw IdentifierFromJsonException{"One or more fields UD are not valid"};
  }

  _first = first;
  _second = second;
}

Identifier Identifier::fromJsonObject(const QJsonObject &json) {
  return Identifier(json);
}

Identifier Identifier::fromJsonObject(QJsonObject &&json) {
  return Identifier(json);
}

QJsonObject Identifier::toJsonObject() const {
  QJsonObject json;

  json["1"] = QJsonValue(_first);
  json["2"] = QJsonValue(_second);

  return json;
}

bool operator==(const Identifier& lhs, const Identifier& rhs) {
  return lhs._first == rhs._first && lhs._second == rhs._second;
}

bool operator!=(const Identifier& lhs, const Identifier& rhs) {
  return !(lhs == rhs);
}


bool operator<(const Identifier& lhs, const Identifier& rhs) {
  if(lhs._first == rhs._first)
    return lhs._second < rhs._second;

  return lhs._first < rhs._first;
}

int Identifier::getFirst() const {
  return _first;
}

int Identifier::getSecond() const {
  return _second;
}

QString Identifier::toString() const {
  return QString::number(_first) + "-" + QString::number(_second);
}

std::string Identifier::to_string() const {
  std::stringstream ss;
  ss << _first << "-" << _second;
  return ss.str();
}
