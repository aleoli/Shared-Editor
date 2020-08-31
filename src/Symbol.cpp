#include "Symbol.h"

#include <sstream>
#include <utility>

#include "exceptions.h"
#include "utils.h"

using namespace se_exceptions;

Symbol::Symbol() = default;

Symbol::Symbol(SymbolId id, QChar chr) : _id(id), _char(chr) {}

Symbol::Symbol(SymbolId id, QChar chr, QTextCharFormat fmt)
  : _id(id), _char(chr), _fmt(std::move(fmt)) {}

Symbol::Symbol(const QJsonObject &json, bool readPos) {
  checkAndAssign(json, readPos);
}

Symbol::Symbol(QJsonObject &&json, bool readPos) {
  checkAndAssign(json, readPos);
}

Symbol::Symbol(const Symbol& s) = default;
Symbol::Symbol(Symbol&& s) noexcept: _id(s._id), _char(s._char), _pos(std::move(s._pos)), _fmt(std::move(s._fmt)) {}

Symbol &Symbol::operator=(const Symbol &s) = default;
Symbol &Symbol::operator=(Symbol &&s) noexcept {
  if(this == &s) {
    return *this;
  }
  this->_id = s._id;
  this->_char = s._char;
  this->_pos = std::move(s._pos);
  this->_fmt = std::move(s._fmt);
  return *this;
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
    Symbol::compareFormats(lhs._fmt, rhs._fmt);
}

bool operator!=(const Symbol& lhs, const Symbol& rhs) {
  return !operator==(lhs, rhs);
}

bool Symbol::compareFormats(const QTextCharFormat &fmt1, const QTextCharFormat &fmt2, bool ignoreBackground) {
  return fmt1.font() == fmt2.font() &&
         fmt1.foreground() == fmt2.foreground() &&
         ignoreBackground || fmt1.background() == fmt2.background();
}

bool Symbol::hasSameAttributes(const QChar &chr, const QTextCharFormat &fmt, bool ignoreBackground) const {
  return _char == chr && compareFormats(_fmt, fmt, ignoreBackground);
}

void Symbol::checkAndAssign (const QJsonObject &json, bool readPos) {
  auto idValue = json["id"];
  auto charValue = json["ch"];
  auto fmtValue = json["fmt"];

  if(idValue.isUndefined() || charValue.isUndefined() || fmtValue.isUndefined()) {
    throw SymbolFromJsonException{"The QJsonObject has some fields missing"};
  }

  if(!idValue.isObject() || !fmtValue.isObject()) {
    throw SymbolFromJsonException{"One or more fields are not valid"};
  }

  auto charUnicode = charValue.toInt(-1);

  if(charUnicode < 0) {
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

    _pos = utils::jsonArrayToVector<Identifier>(posValue.toArray());
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
  json["ch"] = QJsonValue(_char.unicode());
  json["fmt"] = QJsonValue(serializeFormat(_fmt));

  if(writePos)
    json["pos"] = QJsonValue(utils::vectorToJsonArray(_pos));

  return json;
}

QJsonObject Symbol::serializeFormat(const QTextCharFormat &fmt) {
  QJsonObject json;

  json["fnt"] = QJsonValue(fontToString(fmt.font()));
  json["col"] = QJsonValue(brushToString(fmt.foreground(), false));
  json["bac"] = QJsonValue(brushToString(fmt.background(), true));

  return json;
}

QTextCharFormat Symbol::deserializeFormat(const QJsonObject &json) {
  QTextCharFormat fmt;

  auto fontValue = json["fnt"];
  auto colorValue = json["col"];
  auto backgroundColorValue = json["bac"];

  if(fontValue.isUndefined() || colorValue.isUndefined()
      || backgroundColorValue.isUndefined()) {
    throw SymbolFromJsonException{"The QJsonObject has some fields missing"};
  }

  if(!fontValue.isString() || !colorValue.isString() || !backgroundColorValue.isString()) {
    throw SymbolFromJsonException{"One or more fields are not valid"};
  }

  fmt.setFont(stringToFont(fontValue.toString()));
  fmt.setForeground(stringToBrush(colorValue.toString()));
  fmt.setBackground(stringToBrush(backgroundColorValue.toString()));

  return fmt;
}

QString Symbol::brushToString(const QBrush &b, bool isBackground) {
  auto color = b.color();

  if(color.alpha() == 255 && !b.isOpaque()) {
    //FIX bug
    if(isBackground) color.setAlpha(0);
    else color.setAlpha(255);
  }

  return color.name(QColor::HexArgb);
}

QBrush Symbol::stringToBrush(const QString &string) {
  QColor color(string);

  if(!color.isValid()) {
    throw SymbolFromJsonException{"Color is not valid"};
  }

  return QBrush(color);
}

QJsonArray Symbol::fontToJsonArray(const QFont &f) {
  QByteArray data;
  QJsonArray array;

  QDataStream ds(&data, QIODevice::WriteOnly);
  ds.setByteOrder(QDataStream::BigEndian);
  ds << f;

  for(auto by : data) {
    array.push_back(by);
  }

  return array;
}

QFont Symbol::jsonArrayToFont(const QJsonArray &array) {
  QByteArray data;
  QFont font;

  for(auto a : array) {
    data.append(static_cast<char>(a.toInt()));
  }

  QDataStream ds(&data, QIODevice::ReadOnly);
  ds.setByteOrder(QDataStream::BigEndian);
  ds >> font;

  return font;
}

QString Symbol::fontToString(const QFont &f) {
  return f.toString();
}

QFont Symbol::stringToFont(const QString &string) {
  QFont font;
  font.fromString(string);

  return font;
}

std::string Symbol::posToString() const {
  std::stringstream ss;

  for(auto& el : _pos) {
    ss << el.to_string() << " ";
  }

  return ss.str();
}

void Symbol::setSymbolId(const SymbolId &id) {
  _id = id;
}

SymbolId Symbol::getSymbolId() const{
  return _id;
}

void Symbol::setChar(const QChar &chr) {
  _char = chr;
}

QChar Symbol::getChar() const{
  return _char;
}

void Symbol::setPos(const std::vector<Symbol::Identifier> &pos) {
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

void Symbol::update(const Symbol &s) {
  _char = s._char;
  _fmt = s._fmt;
}

void Symbol::setFormat(const QTextCharFormat &fmt) {
  _fmt = fmt;
}

QTextCharFormat Symbol::getFormat() const {
  return _fmt;
}

void Symbol::setFont(const QFont& font) {
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
  _fmt.font().setPointSize(size);
}

int Symbol::getSize() const {
  return _fmt.font().pointSize();
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
