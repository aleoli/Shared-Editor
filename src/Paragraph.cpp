#include "Paragraph.h"

#include <utility>

#include "exceptions.h"
#include "utils.h"

using namespace se_exceptions;

Paragraph::Paragraph() : _timestamp(QDateTime()) {}

Paragraph::Paragraph(ParagraphId id) : _id(id), _alignment(Qt::AlignLeft), _timestamp(QDateTime()) {}

Paragraph::Paragraph(ParagraphId id, Qt::Alignment alignment)
  : _id(id), _alignment(alignment), _timestamp(QDateTime()) {}

Paragraph::Paragraph(const QJsonObject &json) : _timestamp(QDateTime()) {
  checkAndAssign(json);
}

Paragraph::Paragraph(QJsonObject &&json) : _timestamp(QDateTime()) {
  checkAndAssign(json);
}

Paragraph::Paragraph(const Paragraph& s) = default;
Paragraph::Paragraph(Paragraph&& s) noexcept: _id(s._id), _alignment(s._alignment),
  _pos(std::move(s._pos)), _timestamp(std::move(s._timestamp)), _lastUser(s._lastUser){}

Paragraph &Paragraph::operator=(const Paragraph &s) = default;
Paragraph &Paragraph::operator=(Paragraph &&s) noexcept {
  if(this == &s) {
    return *this;
  }
  this->_id = s._id;
  this->_alignment = s._alignment;
  this->_pos = std::move(s._pos);
  this->_timestamp = std::move(s._timestamp);
  this->_lastUser == s._lastUser;
  return *this;
}

bool operator<(const Paragraph& lhs, const Paragraph& rhs) {
  if(lhs._pos == rhs._pos) {
    return lhs._id < rhs._id;
  }

  return lhs._pos < rhs._pos;
}

bool operator==(const Paragraph& lhs, const Paragraph& rhs) {
  return lhs._id == rhs._id && lhs._alignment == rhs._alignment &&
    lhs._pos == rhs._pos && lhs._timestamp == rhs._timestamp && lhs._lastUser == rhs._lastUser;
}

bool operator!=(const Paragraph& lhs, const Paragraph& rhs) {
  return !operator==(lhs, rhs);
}

bool Paragraph::isDifferent(const Paragraph &other) {
  return _alignment != other._alignment;
}

bool Paragraph::isDifferent(Qt::Alignment alignment) {
  return _alignment != alignment;
}

void Paragraph::localUpdate(Qt::Alignment alignment) {
  _alignment = alignment;
  _timestamp = QDateTime::currentDateTimeUtc();
  _lastUser = _id.getFirst();
}

void Paragraph::remoteUpdate(const Paragraph &other) {
  _alignment = other._alignment;
  _timestamp = other._timestamp;
  _lastUser = other._lastUser;
}

void Paragraph::checkAndAssign (const QJsonObject &json) {
  auto idValue = json["i"];
  auto alignmentValue = json["a"];
  auto posValue = json["p"];

  if(idValue.isUndefined() || alignmentValue.isUndefined() || posValue.isUndefined()) {
    throw SymbolFromJsonException{"The QJsonObject has some fields missing"};
  }

  if(!idValue.isObject() || !posValue.isArray()) {
    throw SymbolFromJsonException{"One or more fields are not valid"};
  }

  auto alignment = alignmentValue.toInt(-1);

  if(alignment < 0) {
    throw SymbolFromJsonException{"One or more fields are not valid"};
  }

  auto idJson = idValue.toObject();

  _pos = utils::jsonArrayToVector<Identifier>(posValue.toArray());
  _id = ParagraphId{idJson};
  _alignment = Qt::AlignmentFlag(alignment);
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
  json["a"] = QJsonValue(static_cast<int>(_alignment));
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

void Paragraph::setAlignment(Qt::Alignment alignment) {
  _alignment = alignment;
}

Qt::Alignment Paragraph::getAlignment() const {
  return _alignment;
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
