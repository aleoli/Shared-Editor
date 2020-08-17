#include "user.h"

#include <QCryptographicHash>
#include <utility>

#include "utils.h"
#include "session.h"
#include "FSElement_db.h"
#include "errors.h"

#include "exceptions.h"

using namespace se_exceptions;

#define RND_LENGTH 16

const QString User::table_name = "user";

User::User(): Persistent(), _username(""), _password(""), _imagePath("") {}

User::User(const User& u): Persistent(u) {
  this->_username = u._username;
  this->_password = u._password;
  this->_imagePath = u._imagePath;
}

User::User(QString  username, const QString& password): Persistent(), _username(std::move(username)), _imagePath("") {
  this->_password = User::encrypt(password);
}

User::User(User&& u) noexcept: Persistent(std::move(u)) {
  this->_username = u._username;
  this->_password = u._password;
  this->_imagePath = u._imagePath;
}

User::User(const QSqlRecord& r): Persistent(r) {
  this->_username = r.value("username").toString();
  this->_password = r.value("password").toString();
  this->_imagePath = r.value("imagePath").toString();
}

User::~User() = default;

User& User::operator=(const User &u) {
  if(this == &u) {
    return *this;
  }
  Persistent::operator=(u);
  this->_username = u._username;
  this->_password = u._password;
  this->_imagePath = u._imagePath;
  return *this;
}

void User::save() {
  this->_save<User>();
}

void User::save_record(QSqlRecord &r) {
  r.setValue("username", this->_username);
  r.setValue("password", this->_password);
  r.setValue("imagePath", this->_imagePath);
}

void User::remove() {
  this->_remove<User>();
}

User User::registra(QString username, const QString& password, const std::optional<QString>& icon) {
  User u{std::move(username), password};
  if(icon) {
    u.setIcon(*icon);
  }
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

bool User::check_icon(const QString &icon) {
  int mb = 1 << 20;
  return mb > icon.size();      // size is less than 1MB
}

QString User::getUsername() const {
  return this->_username;
}

QString User::getIcon() const {
  if(this->_imagePath == "") {
    return "";
  }
  auto file = QFile{QDir::homePath()+FILE_PATH+this->_imagePath};
  if(!file.open(QIODevice::ReadOnly)){
    return "";
  }
  return QString::fromUtf8(file.readAll());
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
  if(icon == "") {
    return;
  }
  if(!User::check_icon(icon)) {
    throw se_exceptions::UserException{ICON_REQUIREMENTS};
  }
  if(this->_imagePath == "") {
    // create new icon path
   this->_imagePath = "icon-user-" + this->getUsername() + "-" + rndString(8);
  }
  // else replace old one
  auto file = QFile{QDir::homePath()+FILE_PATH+this->_imagePath};
  if(!file.open(QIODevice::WriteOnly)){
    error("cannot open file "+this->_imagePath);
    throw se_exceptions::NoFileOnDiskException{"cannot open file "+this->_imagePath};
  }
  file.write(icon.toUtf8());
  file.close();
}
