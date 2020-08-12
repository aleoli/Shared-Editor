#pragma once

#include <memory>

class User {
public:
  User(const User&) = delete;
  User(User&&) = delete;
  User& operator=(const User&) = delete;
  User& operator=(User&&) = delete;

  ~User();
  static std::shared_ptr<User> get();

  bool isLogged();

private:
  static std::shared_ptr<User> instance;
  User();

  bool _logged;
  //TODO altri dati
};
