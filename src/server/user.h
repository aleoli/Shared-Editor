#pragma once

#include <QString>
#include <optional>

#include "persistence_global.h"

#include "dependencies.h"

using persistence::Persistent;

class User: public Persistent {
public:
  User(const User &u);
  User(User &&u);
  explicit User();
  explicit User(QSqlRecord r);

  virtual ~User();

  virtual User& operator=(const User& u);

  static User registra(QString username, QString password);
  static Session login(QString username, QString password);

  virtual void save();
  virtual void save_record(QSqlRecord &r);
  virtual void remove();

  const static QString table_name;

  QString getUsername() const;

  bool setPassword(QString old_passwork, QString password);
  void setUsername(QString username);
  void setIcon(QString icon);

	static bool check_pass(QString pass);

private:
  User(QString username, QString password);

  static QString encrypt(QString str);
  static bool check_pass(QString pass, QString db_pass);

  QString _username;
  QString _password;

};
