#include "FSElement_db.h"

#include <QFile>
#include <QDir>
#include <QJsonDocument>

#include "utils.h"
#include "File.h"

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
  auto children = this->_children.getValues();
  for(auto& c: children) {
    c->remove();
  }
  if(!this->is_link()) {
    // è il proprietario
    debug("Links: "+QString::number(this->id));
    for(auto& link: this->_links.getValues()) {
      // TODO: va notificato
      debug(link->getName());
      link->remove();
    }
    if(this->_type == FSElement::Type::FILE) {
      this->del_file();
    }
  }
  // se è un link, è sufficiente eliminare la entry
  info(QString{"Remove "}+(this->is_link() ? "Link" : "File")+" ID: "+QString::number(this->id)+(this->_type == FSElement::Type::FILE ? " path "+this->_path : ""));
  this->_remove<FSElement_db>();
}

FSElement_db FSElement_db::create(const Session &s, int parent_id, QString name, bool is_file) {
  FSElement_db fs_e{};
  // TODO: controlla che non ci sia già
  fs_e._path = is_file ? rndString(64) : "";
  fs_e._name = name;
  fs_e._type = is_file ? FSElement::Type::FILE : FSElement::Type::DIRECTORY;
  fs_e._parent_id = parent_id;
  fs_e._owner_id = s.getUserId();
  fs_e._creator_id = s.getUserId();
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
    info("New File ID: "+QString::number(fs_e.id)+" path "+fs_e._path);
  } else {
    info("New Dir ID: "+QString::number(fs_e.id));
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
    warn("You are tring to store to an element that is not a file");
    // TODO: lancia eccezione
    exit(1);
  }
  QFile file(this->getPath());
  file.remove();
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
