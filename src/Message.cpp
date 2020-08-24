#include "Message.h"

#include "exceptions.h"
#include "def.h"

#include <QJsonDocument>
#include <sstream>
#include <QStringList>
#include <utility>

using namespace se_exceptions;

Message::Message(): _type(Type::ERROR), _action(0), _status(Status::RESPONSE) {
}

Message::Message(Message::Type type, int action, Message::Status status)
  : _type(type), _action(action), _status(status) {}

Message::Message(Message::Type type, int action, Message::Status status, QJsonObject data)
  : _type(type), _action(action), _status(status), _data(std::move(data)) {}

Message::Message(const QJsonObject &json) {
  checkAndAssign(json);
}

Message::Message(QJsonObject &&json) {
  checkAndAssign(json);
}

void Message::checkAndAssign(const QJsonObject &json) {
  auto typeValue = json["type"];
  auto actionValue = json["action"];
  auto statusValue = json["status"];
  auto dataValue = json["data"];

  if(typeValue.isUndefined() || actionValue.isUndefined()
        || statusValue.isUndefined() || dataValue.isUndefined()) {
          throw MessageFromJsonException{"The QJsonObject has some fields missing"};
        }

  auto type = static_cast<Message::Type>(typeValue.toInt(-1));
  auto status = static_cast<Message::Status>(statusValue.toInt(-1));

  if(type < Message::Type::ERROR || type > Message::Type::FILESYSTEM
      || status < Message::Status::QUERY || status > Message::Status::RESPONSE) {
        throw MessageFromJsonException{"One or more fields are not valid"};
      }

  auto action = actionValue.toInt(-1);
  if(action == -1) {
    throw MessageFromJsonException{"One or more fields are not valid"};
  }

  if(!dataValue.isObject()) {
    throw MessageFromJsonException{"One or more fields are not valid"};
  }
  auto data = dataValue.toObject();

  // setting parameters
  _type = type;
  _action = action;
  _status = status;
  _data = data;
}

Message Message::fromJsonObject(const QJsonObject &json) {
  return Message(json);
}

Message Message::fromJsonObject(QJsonObject &&json) {
  return Message(json);
}

Message Message::fromQByteArray(const QByteArray &array) {
  #if BINARY_MESSAGE
      auto doc = QJsonDocument::fromBinaryData(array);
  #else
      auto doc = QJsonDocument::fromJson(array);
  #endif

  return Message(doc.object());
}

Message Message::fromQByteArray(QByteArray &&array) {
  #if BINARY_MESSAGE
      auto doc = QJsonDocument::fromBinaryData(array);
  #else
      auto doc = QJsonDocument::fromJson(array);
  #endif

  return Message(doc.object());
}

QJsonObject Message::toJsonObject() const {
  QJsonObject json;

  json["type"] = QJsonValue(static_cast<int>(_type));
  json["action"] = QJsonValue(_action);
  json["status"] = QJsonValue(static_cast<int>(_status));
  json["data"] = QJsonValue(_data);

  return json;
}

QByteArray Message::toQByteArray() const {
  QJsonDocument doc(toJsonObject());

#if BINARY_MESSAGE
  return doc.toBinaryData();
#else
  return doc.toJson(QJsonDocument::Compact);
#endif
}

Message::Type Message::getType() const {
  return _type;
}

void Message::setType(Message::Type type) {
  _type = type;
}

int Message::getAction() const {
  return _action;
}

void Message::setAction(int action) {
  _action = action;
}

Message::Status Message::getStatus() const {
  return _status;
}

void Message::setStatus(Message::Status status) {
  _status = status;
}

QJsonObject Message::getData() const {
  return _data;
}

void Message::setData(const QJsonObject &data) {
  _data = data;
}

