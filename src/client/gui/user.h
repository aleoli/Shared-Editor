#pragma once

#include <memory>
#include <QString>

class User {
public:
  User(const User&) = delete;
  User(User&&) = delete;
  User& operator=(const User&) = delete;
  User& operator=(User&&) = delete;

  ~User();
  static std::shared_ptr<User> get();

  bool isLogged();

  void setUsername(const QString &username);
  void loginSuccessful(const QString &token, int userId, const std::optional<QString> &icon);
  void logout();

private:
  static std::shared_ptr<User> instance;
  User();

  bool _logged;
  //TODO altri dati
  QString _username;
  QString _token;
  int _userId;
  std::optional<QString> _icon;
};
