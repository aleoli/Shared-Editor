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

File::File(int id) : _id(id) {}

File::File(int id, std::unordered_map<int, File::ClientInfo> clients, std::vector<Symbol> symbols)
  : _id(id), _clients(clients), _symbols(symbols) {}

File::File(const QJsonObject &json){
  checkAndAssign(json);
}

File::File(QJsonObject &&json){
  checkAndAssign(json);
}

void File::checkAndAssign(const QJsonObject &json) {
  auto idValue = json["id"];
  auto clientsValue = json["clients"];
  auto symbolsValue = json["symbols"];

  if(idValue.isUndefined() || clientsValue.isUndefined() || symbolsValue.isUndefined()) {
    throw FileFromJsonException{"The QJsonObject has some fields missing"};
  }

  if(!clientsValue.isArray() || !symbolsValue.isArray()) {
    throw FileFromJsonException{"One or more fields are not valid"};
  }

  auto id = idValue.toInt(-1);

  if(id < 0) {
    throw FileFromJsonException{"One or more fields are not valid"};
  }

  auto clients = clientsValue.toArray();
  auto symbols = symbolsValue.toArray();

  _id = id;
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

  json["id"] = QJsonValue(_id);
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

    value["clientId"] = el.second.clientId;
    value["username"] = el.second.username;

    array.append(value);
  }

  return array;
}

std::string File::clientsToString() const {
  std::stringstream ss;

  for(auto &el : _clients) {
    ss << "\tclientId: " << el.second.clientId << std::endl;
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
    auto clientIdValue = obj["clientId"];
    auto usernameValue = obj["username"];

    if(clientIdValue.isUndefined() || usernameValue.isUndefined()) {
        throw FileFromJsonException{"The QJsonObject has some fields missing"};
      }

    auto clientId = clientIdValue.toInt(-1);

    if(clientId == -1) {
      throw FileFromJsonException{"One or more fields in clients array are not valid"};
    }

    if(!usernameValue.isString()) {
      throw FileFromJsonException{"One or more fields in clients array are not valid"};
    }

    File::ClientInfo info { clientId, usernameValue.toString() };

    clients[clientId] = info;
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

int File::getId() const {
  return _id;
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
  auto result = std::find_if(_symbols.begin(), _symbols.end(), [id](const Symbol &cmp) {
        return cmp.getSymbolId() == id;
  });

  if(result == std::end(_symbols)) {
    throw FileSymbolsException{"Symbol does not exist"};
  }

  return {result - _symbols.begin(), *result};
}

int File::getPosition(SymbolId id) {
  auto result = std::find_if(_symbols.begin(), _symbols.end(), [id](const Symbol &cmp) {
        return cmp.getSymbolId() == id;
  });

  if(result == std::end(_symbols)) {
    throw FileSymbolsException{"Symbol does not exist"};
  }

  return result - _symbols.begin();
}

int File::numSymbols() const {
  return _symbols.size();
}

std::string File::to_string() const {
  std::stringstream ss;

  ss << "ID: " << _id << std::endl;
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

void File::addClient(int clientId, QString username) {
  if(_clients.count(clientId) != 0) {
    throw FileClientException{"Client already exists"};
  }

  _clients[clientId] = { clientId, username };
}

void File::removeClient(int clientId) {
  if(_clients.count(clientId) == 0) {
    throw FileClientException{"Client does not exist"};
  }

  _clients.erase(clientId);
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

  findPosition(sym.getSymbolId().getClientId(), v1, v2, position);

  sym.setPos(position);
  _symbols.emplace(_symbols.begin() + pos, sym);
}

void File::findPosition(int clientId, std::vector<Symbol::Identifier> v1,
  std::vector<Symbol::Identifier> v2, std::vector<Symbol::Identifier> &position,
  int level) {

  Symbol::Identifier pos1, pos2;

  if(!v1.empty()) pos1 = v1.at(0);
  else pos1 = {0, clientId};

  if(!v2.empty()) pos2 = v2.at(0);
  else pos2 = {static_cast<int>(pow(2, level) * 32), clientId};

  int digit1 = pos1.getDigit();
  int digit2 = pos2.getDigit();

  if(digit2 - digit1 > 1){
    //finished, found the position
    int digit = (digit2 + digit1) / 2;
    position.emplace_back(digit, clientId);
    return;
  }

  else if(digit2 - digit1 == 1){
    //must go deeper
    position.push_back(pos1);

    if(!v1.empty()) v1.erase(v1.begin());
    v2.clear();

    findPosition(clientId, v1, v2, position, level + 1);
  }

  else if(digit2 == digit1) {
    //must go deeper
    int clientId1 = pos1.getClientId();
    int clientId2 = pos2.getClientId();

    if (clientId1 < clientId2) {
      position.push_back(pos1);

      if (!v1.empty()) v1.erase(v1.begin());
      v2.clear();

      findPosition(clientId, v1, v2, position, level + 1);
    }
    else if (clientId1 == clientId2) {
      position.push_back(pos1);

      if (!v1.empty()) v1.erase(v1.begin());
      if (!v2.empty()) v2.erase(v2.begin());

      findPosition(clientId, v1, v2, position, level + 1);
    }
    else {
      throw FileLocalInsertException{"vector _pos is not sorted!"};
    }
  }
}

int File::remoteInsert(const Symbol &sym) {
  auto result = std::find_if(_symbols.begin(), _symbols.end(), [sym](const Symbol &cmp) {
        return sym < cmp;
    });

  _symbols.emplace(result, sym);

  return result - _symbols.begin();
}

void File::localDelete(int pos) {
  if(pos < 0 || _symbols.size() <= pos) {
    throw FileSymbolsException{"Invalid delete position"};
  }

  _symbols.erase(_symbols.begin()+pos);
}

int File::remoteDelete(SymbolId id) {
  auto result = std::find_if(_symbols.begin(), _symbols.end(), [id](const Symbol &cmp) {
        return id == cmp.getSymbolId();
    });

  if(result == std::end(_symbols)) {
    // no eccezione qui, è un fatto che può accadere e non è un errore
    return -1;
  }

  _symbols.erase(result);
  return result - _symbols.begin();
}

void File::remoteUpdate(const Symbol &sym) {
  auto &symbol = symbolById(sym.getSymbolId()).second;

  symbol.setChar(sym.getChar());
  symbol.setFormat(sym.getFormat());
}

bool operator==(const File& lhs, const File& rhs) {
  return lhs._id == rhs._id && lhs._clients == rhs._clients && lhs._symbols == rhs._symbols;
}

bool operator!=(const File& lhs, const File& rhs) {
  return !operator==(lhs, rhs);
}

bool operator==(const File::ClientInfo& lhs, const File::ClientInfo& rhs) {
  return lhs.clientId == rhs.clientId && lhs.username == rhs.username;
}
