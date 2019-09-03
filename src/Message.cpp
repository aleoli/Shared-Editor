#include "Message.h"

#include <QJsonValue>

Message::Message(Message::Type type, int action, bool error, Message::Status status, QJsonObject data)
  : _type(type), _action(action), _error(error), _status(status), _data(data) {}

Message Message::fromJsonObject(QJsonObject &json) {
  Message::Type type = static_cast<Message::Type>(json["type"].toInt());
  int action = json["action"].toInt();
  bool error = json["error"].toBool();
  Message::Status status = static_cast<Message::Status>(json["status"].toInt());
  QJsonObject data = json["data"].toObject();

  return Message(type, action, error, status, data);
}

QJsonObject Message::toJsonObject() {
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
