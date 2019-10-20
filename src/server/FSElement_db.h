#pragma once

#include <QString>

#include "persistence_global.h"

#include "dependencies.h"
#include "session.h"
#include "FSElement.h"
#include "File.h"

using persistence::Persistent;
using persistence::Lazy;
using persistence::LazyList;

class FSElement_db: public Persistent {
public:
  FSElement_db(const FSElement_db &);
  FSElement_db(FSElement_db &&);
  explicit FSElement_db();
  explicit FSElement_db(QSqlRecord);

  virtual ~FSElement_db();

  virtual FSElement_db& operator=(const FSElement_db &);

  virtual void save();
  virtual void save_record(QSqlRecord &);
  virtual void remove();

  static const QString table_name;

  QString getName() const;
  std::vector<FSElement_db*> getChildren();

  static FSElement_db create(const Session &s, int parent_id, QString name, bool is_file = false);
  FSElement_db clone(const Session &s);
  FSElement getFSElement() const;
  File load() const;
  void store(const File &f);

private:
  FSElement load_dir() const;
  void del_file();
  bool is_link() const;

  QString getPath() const;

  QString _path;
  QString _name;
  FSElement::Type _type;
  int _parent_id;
  int _owner_id;
  int _creator_id;

  Lazy<FSElement_db> _original;
  LazyList<FSElement_db> _links;
  LazyList<FSElement_db> _children;
};
