#include "FSElement_db.h"

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QJsonDocument>
#include <utility>

#include "utils.h"
#include "File.h"
#include "SharedLink.h"
#include "exceptions.h"

const QString FSElement_db::table_name = "fs_element";

FSElement_db::FSElement_db(): Persistent() {
  this->_path = "";
  this->_name = "";
  this->_type = FSElement::Type::FILE;
  this->_parent_id = -1;
  this->_owner_id = -1;
  this->_char_id = 0;
  this->_comment_id = 0;
  this->_creator = Lazy<User>{-1};
  this->_original = Lazy<FSElement_db>{-1};
  this->_shared_links = LazyList<SharedLink>{SharedLink::table_name, "element_id = "+QString::number(this->id)};
  this->_creation_date = QDateTime::currentDateTimeUtc();
}

FSElement_db::FSElement_db(const FSElement_db &e): Persistent(e) {
  this->_path = e._path;
  this->_name = e._name;
  this->_type = e._type;
  this->_parent_id = e._parent_id;
  this->_owner_id = e._owner_id;
  this->_char_id = e._char_id;
  this->_comment_id = e._comment_id;
  this->_creator = e._creator;
  this->_children = e._children;
  this->_original = e._original;
  this->_links = e._links;
  this->_shared_links = e._shared_links;
  this->_creation_date = e._creation_date;
}

FSElement_db::FSElement_db(FSElement_db &&e) noexcept: Persistent(std::move(e)) {
  this->_path = e._path;
  this->_name = e._name;
  this->_type = e._type;
  this->_parent_id = e._parent_id;
  this->_owner_id = e._owner_id;
  this->_char_id = e._char_id;
  this->_comment_id = e._comment_id;
  this->_creator = e._creator;
  this->_children = e._children;
  this->_original = e._original;
  this->_links = e._links;
  this->_shared_links = e._shared_links;
  this->_creation_date = e._creation_date;
}

FSElement_db::FSElement_db(const QSqlRecord& r): Persistent(r) {
  this->_path = r.value("path").toString();
  this->_name = r.value("name").toString();
  this->_type = static_cast<FSElement::Type>(r.value("type").toInt());
  this->_parent_id = r.value("parent_id").toInt();
  this->_owner_id = r.value("owner_id").toInt();
  this->_char_id = r.value("char_id").toInt();
  this->_comment_id = r.value("comment_id").toInt();
  this->_creator = Lazy<User>{r.value("creator_id").toInt()};
  this->_children = LazyList<FSElement_db>{FSElement_db::table_name, "parent_id = "+r.value("id").toString()};
  this->_original = Lazy<FSElement_db>{r.value("linked_from").toInt()};
  this->_links = LazyList<FSElement_db>{FSElement_db::table_name, "linked_from = "+r.value("id").toString()};
  this->_shared_links = LazyList<SharedLink>{SharedLink::table_name, "element_id = "+r.value("id").toString()};
  this->_creation_date = r.value("creation_date").toDateTime();
}

FSElement_db::~FSElement_db() = default;

FSElement_db& FSElement_db::operator=(const FSElement_db& e) {
  if(this == &e) {
    return *this;
  }
  this->_path = e._path;
  this->_name = e._name;
  this->_type = e._type;
  this->_parent_id = e._parent_id;
  this->_owner_id = e._owner_id;
  this->_char_id = e._char_id;
  this->_comment_id = e._comment_id;
  this->_creator = e._creator;
  this->_children = e._children;
  this->_original = e._original;
  this->_links = e._links;
  this->_shared_links = e._shared_links;
  this->_creation_date = e._creation_date;
  Persistent::operator=(e);
  return *this;
}

void FSElement_db::save() {
  this->_save<FSElement_db>();
}

