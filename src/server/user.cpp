#include "user.h"

#include <QCryptographicHash>

#include "utils.h"
#include "session.h"
#include "FSElement_db.h"

#include "exceptions.h"

using namespace se_exceptions;

#define RND_LENGTH 16

const QString User::table_name = "user";

User::User(): Persistent(), _nickname(""), _email(""), _password("") {}

User::User(const User& u): Persistent(u) {
  this->_nickname = u._nickname;
  this->_email = u._email;
  this->_password = u._password;
}

User::User(QString nickname, QString email, QString password): Persistent(), _nickname(nickname), _email(email) {
  this->_password = User::encrypt(password);
}

User::User(User&& u): Persistent(u) {
  this->_nickname = u._nickname;
  this->_email = u._email;
  this->_password = u._password;
}

User::User(QSqlRecord r): Persistent(r) {
  this->_nickname = r.value("nickname").toString();
  this->_email = r.value("email").toString();
  this->_password = r.value("password").toString();
}

User::~User() {}

User& User::operator=(const User &u) {
  if(this == &u) {
    return *this;
  }
  Persistent::operator=(u);
  this->_nickname = u._nickname;
  this->_email = u._email;
  this->_password = u._password;
  return *this;
}

void User::save() {
  this->_save<User>();
}

void User::save_record(QSqlRecord &r) {
  r.setValue("nickname", this->_nickname);
  r.setValue("email", this->_email);
  r.setValue("password", this->_password);
}

void User::remove() {
  this->_remove<User>();
}

User User::registra(QString nickname, QString email, QString password) {
  User u{nickname, email, password};
  u.save();   // lancia un'eccezione se non ci riesce
  FSElement_db::mkroot(u.id);
  return u;
}

QString User::encrypt(QString str) {
  QString rnd = rndString(RND_LENGTH);
  QString encr = QString(QCryptographicHash::hash((str+rnd).toUtf8(), QCryptographicHash::Md5).toHex());
  return encr+rnd;
}

Session User::login(QString username, QString password) {
  QSqlQuery query("SELECT id, password FROM "+User::table_name+" WHERE nickname=? OR email=?");
  query.addBindValue(username);
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

bool User::check_pass(QString pass, QString db_pass) {
  int p = db_pass.length() - RND_LENGTH;
  QString rnd = db_pass.right(RND_LENGTH);
  QString str = db_pass.left(p);
  QString encr = QString(QCryptographicHash::hash((pass+rnd).toUtf8(), QCryptographicHash::Md5).toHex());
  return encr == str;
}

QString User::getNickname() const {
  return this->_nickname;
}

QString User::getEmail() const {
  return this->_email;
}

bool User::setPassword(QString old_password, QString password) {
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

void User::setNickname(QString nickname) {
  this->_nickname = nickname;
}

void User::setIcon(QString icon) {
  // TODO: set icon
  warn("TODO: set icon");
}
