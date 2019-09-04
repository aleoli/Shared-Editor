#include "Symbol.h"

#include "exceptions.h"

using namespace se_exceptions;

Symbol::Symbol() {}

Symbol::Symbol(SymbolId id, QChar chr, std::vector<int> pos)
  : _id(id), _char(chr), _pos(pos) {}

Symbol::Symbol(QJsonObject &json) {
  auto idValue = json["id"];
  auto charValue = json["char"];
  auto posValue = json["pos"];

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

Symbol Symbol::fromJsonObject(QJsonObject &json) {
  return Symbol(json);
}

QJsonObject Symbol::toJsonObject() {
  QJsonObject json;

  json["id"] = QJsonValue(_id.toJsonObject());
  json["char"] = QJsonValue(_char.unicode());
  json["pos"] = QJsonValue(posToJsonArray());

  return json;
}

QJsonArray Symbol::posToJsonArray() {
  QJsonArray array;

  for(int &el : _pos) {
    array.append(el);
  }

  return array;
}

std::vector<int> Symbol::jsonArrayToPos(QJsonArray &array) {
  std::vector<int> pos;

  for(auto el : array) {
    auto val = el.toInt(-1);

    if(val == -1) {
      throw SymbolIdFromJsonException{"One or more fields in pos array are not valid"};
    }

    pos.push_back(val);
  }

  return pos;
}
