#include "FSElement_db.h"

#include <QFile>
#include <QDir>
#include <QJsonDocument>

#include "utils.h"
#include "File.h"
#include "SharedLink.h"

const QString FSElement_db::table_name = "fs_element";

FSElement_db::FSElement_db(): Persistent() {
  this->_path = "";
  this->_name = "";
  this->_type = FSElement::Type::FILE;
  this->_parent_id = -1;
  this->_owner_id = -1;
  this->_creator_id = -1;
  this->_original = Lazy<FSElement_db>{-1};
}

FSElement_db::FSElement_db(const FSElement_db &e): Persistent(e) {
  this->_path = e._path;
  this->_name = e._name;
  this->_type = e._type;
  this->_parent_id = e._parent_id;
  this->_owner_id = e._owner_id;
  this->_creator_id = e._creator_id;
  this->_children = e._children;
  this->_original = e._original;
  this->_links = e._links;
}

FSElement_db::FSElement_db(FSElement_db &&e): Persistent(e) {
  this->_path = e._path;
  this->_name = e._name;
  this->_type = e._type;
  this->_parent_id = e._parent_id;
  this->_owner_id = e._owner_id;
  this->_creator_id = e._creator_id;
  this->_children = e._children;
  this->_original = e._original;
  this->_links = e._links;
}

FSElement_db::FSElement_db(QSqlRecord r): Persistent(r) {
  this->_path = r.value("path").toString();
  this->_name = r.value("name").toString();
  this->_type = static_cast<FSElement::Type>(r.value("type").toInt());
  this->_parent_id = r.value("parent_id").toInt();
  this->_owner_id = r.value("owner_id").toInt();
  this->_creator_id = r.value("creator_id").toInt();
  this->_children = LazyList<FSElement_db>{FSElement_db::table_name, "parent_id = "+r.value("id").toString()};
  this->_original = Lazy<FSElement_db>{r.value("linked_from").toInt()};
  this->_links = LazyList<FSElement_db>{FSElement_db::table_name, "linked_from = "+r.value("id").toString()};
}

FSElement_db::~FSElement_db() {
}

FSElement_db& FSElement_db::operator=(const FSElement_db& e) {
  if(this == &e) {
    return *this;
  }
  this->_path = e._path;
  this->_name = e._name;
  this->_type = e._type;
  this->_parent_id = e._parent_id;
  this->_owner_id = e._owner_id;
  this->_creator_id = e._creator_id;
  this->_children = e._children;
  this->_original = e._original;
  this->_links = e._links;
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
  r.setValue("creator_id", this->_creator_id);
  r.setValue("linked_from", this->_original.getId());
}

void FSElement_db::remove() {
  // TODO: controlla che non si stia eliminando la root dir
  auto children = this->_children.getValues();
  for(auto& c: children) {
    c->remove();
  }
  if(!this->is_link()) {
    // è il proprietario
    for(auto& link: this->_links.getValues()) {
      // TODO: va notificato
      debug(link->getName());
      link->remove();
    }
    if(this->_type == FSElement::Type::FILE) {
      this->del_file();
    }
    // TODO: remove shared_links
  }
  // se è un link, è sufficiente eliminare la entry
  info(QString{"Remove "}+(this->is_link() ? "Link" : "File")+" ID: "+QString::number(this->id)+(this->_type == FSElement::Type::FILE ? " path "+this->_path : ""));
  this->_remove<FSElement_db>();
}

FSElement_db FSElement_db::create(int user_id, int parent_id, QString name, bool is_file) {
  FSElement_db fs_e{};
  // TODO: controlla che non ci sia già
  fs_e._path = is_file ? rndString(64) : "";
  fs_e._name = name;
  fs_e._type = is_file ? FSElement::Type::FILE : FSElement::Type::DIRECTORY;
  fs_e._parent_id = parent_id;
  fs_e._owner_id = user_id;
  fs_e._creator_id = user_id;
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
      // TODO: lancia eccezione
      exit(1);
    }
    File el{fs_e.id};
    f.write(el.toQByteArray());
    f.close();
    info("New File ID: "+QString::number(fs_e.id)+" path "+fs_e._path+" parent_id: "+QString::number(fs_e._parent_id));
  } else {
    info("New Dir ID: "+QString::number(fs_e.id)+" parent_id: "+QString::number(fs_e._parent_id));
  }
  return fs_e;
}

FSElement_db FSElement_db::clone(const Session &s) {
  if(this->_creator_id == s.getUserId()) {
    error("Link creation of own element");
    // TODO: lancia eccezione
    exit(1);
  }
  if(this->is_link()) {
    error("Cannot create a link of a link");
    // TODO: lancia eccezione
    exit(1);
  }
  FSElement_db fs_e{*this};
  fs_e.id = 0;
  fs_e._parent_id = -1;
  fs_e._owner_id = s.getUserId();
  fs_e._original = Lazy<FSElement_db>{this->id};
  fs_e.save();
  fs_e._children = LazyList<FSElement_db>{FSElement_db::table_name, "parent_id = "+QString::number(fs_e.id)};
  fs_e._links = LazyList<FSElement_db>{FSElement_db::table_name, "linked_from = "+QString::number(fs_e.id)};
  info("New Link ID: "+QString::number(fs_e.id)+" from original "+QString::number(this->id));
  return fs_e;
}

