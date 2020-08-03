#pragma once

#include <QString>

#include "persistence_global.h"

#include "dependencies.h"

using persistence::Persistent;
using persistence::Lazy;

class Session: public Persistent {
public:
  Session(const Session &s);
  Session(Session &&s) noexcept;
  explicit Session();
  explicit Session(const QSqlRecord& r);

  ~Session() override;

  Session& operator=(const Session &s);

  static Session start(int user_id);
  static Session get(const QString& token);
  void close();

  void save() override;
  void save_record(QSqlRecord &r) override;
  void remove() override;

  const static QString table_name;

  [[nodiscard]] int getUserId() const;
  User getUser();
  [[nodiscard]] QString getToken() const;

private:
  static QString new_token();

  QString _token;
  bool _active;
  Lazy<User> _user;

};
