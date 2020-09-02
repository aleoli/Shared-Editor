#include "Paragraph.h"

#include <utility>

#include "exceptions.h"
#include "utils.h"

using namespace se_exceptions;

Paragraph::Paragraph() = default;

Paragraph::Paragraph(ParagraphId id) : _id(id), _alignment(Qt::AlignLeft) {}

Paragraph::Paragraph(ParagraphId id, Qt::Alignment alignment)
  : _id(id), _alignment(alignment) {}

Paragraph::Paragraph(const QJsonObject &json) {
  checkAndAssign(json);
}

Paragraph::Paragraph(QJsonObject &&json) {
  checkAndAssign(json);
}

Paragraph::Paragraph(const Paragraph& s) = default;
Paragraph::Paragraph(Paragraph&& s) noexcept: _id(s._id), _alignment(s._alignment), _pos(std::move(s._pos)) {}

Paragraph &Paragraph::operator=(const Paragraph &s) = default;
Paragraph &Paragraph::operator=(Paragraph &&s) noexcept {
  if(this == &s) {
    return *this;
  }
  this->_id = s._id;
  this->_alignment = s._alignment;
  this->_pos = std::move(s._pos);
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
    lhs._pos == rhs._pos;
}

bool operator!=(const Paragraph& lhs, const Paragraph& rhs) {
  return !operator==(lhs, rhs);
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
