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

  static User registra(QString username, const QString& password, const std::optional<QString>& icon);
  static Session login(const QString& username, const QString& password);

  void save() override;
  void save_record(QSqlRecord &r) override;
  void remove() override;

  const static QString table_name;

  [[nodiscard]] QString getUsername() const;
  [[nodiscard]] QString getIcon() const;

  bool setPassword(const QString& old_password, const QString& password);
  void setUsername(QString username);
  void setIcon(const QString& icon);

private:
  User(QString username, const QString& password);

  static QString encrypt(const QString& str);
  static bool check_pass(const QString& pass, const QString& db_pass);
  static bool check_icon(const QString& icon);

  QString _username;
  QString _password;
  QString _imagePath;

};
