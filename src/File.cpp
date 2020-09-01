#include "File.h"

#include "utils.h"
#include "exceptions.h"
#include "def.h"

using namespace se_exceptions;

#include <sstream>
#include <limits>
#include <algorithm>
#include <QChar>
#include <QJsonDocument>
#include <QFile>
#include <utility>

File::File() = default;

File::File(const File &file) {
  this->_users = file._users;
  this->_symbols = file._symbols;
  this->_comments = file._comments;
  this->dirty = file.dirty;
}

File::File(File &&file) noexcept: _users(std::move(file._users)), _symbols(std::move(file._symbols)), _comments(std::move(file._comments)), dirty(file.dirty) {}

File::File(std::unordered_map<int, File::UserInfo> users, std::list<Symbol> symbols, std::map<CommentIdentifier, Comment> comments)
  : _users(std::move(users)), _symbols(std::move(symbols)), _comments(std::move(comments)) {}

File::File(const QJsonObject &json){
  checkAndAssign(json);
}

File::File(QJsonObject &&json){
  checkAndAssign(json);
}

File& File::operator=(const File &file) {
  if(this == &file) {
    return *this;
  }
  this->_users = file._users;
  this->_symbols = file._symbols;
  this->_comments = file._comments;
  this->dirty = file.dirty;
  return *this;
}

File& File::operator=(File &&file) noexcept {
  if(this == &file) {
    return *this;
  }
  this->_users = std::move(file._users);
  this->_symbols = std::move(file._symbols);
  this->_comments = std::move(file._comments);
  this->dirty = file.dirty;
  return *this;
}

void File::checkAndAssign(const QJsonObject &json) {
  auto usersValue = json["users"];
  auto symbolsValue = json["symbols"];
  auto commentsValue = json["comments"];

  if(usersValue.isUndefined() || symbolsValue.isUndefined() || commentsValue.isUndefined()) {
    throw FileFromJsonException{"The QJsonObject has some fields missing"};
  }

  if(!usersValue.isArray() || !symbolsValue.isArray() || !commentsValue.isArray()) {
    throw FileFromJsonException{"One or more fields are not valid"};
  }

  auto users = usersValue.toArray();
  auto symbols = symbolsValue.toArray();
  auto comments = commentsValue.toArray();

  auto ul = std::unique_lock{this->_mutex};
  _users = jsonArrayToUsers(users);
  _symbols = jsonArrayToSymbols(symbols);
  _comments = jsonArrayToComments(comments);
}

File File::fromJsonObject(const QJsonObject &json) {
  return File(json);
}

File File::fromJsonObject(QJsonObject &&json) {
  return File(json);
}

File File::fromQByteArray(const QByteArray &array) {
  #if BINARY_FILE
      auto doc = QJsonDocument::fromBinaryData(array);
  #else
      auto doc = QJsonDocument::fromJson(array);
  #endif

  return File(doc.object());
}

File File::fromQByteArray(QByteArray &&array) {
  #if BINARY_FILE
      auto doc = QJsonDocument::fromBinaryData(array);
  #else
      auto doc = QJsonDocument::fromJson(array);
  #endif

  return File(doc.object());
}

QJsonObject File::toJsonObject() const {
  QJsonObject json;

  json["users"] = QJsonValue(usersToJsonArray());
  json["comments"] = QJsonValue(commentsToJsonArray());
  auto sl = std::shared_lock{this->_mutex};
  json["symbols"] = QJsonValue(symbolsToJsonArray());

  return json;
}

QByteArray File::toQByteArray() const {
  QJsonDocument doc(toJsonObject());

#if BINARY_FILE
  return doc.toBinaryData();
#else
  #if ASCII_COMPACT
    return doc.toJson(QJsonDocument::Compact);
  #else
    return doc.toJson();
  #endif
#endif
}

QJsonArray File::usersToJsonArray() const {
  QJsonArray array;

  auto sl = std::shared_lock{this->_mutex};
  for(auto &el : _users) {
    QJsonObject value;

    value["uid"] = el.second.userId;
    value["name"] = el.second.username;
    value["on"] = el.second.online;

    array.append(value);
  }

  return array;
}