void FSElement_db::save_record(QSqlRecord &r) {
  r.setValue("path", this->_path);
  r.setValue("name", this->_name);
  r.setValue("type", static_cast<int>(this->_type));
  r.setValue("parent_id", this->_parent_id);
  r.setValue("owner_id", this->_owner_id);
  r.setValue("char_id", this->_char_id);
  r.setValue("comment_id", this->_comment_id);
  r.setValue("creator_id", this->_creator.getId());
  r.setValue("linked_from", this->_original.getId());
  r.setValue("creation_date", this->_creation_date);
}

void FSElement_db::remove() {
  this->remove([](int e_id, int owner_id) -> void {});
}

void FSElement_db::remove(const std::function<void(int, int)>& notify_fn) {
  if(this->_type == FSElement::Type::DIRECTORY) {
    if(this->_parent_id <= 0) {
      // root dir
      warn("cannot remove root dir");
      throw se_exceptions::RootDirException{"cannot remove root dir"};
    }
    auto children = this->_children.getValues();
    for(auto& c: children) {
      c->remove(notify_fn);
    }
  } else if(!this->is_link()) {
    // file non linkato
    for(auto& s_link: this->_shared_links.getValues()) {
        s_link->remove();
    }
    for(auto& link: this->_links.getValues()) {
      notify_fn(link->getId(), link->_owner_id);
      debug(link->getName());
      link->remove();
    }
    this->del_file();
  }
  // se è un link, è sufficiente eliminare la entry
  info(QString{"Remove "}+(this->is_link() ? "Link" : "File")+" ID: "+QString::number(this->id)+(this->_type == FSElement::Type::FILE ? " path "+this->_path : ""));
  this->_remove<FSElement_db>();
}

void FSElement_db::checkName(int user_id, std::optional<int> file_id, int parent_id, QString &name, std::optional<QString> append) {
  if(parent_id != -1) {
    auto parent = FSElement_db::get(user_id, parent_id);
    auto brothers = parent.ls(user_id);
    for(const auto& b: brothers) {
      if((!file_id || *file_id != b->getId()) && name == b->getName()) {
        if(append) {
          name += *append;
        } else {
          throw DuplicatedNameException{"File " + name + " already exists in this directory"};
        }
      }
    }
  }
}

FSElement_db FSElement_db::create(int user_id, int parent_id, QString name, bool is_file) {
  FSElement_db fs_e{};
  if(is_file) {
    do {
      fs_e._path = rndString(64);
    } while(!fs_e.path_on_disk_available());
  } else {
    fs_e._path = "";
  }
  FSElement_db::checkName(user_id, std::nullopt, parent_id, name);
  fs_e._name = std::move(name);
  fs_e._type = is_file ? FSElement::Type::FILE : FSElement::Type::DIRECTORY;
  fs_e._parent_id = parent_id;
  fs_e._owner_id = user_id;
  fs_e._creator = Lazy<User>{user_id};
  fs_e._creation_date = QDateTime::currentDateTimeUtc();
  fs_e.save();
  fs_e._children = LazyList<FSElement_db>{FSElement_db::table_name, "parent_id = "+QString::number(fs_e.id)};
  fs_e._links = LazyList<FSElement_db>{FSElement_db::table_name, "linked_from = "+QString::number(fs_e.id)};
  if(is_file) {
    QDir dir(QDir::homePath()+"/.shared_editor/data");
    if(!dir.exists()) {
      dir.mkdir(QDir::homePath()+"/.shared_editor/data");
    }
    QFile f(fs_e.getPath());
    if(!f.open(QIODevice::WriteOnly)) {
      error("cannot open file "+fs_e._path);
      throw se_exceptions::NoFileOnDiskException{"cannot open file "+fs_e._path};
    }
    File el{};
    f.write(el.toQByteArray());
    f.close();
    info("New File ID: "+QString::number(fs_e.id)+" path "+fs_e._path+" parent_id: "+QString::number(fs_e._parent_id));
  } else {
    info("New Dir ID: "+QString::number(fs_e.id)+" parent_id: "+QString::number(fs_e._parent_id));
  }
  return fs_e;
}

