#include "File.h"

#include "utils.h"
#include "exceptions.h"
#include "def.h"

using namespace se_exceptions;

#include <sstream>
#include <cmath>
#include <algorithm>
#include <QChar>
#include <QJsonDocument>

File::File() {}

File::File(std::unordered_map<int, File::ClientInfo> clients, std::vector<Symbol> symbols)
  : _clients(clients), _symbols(symbols) {}

File::File(const QJsonObject &json){
  checkAndAssign(json);
}

File::File(QJsonObject &&json){
  checkAndAssign(json);
}

void File::checkAndAssign(const QJsonObject &json) {
  auto clientsValue = json["clients"];
  auto symbolsValue = json["symbols"];

  if(clientsValue.isUndefined() || symbolsValue.isUndefined()) {
    throw FileFromJsonException{"The QJsonObject has some fields missing"};
  }

  if(!clientsValue.isArray() || !symbolsValue.isArray()) {
    throw FileFromJsonException{"One or more fields are not valid"};
  }

  auto clients = clientsValue.toArray();
  auto symbols = symbolsValue.toArray();

  _clients = jsonArrayToclients(clients);
  _symbols = utils::jsonArrayToVector<Symbol>(symbols);
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

  json["clients"] = QJsonValue(clientsToJsonArray());
  json["symbols"] = QJsonValue(utils::vectorToJsonArray(_symbols));

  return json;
}

QByteArray File::toQByteArray() const {
  QJsonDocument doc(toJsonObject());

#if BINARY_FILE
  return doc.toBinaryData();
#else
  return doc.toJson(QJsonDocument::Compact);
#endif
}

QJsonArray File::clientsToJsonArray() const {
  QJsonArray array;

  for(auto &el : _clients) {
    QJsonObject value;

    value["userId"] = el.second.userId;
    value["username"] = el.second.username;

    array.append(value);
  }

  return array;
}

std::string File::clientsToString() const {
  std::stringstream ss;

  for(auto &el : _clients) {
    ss << "\tuserId: " << el.second.userId << std::endl;
    ss << "\t\tusername: " << el.second.username.toStdString() << std::endl;
  }

  return ss.str();
}

std::unordered_map<int, File::ClientInfo> File::jsonArrayToclients(const QJsonArray &array) {
  std::unordered_map<int, File::ClientInfo> clients;

  for(auto&& el : array) {
    if(!el.isObject()) {
      throw FileFromJsonException{"One or more fields in clients array are not valid"};
    }

    auto obj = el.toObject();
    auto userIdValue = obj["userId"];
    auto usernameValue = obj["username"];

    if(userIdValue.isUndefined() || usernameValue.isUndefined()) {
        throw FileFromJsonException{"The QJsonObject has some fields missing"};
      }

    auto userId = userIdValue.toInt(-1);

    if(userId == -1) {
      throw FileFromJsonException{"One or more fields in clients array are not valid"};
    }

    if(!usernameValue.isString()) {
      throw FileFromJsonException{"One or more fields in clients array are not valid"};
    }

    File::ClientInfo info { userId, usernameValue.toString() };

    clients[userId] = info;
  }

  return clients;
}

std::string File::symbolsToString() const {
  std::stringstream ss;

  for(auto it = _symbols.begin(); it != _symbols.end(); it++) {
    auto &el = *it;

    ss << '\t' << el.to_string();

    if(it+1 != _symbols.end())
      ss << std::endl;
  }

  return ss.str();
}

std::unordered_map<int, File::ClientInfo> File::getClients() const {
  return _clients;
}

std::vector<Symbol> File::getSymbols() const {
  return _symbols;
}

Symbol& File::symbolAt(int pos) {
  if(_symbols.size() <= pos) {
    throw FileSymbolsException{"Invalid position"};
  }

  return _symbols[pos];
}

std::pair<int, Symbol&> File::symbolById(SymbolId id) {
  int pos = 0;
  auto result = std::find_if(_symbols.begin(), _symbols.end(), [id, &pos](const Symbol &cmp) {
        bool val = cmp.getSymbolId() == id;
        if(!val) pos++;
        return val;
  });

  if(result == std::end(_symbols)) {
    throw FileSymbolsException{"Symbol does not exist"};
  }

  return {pos, *result};
}