FSElement FSElement_db::getFSElement() const {
  return FSElement{this->id, this->_parent_id, this->_name, this->_type};
}

File FSElement_db::load() const {
  if(this->_type != FSElement::Type::FILE) {
    warn("You are tring to load an element that is not a file");
    // TODO: lancia eccezione
    exit(1);
  }
  QFile f(this->getPath());
  if(!f.open(QIODevice::ReadOnly)){
    error("cannot open file "+this->_path);
    // TODO: lancia eccezione
    exit(1);
  }
  auto data = f.readAll();
  f.close();
  return File::fromQByteArray(data);
}

void FSElement_db::store(const File &f) {
  if(this->_type != FSElement::Type::FILE) {
    warn("You are tring to store to an element that is not a file");
    // TODO: lancia eccezione
    exit(1);
  }
  QFile file(this->getPath());
  if(!file.open(QIODevice::WriteOnly)){
    error("cannot open file "+this->_path);
    // TODO: lancia eccezione
    exit(1);
  }
  file.write(f.toQByteArray());
  file.close();
}

void FSElement_db::del_file() {
  if(this->_type != FSElement::Type::FILE) {
    warn("You are tring to delete to an element that is not a file");
    // TODO: lancia eccezione
    exit(1);
  }
  QFile file(this->getPath());
  file.remove();
}

FSElement_db FSElement_db::get(const Session &s, int id) {
  auto fs_e = DB::get()->getOne<FSElement_db>(id);
  if(fs_e._owner_id != s.getUserId()) {
    warn("User "+QString::number(s.getUserId())+" has tryed to access a file of user "+QString::number(fs_e._owner_id));
    // TODO: lancia eccezione
    exit(1);
  }
  return fs_e;
}

FSElement_db FSElement_db::mkroot(int user_id) {
  bool no_folder = false;
  try {
    FSElement_db::root(user_id);
  } catch(...) {
    // TODO: controlla che l'ecezione sia del tipo giusto
    no_folder = true;
  }
  if(!no_folder) {
    error("Double creation of root dir");
    // TODO: lancia eccezione
    exit(1);
  }
  return FSElement_db::create(user_id, -1, "root", false);
}

FSElement_db FSElement_db::root(int user_id) {
  auto l = DB::get()->get<FSElement_db>("parent_id = -1 AND creator_id = "+QString::number(user_id));
  if(l.size() != 1) {
    // TODO: lancia eccezione
    throw 1;
  }
  return l[0];
}

FSElement_db FSElement_db::mkdir(const Session &s, QString name) {
  if(this->_type != FSElement::Type::DIRECTORY) {
    error("Trying to create a directory in not-dir");
    // TODO: lancia eccezione
    exit(1);
  }
  if(this->_owner_id != s.getUserId()) {
    error("Trying to modify not your directory");
    // TODO: lancia eccezione
    exit(1);
  }
  return FSElement_db::create(s.getUserId(), this->id, name, false);
}

FSElement_db FSElement_db::mkfile(const Session &s, QString name) {
  if(this->_type != FSElement::Type::DIRECTORY) {
    error("Trying to create a file in not-dir");
    // TODO: lancia eccezione
    exit(1);
  }
  if(this->_owner_id != s.getUserId()) {
    error("Trying to modify not your directory");
    // TODO: lancia eccezione
    exit(1);
  }
  return FSElement_db::create(s.getUserId(), this->id, name, true);
}

std::vector<FSElement_db*> FSElement_db::ls(const Session &s) {
	if(this->_type != FSElement::Type::DIRECTORY) {
		warn("Trying to list a not-directory element");
		// TODO: lancia eccezione
		exit(1);
	}
  if(s.getUserId() != this->_owner_id) {
    warn("Trying to access not your directory");
    // TODO: lancia eccezione
    exit(1);
  }
  return this->getChildren();
}

SharedLink FSElement_db::share(const Session &s) {
  if(this->_type != FSElement::Type::FILE) {
    error("Trying to share a not-file element");
    // TODO: lancia eccezione
    exit(1);
  }
  if(s.getUserId() != this->_owner_id) {
    warn("Trying to share not your file");
    // TODO: lancia eccezione
    exit(1);
  }
	if(this->is_link()) {
		warn("Trying to share a shared file");
		// TODO: lancia eccezione
		exit(1);
	}
  return SharedLink::create(this->id);
}

FSElement_db FSElement_db::link(const Session &s, const QString &token) {
  auto orig = SharedLink::get(token).element();
  return orig.clone(s);
}

QString FSElement_db::getPath() const {
  return QDir::homePath()+"/.shared_editor/data/"+this->_path;
}

QString FSElement_db::getName() const {
  return this->_name;
}

std::vector<FSElement_db*> FSElement_db::getChildren() {
  return this->_children.getValues();
}

bool FSElement_db::is_link() const {
  return this->_owner_id != this->_creator_id;
}
