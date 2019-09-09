#include "Symbol.h"

#include <sstream>

#include "exceptions.h"

using namespace se_exceptions;

Symbol::Symbol() {}

Symbol::Symbol(QChar chr) : _char(chr) {}

Symbol::Symbol(QChar chr, QFont font) : _char(chr), _font(font) {}

Symbol::Symbol(SymbolId id, QChar chr, std::vector<int> pos)
  : _id(id), _char(chr), _pos(pos) {}

Symbol::Symbol(SymbolId id, QChar chr, std::vector<int> pos, QFont font)
  : _id(id), _char(chr), _pos(pos), _font(font) {}

  Symbol::Symbol(SymbolId id, QChar chr, std::vector<int> pos, QFont font, QColor color)
    : _id(id), _char(chr), _pos(pos), _font(font), _color(color) {}

Symbol::Symbol(const QJsonObject &json) {
  auto idValue = json["id"];
  auto charValue = json["char"];
  auto posValue = json["pos"];
  auto fontValue = json["font"];
  auto colorValue = json["color"];

  checkAndAssign(idValue, charValue, posValue, fontValue, colorValue);
}

Symbol::Symbol(const QJsonObject &&json) {
  auto idValue = json["id"];
  auto charValue = json["char"];
  auto posValue = json["pos"];
  auto fontValue = json["font"];
  auto colorValue = json["color"];

  checkAndAssign(idValue, charValue, posValue, fontValue, colorValue);
}

void Symbol::checkAndAssign (const QJsonValue &idValue, const QJsonValue &charValue,
  const QJsonValue &posValue, const QJsonValue &fontValue, const QJsonValue &colorValue) {
  if(idValue.isUndefined() || charValue.isUndefined() || posValue.isUndefined()
      || fontValue.isUndefined() || colorValue.isUndefined()) {
    throw SymbolFromJsonException{"The QJsonObject has some fields missing"};
  }

  if(!idValue.isObject() || !posValue.isArray() || !fontValue.isString()) {
    throw SymbolFromJsonException{"One or more fields are not valid"};
  }

  auto charUnicode = charValue.toInt(-1);

  if(charUnicode <= 0) {
    throw SymbolFromJsonException{"One or more fields are not valid"};
  }

  auto idJson = idValue.toObject();
  auto posArray = posValue.toArray();

  auto fontString = fontValue.toString();

  if(!_font.fromString(fontString)) {
    throw SymbolFromJsonException{"One or more fields are not valid"};
  }

  int colorInt = colorValue.toInt(0);
  if(!(colorInt & 0xFF000000)) {
    //TODO vedi se c'è un modo migliore.. il colore è in formato RGBA
    // salvato come unsigned int (0xAARRGGBB).. quindi non ho un valore che mi dica
    // che il colore non è valido.. io ho ipotizzato che se AA è zero allora non vale
    // AA sarebbe l'opacità
    throw SymbolFromJsonException{"One or more fields are not valid"};
  }

  _id = SymbolId{idJson};
  _char = QChar(charUnicode);
  _pos = jsonArrayToPos(posArray);
  _color = QColor::fromRgba(colorInt);
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
  json["font"] = QJsonValue(_font.toString());
  json["color"] = QJsonValue(static_cast<int>(_color.rgba())); //TODO vedi se mettere rgb o rgba

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

  ss << "-------------------------------------" << std::endl;
  ss << "ID: " << _id.to_string() << std::endl;
  ss << "Char: " << _char.toLatin1() << std::endl;
  ss << "Pos: " << posToString() << std::endl;
  ss << "Style:" << std::endl << getFontInfo() << std::endl;
  ss << "-------------------------------------";

  return ss.str();
}

const QFont& Symbol::getFont() const {
  return _font;
}

void Symbol::setBold(bool enable) {
  _font.setBold(enable);
}

bool Symbol::isBold() const {
  return _font.bold();
}

void Symbol::setSize(int size){
  _font.setPointSize(size);
}

int Symbol::getSize() const {
  return _font.pointSize();
}

void Symbol::setUnderline(bool enable) {
  _font.setUnderline(enable);
}

bool Symbol::isUnderline()  const {
  return _font.underline();
}

void Symbol::setItalic(bool enable) {
  _font.setItalic(enable);
}

bool Symbol::isItalic() const {
  return _font.italic();
}

std::string Symbol::getFontInfo() const {
  std::stringstream ss;

  ss << "size: " << getSize() << std::endl;
  ss << "bold: " << isBold() << std::endl;
  ss << "underline: " << isUnderline() << std::endl;
  ss << "italic: " << isItalic() << std::endl;
  ss << "family: " << getFamily().toStdString() << std::endl;
  ss << "color: " << _color.name(QColor::HexArgb).toStdString();

  return ss.str();
}

void Symbol::setFamily(const QString &family) {
  _font.setFamily(family);
}

QString Symbol::getFamily() const {
  return _font.family();
}

void Symbol::setColor(const QColor &color) {
  _color = color;
}

QColor Symbol::getColor() const {
  return _color;
}