FSElement_db FSElement_db::clone(const Session &s) {
  if(this->_creator.getId() == s.getUserId()) {
    error("Link creation of own element");
    throw se_exceptions::LinkException{"Link creation of own element"};
  }
  if(this->is_link()) {
    error("Cannot create a link of a link");
    throw se_exceptions::LinkException{"Cannot create a link of a link"};
  }
  if(this->availableForUser(s.getUserId())) {
    error("Double creation of link");
    throw se_exceptions::DoubleLinkException{"Double creation of link"};
  }
  FSElement_db fs_e{*this};
  fs_e.id = 0;
  fs_e._parent_id = -1;
  fs_e._owner_id = s.getUserId();
  fs_e._char_id = 0;
  fs_e._original = Lazy<FSElement_db>{this->id};
  fs_e.save();
  fs_e._children = LazyList<FSElement_db>{FSElement_db::table_name, "parent_id = "+QString::number(fs_e.id)};
  fs_e._links = LazyList<FSElement_db>{FSElement_db::table_name, "linked_from = "+QString::number(fs_e.id)};
  info("New Link ID: "+QString::number(fs_e.id)+" from original "+QString::number(this->id));
  return fs_e;
}

FSElement FSElement_db::getFSElement() const {
  return FSElement{this->id, this->_parent_id, this->_name, this->_type, this->_creation_date};
}

File FSElement_db::load() const {
  if(this->_type != FSElement::Type::FILE) {
    warn("You are tring to load an element that is not a file");
    throw se_exceptions::ReadException{"You are tring to load an element that is not a file"};
  }
  QFile f(this->getPath());
  if(!f.open(QIODevice::ReadOnly)){
    error("cannot open file "+this->_path);
    throw se_exceptions::NoFileOnDiskException{"cannot open file "+this->_path};
  }
  auto data = f.readAll();
  f.close();
  return File::fromQByteArray(data);
}

void FSElement_db::store(const File &f) {
  if(this->_type != FSElement::Type::FILE) {
    warn("You are tring to store to an element that is not a file");
    throw se_exceptions::WriteException{"You are tring to store to an element that is not a file"};
  }
  QFile file(this->getPath());
  if(!file.open(QIODevice::WriteOnly)){
    error("cannot open file "+this->_path);
    throw se_exceptions::NoFileOnDiskException{"cannot open file "+this->_path};
  }
  file.write(f.toQByteArray());
  file.close();
}

void FSElement_db::del_file() {
  if(this->_type != FSElement::Type::FILE) {
    warn("You are tring to delete to an element that is not a file");
    throw se_exceptions::WriteException{"You are tring to delete to an element that is not a file"};
  }
  QFile file(this->getPath());
  file.remove();
}

FSElement_db FSElement_db::get(const Session &s, int id) {
  return FSElement_db::get(s.getUserId(), id);
}

FSElement_db FSElement_db::get(int userId, int id) {
  auto fs_e = DB::get()->getOne<FSElement_db>(id);
  if(fs_e._owner_id != userId && !fs_e.availableForUser(userId)) {
    warn("User "+QString::number(userId)+" has tryed to access a file of user "+QString::number(fs_e._owner_id));
    throw se_exceptions::IllegalAccessException{"User "+QString::number(userId)+" has tryed to access a file of user "+QString::number(fs_e._owner_id)};
  }
  return fs_e;
}

FSElement_db FSElement_db::mkroot(int user_id) {
  bool no_folder = false;
  try {
    FSElement_db::root(user_id);
  } catch(const se_exceptions::RootDirException &ex) {
    no_folder = true;
  }
  if(!no_folder) {
    error("Double creation of root dir");
    throw se_exceptions::RootDirException{"Double creation of root dir"};
  }
  return FSElement_db::create(user_id, -1, "root", false);
}

FSElement_db FSElement_db::root(int user_id) {
  auto l = DB::get()->get<FSElement_db>("parent_id = -1 AND creator_id = "+QString::number(user_id));
  if(l.size() != 1) {
    throw se_exceptions::RootDirException{"User "+QString::number(user_id)+" has not root dir"};
  }
  return l[0];
}

