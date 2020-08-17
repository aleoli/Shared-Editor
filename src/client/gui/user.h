#pragma once

#include <memory>
#include <QString>
#include <QIcon>
#include <QObject>

#include "File.h"

class User : public QObject {
  Q_OBJECT

public:
  User(const User&) = delete;
  User(User&&) = delete;
  User& operator=(const User&) = delete;
  User& operator=(User&&) = delete;

  ~User();
  static std::shared_ptr<User> get();

  bool isLogged();
  bool isFileOpen();

  void setUsername(const QString &username);
  QString getUsername() const;

  void setIcon(const QIcon &icon);
  void setDecodedIcon(const QString &icon);
  QIcon getIcon() const;

  QString getToken() const;
  int getUserId() const;

  int getFileId() const;
  int getCharId(); // get and increment
  int getCommentId(); // get and increment

  void login(const QString &token, int userId, const std::optional<QString> &icon);
  void logout();
  void openFile(int fileId, const File &file = File(), int charId = 0, int commentId = 0);
  void closeFile();

signals:
  void iconChanged();
  void loggedIn();
  void loggedOut();
  //TODO others

private:
  static std::shared_ptr<User> instance;
  User();

  bool _logged;
  QString _username;
  QString _token;
  int _userId;
  QIcon _icon;
  //TODO dati relativi al file: File, charId, commentId
  bool _fileOpen;
  File _file;
  int _fileId;
  int _charId;
  int _commentId;
};
