#include "Symbol.h"

#include <sstream>

#include "exceptions.h"

using namespace se_exceptions;

Symbol::Symbol() {}

Symbol::Symbol(SymbolId id, QChar chr, std::vector<int> pos)
  : _id(id), _char(chr), _pos(pos) {}

Symbol::Symbol(const QJsonObject &json) {
  auto idValue = json["id"];
  auto charValue = json["char"];
  auto posValue = json["pos"];

  checkAndAssign(idValue, charValue, posValue);
}

Symbol::Symbol(const QJsonObject &&json) {
  auto idValue = json["id"];
  auto charValue = json["char"];
  auto posValue = json["pos"];

  checkAndAssign(idValue, charValue, posValue);
}

void Symbol::checkAndAssign (const QJsonValue &idValue, const QJsonValue &charValue, const QJsonValue &posValue) {
  if(idValue.isUndefined() || charValue.isUndefined() || posValue.isUndefined()) {
    throw SymbolFromJsonException{"The QJsonObject has some fields missing"};
  }

  if(!idValue.isObject() || !posValue.isArray()) {
    throw SymbolFromJsonException{"One or more fields are not valid"};
  }

  auto charUnicode = charValue.toInt(-1);

  if(charUnicode <= 0) {
    throw SymbolFromJsonException{"One or more fields are not valid"};
  }

  auto idJson = idValue.toObject();
  auto posArray = posValue.toArray();

  _id = SymbolId{idJson};
  _char = QChar(charUnicode);
  _pos = jsonArrayToPos(posArray);
}

Symbol Symbol::fromJsonObject(const QJsonObject &json) {
  return Symbol(json);
}

Symbol Symbol::fromJsonObject(const QJsonObject &&json) {
  return Symbol(json);
}

QJsonObject Symbol::toJsonObject() const {
  QJsonObject json;

  json["id"] = QJsonValue(_id.toJsonObject());
  json["char"] = QJsonValue(_char.unicode());
  json["pos"] = QJsonValue(posToJsonArray());

  return json;
}

QJsonArray Symbol::posToJsonArray() const {
  QJsonArray array;

  for(int el : _pos) {
    array.append(el);
  }

  return array;
}

std::string Symbol::posToString() const {
  std::stringstream ss;

  for(int el : _pos) {
    ss << el << ' ';
  }

  return ss.str();
}

std::vector<int> Symbol::jsonArrayToPos(const QJsonArray &array) {
  std::vector<int> pos;

  for(auto&& el : array) {
    auto val = el.toInt(-1);

    if(val == -1) {
      throw SymbolFromJsonException{"One or more fields in pos array are not valid"};
    }

    pos.push_back(val);
  }

  return pos;
}

SymbolId Symbol::getSymbolId() const{
  return _id;
}

QChar Symbol::getChar() const{
  return _char;
}

std::vector<int> Symbol::getPos() const{
  return _pos;
}

std::string Symbol::to_string() const{
  std::stringstream ss;

  ss << "ID: " << _id.to_string() << std::endl;
  ss << "Char: " << _char.toLatin1() << std::endl;
  ss << "Pos: " << posToString();

  return ss.str();
}
