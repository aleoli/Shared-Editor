#pragma once

#include <QString>
#include <optional>

#include "persistence_global.h"

#include "dependencies.h"

using persistence::Persistent;

class User: public Persistent {
public:
  User(const User &u);
  User(User &&u) noexcept;
  explicit User();
  explicit User(const QSqlRecord& r);

  ~User() override;

  User& operator=(const User& u);

  static User registra(QString username, QString password);
  static Session login(const QString& username, const QString& password);

  void save() override;
  void save_record(QSqlRecord &r) override;
  void remove() override;

  const static QString table_name;

  [[nodiscard]] QString getUsername() const;

  bool setPassword(const QString& old_passwork, const QString& password);
  void setUsername(QString username);
  void setIcon(const QString& icon);

  static bool check_pass(const QString& pass);

private:
  User(QString username, QString password);

  static QString encrypt(const QString& str);
  static bool check_pass(const QString& pass, const QString& db_pass);

  QString _username;
  QString _password;

};
