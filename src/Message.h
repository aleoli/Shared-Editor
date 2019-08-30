#pragma once

#include <QJsonObject>

enum class MessageTypeEnum {
  FILE,
  USER,
  FILESYSTEM
};

enum class MessageActionEnum {
  CREATE,
  LOGIN,
  REMOVE,
  RENAME,
  LIST,
  GET,
  SYMBOL,
  CURSOR,
  CLOSE,
  MOVE,
  SHARE,
  LINK,
  CHANGE_ICON,
  CHANGE_PASSWORD
};

class Message {
public:
  Message(MessageTypeEnum type, MessageActionEnum action, bool error, QJsonObject data);

  static Message fromJsonObject(QJsonObject &json);
  QJsonObject toJsonObject();

  MessageTypeEnum getType();
  MessageActionEnum getAction();
  bool getError();
  QJsonObject getData();

private:
  MessageTypeEnum _type;
  MessageActionEnum _action;
  bool _error;
  QJsonObject _data;
};