FSElement_db FSElement_db::mkdir(const Session &s, QString name) {
  if(this->_type != FSElement::Type::DIRECTORY) {
    error("Trying to create a directory in not-dir");
    throw se_exceptions::WriteException{"Trying to create a directory in not-dir"};
  }
  if(this->_owner_id != s.getUserId()) {
    error("Trying to modify not your directory");
    throw se_exceptions::IllegalAccessException{"Trying to modify not your directory"};
  }
  auto child = FSElement_db::create(s.getUserId(), this->id, std::move(name), false);
  this->_children.addValue(new FSElement_db{child});
  return child;
}

FSElement_db FSElement_db::mkfile(const Session &s, QString name) {
  if(this->_type != FSElement::Type::DIRECTORY) {
    error("Trying to create a file in not-dir");
    throw se_exceptions::WriteException{"Trying to create a file in not-dir"};
  }
  if(this->_owner_id != s.getUserId()) {
    error("Trying to modify not your directory");
    throw se_exceptions::IllegalAccessException{"Trying to modify not your directory"};
  }
  auto child = FSElement_db::create(s.getUserId(), this->id, std::move(name), true);
  this->_children.addValue(new FSElement_db{child});
  return child;
}

std::vector<FSElement_db*> FSElement_db::ls(const Session &s) {
  return this->ls(s.getUserId());
}

std::vector<FSElement_db*> FSElement_db::ls(int userId) {
  if(this->_type != FSElement::Type::DIRECTORY) {
    warn("Trying to list a not-directory element");
    throw se_exceptions::ReadException{"Trying to create a file in not-dir"};
  }
  if(userId != this->_owner_id) {
    warn("Trying to access not your directory");
    throw se_exceptions::IllegalAccessException{"Trying to access not your directory"};
  }
  return this->getChildren();
}

void FSElement_db::mv(const Session &s, FSElement_db &fs_e, const std::optional<QString>& appendIfNameExists) {
  if(s.getUserId() != this->_owner_id) {
    warn("Trying to move not your element");
    throw se_exceptions::IllegalAccessException{"Trying to move not your element"};
  }
  if(s.getUserId() != fs_e._owner_id) {
    warn("Trying to move to not your directory");
    throw se_exceptions::IllegalAccessException{"Trying to move to not your directory"};
  }
  if(fs_e._type != FSElement::Type::DIRECTORY) {
    error("Trying to move to not-directory element");
    throw se_exceptions::WriteException{"Trying to move to not-directory element"};
  }
  debug("Moving file "+QString::number(this->id)+" from dir "+QString::number(this->_parent_id)+" to dir "+QString::number(fs_e.id));
  this->_parent_id = fs_e.id;
  FSElement_db::checkName(s.getUserId(), this->id, this->_parent_id, this->_name, appendIfNameExists);
  this->save();
  fs_e._children.addValue(new FSElement_db{*this});
}

void FSElement_db::mv(const Session &s, int new_dir_id) {
  auto dir = FSElement_db::get(s, new_dir_id);
  this->mv(s, dir);
}

void FSElement_db::rename(const Session &s, QString name) {
  if(s.getUserId() != this->_owner_id) {
    warn("Trying to rename not your element");
    throw se_exceptions::IllegalAccessException{"Trying to rename not your element"};
  }
  FSElement_db::checkName(s.getUserId(), this->id, this->_parent_id, name);
  this->_name = std::move(name);
  this->save();
}

std::pair<QString, int> FSElement_db::pwd(const Session &s) {
  if(s.getUserId() != this->_owner_id) {
    warn("Trying to get path on not your element");
    throw se_exceptions::IllegalAccessException{"Trying to get path on not your element"};
  }
  QStringList l;
  l.push_back(this->getName());
  auto el = FSElement_db::get(s, this->id);
  while(el._parent_id >= 0) {
    el = FSElement_db::get(s, el._parent_id);
    l.push_front(el.getName());
  }
  return std::pair<QString, int>{'/'+l.join('/'), this->getId()};
}

