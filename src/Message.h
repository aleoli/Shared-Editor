#pragma once

#include <QJsonObject>

class Message {
public:
  enum class Type {
    FILE,
    USER,
    FILESYSTEM
  };

  enum class FileAction {
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

  enum class UserAction {
    NEW_USER,
    LOGIN,
    CHANGE_NICKNAME,
    CHANGE_ICON,
    CHANGE_PASSWORD
  };

  enum class FileSystemAction {
    NEW_DIR,
    REMOVE_DIR,
    RENAME_DIR,
    LIST_FILES
  };

  enum class Status {
    QUERY,
    RESPONSE
  };

  Message(Message::Type type, int action, bool error, Message::Status status, QJsonObject data);

  static Message fromJsonObject(QJsonObject &json);
  QJsonObject toJsonObject();

  Message::Type getType() const;
  int getAction() const;
  bool getError() const;
  Message::Status getStatus() const;
  QJsonObject getData() const;

private:
  Message::Type _type;
  int _action;
  bool _error;
  Message::Status _status;
  QJsonObject _data;
};