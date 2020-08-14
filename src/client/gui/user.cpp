#include "user.h"

#include "exceptions.h"
#include "utils.h"
#include "image_utils.h"

using namespace se_exceptions;

std::shared_ptr<User> User::instance = nullptr;

User::User() : _logged(false), _icon(image_utils::loadRoundedImage(DEFAULT_ICON)){}

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

void User::setIcon(const QIcon &icon) {
  _icon = icon;
  emit iconChanged();
}

void User::login(const QString &token, int userId, const std::optional<QString> &icon) {
  _token = token;
  _userId = userId;

  if(icon) {
    _icon = image_utils::decodeImage(*icon);
  }

  _logged = true;
  emit loggedIn();
}

void User::logout() {
  _logged = false;
  emit loggedOut();
}

QString User::getUsername() const {
  return _username;
}

QIcon User::getIcon() const {
  return _icon;
}

QString User::getToken() const {
  if(!_logged) {
    throw UserException{"User not logged!"};
  }

  return _token;
}

int User::getUserId() const {
  if(!_logged) {
    throw UserException{"User not logged!"};
  }

  return _userId;
}
