#include "Symbol.h"

#include <sstream>

#include "exceptions.h"

using namespace se_exceptions;

Symbol::Symbol() {}

Symbol::Symbol(SymbolId id, QChar chr) : _id(id), _char(chr) {}

Symbol::Symbol(SymbolId id, QChar chr, QTextCharFormat fmt)
  : _id(id), _char(chr), _fmt(fmt) {}

Symbol::Symbol(const QJsonObject &json, bool readPos) {
  checkAndAssign(json, readPos);
}

Symbol::Symbol(QJsonObject &&json, bool readPos) {
  checkAndAssign(json, readPos);
}

bool operator<(const Symbol& lhs, const Symbol& rhs) {
  if(lhs._pos == rhs._pos) {
    return lhs._id < rhs._id;
  }

  return lhs._pos < rhs._pos;
}

bool operator==(const Symbol& lhs, const Symbol& rhs) {
  return lhs._id == rhs._id && lhs._char == rhs._char &&
    lhs._pos == rhs._pos &&
    lhs.getFont() == rhs.getFont() &&
    lhs.getColor() == rhs.getColor() &&
    lhs.getBackgroundColor() == rhs.getBackgroundColor();
}

bool operator!=(const Symbol& lhs, const Symbol& rhs) {
  return !operator==(lhs, rhs);
}

void Symbol::checkAndAssign (const QJsonObject &json, bool readPos) {
  auto idValue = json["id"];
  auto charValue = json["char"];
  auto fmtValue = json["fmt"];

  if(idValue.isUndefined() || charValue.isUndefined() || fmtValue.isUndefined()) {
    throw SymbolFromJsonException{"The QJsonObject has some fields missing"};
  }

  if(!idValue.isObject() || !fmtValue.isObject()) {
    throw SymbolFromJsonException{"One or more fields are not valid"};
  }

  auto charUnicode = charValue.toInt(-1);

  if(charUnicode <= 0) {
    throw SymbolFromJsonException{"One or more fields are not valid"};
  }

  auto idJson = idValue.toObject();

  auto fmt = deserializeFormat(fmtValue.toObject());

  if(readPos) {
    auto posValue = json["pos"];
    if(posValue.isUndefined()) {
      throw SymbolFromJsonException{"The QJsonObject has some fields missing"};
    }

    if(!posValue.isArray()) {
      throw SymbolFromJsonException{"One or more fields are not valid"};
    }

    _pos = jsonArrayToPos(posValue.toArray());
  }

  _id = SymbolId{idJson};
  _char = QChar(charUnicode);
  _fmt = fmt;
}

Symbol Symbol::fromJsonObject(const QJsonObject &json, bool readPos) {
  return Symbol(json, readPos);
}

Symbol Symbol::fromJsonObject(QJsonObject &&json, bool readPos) {
  return Symbol(json, readPos);
}

QJsonObject Symbol::toJsonObject(bool writePos) const {
  QJsonObject json;

  json["id"] = QJsonValue(_id.toJsonObject());
  json["char"] = QJsonValue(_char.unicode());
  json["fmt"] = QJsonValue(serializeFormat(_fmt));

  if(writePos)
    json["pos"] = QJsonValue(posToJsonArray());

  return json;
}

QJsonObject Symbol::serializeFormat(const QTextCharFormat &fmt) {
  QJsonObject json;

  json["font"] = QJsonValue(fmt.font().toString());
  json["color"] = QJsonValue(fmt.foreground().color().name(QColor::HexArgb));
  json["backgroundColor"] = QJsonValue(fmt.background().color().name(QColor::HexArgb));

  return json;
}

QTextCharFormat Symbol::deserializeFormat(const QJsonObject &json) {
  QTextCharFormat fmt;
  QFont font;
  QColor color, backgroundColor;

  auto fontValue = json["font"];
  auto colorValue = json["color"];
  auto backgroundColorValue = json["backgroundColor"];

  if(fontValue.isUndefined() || colorValue.isUndefined()
      || backgroundColorValue.isUndefined()) {
    throw SymbolFromJsonException{"The QJsonObject has some fields missing"};
  }

  if(!fontValue.isString() || !colorValue.isString()
      || !backgroundColorValue.isString()) {
    throw SymbolFromJsonException{"One or more fields are not valid"};
  }

  auto fontString = fontValue.toString();
  if(!font.fromString(fontString)) {
    throw SymbolFromJsonException{"One or more fields are not valid"};
  }

  auto colorString = colorValue.toString();
  if(!QColor::isValidColor(colorString)) {
    throw SymbolFromJsonException{"One or more fields are not valid"};
  }
  color.setNamedColor(colorString);

  auto backgroundColorString = backgroundColorValue.toString();
  if(!QColor::isValidColor(backgroundColorString)) {
    throw SymbolFromJsonException{"One or more fields are not valid"};
  }
  backgroundColor.setNamedColor(backgroundColorString);

  fmt.setFont(font);
  fmt.setForeground(color);
  fmt.setBackground(backgroundColor);

  return fmt;
}

QJsonArray Symbol::posToJsonArray() const {
  QJsonArray array;

  for(auto& el : _pos) {
    array.append(el.toJsonObject());
  }

  return array;
}

std::string Symbol::posToString() const {
  std::stringstream ss;

  for(auto& el : _pos) {
    ss << el.to_string() << " ";
  }

  return ss.str();
}

