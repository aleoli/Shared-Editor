#pragma once

#define FILE_PATH "/.shared_editor/data/"

#include <QString>

#include "persistence_global.h"

#include "dependencies.h"
#include "session.h"
#include "FSElement.h"
#include "File.h"
#include "user.h"
#include "SharedLink.h"

using persistence::Persistent;
using persistence::Lazy;
using persistence::LazyList;

class SharedLink;   // fix circular include

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
  virtual void remove(std::function<void(int e_id, int owner_id)> notify_fn);

  static const QString table_name;

  QString getName() const;

  static FSElement_db get(const Session &s, int id);
  static FSElement_db mkroot(int user_id);

  static FSElement_db root(int user_id);
  FSElement_db mkdir(const Session &s, QString name);
  FSElement_db mkfile(const Session &s, QString name);
  std::vector<FSElement_db*> ls(const Session &s);
  static FSElement_db link(const Session &s, const QString &token);
  SharedLink share(const Session &s);
	void mv(const Session &s, FSElement_db &fs_e);
	void mv(const Session &s, int new_dir_id);
	void rename(const Session &s, QString name);

	User getCreator();

  FSElement getFSElement() const;
  File load() const;
  void store(const File &f);

	void clearCache();

private:
  static FSElement_db create(int user_id, int parent_id, QString name, bool is_file = false);
  FSElement_db clone(const Session &s);
  std::vector<FSElement_db*> getChildren();

  FSElement load_dir() const;
  void del_file();
  bool is_link() const;

  QString getPath() const;
  bool path_on_disk_available();

  QString _path;
  QString _name;
  FSElement::Type _type;
  int _parent_id;
  int _owner_id;

	Lazy<User> _creator;
  Lazy<FSElement_db> _original;
  LazyList<FSElement_db> _links;
  LazyList<FSElement_db> _children;
	LazyList<SharedLink> _shared_links;
};
