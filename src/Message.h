#pragma once

#include <QJsonObject>

enum class MessageTypeEnum {
  FILE,
  USER,
  FILESYSTEM
};

enum class MessageFileActionEnum {
  NEW_FILE,
  GET_FILE,
  MOD_FILE,
  MOVE_CURSOR,
  CLOSE_FILE,
  RENAME_FILE,
  MOVE_FILE,
  SHARE_FILE,
  LINK_FILE
};

enum class MessageUserActionEnum {
  NEW_USER,
  LOGIN,
  CHANGE_NICKNAME,
  CHANGE_ICON,
  CHANGE_PASSWORD
};

enum class MessageFileSystemActionEnum {
  NEW_DIR,
  REMOVE_DIR,
  RENAME_DIR,
  LIST_FILES
};

enum class MessageStatusEnum {
  QUERY,
  RESPONSE
};

class Message {
public:
  Message(MessageTypeEnum type, int action, bool error, MessageStatusEnum status, QJsonObject data);

  static Message fromJsonObject(QJsonObject &json);
  QJsonObject toJsonObject();

  MessageTypeEnum getType();
  int getAction();
  bool getError();
  MessageStatusEnum getStatus();
  QJsonObject getData();

private:
  MessageTypeEnum _type;
  int _action;
  bool _error;
  MessageStatusEnum _status;
  QJsonObject _data;
};
