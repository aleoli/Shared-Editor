#include "FSElement.h"

#include <sstream>
#include <QJsonValue>
#include <utility>

#include "exceptions.h"
#include "utils.h"

using namespace se_exceptions;

FSElement::FSElement() : _id(-1), _parentId(-1), _type(Type::FILE), _creationDate(QDateTime::currentDateTimeUtc()) {}

FSElement::FSElement(int id, int parentId, QString name, Type type, QDateTime creationDate)
  : _id(id), _parentId(parentId), _name(std::move(name)), _type(type), _creationDate(std::move(creationDate)) {}

FSElement::FSElement(const QJsonObject &json) {
  checkAndAssign(json);
}

FSElement::FSElement(QJsonObject &&json) {
  checkAndAssign(json);
}

void FSElement::checkAndAssign(const QJsonObject &json) {
  auto idValue = json["id"];
  auto parentIdValue = json["parentId"];
  auto nameValue = json["name"];
  auto typeValue = json["type"];
  auto dateValue = json["creationDate"];

  if(idValue.isUndefined() || parentIdValue.isUndefined()
    || nameValue.isUndefined() || typeValue.isUndefined() || dateValue.isUndefined()) {
    throw FSElementFromJsonException{"The QJsonObject has some fields missing"};
  }

  auto id = idValue.toInt(std::numeric_limits<int>::min());
  auto parentId = parentIdValue.toInt(std::numeric_limits<int>::min());

  if(id == std::numeric_limits<int>::min() || parentId == std::numeric_limits<int>::min()) {
    throw FSElementFromJsonException{"One or more fields are not valid"};
  }

  if(!nameValue.isString() || !dateValue.isString()) {
    throw FSElementFromJsonException{"One or more fields are not valid"};
  }

  auto type = static_cast<FSElement::Type>(typeValue.toInt(-1));
  if(type < FSElement::Type::FILE || type > FSElement::Type::DIRECTORY) {
        throw FSElementFromJsonException{"One or more fields are not valid"};
  }

  _id = id;
  _parentId = parentId;
  _name = nameValue.toString();
  _type = type;
  _creationDate = QDateTime::fromString(dateValue.toString());
}

FSElement FSElement::fromJsonObject(const QJsonObject &json) {
  return FSElement(json);
}

FSElement FSElement::fromJsonObject(QJsonObject &&json) {
  return FSElement(json);
}

QJsonObject FSElement::toJsonObject() const {
  QJsonObject json;

  json["id"] = QJsonValue(_id);
  json["parentId"] = QJsonValue(_parentId);
  json["name"] = QJsonValue(_name);
  json["type"] = QJsonValue(static_cast<int>(_type));
  json["creationDate"] = QJsonValue(_creationDate.toString());

  return json;
}

int FSElement::getId() const {
  return _id;
}

int FSElement::getParentId() const {
  return _parentId;
}

void FSElement::setParentId(int parentId) {
  _parentId = parentId;
}

QString FSElement::getName() const {
  return _name;
}

void FSElement::setName(const QString &name) {
  _name = name;
}

FSElement::Type FSElement::getType() const {
  return _type;
}

QDateTime FSElement::getCreationDate() const {
  return _creationDate;
}

std::string FSElement::to_string() const {
  std::stringstream ss;

  ss << "id:" << _id << std::endl;
  ss << "parent id:" << _parentId << std::endl;
  ss << "name:" << _name.toStdString() << std::endl;
  ss << "type:" << static_cast<int>(_type);

  return ss.str();
}

bool operator==(const FSElement& lhs, const FSElement& rhs) {
  return lhs._id == rhs._id && lhs._parentId == rhs._parentId &&
    lhs._name == rhs._name && lhs._type == rhs._type;
}

bool operator!=(const FSElement& lhs, const FSElement& rhs) {
  return !operator==(lhs, rhs);
}
