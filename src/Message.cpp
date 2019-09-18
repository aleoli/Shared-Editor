#include "Message.h"

#include "exceptions.h"
#include <QJsonValue>

using namespace se_exceptions;

Message::Message() {
}

Message::Message(Message::Type type, int action, bool error, Message::Status status, QJsonObject data)
  : _type(type), _action(action), _error(error), _status(status), _data(data) {}

Message::Message(const QJsonObject &json) {
  checkAndAssign(json);
}

Message::Message(QJsonObject &&json) {
  checkAndAssign(json);
}

void Message::checkAndAssign(const QJsonObject &json) {
  auto typeValue = json["type"];
  auto actionValue = json["action"];
  auto errorValue = json["error"];
  auto statusValue = json["status"];
  auto dataValue = json["data"];

  if(typeValue.isUndefined() || actionValue.isUndefined() || errorValue.isUndefined()
        || statusValue.isUndefined() || dataValue.isUndefined()) {
          throw MessageFromJsonException{"The QJsonObject has some fields missing"};
        }

  auto type = static_cast<Message::Type>(typeValue.toInt(-1));
  auto status = static_cast<Message::Status>(statusValue.toInt(-1));

  if(type < Message::Type::FILE || type > Message::Type::FILESYSTEM
      || status < Message::Status::QUERY || status > Message::Status::RESPONSE) {
        throw MessageFromJsonException{"One or more fields are not valid"};
      }

  auto action = actionValue.toInt(-1);
  if(action == -1) {
    throw MessageFromJsonException{"One or more fields are not valid"};
  }

  if(!errorValue.isBool() || !dataValue.isObject()) {
    throw MessageFromJsonException{"One or more fields are not valid"};
  }

  auto error = errorValue.toBool();
  auto data = dataValue.toObject();

  // setting parameters
  _type = type;
  _action = action;
  _error = error;
  _status = status;
  _data = data;
}

Message Message::fromJsonObject(const QJsonObject &json) {
  return Message(json);
}

Message Message::fromJsonObject(QJsonObject &&json) {
  return Message(json);
}

QJsonObject Message::toJsonObject() const {
  QJsonObject json;

  json["type"] = QJsonValue(static_cast<int>(_type));
  json["action"] = QJsonValue(_action);
  json["error"] = QJsonValue(_error);
  json["status"] = QJsonValue(static_cast<int>(_status));
  json["data"] = QJsonValue(_data);

  return json;
}

Message::Type Message::getType() const {
  return _type;
}

int Message::getAction() const {
  return _action;
}

bool Message::getError() const {
  return _error;
}

Message::Status Message::getStatus() const {
  return _status;
}

QJsonObject Message::getData() const {
  return _data;
}
