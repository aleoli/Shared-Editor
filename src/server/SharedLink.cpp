#include "SharedLink.h"

#include "utils.h"
#include "exceptions.h"

const QString SharedLink::table_name = "shared_link";

SharedLink::SharedLink(): Persistent() {
  this->_token = "";
  this->_fs_element = Lazy<FSElement_db>{-1};
}

SharedLink::SharedLink(const SharedLink &e): Persistent(e) {
  this->_token = e._token;
  this->_fs_element = e._fs_element;
}

SharedLink::SharedLink(SharedLink &&e) noexcept: Persistent(std::move(e)) {
  this->_token = e._token;
  this->_fs_element = e._fs_element;
}

SharedLink::SharedLink(const QSqlRecord& r): Persistent(r) {
  this->_token = r.value("token").toString();
  this->_fs_element = Lazy<FSElement_db>{r.value("element_id").toInt()};
}

SharedLink::~SharedLink() = default;

SharedLink& SharedLink::operator=(const SharedLink& e) {
  if(this == &e) {
    return *this;
  }
  this->_token = e._token;
  this->_fs_element = e._fs_element;
  Persistent::operator=(e);
  return *this;
}

void SharedLink::save() {
  this->_save<SharedLink>();
}

void SharedLink::save_record(QSqlRecord &r) {
  r.setValue("token", this->_token);
  r.setValue("element_id", this->_fs_element.getId());
}

void SharedLink::remove() {
  this->_remove<SharedLink>();
}

SharedLink SharedLink::create(int file_id) {
  SharedLink sl{};
  do {
    sl._token = rndString(64);
  } while(!SharedLink::token_available(sl._token));
  sl._fs_element = Lazy<FSElement_db>{file_id};
  sl.save();
  return sl;
}

SharedLink SharedLink::get(const QString &token) {
  auto l = DB::get()->get<SharedLink>("token = '"+token+"'");
  if(l.size() != 1) {
    throw se_exceptions::ShareException{"No token '"+token+"' found"};
  }
  return l[0];
}

FSElement_db SharedLink::element() {
  return this->_fs_element.getValue();
}

QString SharedLink::getToken() const {
  return this->_token;
}

bool SharedLink::token_available(const QString &token) {
  auto l = DB::get()->get<SharedLink>("token = '"+token+"'");
  return l.empty();
}
