#pragma once

#include <QJsonObject>
#include <QJsonValue>

class Message {
public:
  enum class Type {
      ERROR,
      USER,
      FILE,
      FILE_EDIT,
      FILESYSTEM
  };

  enum class UserAction {
      LOGIN,
      LOGOUT,
      NEW,
      EDIT,
      DELETE
  };

  enum class FileAction {
      NEW,
      GET,
      CLOSE,
      EDIT,
      DELETE,
      GET_LINK,
      ACTIVATE_LINK
  };

  enum class FileEditAction {
      LOCAL_INSERT,
      REMOTE_INSERT,
      LOCAL_DELETE,
      REMOTE_DELETE,
      LOCAL_UPDATE,
      REMOTE_UPDATE,
      USER_CONNECTED,
      USER_DISCONNECTED,
      LOCAL_MOVE,
      REMOTE_MOVE
  };

  enum class FileSystemAction {
      NEW_DIR,
      EDIT_DIR,
      DELETE_DIR,
      GET_DIR,
      MOVE_FILE
  };

  enum class Status {
    QUERY,
    RESPONSE
  };

  explicit Message();
  Message(Message::Type type, int action, Message::Status status, QJsonObject data);
  explicit Message(const QJsonObject &json);
  explicit Message(QJsonObject &&json);

  static Message fromJsonObject(const QJsonObject &json);
  static Message fromJsonObject(QJsonObject &&json);
  static Message fromQByteArray(const QByteArray &array);
  static Message fromQByteArray(QByteArray &&array);
  QJsonObject toJsonObject() const;
  QByteArray toQByteArray() const;

  Message::Type getType() const;
  int getAction() const;
  Message::Status getStatus() const;
  QJsonObject getData() const;

  std::string to_string() const; // for debug

private:
  void checkAndAssign(const QJsonObject &json);

  Message::Type _type;
  int _action;
  Message::Status _status;
  QJsonObject _data;
};
