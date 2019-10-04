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

File::File(int id, std::vector<int> user_ids, std::vector<Symbol> symbols)
  : _id(id), _userIds(user_ids), _symbols(symbols) {}

File::File(const QJsonObject &json){
  checkAndAssign(json);
}

File::File(QJsonObject &&json){
  checkAndAssign(json);
}

void File::checkAndAssign(const QJsonObject &json) {
  auto idValue = json["id"];
  auto userIdsValue = json["userIds"];
  auto symbolsValue = json["symbols"];

  if(idValue.isUndefined() || userIdsValue.isUndefined() || symbolsValue.isUndefined()) {
    throw FileFromJsonException{"The QJsonObject has some fields missing"};
  }

  if(!userIdsValue.isArray() || !symbolsValue.isArray()) {
    throw FileFromJsonException{"One or more fields are not valid"};
  }

  auto id = idValue.toInt(-1);

  if(id < 0) {
    throw FileFromJsonException{"One or more fields are not valid"};
  }

  auto userIds = userIdsValue.toArray();
  auto symbols = symbolsValue.toArray();

  _id = id;
  _userIds = jsonArrayToUserIds(userIds);
  _symbols = jsonArrayToSymbols(symbols);
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
  json["userIds"] = QJsonValue(userIdsToJsonArray());
  json["symbols"] = QJsonValue(symbolsToJsonArray());

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

QJsonArray File::userIdsToJsonArray() const {
  QJsonArray array;

  for(int el : _userIds) {
    array.append(el);
  }

  return array;
}

std::string File::userIdsToString() const {
  std::stringstream ss;

  for(int el : _userIds) {
    ss << el << ' ';
  }

  return ss.str();
}

std::vector<int> File::jsonArrayToUserIds(const QJsonArray &array) {
  std::vector<int> userIds;

  for(auto&& el : array) {
    auto val = el.toInt(-1);

    if(val == -1) {
      throw FileFromJsonException{"One or more fields in user ids array are not valid"};
    }

    userIds.push_back(val);
  }

  return userIds;
}

QJsonArray File::symbolsToJsonArray() const {
  QJsonArray array;

  for(auto& el : _symbols) {
    array.append(el.toJsonObject());
  }

  return array;
}

std::string File::symbolsToString() const {
  std::stringstream ss;

  for(auto& el : _symbols) {
    ss << el.to_string() << std::endl;
  }

  return ss.str();
}

std::vector<Symbol> File::jsonArrayToSymbols(const QJsonArray &array) {
  std::vector<Symbol> symbols;

  for(auto&& el : array) {
    if(!el.isObject()) {
      throw FileFromJsonException{"One or more fields in symbols array are not valid"};
    }

    auto val = el.toObject();

    symbols.emplace_back(val);
  }

  return symbols;
}

int File::getId() const {
  return _id;
}

std::vector<int> File::getUserIds() const {
  return _userIds;
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

Symbol& File::symbolById(SymbolId id) {
  auto result = std::find_if(_symbols.begin(), _symbols.end(), [id](const Symbol &cmp) {
        return cmp.getSymbolId() == id;
  });

  if(result == std::end(_symbols)) {
    throw FileSymbolsException{"Symbol does not exist"};
  }

  return *result;
}

int File::numSymbols() const {
  return _symbols.size();
}

std::string File::to_string() const {
  std::stringstream ss;

  ss << "ID: " << _id << std::endl;
  ss << "User IDs: " << userIdsToString() << std::endl;
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

void File::addUserId(int id) {
  _userIds.push_back(id);
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

void File::remoteInsert(const Symbol &sym) {
  auto result = std::find_if(_symbols.begin(), _symbols.end(), [sym](const Symbol &cmp) {
        return sym < cmp;
    });

  _symbols.emplace(result, sym);
}

void File::localDelete(int pos) {
  if(pos < 0 || _symbols.size() <= pos) {
    throw FileSymbolsException{"Invalid delete position"};
  }

  _symbols.erase(_symbols.begin()+pos);
}

void File::remoteDelete(SymbolId id) {
  auto result = std::find_if(_symbols.begin(), _symbols.end(), [id](const Symbol &cmp) {
        return id == cmp.getSymbolId();
    });

  _symbols.erase(result);
}