std::unordered_map<int, File::UserInfo> File::jsonArrayToUsers(const QJsonArray &array) {
  std::unordered_map<int, File::UserInfo> users;

  for(auto&& el : array) {
    if(!el.isObject()) {
      throw FileFromJsonException{"One or more fields in users array are not valid"};
    }

    auto obj = el.toObject();
    auto userIdValue = obj["uid"];
    auto usernameValue = obj["name"];
    auto onlineValue = obj["on"];

    if(userIdValue.isUndefined() || usernameValue.isUndefined() || onlineValue.isUndefined()) {
      throw FileFromJsonException{"The QJsonObject has some fields missing"};
    }

    auto userId = userIdValue.toInt(-1);

    if(userId == -1) {
      throw FileFromJsonException{"One or more fields in users array are not valid"};
    }

    if(!usernameValue.isString() || !onlineValue.isBool()) {
      throw FileFromJsonException{"One or more fields in users array are not valid"};
    }

    File::UserInfo info { userId, usernameValue.toString(), onlineValue.toBool() };

    users[userId] = info;
  }

  return users;
}

std::list<Symbol> File::jsonArrayToSymbols(const QJsonArray &array) {
  std::list<Symbol> symbols;

  for(auto&& el : array) {
    if(!el.isObject()) {
      throw FileFromJsonException{"One or more fields in symbols array are not valid"};
    }

    symbols.emplace_back(el.toObject());
  }

  return symbols;
}

QJsonArray File::symbolsToJsonArray() const {
  QJsonArray array;

  auto sl = std::shared_lock{this->_mutex};
  for(auto &sym : _symbols) {
    array.append(QJsonValue(sym.toJsonObject()));
  }

  return array;
}

File::Comment File::commentFromJsonObject(const QJsonObject &obj) {
  auto idV = obj["id"];
  auto textV = obj["text"];
  auto creationDateV = obj["date"];

  if(idV.isUndefined() || textV.isUndefined() || creationDateV.isUndefined()) {
    throw FileFromJsonException{"The QJsonObject has some fields missing"};
  }

  if(!idV.isObject() || !textV.isString() || !creationDateV.isString()) {
    throw FileFromJsonException{"One or more fields in users array are not valid"};
  }

  auto text = textV.toString();
  auto creationDateString = creationDateV.toString();
  auto creationDate = QDateTime::fromString(creationDateString);

  if(!creationDate.isValid()) {
    debug("creation date is not valid");
    throw FileFromJsonException{"One or more fields in users array are not valid"};
  }

  Comment comment;
  comment.identifier = CommentIdentifier::fromJsonObject(idV.toObject());
  comment.text = text;
  comment.creationDate = creationDate;

  return comment;
}

QJsonObject File::commentToJsonObject(const Comment &comment) {
  QJsonObject value;

  value["id"] = comment.identifier.toJsonObject();
  value["text"] = comment.text;
  value["date"] = comment.creationDate.toString();

  return value;
}

QJsonArray File::commentsToJsonArray() const {
  QJsonArray array;

  auto sl = std::shared_lock{this->_mutex};
  std::transform(_comments.begin(), _comments.end(), std::back_inserter(array), [](const auto &comment) {
    return File::commentToJsonObject(comment.second);
  });

  return array;
}

std::map<File::CommentIdentifier, File::Comment> File::jsonArrayToComments(const QJsonArray &array) {
  std::map<File::CommentIdentifier, File::Comment> comments;

  for(const auto& el: array) {
    if(!el.isObject()) {
      throw FileFromJsonException{"One or more fields in comments array are not valid"};
    }

    auto obj = el.toObject();
    auto comment = File::commentFromJsonObject(obj);

    comments[comment.identifier] = comment;
  }

  return comments;
}

std::unordered_map<int, File::UserInfo> File::getUsers() const {
  auto sl = std::shared_lock{this->_mutex};
  return _users;
}

std::list<Symbol> File::getSymbols() const {
  warn("File::getSymbols deprecated. Use forEachSymbol instead");
  auto sl = std::shared_lock{this->_mutex};
  return _symbols;
}

