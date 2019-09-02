#pragma once

#include <QString>

#include "persistence_global.h"

#include "dependencies.h"

using persistence::Persistent;
using persistence::Lazy;

class Session: public Persistent {
public:
  Session(const Session &s);
  Session(Session &&s);
  explicit Session();
  explicit Session(QSqlRecord r);

  virtual ~Session();

  virtual Session& operator=(const Session &s);

  static Session start(int user_id);
  static Session get(QString token);    // TODO

  virtual void save();
  virtual void save_record(QSqlRecord &r);
  virtual void remove();

  const static QString table_name;

  int getUserId() const;
  User getUser();

private:
  static QString new_token();

  QString _token;
  bool _active;
  Lazy<User> _user;

};
