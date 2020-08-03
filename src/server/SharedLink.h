#pragma once

#include <QString>

#include "persistence_global.h"

#include "dependencies.h"

#include "FSElement_db.h"

using persistence::Persistent;
using persistence::Lazy;

class FSElement_db;   // fix circular include

class SharedLink: public Persistent {
public:
  SharedLink(const SharedLink &);
  SharedLink(SharedLink &&) noexcept;
  explicit SharedLink();
  explicit SharedLink(const QSqlRecord&);

  ~SharedLink() override;

  SharedLink& operator=(const SharedLink &);

  void save() override;
  void save_record(QSqlRecord &) override;
  void remove() override;

  static const QString table_name;

  static SharedLink create(int file_id);
  static SharedLink get(const QString &token);

  FSElement_db element();
  [[nodiscard]] QString getToken() const;

private:
  QString _token;
  Lazy<FSElement_db> _fs_element;

  static bool token_available(const QString &token);
};
