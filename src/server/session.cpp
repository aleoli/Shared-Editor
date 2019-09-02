#include "session.h"

#include "user.h"
#include "utils.h"

const QString Session::table_name = "session";

Session::Session(): Persistent(), _token(""), _active(false) {}

Session::Session(const Session &s): Persistent(s) {
  this->_token = s._token;
  this->_active = s._active;
  this->_user = s._user;
}

Session::Session(Session &&s): Persistent(s) {
  this->_token = s._token;
  this->_active = s._active;
  this->_user = s._user;
}

Session::Session(QSqlRecord r): Persistent(r) {
  this->_token = r.value("token").toString();
  this->_active = r.value("active").toInt() == 1;
  this->_user = Lazy<User>{r.value("user_id").toInt()};
}

Session::~Session() {}

Session& Session::operator=(const Session& s) {
  if(this == &s) {
    return *this;
  }
  Persistent::operator=(s);
  this->_token = s._token;
  this->_active = s._active;
  this->_user = s._user;
  return *this;
}

void Session::save() {
  this->_save<Session>();
}

void Session::save_record(QSqlRecord &r) {
  r.setValue("token", this->_token);
  r.setValue("active", (this->_active ? 1 : 0));
  r.setValue("user_id", this->_user.getId());
}

void Session::remove() {
  this->_remove<Session>();
}

Session Session::start(int user_id) {
  Session s{};
  s._token = Session::new_token();
  s._user = Lazy<User>{user_id};
  s._active = true;
  s.save();     // lancia un'eccezione se non ci riesce
  return s;
}

QString Session::new_token() {
  // TODO: controlla che non ci sia già nel db
  return rndString(128);
}

int Session::getUserId() const {
  return this->_user.getId();
}

User Session::getUser() {
  return this->_user.getValue();
}
