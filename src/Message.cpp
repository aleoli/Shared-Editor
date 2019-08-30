#include "Message.h"

#include <QJsonValue>

Message::Message(MessageTypeEnum type, MessageActionEnum action, bool error, QJsonObject data)
  : _type(type), _action(action), _error(error), _data(data) {}

Message Message::fromJsonObject(QJsonObject &json) {
  MessageTypeEnum type = static_cast<MessageTypeEnum>(json["type"].toInt());
  MessageActionEnum action = static_cast<MessageActionEnum>(json["action"].toInt());
  bool error = json["error"].toBool();
  QJsonObject data = json["data"].toObject();

  return Message(type, action, error, data);
}

QJsonObject Message::toJsonObject() {
  QJsonObject json;

  json["type"] = QJsonValue(static_cast<int>(_type));
  json["action"] = QJsonValue(static_cast<int>(_action));
  json["error"] = QJsonValue(_error);
  json["data"] = QJsonValue(_data);

  return json;
}

MessageTypeEnum Message::getType(){
  return _type;
}

MessageActionEnum Message::getAction(){
  return _action;
}

bool Message::getError(){
  return _error;
}

QJsonObject Message::getData(){
  return _data;
}