std::list<Symbol>::iterator File::getSymbolsStart() {
  return _symbols.begin();
}

void File::forEachSymbol(const std::function<void(const Symbol&) >& lambda) {
  auto sl = std::shared_lock{this->_mutex};

  for(auto &sym : _symbols) {
    lambda(sym);
  }
}

std::map<File::CommentIdentifier, File::Comment> File::getComments() const {
  auto sl = std::shared_lock{this->_mutex};
  return _comments;
}

Symbol& File::symbolAt(int pos, std::list<Symbol>::iterator *it) {
  auto sl = std::shared_lock{this->_mutex};
  return _symbolAt(pos, it);
}

Symbol &File::_symbolAt(int pos, std::list<Symbol>::iterator *it) {
  auto target = iteratorAt(pos);

  if(target == _symbols.end()) {
    throw FileSymbolsException{"Invalid position"};
  }

  if(it != nullptr) *it = target;
  return *target;
}

std::list<Symbol>::iterator File::iteratorAt(int pos) {
  auto size = _symbols.size();

  if(pos > size || pos < 0)
    throw FileSymbolsException{"File::iteratorAt: bad pos"};

  std::list<Symbol>::iterator it;
  int middle = size / 2;
  if(pos <= middle) {
    //debug("Closer to the start");
    it = _symbols.begin();
    std::advance(it, pos);
  }
  else {
    //debug("Closer to the end");
    it = _symbols.end();
    std::advance(it, pos - size);
  }

  return it;
}

std::pair<int, Symbol&> File::symbolById(const SymbolId &id, std::list<Symbol>::iterator *it) {
  auto sl = std::shared_lock{this->_mutex};
  return _symbolById(id, it);
}

std::pair<int, Symbol&> File::_symbolById(const SymbolId &id, std::list<Symbol>::iterator *it) {
  auto res = _iteratorById(id, it);

  return {res.first, *res.second};
}

std::pair<int, std::list<Symbol>::iterator> File::iteratorById(const SymbolId &id, std::list<Symbol>::iterator *it) {
  auto sl = std::shared_lock{this->_mutex};
  return _iteratorById(id, it);
}

std::pair<int, std::list<Symbol>::iterator> File::_iteratorById(const SymbolId &id, std::list<Symbol>::iterator *it) {
  int pos = 0;
  auto start = it == nullptr ? _symbols.begin() : *it;

  auto result = std::find_if(start, _symbols.end(), [&id, &pos](const Symbol &cmp) {
        bool val = cmp.getSymbolId() == id;
        if(!val) pos++;
        return val;
  });

  if(result == std::end(_symbols)) {
    throw FileSymbolsException{"Symbol does not exist"};
  }

  if(it != nullptr) *it = result;
  return {pos, result};
}

int File::getPosition(const SymbolId &id, std::list<Symbol>::iterator *it) {
  return symbolById(id, it).first;
}

int File::_getPosition(const SymbolId &id, std::list<Symbol>::iterator *it) {
  return _symbolById(id, it).first;
}

int File::numSymbols() const {
  auto sl = std::shared_lock{this->_mutex};
  return _symbols.size();
}

std::string File::to_string() const {
  return text();
}

std::string File::text() const {
  std::string res;
  auto sl = std::shared_lock{this->_mutex};
  std::transform(_symbols.begin(), _symbols.end(), std::back_inserter(res), [](const auto &s) {
    return s.getChar().toLatin1();
  });
  return res;
}

void File::clear() {
  auto ul = std::unique_lock{this->_mutex};
  dirty = true;
  _symbols.clear();
}

void File::addUser(int userId, const QString &username) {
  auto ul = std::unique_lock{this->_mutex};
  if(_users.count(userId) != 0) {
    throw FileUserException{"User already exists"};
  }

  dirty = true;
  _users[userId] = { userId, username, true };
}

void File::removeUser(int userId) {
  auto ul = std::unique_lock{this->_mutex};
  if(_users.count(userId) == 0) {
    throw FileUserException{"User does not exist"};
  }

  dirty = true;
  _users.erase(userId);
}

