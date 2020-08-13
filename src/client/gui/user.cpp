#include "user.h"

#include "exceptions.h"
#include "utils.h"

using namespace se_exceptions;

std::shared_ptr<User> User::instance = nullptr;

User::User() : _logged(false) {
}

User::~User() {
}

std::shared_ptr<User> User::get() {
  if(instance == nullptr) {
    instance.reset(new User{});
  }
  return instance;
}

bool User::isLogged() {
  return _logged;
}

void User::setUsername(const QString &username) {
  _username = username;
}

void User::loginSuccessful(const QString &token, int userId, const std::optional<QString> &icon) {
  _token = token;
  _userId = userId;
  _icon = icon;
  _logged = true;
}

void User::logout() {
  _logged = false;
}