int File::getPosition(SymbolId id) {
  int pos = 0;
  auto result = std::find_if(_symbols.begin(), _symbols.end(), [id, &pos](const Symbol &cmp) {
        bool val = cmp.getSymbolId() == id;
        if(!val) pos++;
        return val;
  });

  if(result == std::end(_symbols)) {
    throw FileSymbolsException{"Symbol does not exist"};
  }

  return pos;
}

int File::numSymbols() const {
  return _symbols.size();
}

std::string File::to_string() const {
  std::stringstream ss;

  ss << "User IDs: " << std::endl << clientsToString() << std::endl;
  ss << "Symbols:" << std::endl << symbolsToString();

  return ss.str();
}

std::string File::text() const {
  std::stringstream ss;

  for(auto &sym : _symbols) {
    ss << sym.getChar().toLatin1();
  }

  return ss.str();
}

void File::clear() {
  _symbols.clear();
}

void File::addClient(int userId, QString username) {
  if(_clients.count(userId) != 0) {
    throw FileClientException{"Client already exists"};
  }

  _clients[userId] = { userId, username };
}

void File::removeClient(int userId) {
  if(_clients.count(userId) == 0) {
    throw FileClientException{"Client does not exist"};
  }

  _clients.erase(userId);
}

void File::localInsert(Symbol &sym, int pos) {
  // (chi ha chiamato questo metodo poi si preoccuperà di mandare info al server)
  auto size = _symbols.size();

  if(pos < 0 || pos > size) {
    throw FileSymbolsException{"Invalid insert position"};
  }

  //create array position
  int previous = pos > 0 ? pos - 1 : -1;
  int next = pos == size ? -1 : pos;

  std::vector<Symbol::Identifier> v1, v2;

  if(previous != -1) {
      auto sym = _symbols.at(previous);
      v1 = sym.getPos();
  }
  if(next != -1) {
      auto sym = _symbols.at(next);
      v2 = sym.getPos();
  }

  std::vector<Symbol::Identifier> position;

  findPosition(sym.getSymbolId().getUserId(), v1, v2, position);

  sym.setPos(position);
  _symbols.emplace(_symbols.begin() + pos, sym);
}

void File::findPosition(int userId, std::vector<Symbol::Identifier> v1,
  std::vector<Symbol::Identifier> v2, std::vector<Symbol::Identifier> &position,
  int level) {

  Symbol::Identifier pos1, pos2;

  if(!v1.empty()) pos1 = v1.at(0);
  else pos1 = {0, userId};

  if(!v2.empty()) pos2 = v2.at(0);
  else pos2 = {static_cast<int>(pow(2, level) * 32), userId};

  int digit1 = pos1.getDigit();
  int digit2 = pos2.getDigit();

  if(digit2 - digit1 > 1){
    //finished, found the position
    int digit = (digit2 + digit1) / 2;
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

int File::remoteInsert(const Symbol &sym) {
  int pos = 0;
  auto result = std::find_if(_symbols.begin(), _symbols.end(), [sym, &pos](const Symbol &cmp) {
        bool val = sym < cmp;
        if(!val) pos++;
        return val;
    });

  _symbols.emplace(result, sym);

  return pos;
}

void File::localDelete(int pos) {
  if(pos < 0 || _symbols.size() <= pos) {
    throw FileSymbolsException{"Invalid delete position"};
  }

  _symbols.erase(_symbols.begin()+pos);
}

int File::remoteDelete(SymbolId id) {
  int pos = 0;
  auto result = std::find_if(_symbols.begin(), _symbols.end(), [id, &pos](const Symbol &cmp) {
        bool val = id == cmp.getSymbolId();
        if(!val) pos++;
        return val;
    });

  if(result == std::end(_symbols)) {
    // no eccezione qui, è un fatto che può accadere e non è un errore
    return -1;
  }

  _symbols.erase(result);
  return pos;
}

int File::remoteUpdate(const Symbol &sym) {
  try {
    auto pos = symbolById(sym.getSymbolId());
    auto &symbol = pos.second;

    symbol.update(sym);

    return pos.first;
  }
  catch(FileSymbolsException e) {
    //il simbolo non esiste più
    return -1;
  }
}

bool operator==(const File& lhs, const File& rhs) {
  return lhs._clients == rhs._clients && lhs._symbols == rhs._symbols;
}

bool operator!=(const File& lhs, const File& rhs) {
  return !operator==(lhs, rhs);
}

bool operator==(const File::ClientInfo& lhs, const File::ClientInfo& rhs) {
  return lhs.userId == rhs.userId && lhs.username == rhs.username;
}
