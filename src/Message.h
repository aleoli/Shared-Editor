#pragma once

#include <QJsonObject>
#include <QJsonValue>
#include <QString>
#include <QJsonArray>

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
      FILE_DELETED,
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
  Message(Message::Type type, int action, Message::Status status);
  Message(Message::Type type, int action, Message::Status status, QJsonObject data);
  explicit Message(const QJsonObject &json);
  explicit Message(QJsonObject &&json);

  static Message fromJsonObject(const QJsonObject &json);
  static Message fromJsonObject(QJsonObject &&json);
  static Message fromQByteArray(const QByteArray &array);
  static Message fromQByteArray(QByteArray &&array);
  QJsonObject toJsonObject() const;
  QByteArray toQByteArray() const;

  friend bool operator==(const Message& lhs, const Message& rhs);
  friend bool operator!=(const Message& lhs, const Message& rhs);

  Message::Type getType() const;
  void setType(Message::Type type);
  int getAction() const;
  void setAction(int action);
  Message::Status getStatus() const;
  void setStatus(Message::Status status);
  QJsonObject getData() const;
  void setData(const QJsonObject &data);
  std::string to_string() const;

  int getInt(const QString &key) const;
  QString getString(const QString &key) const;
  QJsonObject getObject(const QString &key) const;
  QJsonArray getArray(const QString &key) const;

  void setValue(const QString &key, int value);
  void setValue(const QString &key, const QString &value);
  void setValue(const QString &key, const QJsonObject &value);
  void setValue(const QString &key, const QJsonArray &value);

private:
  void checkAndAssign(const QJsonObject &json);

  Message::Type _type;
  int _action;
  Message::Status _status;
  QJsonObject _data;
};
