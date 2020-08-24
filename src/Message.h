#pragma once

#include <QJsonObject>
#include <QJsonValue>
#include <QString>
#include <QJsonArray>
#include <optional>

class Message {
public:
  enum class Type {
      ERROR,
      USER,
      FILE,
      FILE_EDIT,
      COMMENT,
      FILESYSTEM
  };

  enum class UserAction {
      LOGIN,
      LOGOUT,
      NEW,
      EDIT,
      DELETE,
      GET_USER_ICON
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

  enum class CommentAction {
      COMMENT_LOCAL_INSERT,
      COMMENT_LOCAL_UPDATE,
      COMMENT_LOCAL_DELETE,
      COMMENT_REMOTE_INSERT,
      COMMENT_REMOTE_UPDATE,
      COMMENT_REMOTE_DELETE,
  };

  enum class FileSystemAction {
      NEW_DIR,
      EDIT_DIR,
      DELETE_DIR,
      GET_DIR,
      MOVE_FILE,
      GET_PATH
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
  [[nodiscard]] QJsonObject toJsonObject() const;
  [[nodiscard]] QByteArray toQByteArray() const;

  friend bool operator==(const Message& lhs, const Message& rhs);
  friend bool operator!=(const Message& lhs, const Message& rhs);

  [[nodiscard]] Message::Type getType() const;
  void setType(Message::Type type);
  [[nodiscard]] int getAction() const;
  void setAction(int action);
  [[nodiscard]] Message::Status getStatus() const;
  void setStatus(Message::Status status);
  [[nodiscard]] QJsonObject getData() const;
  void setData(const QJsonObject &data);
  [[nodiscard]] std::string to_string() const;

  [[nodiscard]] int getInt(const QString &key) const;
  [[nodiscard]] QString getString(const QString &key) const;
  [[nodiscard]] QJsonObject getObject(const QString &key) const;
  [[nodiscard]] QJsonArray getArray(const QString &key) const;

  // getters with optional (to avoid exceptions)
  [[nodiscard]] std::optional<int> getIntOpt(const QString &key) const;
  [[nodiscard]] std::optional<QString> getStringOpt(const QString &key) const;
  [[nodiscard]] std::optional<QJsonObject> getObjectOpt(const QString &key) const;
  [[nodiscard]] std::optional<QJsonArray> getArrayOpt(const QString &key) const;

  void setValue(const QString &key, int value);
  void setValue(const QString &key, const QString &value);
  void setValue(const QString &key, const QJsonObject &value);
  void setValue(const QString &key, const QJsonArray &value);

private:
  void checkAndAssign(const QJsonObject &json);

  Message::Type _type;
  int _action{};
  Message::Status _status;
  QJsonObject _data;
};
