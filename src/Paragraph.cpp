#include "Paragraph.h"

#include <utility>

#include "exceptions.h"
#include "utils.h"

using namespace se_exceptions;

Paragraph::Paragraph() : _timestamp(QDateTime()) {}

Paragraph::Paragraph(ParagraphId id) : _id(id), _fmt(QTextBlockFormat()), _timestamp(QDateTime()) {}

Paragraph::Paragraph(ParagraphId id, const QTextBlockFormat &fmt)
  : _id(id), _fmt(fmt), _timestamp(QDateTime()) {}

Paragraph::Paragraph(const QJsonObject &json) : _timestamp(QDateTime()) {
  checkAndAssign(json);
}

Paragraph::Paragraph(QJsonObject &&json) : _timestamp(QDateTime()) {
  checkAndAssign(json);
}

Paragraph::Paragraph(const Paragraph& s) = default;
Paragraph::Paragraph(Paragraph&& s) noexcept: _id(s._id), _fmt(s._fmt),
  _pos(std::move(s._pos)), _timestamp(std::move(s._timestamp)), _lastUser(s._lastUser){}

Paragraph &Paragraph::operator=(const Paragraph &s) = default;
Paragraph &Paragraph::operator=(Paragraph &&s) noexcept {
  if(this == &s) {
    return *this;
  }
  this->_id = s._id;
  this->_fmt = s._fmt;
  this->_pos = std::move(s._pos);
  this->_timestamp = std::move(s._timestamp);
  this->_lastUser = s._lastUser;
  return *this;
}

bool operator<(const Paragraph& lhs, const Paragraph& rhs) {
  if(lhs._pos == rhs._pos) {
    return lhs._id < rhs._id;
  }

  return lhs._pos < rhs._pos;
}

bool operator==(const Paragraph& lhs, const Paragraph& rhs) {
  return lhs._id == rhs._id && !lhs.isDifferent(rhs) &&
    lhs._pos == rhs._pos && lhs._timestamp == rhs._timestamp && lhs._lastUser == rhs._lastUser;
}

bool operator!=(const Paragraph& lhs, const Paragraph& rhs) {
  return !operator==(lhs, rhs);
}

bool Paragraph::isDifferent(const Paragraph &other) const {
  return isDifferent(other._fmt);
}

bool Paragraph::isDifferent(const QTextBlockFormat &fmt) const {
  // to add new attributes, add a check here
  return _fmt.alignment() != fmt.alignment();
}

void Paragraph::localUpdate(const QTextBlockFormat &fmt) {
  _fmt = fmt;
  _timestamp = QDateTime::currentDateTimeUtc();
  _lastUser = _id.getFirst();
}

void Paragraph::remoteUpdate(const Paragraph &other, const QDateTime &timestamp, int userId) {
  _fmt = other._fmt;
  _timestamp = timestamp;
  _lastUser = userId;
}

void Paragraph::checkAndAssign(const QJsonObject &json) {
  auto idValue = json["i"];
  auto fmtValue = json["f"];
  auto posValue = json["p"];

  if(idValue.isUndefined() || fmtValue.isUndefined() || posValue.isUndefined()) {
    throw ParagraphFromJsonException{"The QJsonObject has some fields missing"};
  }

  if(!fmtValue.isObject() || !idValue.isObject() || !posValue.isArray()) {
    throw ParagraphFromJsonException{"One or more fields are not valid"};
  }

  _pos = utils::jsonArrayToVector<Identifier>(posValue.toArray());
  _id = ParagraphId{idValue.toObject()};
  _fmt = deserializeFormat(fmtValue.toObject());
}

Paragraph Paragraph::fromJsonObject(const QJsonObject &json) {
  return Paragraph(json);
}

Paragraph Paragraph::fromJsonObject(QJsonObject &&json) {
  return Paragraph(json);
}

QJsonObject Paragraph::toJsonObject() const {
  QJsonObject json;

  json["i"] = QJsonValue(_id.toJsonObject());
  json["f"] = QJsonValue(serializeFormat(_fmt));
  json["p"] = QJsonValue(utils::vectorToJsonArray(_pos));

  return json;
}

std::list<Paragraph> Paragraph::jsonArrayToParagraphs(const QJsonArray &array){
  std::list<Paragraph> paragraphs;

  for(auto &&el : array) {
    paragraphs.push_back(Paragraph::fromJsonObject(el.toObject()));
  }

  return paragraphs;
}

QJsonArray Paragraph::paragraphsToJsonArray(const std::list<Paragraph> &paragraphs){
  QJsonArray array;

  for(auto &&par : paragraphs) {
    array.push_back(par.toJsonObject());
  }

  return array;
}

void Paragraph::setParagraphId(const ParagraphId &id) {
  _id = id;
}

ParagraphId Paragraph::getParagraphId() const{
  return _id;
}

void Paragraph::setPos(const std::vector<Identifier> &pos) {
  _pos = pos;
}

std::vector<Identifier> Paragraph::getPos() const{
  return _pos;
}

void Paragraph::setFormat(const QTextBlockFormat &fmt) {
  _fmt = fmt;
}

QTextBlockFormat Paragraph::getFormat() const {
  return _fmt;
}

QTextBlockFormat Paragraph::getPartialFormat() const {
  QTextBlockFormat fmt;
  fmt.setAlignment(_fmt.alignment());

  return fmt;
}

QDateTime Paragraph::getTimestamp() const {
  return _timestamp;
}

void Paragraph::setTimestamp(const QDateTime &time) {
  _timestamp = time;
}

bool Paragraph::isOlder(const QDateTime &time, int userId) {
  if(_timestamp == time) {
    return _lastUser > userId;
  }

  return _timestamp < time;
}

QJsonObject Paragraph::serializeFormat(const QTextBlockFormat &fmt) {
  QJsonObject json;

  json["a"] = static_cast<int>(fmt.alignment());
  //to add new attributes, serialize them here

  return json;
}

QTextBlockFormat Paragraph::deserializeFormat(const QJsonObject &obj) {
  QTextBlockFormat fmt;
  auto alignmentValue = obj["a"];
  //to add new attributes, deserialize them here

  if(alignmentValue.isUndefined()) {
    throw ParagraphFromJsonException{"The QJsonObject has some fields missing"};
  }

  auto alignment = alignmentValue.toInt(-1);

  if(alignment < 0) {
    throw ParagraphFromJsonException{"One or more fields are not valid"};
  }

  fmt.setAlignment(static_cast<Qt::Alignment>(alignment));
  return fmt;
}
