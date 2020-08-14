#pragma once

#include <memory>
#include <QString>
#include <QIcon>
#include <QObject>

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

  void setUsername(const QString &username);
  QString getUsername() const;

  void setIcon(const QIcon &icon);
  QIcon getIcon() const;

  QString getToken() const;
  int getUserId() const;

  void login(const QString &token, int userId, const std::optional<QString> &icon);
  void logout();

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
};
