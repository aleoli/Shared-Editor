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
  SharedLink(SharedLink &&);
  explicit SharedLink();
  explicit SharedLink(QSqlRecord);

  virtual ~SharedLink();

  virtual SharedLink& operator=(const SharedLink &);

  virtual void save();
  virtual void save_record(QSqlRecord &);
  virtual void remove();

  static const QString table_name;

  static SharedLink create(int file_id);
  static SharedLink get(const QString &token);

  FSElement_db element();
  QString getToken() const;

private:
  QString _token;
  Lazy<FSElement_db> _fs_element;

  static bool token_available(const QString &token);
};