void File::setOnline(int userId, bool val) {
  auto ul = std::unique_lock{this->_mutex};
  if(_users.count(userId) == 0) {
    throw FileUserException{"User is not in map"};
  }

  dirty = true;
  _users[userId].online = val;
}

bool File::isOnline(int userId) {
  auto sl = std::shared_lock{this->_mutex};
  if(_users.count(userId) == 0) {
    throw FileUserException{"User is not in map"};
  }

  return _users[userId].online;
}

QString File::getUsername(int userId) {
  auto sl = std::shared_lock{this->_mutex};
  if(_users.count(userId) == 0) {
    throw FileUserException{"User is not in map"};
  }

  return _users[userId].username;
}

void File::localInsert(Symbol &sym, int pos, std::list<Symbol>::iterator *it) {
  auto ul = std::unique_lock{this->_mutex};
  // (chi ha chiamato questo metodo poi si preoccuperà di mandare info al server)
  auto size = _symbols.size();

  if(pos < 0 || pos > size) {
    throw FileSymbolsException{"Invalid insert position"};
  }

  //debug("Pos: " + QString::number(pos));

  std::vector<Symbol::Identifier> v1, v2;
  std::list<Symbol>::iterator insertPos;
  if(it == nullptr) insertPos = iteratorAt(pos);
  else insertPos = *it;

  if(size == 0) {}
  else if(pos == 0) {
    //debug("Front insertion");
    v2 = std::move(insertPos->getPos());
  }
  else if(pos == size) {
    //debug("Back insertion");
    v1 = std::move(std::prev(insertPos)->getPos());
  }
  else {
    //debug("Middle insertion");
    v1 = std::move(std::prev(insertPos)->getPos());
    v2 = std::move(insertPos->getPos());
  }

  std::vector<Symbol::Identifier> position;

  findPosition(sym.getSymbolId().getUserId(), v1, v2, position);

  sym.setPos(position);
  dirty = true;
  _symbols.emplace(insertPos, sym);
}

void File::findPosition(int userId, std::vector<Symbol::Identifier> &v1,
  std::vector<Symbol::Identifier> &v2, std::vector<Symbol::Identifier> &position,
  int level) {

  Symbol::Identifier pos1, pos2;

  if(!v1.empty()) pos1 = v1[0];
  else pos1 = {0, userId};

  if(!v2.empty()) pos2 = v2[0];
  else pos2 = {std::numeric_limits<int>::max(), userId};

  int digit1 = pos1.getDigit();
  int digit2 = pos2.getDigit();

  if(digit2 - digit1 > 1){
    //finished, found the position
    //debug("Level: " + QString::number(level));
    int digit = generateDigit(digit1, digit2);
    position.emplace_back(digit, userId);
    return;
  }

  else if(digit2 - digit1 == 1){
    //must go deeper
    position.push_back(pos1);

    if(!v1.empty()) v1.erase(v1.begin());
    v2.clear();

    findPosition(userId, v1, v2, position, level + 1);
  }

  else if(digit2 == digit1) {
    //must go deeper
    int userId1 = pos1.getUserId();
    int userId2 = pos2.getUserId();

    if (userId1 < userId2) {
      position.push_back(pos1);

      if (!v1.empty()) v1.erase(v1.begin());
      v2.clear();

      findPosition(userId, v1, v2, position, level + 1);
    }
    else if (userId1 == userId2) {
      position.push_back(pos1);

      if (!v1.empty()) v1.erase(v1.begin());
      if (!v2.empty()) v2.erase(v2.begin());

      findPosition(userId, v1, v2, position, level + 1);
    }
    else {
      throw FileLocalInsertException{"vector _pos is not sorted!"};
    }
  }
}

int File::generateDigit(int digit1, int digit2) {
  quint64 val = digit1;

  if(digit2 - digit1 > CRDT_STEP) val += CRDT_STEP;
  else val = (val + digit2) / 2;

  //debug("Dig1: " + QString::number(digit1) + " dig2: " + QString::number(digit2) + " val: " + QString::number(val));

  return static_cast<int>(val); //surely it fits into an int
}