std::vector<Symbol::Identifier> Symbol::jsonArrayToPos(const QJsonArray &array) {
  std::vector<Symbol::Identifier> pos;

  for(auto&& el : array) {
    if(!el.isObject()) {
      throw SymbolFromJsonException{"One or more fields in symbols array are not valid"};
    }

    auto val = el.toObject();

    pos.emplace_back(val);
  }

  return pos;
}

void Symbol::setSymbolId(SymbolId id) {
  _id = id;
}

SymbolId Symbol::getSymbolId() const{
  return _id;
}

void Symbol::setChar(QChar chr) {
  _char = chr;
}

QChar Symbol::getChar() const{
  return _char;
}

void Symbol::setPos(std::vector<Symbol::Identifier> pos) {
  _pos = pos;
}

std::vector<Symbol::Identifier> Symbol::getPos() const{
  return _pos;
}

std::string Symbol::to_string() const{
  std::stringstream ss;

  ss << "Symbol: " << _id.to_string() << std::endl;
  ss << "\tChar: " << _char.toLatin1() << std::endl;
  ss << "\tPos: " << posToString() << std::endl;
  ss << "\tStyle:" << std::endl << getFontInfo() << std::endl;

  return ss.str();
}

void Symbol::setFormat(QTextCharFormat fmt) {
  _fmt = fmt;
}

QTextCharFormat Symbol::getFormat() const {
  return _fmt;
}

void Symbol::setFont(QFont font) {
  _fmt.setFont(font);
}

QFont Symbol::getFont() const {
  return _fmt.font();
}

void Symbol::setBold(bool enable) {
  _fmt.setFontWeight(QFont::Bold);
}

bool Symbol::isBold() const {
  return _fmt.font().bold();
}

void Symbol::setSize(int size){
  _fmt.setFontPointSize(size);
}

int Symbol::getSize() const {
  return _fmt.fontPointSize();
}

void Symbol::setUnderline(bool enable) {
  _fmt.setFontUnderline(enable);
}

bool Symbol::isUnderline()  const {
  return _fmt.fontUnderline();
}

void Symbol::setItalic(bool enable) {
  _fmt.setFontItalic(enable);
}

bool Symbol::isItalic() const {
  return _fmt.fontItalic();
}

std::string Symbol::getFontInfo() const {
  std::stringstream ss;

  ss << "\t\tsize: " << getSize() << std::endl;
  ss << "\t\tbold: " << isBold() << std::endl;
  ss << "\t\tunderline: " << isUnderline() << std::endl;
  ss << "\t\titalic: " << isItalic() << std::endl;
  ss << "\t\tfamily: " << getFamily().toStdString() << std::endl;
  ss << "\t\tcolor: " << getColor().name(QColor::HexArgb).toStdString() << std::endl;
  ss << "\t\tbackground color: " << getBackgroundColor().name(QColor::HexArgb).toStdString();

  return ss.str();
}

void Symbol::setFamily(const QString &family) {
  _fmt.setFontFamily(family);
}

QString Symbol::getFamily() const {
  return _fmt.fontFamily();
}

void Symbol::setColor(const QColor &color) {
  _fmt.setForeground(color);
}

QColor Symbol::getColor() const {
  return _fmt.foreground().color();
}

void Symbol::setBackgroundColor(const QColor &color) {
  _fmt.setBackground(color);
}

QColor Symbol::getBackgroundColor() const {
  return _fmt.background().color();
}

//IDENTIFIER
Symbol::Identifier::Identifier() : _digit(-1), _clientId(-1) {}

Symbol::Identifier::Identifier(int digit, int clientId)
  : _digit(digit), _clientId(clientId) {}

Symbol::Identifier::Identifier(const QJsonObject &json) {
  checkAndAssign(json);
}

Symbol::Identifier::Identifier(QJsonObject &&json) {
  checkAndAssign(json);
}

void Symbol::Identifier::checkAndAssign(const QJsonObject &json) {
  auto digitValue = json["digit"];
  auto clientIdValue = json["clientId"];

  if(digitValue.isUndefined() || clientIdValue.isUndefined()) {
    throw SymbolIdentifierFromJsonException{"The QJsonObject has some fields missing"};
  }

  auto digit = digitValue.toInt(-1);
  auto clientId = clientIdValue.toInt(-1);

  if(digit < 0 || clientId < 0) {
    throw SymbolIdentifierFromJsonException{"One or more fields are not valid"};
  }

  _digit = digit;
  _clientId = clientId;
}

Symbol::Identifier Symbol::Identifier::fromJsonObject(const QJsonObject &json) {
  return Symbol::Identifier(json);
}

Symbol::Identifier Symbol::Identifier::fromJsonObject(QJsonObject &&json) {
  return Symbol::Identifier(json);
}

QJsonObject Symbol::Identifier::toJsonObject() const {
  QJsonObject json;

  json["digit"] = QJsonValue(_digit);
  json["clientId"] = QJsonValue(_clientId);

  return json;
}

bool operator==(const Symbol::Identifier& lhs, const Symbol::Identifier& rhs) {
  return lhs._digit == rhs._digit && lhs._clientId == rhs._clientId;
}

bool operator<(const Symbol::Identifier& lhs, const Symbol::Identifier& rhs) {
  if(lhs._digit == rhs._digit)
    return lhs._clientId < rhs._clientId;

  return lhs._digit < rhs._digit;
}

int Symbol::Identifier::getDigit() const {
  return _digit;
}

int Symbol::Identifier::getClientId() const {
  return _clientId;
}

std::string Symbol::Identifier::to_string() const {
  std::stringstream ss;
  ss << _digit << "-" << _clientId;
  return ss.str();
}
