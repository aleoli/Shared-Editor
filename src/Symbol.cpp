#include "Symbol.h"

#include <sstream>
#include <utility>

#include "exceptions.h"
#include "utils.h"

using namespace se_exceptions;

Symbol::Symbol() : _timestamp(QDateTime()) {}

Symbol::Symbol(SymbolId id, QChar chr) : _id(id), _char(chr), _timestamp(QDateTime()) {}

Symbol::Symbol(SymbolId id, QChar chr, QTextCharFormat fmt)
  : _id(id), _char(chr), _fmt(std::move(fmt)), _timestamp(QDateTime()) {}

Symbol::Symbol(const QJsonObject &json, QFont *font, QBrush *col, QBrush *bac)
   : _timestamp(QDateTime()) {
  checkAndAssign(json, font, col, bac);
}

Symbol::Symbol(QJsonObject &&json, QFont *font, QBrush *col, QBrush *bac)
   : _timestamp(QDateTime()) {
  checkAndAssign(json, font, col, bac);
}

Symbol::Symbol(const Symbol& s) = default;
Symbol::Symbol(Symbol&& s) noexcept: _id(s._id), _char(s._char), _pos(std::move(s._pos)), _fmt(std::move(s._fmt)), _timestamp(std::move(s._timestamp)), _lastUser(s._lastUser) {}

Symbol &Symbol::operator=(const Symbol &s) = default;
Symbol &Symbol::operator=(Symbol &&s) noexcept {
  if(this == &s) {
    return *this;
  }
  this->_id = s._id;
  this->_char = s._char;
  this->_pos = std::move(s._pos);
  this->_fmt = std::move(s._fmt);
  this->_timestamp = std::move(s._timestamp);
  this->_lastUser = s._lastUser;
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
    Symbol::compareFormats(lhs._fmt, rhs._fmt) && lhs._timestamp == rhs._timestamp &&
    lhs._lastUser == rhs._lastUser;
}

bool operator!=(const Symbol& lhs, const Symbol& rhs) {
  return !operator==(lhs, rhs);
}

bool Symbol::isDifferent(const Symbol &other, bool ignoreBackground) {
  return !Symbol::compareFormats(_fmt, other._fmt, ignoreBackground);
}

bool Symbol::isDifferent(const QTextCharFormat &fmt, bool ignoreBackground) {
  return !Symbol::compareFormats(_fmt, fmt, ignoreBackground);
}

bool Symbol::compareFormats(const QTextCharFormat &fmt1, const QTextCharFormat &fmt2, bool ignoreBackground) {
  auto col1 = fixColor(fmt1.foreground(), false);
  auto col2 = fixColor(fmt2.foreground(), false);
  bool cmp = true;
  if(!ignoreBackground) {
    auto backCol1 = fixColor(fmt1.background(), true);
    auto backCol2 = fixColor(fmt2.background(), true);
      cmp = backCol1 == backCol2;
  }

  return fmt1.font() == fmt2.font() && col1 == col2 && cmp;
}

void Symbol::checkAndAssign (const QJsonObject &json, QFont *font, QBrush *col, QBrush *bac) {
  auto idValue = json["i"];
  auto charValue = json["c"];
  auto fmtValue = json["f"];
  auto posValue = json["p"];

  if(idValue.isUndefined() || charValue.isUndefined() || fmtValue.isUndefined() || posValue.isUndefined()) {
    throw SymbolFromJsonException{"The QJsonObject has some fields missing"};
  }

  if(!idValue.isObject() || !fmtValue.isObject() || !posValue.isArray()) {
    throw SymbolFromJsonException{"One or more fields are not valid"};
  }

  auto charUnicode = charValue.toInt(-1);

  if(charUnicode < 0) {
    throw SymbolFromJsonException{"One or more fields are not valid"};
  }

  auto idJson = idValue.toObject();
  auto fmt = deserializeFormat(fmtValue.toObject(), font, col, bac);

  _pos = utils::jsonArrayToVector<Identifier>(posValue.toArray());
  _id = SymbolId{idJson};
  _char = QChar(charUnicode);
  _fmt = fmt;
}

Symbol Symbol::fromJsonObject(const QJsonObject &json, QFont *font, QBrush *col, QBrush *bac) {
  return Symbol(json, font, col, bac);
}

Symbol Symbol::fromJsonObject(QJsonObject &&json, QFont *font, QBrush *col, QBrush *bac) {
  return Symbol(json, font, col, bac);
}

QJsonObject Symbol::toJsonObject(QFont *font, QBrush *col, QBrush *bac) const {
  QJsonObject json;

  json["i"] = QJsonValue(_id.toJsonObject());
  json["c"] = QJsonValue(_char.unicode());
  json["f"] = QJsonValue(serializeFormat(_fmt, font, col, bac));
  json["p"] = QJsonValue(utils::vectorToJsonArray(_pos));

  return json;
}

