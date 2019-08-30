#include "Message.h"

#include <QJsonValue>

Message::Message(MessageTypeEnum type, int action, bool error, MessageStatusEnum status, QJsonObject data)
  : _type(type), _action(action), _error(error), _status(status), _data(data) {}

Message Message::fromJsonObject(QJsonObject &json) {
  MessageTypeEnum type = static_cast<MessageTypeEnum>(json["type"].toInt());
  int action = json["action"].toInt();
  bool error = json["error"].toBool();
  MessageStatusEnum status = static_cast<MessageStatusEnum>(json["status"].toInt());
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

MessageTypeEnum Message::getType() {
  return _type;
}

int Message::getAction() {
  return _action;
}

bool Message::getError() {
  return _error;
}

MessageStatusEnum Message::getStatus() {
  return _status;
}

QJsonObject Message::getData() {
  return _data;
}