int File::remoteInsert(const Symbol &sym, std::list<Symbol>::iterator *it, int oldPos) {
  auto ul = std::unique_lock{this->_mutex};

  auto start = it == nullptr ? _symbols.begin() : *it;
  int pos = 0;
  auto result = std::find_if(start, _symbols.end(), [&sym, &pos](const Symbol &cmp) {
        bool val = sym < cmp;
        if(!val) pos++;
        return val;
    });

  dirty = true;
  _symbols.emplace(result, sym);

  if(it != nullptr) *it = result;
  return oldPos + pos + 1;
}

void File::localDelete(int pos, std::list<Symbol>::iterator *it) {
  auto ul = std::unique_lock{this->_mutex};
  if(pos < 0 || _symbols.size() <= pos) {
    throw FileSymbolsException{"Invalid delete position"};
  }

  dirty = true;
  auto target = it == nullptr ? iteratorAt(pos) : *it;
  if(it != nullptr) *it = std::next(*it);

  _symbols.erase(target);
}

int File::remoteDelete(const SymbolId &id, std::list<Symbol>::iterator *it, int oldPos) {
  auto ul = std::unique_lock{this->_mutex};

  try {
    int pos = 0;
    std::list<Symbol>::iterator target;
    if(it == nullptr || (*it)->getSymbolId() != id) {
      auto res = _iteratorById(id, it);
      pos = res.first;
      target = res.second;
      if(it != nullptr) *it = std::next(target);
    }
    else {
      target = *it;
      *it = std::next(*it);
    }

    _symbols.erase(target);
    dirty = true;

    return oldPos + pos;
  }
  catch(FileSymbolsException& e) {
    //il simbolo non esiste più
    return -1;
  }
}

int File::remoteUpdate(const Symbol &sym) {
  auto ul = std::unique_lock{this->_mutex};

  try {
    auto pos = _symbolById(sym.getSymbolId());
    auto &symbol = pos.second;

    symbol.update(sym);
    dirty = true;

    return pos.first;
  }
  catch(FileSymbolsException& e) {
    //il simbolo non esiste più
    return -1;
  }
}

void File::remoteAddComment(const Comment &comment) {
  auto ul = std::unique_lock{this->_mutex};
  _comments[comment.identifier] = comment;
  dirty = true;
}

void File::remoteUpdateComment(const Comment &comment) {
  auto ul = std::unique_lock{this->_mutex};
  if(_comments.count(comment.identifier) == 0) {
    // it does not exist
    throw CommentException{"comment not found"};
  }
  _comments[comment.identifier] = comment;
  dirty = true;
}

void File::remoteDeleteComment(const Comment &comment) {
  auto ul = std::unique_lock{this->_mutex};
  if(_comments.count(comment.identifier) == 0)return;
  _comments.erase(comment.identifier);
  dirty = true;
}

void File::store(const QString &path) {
  auto sl = std::shared_lock{this->_mutex};
  if(!this->dirty) {
    return;
  }

  QFile file(path);
  if(!file.open(QIODevice::WriteOnly)) {
    error("cannot open file "+path);
    throw se_exceptions::NoFileOnDiskException{"cannot open file "+path};
  }
  file.write(this->toQByteArray());
  file.close();
  this->dirty = false;
}

bool operator==(const File& lhs, const File& rhs) {
  auto sl = std::shared_lock{lhs._mutex};
  auto sl2 = std::shared_lock{rhs._mutex};
  return lhs._users == rhs._users && lhs._symbols == rhs._symbols && lhs._comments == rhs._comments;
}

bool operator!=(const File& lhs, const File& rhs) {
  return !operator==(lhs, rhs);
}

bool operator==(const File::Comment & lhs, const File::Comment & rhs) {
  return lhs.identifier == rhs.identifier && lhs.text == rhs.text && lhs.creationDate == rhs.creationDate;
}

bool operator==(const File::UserInfo& lhs, const File::UserInfo& rhs) {
  return lhs.userId == rhs.userId && lhs.username == rhs.username;
}