std::list<Symbol> Symbol::jsonArrayToSymbols(const QJsonArray &array){
  std::list<Symbol> symbols;
  QFont font;
  QBrush col, bac;

  for(auto &&el : array) {
    symbols.push_back(Symbol::fromJsonObject(el.toObject(), &font, &col, &bac));
  }

  return symbols;
}

QJsonArray Symbol::symbolsToJsonArray(const std::list<Symbol> &symbols){
  QJsonArray array;
  QFont font;
  QBrush col, bac;

  //force these objects to be invalid
  font.fromString("a");
  col.setColor(QColor{""});
  bac.setColor(QColor{""});

  for(auto &&sym : symbols) {
    array.push_back(sym.toJsonObject(&font, &col, &bac));
  }

  return array;
}

QJsonObject Symbol::serializeFormat(const QTextCharFormat &fmt, QFont *font, QBrush *col, QBrush *bac) {
  QJsonObject json;

  auto symFont = fmt.font();
  auto symCol = fmt.foreground();
  auto symBac = fmt.background();

  if(font == nullptr || *font != symFont) {
    json["f"] = QJsonValue(fontToString(symFont));
    if(font != nullptr) *font = symFont;
  }

  if(col == nullptr || *col != symCol) {
    json["c"] = QJsonValue(brushToString(symCol, false));
    if(col != nullptr) *col = symCol;
  }

  if(bac == nullptr || *bac != symBac) {
    json["b"] = QJsonValue(brushToString(symBac, true));
    if(bac != nullptr) *bac = symBac;
  }

  return json;
}

QTextCharFormat Symbol::deserializeFormat(const QJsonObject &json, QFont *font, QBrush *col, QBrush *bac) {
  QTextCharFormat fmt;

  auto fontValue = json["f"];
  auto colorValue = json["c"];
  auto backgroundColorValue = json["b"];

  //font
  if(fontValue.isUndefined()) {
    if(font == nullptr) throw SymbolFromJsonException{"The QJsonObject has some fields missing"};
    fmt.setFont(*font);
  }
  else {
    if(!fontValue.isString()) throw SymbolFromJsonException{"One or more fields are not valid"};
    auto f = stringToFont(fontValue.toString());
    fmt.setFont(f);
    if(font != nullptr) *font = f;
  }

  //color
  if(colorValue.isUndefined()) {
    if(col == nullptr) throw SymbolFromJsonException{"The QJsonObject has some fields missing"};
    fmt.setForeground(*col);
  }
  else {
    if(!colorValue.isString()) throw SymbolFromJsonException{"One or more fields are not valid"};
    auto brush = stringToBrush(colorValue.toString());
    fmt.setForeground(brush);
    if(col != nullptr) *col = brush;
  }

  //background color
  if(backgroundColorValue.isUndefined()) {
    if(bac == nullptr) throw SymbolFromJsonException{"The QJsonObject has some fields missing"};
    fmt.setBackground(*bac);
  }
  else {
    if(!backgroundColorValue.isString()) throw SymbolFromJsonException{"One or more fields are not valid"};
    auto brush = stringToBrush(backgroundColorValue.toString());
    fmt.setBackground(brush);
    if(bac != nullptr) *bac = brush;
  }

  return fmt;
}

QColor Symbol::fixColor(const QBrush &brush, bool isBackground) {
  auto color = brush.color();
  if(color.alpha() == 255 && !brush.isOpaque()) {
    //FIX bug
    if(isBackground) color.setAlpha(0);
    else color.setAlpha(255);
  }

  return color;
}

QString Symbol::brushToString(const QBrush &b, bool isBackground) {
  return fixColor(b, isBackground).name(QColor::HexArgb);
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

void Symbol::setPos(const std::vector<Identifier> &pos) {
  _pos = pos;
}

std::vector<Identifier> Symbol::getPos() const{
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

void Symbol::localUpdate(const QTextCharFormat &fmt, bool ignoreBackground) {
  auto background = _fmt.background();
  _fmt = fmt;
  if(ignoreBackground) _fmt.setBackground(background);

  _timestamp = QDateTime::currentDateTimeUtc();
  _lastUser = _id.getFirst();
}

void Symbol::remoteUpdate(const Symbol &other, const QDateTime &timestamp, int userId) {
  _fmt = other._fmt;
  _timestamp = timestamp;
  _lastUser = userId;
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

QDateTime Symbol::getTimestamp() const {
  return _timestamp;
}

void Symbol::setTimestamp(const QDateTime &time) {
  _timestamp = time;
}

bool Symbol::isOlder(const QDateTime &time, int userId) {
  if(_timestamp == time) {
    return _lastUser > userId;
  }

  return _timestamp < time;
}