int Message::getInt(const QString &key) const {
  auto value = _data[key];

  if(value.isUndefined()) {
    throw MessageDataException{"Data object has no value with key "+key};
  }

  int val = value.toInt(std::numeric_limits<int>::min());

  if(val == std::numeric_limits<int>::min()) {
    throw MessageDataException{key+" value is not an integer"};
  }

  return val;
}

QString Message::getString(const QString &key) const {
  auto value = _data[key];

  if(value.isUndefined()) {
    throw MessageDataException{"Data object has no value with key "+key};
  }

  if(!value.isString()) {
    throw MessageDataException{key+" value is not a string"};
  }

  return value.toString();
}

QJsonObject Message::getObject(const QString &key) const {
  auto value = _data[key];

  if(value.isUndefined()) {
    throw MessageDataException{"Data object has no value with key "+key};
  }

  if(!value.isObject()) {
    throw MessageDataException{key+" value is not an object"};
  }

  return value.toObject();
}

QJsonArray Message::getArray(const QString &key) const {
  auto value = _data[key];

  if(value.isUndefined()) {
    throw MessageDataException{"Data object has no value with key "+key};
  }

  if(!value.isArray()) {
    throw MessageDataException{key+" value is not an array"};
  }

  return value.toArray();
}

std::optional<int> Message::getIntOpt(const QString &key) const {
  auto value = _data[key];

  if(value.isUndefined()) {
    return std::nullopt;
  }

  int val = value.toInt(std::numeric_limits<int>::min());

  if(val == std::numeric_limits<int>::min()) {
    return std::nullopt;
  }

  return std::optional<int>(val);
}

std::optional<QString> Message::getStringOpt(const QString &key) const {
  auto value = _data[key];

  if(value.isUndefined()) {
    return std::nullopt;
  }

  if(!value.isString()) {
    return std::nullopt;
  }

  return std::optional<QString>(value.toString());
}

std::optional<QJsonObject> Message::getObjectOpt(const QString &key) const {
  auto value = _data[key];

  if(value.isUndefined()) {
    return std::nullopt;
  }

  if(!value.isObject()) {
    return std::nullopt;
  }

  return std::optional<QJsonObject>(value.toObject());
}

std::optional<QJsonArray> Message::getArrayOpt(const QString &key) const {
  auto value = _data[key];

  if(value.isUndefined()) {
    return std::nullopt;
  }

  if(!value.isArray()) {
    return std::nullopt;
  }

  return std::optional<QJsonArray>(value.toArray());
}

void Message::setValue(const QString &key, int value) {
  if(!_data.value(key).isUndefined()) {
    throw MessageDataException{"Key is already present in data"};
  }

  _data[key] = value;
}

void Message::setValue(const QString &key, const QString &value) {
  if(!_data.value(key).isUndefined()) {
    throw MessageDataException{"Key is already present in data"};
  }

  _data[key] = value;
}

void Message::setValue(const QString &key, const QJsonObject &value) {
  if(!_data.value(key).isUndefined()) {
    throw MessageDataException{"Key is already present in data"};
  }

  _data[key] = value;
}

void Message::setValue(const QString &key, const QJsonArray &value) {
  if(!_data.value(key).isUndefined()) {
    throw MessageDataException{"Key is already present in data"};
  }

  _data[key] = value;
}

std::string Message::to_string() const {
  std::stringstream ss;

  ss << "Type: " << static_cast<int>(_type) << std::endl;
  ss << "Action: " << _action << std::endl;
  ss << "Status: " << static_cast<int>(_status) << std::endl;
  ss << "Data: " << std::endl;

  for(auto &key : _data.keys()) {
    ss << '\t' << key.toStdString() << ", type: " << static_cast<int>(_data[key].type()) << std::endl;
  }

  return ss.str();
}

bool operator==(const Message& lhs, const Message& rhs) {
  return lhs._type == rhs._type && lhs._action == rhs._action
  && lhs._status == rhs._status && lhs._data == rhs._data;
}

bool operator!=(const Message& lhs, const Message& rhs) {
  return !operator==(lhs, rhs);
}
