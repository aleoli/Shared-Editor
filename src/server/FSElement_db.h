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
  FSElement_db(FSElement_db &&) noexcept;
  explicit FSElement_db();
  explicit FSElement_db(const QSqlRecord&);

  ~FSElement_db() override;

  FSElement_db& operator=(const FSElement_db &);

  void save() override;
  void save_record(QSqlRecord &) override;
  void remove() override;
  virtual void remove(const std::function<void(int e_id, int owner_id)>& notify_fn);

  static const QString table_name;

  [[nodiscard]] QString getName() const;
  int getPhysicalId();
  static int getIdForUser(const Session &s, int file_id, int user_id);
  [[nodiscard]] int getCharId() const;
  [[nodiscard]] int getCommentId() const;
  [[nodiscard]] QString getPath() const;
  [[nodiscard]] int getParentId() const;
  std::vector<FSElement_db> getPathElements(const Session &s);
  [[nodiscard]] bool isDir() const;

  void addCharId(int v);
  void incCommentId();

  static FSElement_db get(const Session &s, int id);
  static FSElement_db mkroot(int user_id);

  static FSElement_db root(int user_id);
  FSElement_db mkdir(const Session &s, QString name);
  FSElement_db mkfile(const Session &s, QString name);
  std::vector<FSElement_db*> ls(const Session &s);
  static FSElement_db link(const Session &s, const QString &token);
  SharedLink share(const Session &s);
  void mv(const Session &s, FSElement_db &fs_e, const std::optional<QString>& appendIfNameExists = std::nullopt);
  void mv(const Session &s, int new_dir_id);
  void rename(const Session &s, QString name);
  std::pair<QString, int> pwd(const Session &s);

  User getCreator();

  [[nodiscard]] FSElement getFSElement() const;
  [[nodiscard]] File load() const;
  void store(const File &f);

  [[nodiscard]] bool is_link() const;

  void clearCache();

  std::list<FSElement_db> recursive_ls(const Session &s);

private:
  static FSElement_db create(int user_id, int parent_id, QString name, bool is_file = false);
  FSElement_db clone(const Session &s);
  std::vector<FSElement_db*> getChildren();

  static FSElement_db get(int userId, int id);
  std::vector<FSElement_db*> ls(int userId);

  static void checkName(int user_id, std::optional<int> file_id, int parent_it, QString& name, std::optional<QString> append = std::nullopt);
  bool availableForUser(int user_id);

  void del_file();

  [[nodiscard]] bool path_on_disk_available() const;

  QString _path;
  QString _name;
  FSElement::Type _type;
  int _parent_id;
  int _owner_id;
  int _char_id;
  int _comment_id;
  QDateTime _creation_date;

  Lazy<User> _creator;
  Lazy<FSElement_db> _original;
  LazyList<FSElement_db> _links;
  LazyList<FSElement_db> _children;
  LazyList<SharedLink> _shared_links;
};
