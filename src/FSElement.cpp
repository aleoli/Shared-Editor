#include "FSElement.h"

#include <sstream>
#include <QJsonValue>

#include "exceptions.h"

using namespace se_exceptions;

FSElement::FSElement() : _id(-1), _parentId(-1) {}

FSElement::FSElement(int id, int parentId, QString name, Type type)
  : _id(id), _parentId(parentId), _name(name), _type(type) {}

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

  if(idValue.isUndefined() || parentIdValue.isUndefined()
    || nameValue.isUndefined() || typeValue.isUndefined()) {
    throw FSElementFromJsonException{"The QJsonObject has some fields missing"};
  }

  auto id = idValue.toInt(-1);
  auto parentId = parentIdValue.toInt(-1);

  if(id == -1 || parentId == -1) {
    throw FSElementFromJsonException{"One or more fields are not valid"};
  }

  if(!nameValue.isString()) {
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

  return json;
}

int FSElement::getId() const {
  return _id;
}

int FSElement::getParentId() const {
  return _parentId;
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
