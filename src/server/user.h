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

  static User registra(QString nickname, std::optional<QString> email, QString password);
  static Session login(QString username, QString password);

  virtual void save();
  virtual void save_record(QSqlRecord &r);
  virtual void remove();

  const static QString table_name;

  QString getNickname() const;
  QString getEmail() const;

  bool setPassword(QString old_passwork, QString password);
  void setNickname(QString nickname);
  void setIcon(QString icon);

	static bool check_pass(QString pass);
	static bool check_email(QString email);

private:
  User(QString nickname, std::optional<QString> email, QString password);

  static QString encrypt(QString str);
  static bool check_pass(QString pass, QString db_pass);

  QString _nickname;
  QString _email;
  QString _password;

};