SharedLink FSElement_db::share(const Session &s) {
  if(this->_type != FSElement::Type::FILE) {
    error("Trying to share a not-file element");
    throw se_exceptions::ShareException{"Trying to share a not-file element"};
  }
  if(s.getUserId() != this->_owner_id) {
    warn("Trying to share not your file");
    throw se_exceptions::IllegalAccessException{"Trying to share not your file"};
  }
  if(this->is_link()) {
    warn("Trying to share a shared file");
    throw se_exceptions::ShareException{"Trying to share a shared file"};
  }
  if(!this->_shared_links.getValues().empty()) {
    return SharedLink{*this->_shared_links.getValues()[0]};
  }
  return SharedLink::create(this->id);
}

FSElement_db FSElement_db::link(const Session &s, const QString &token) {
  auto orig = SharedLink::get(token).element();
  return orig.clone(s);
}

void FSElement_db::clearCache() {
  this->_creator.clear();
  this->_original.clear();
  this->_links.clear();
  this->_children.clear();
}

std::list<FSElement_db> FSElement_db::recursive_ls(const Session &s) {
  auto elements = this->ls(s);
  std::list<FSElement_db> res;
  res.emplace_back(*this);
  for(auto &el: elements) {
    if(el->_type == FSElement::Type::DIRECTORY) {
      res.splice(res.end(), el->recursive_ls(s));
    }
  }
  return res;
}

QString FSElement_db::getPath() const {
  return QDir::homePath()+FILE_PATH+this->_path;
}

int FSElement_db::getParentId() const {
  return this->_parent_id;
}

std::vector<FSElement_db> FSElement_db::getPathElements(const Session &s) {
  if(s.getUserId() != this->_owner_id) {
    warn("Trying to access not your element");
    throw se_exceptions::IllegalAccessException{"Trying to access not your element"};
  }
  std::vector<FSElement_db> res{FSElement_db{*this}};
  if(this->_parent_id != this->id && this->_parent_id >= 0) {
    auto parent = FSElement_db::get(s, this->_parent_id);
    auto tmp = parent.getPathElements(s);
    res.insert(res.end(), tmp.begin(), tmp.end());
    return res;
  }
  std::reverse(res.begin(), res.end());
  return res;
}

bool FSElement_db::isDir() const {
  return this->_type == FSElement::Type::DIRECTORY;
}

QString FSElement_db::getName() const {
  return this->_name;
}

int FSElement_db::getPhysicalId() {
  if(this->is_link()) {
    return this->_original.getId();
  } else {
    return this->getId();
  }
}

int FSElement_db::getIdForUser(const Session &s, int file_id, int user_id) {
  auto file = FSElement_db::get(s, file_id);
  if(file.is_link()) {
    file = file._original.getValue();
  }
  if(file._owner_id == user_id) {
    return file.getId();
  }
  auto links = file._links.getValues();
  for(auto &l: links) {
    if(l->_owner_id == user_id) {
      return l->getId();
    }
  }
  throw FileUserException{"This file is not available for this user"};
}

bool FSElement_db::availableForUser(int user_id) {
  auto val = this->_links.getValues();
  return std::any_of(val.begin(), val.end(), [user_id](auto l) {
    return l->_owner_id == user_id;
  });
}

std::vector<FSElement_db*> FSElement_db::getChildren() {
  return this->_children.getValues();
}

bool FSElement_db::is_link() const {
  return this->_owner_id != this->_creator.getId();
}

[[maybe_unused]] User FSElement_db::getCreator() {
  return this->_creator.getValue();
}

bool FSElement_db::path_on_disk_available() const {
  return !QFileInfo::exists(this->getPath());
}

int FSElement_db::getCharId() const {
  return this->_char_id;
}

int FSElement_db::getCommentId() const {
  return this->_comment_id;
}

void FSElement_db::addCharId(int v) {
  this->_char_id += v;
  this->save();
}

void FSElement_db::incCommentId() {
  this->_comment_id++;
  this->save();
}
