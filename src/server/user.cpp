#include "user.h"

#include <QCryptographicHash>
#include <utility>

#include "utils.h"
#include "session.h"
#include "FSElement_db.h"

#include "exceptions.h"

using namespace se_exceptions;

#define RND_LENGTH 16

const QString User::table_name = "user";

User::User(): Persistent(), _username(""), _password("") {}

User::User(const User& u): Persistent(u) {
  this->_username = u._username;
  this->_password = u._password;
}

User::User(QString username, QString password): Persistent(), _username(std::move(username)) {
  this->_password = User::encrypt(std::move(password));
}

User::User(User&& u) noexcept: Persistent(std::move(u)) {
  this->_username = u._username;
  this->_password = u._password;
}

User::User(const QSqlRecord& r): Persistent(r) {
  this->_username = r.value("username").toString();
  this->_password = r.value("password").toString();
}

User::~User() = default;

User& User::operator=(const User &u) {
  if(this == &u) {
    return *this;
  }
  Persistent::operator=(u);
  this->_username = u._username;
  this->_password = u._password;
  return *this;
}

void User::save() {
  this->_save<User>();
}

void User::save_record(QSqlRecord &r) {
  r.setValue("username", this->_username);
  r.setValue("password", this->_password);
}

void User::remove() {
  this->_remove<User>();
}

User User::registra(QString username, QString password) {
  User u{std::move(username), std::move(password)};
  u.save();   // lancia un'eccezione se non ci riesce
  FSElement_db::mkroot(u.id);
  return u;
}

QString User::encrypt(const QString& str) {
  QString rnd = rndString(RND_LENGTH);
  QString encr = QString(QCryptographicHash::hash((str+rnd).toUtf8(), QCryptographicHash::Md5).toHex());
  return encr+rnd;
}

Session User::login(const QString& username, const QString& password) {
  QSqlQuery query("SELECT id, password FROM "+User::table_name+" WHERE username=?");
  query.addBindValue(username);
  if(query.exec()) {
    while(query.next()) {
      auto r = query.record();
      if(User::check_pass(password, r.value("password").toString())) {
        //return DB::getOne<User>(r.value("id").toInt());
        return Session::start(r.value("id").toInt());
      }
    }
    throw SQLNoElementSelectException{"No element found in table '"+User::table_name+"' for username "+username+" and password "+password};
  } else {
    throw SQLException{"No query exec on '"+User::table_name+"'"};
  }
}

bool User::check_pass(const QString& pass, const QString& db_pass) {
  int p = db_pass.length() - RND_LENGTH;
  QString rnd = db_pass.right(RND_LENGTH);
  QString str = db_pass.left(p);
  QString encr = QString(QCryptographicHash::hash((pass+rnd).toUtf8(), QCryptographicHash::Md5).toHex());
  return encr == str;
}

QString User::getUsername() const {
  return this->_username;
}

bool User::setPassword(const QString& old_password, const QString& password) {
  QSqlQuery query("SELECT password FROM "+User::table_name+" WHERE id=?");
  query.addBindValue(this->id);
  if(query.exec()) {
    while(query.next()) {
      auto r = query.record();
      if(User::check_pass(old_password, r.value("password").toString())) {
        this->_password = User::encrypt(password);
        return true;
      } else {
        return false;
      }
    }
    throw SQLNoElementSelectException{"No element found in table '"+User::table_name+"'"};
  } else {
    throw SQLException{"No query exec on '"+User::table_name+"'"};
  }
}

void User::setUsername(QString username) {
  this->_username = std::move(username);
}

void User::setIcon(const QString& icon) {
  // TODO: set icon
  warn("TODO: set icon");
}

bool User::check_pass(const QString& pass) {
  // NOTE: the password has to have at least an Uppercase char, a Lowercase char, a number, and length are to be equal or more than 6
  if(pass.length() < 6) {
    return false;
  }
  bool upper = false, lower = false, number = false;
  for(const auto& ch: pass) {
    if(ch.isUpper()) {
      upper = true;
    }
    if(ch.isLower()) {
      lower = true;
    }
    if(ch.isNumber()) {
      number = true;
    }
    if(upper && lower && number) {
      return true;
    }
  }
  return false;
}
