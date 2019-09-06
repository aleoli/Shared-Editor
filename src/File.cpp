#include "File.h"

#include "utils.h"
#include "exceptions.h"

using namespace se_exceptions;

#include <sstream>

File::File() {}

File::File(int id, std::vector<int> user_ids, std::vector<Symbol> symbols)
  : _id(id), _user_ids(user_ids), _symbols(symbols) {}

File::File(QJsonObject &json){
  auto idValue = json["id"];
  auto userIdsValue = json["userIds"];
  auto symbolsValue = json["symbols"];

  checkAndAssign(idValue, userIdsValue, symbolsValue);
}

File::File(QJsonObject &&json){
  auto idValue = json["id"];
  auto userIdsValue = json["userIds"];
  auto symbolsValue = json["symbols"];

  checkAndAssign(idValue, userIdsValue, symbolsValue);
}

void File::checkAndAssign(QJsonValue idValue, QJsonValue userIdsValue, QJsonValue symbolsValue) {
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
  _user_ids = jsonArrayToUserIds(userIds);
  _symbols = jsonArrayToSymbols(symbols);
}

File File::fromJsonObject(QJsonObject &json) {
  return File(json);
}

QJsonObject File::toJsonObject() const {
  QJsonObject json;

  json["id"] = QJsonValue(_id);
  json["userIds"] = QJsonValue(userIdsToJsonArray());
  json["symbols"] = QJsonValue(symbolsToJsonArray());

  return json;
}

QJsonArray File::userIdsToJsonArray() const {
  QJsonArray array;

  for(int el : _user_ids) {
    array.append(el);
  }

  return array;
}

std::string File::userIdsToString() const {
  std::stringstream ss;

  for(int el : _user_ids) {
    ss << el << ' ';
  }

  return ss.str();
}

std::vector<int> File::jsonArrayToUserIds(QJsonArray &array) {
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

std::vector<Symbol> File::jsonArrayToSymbols(QJsonArray &array) {
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
  return _user_ids;
}

std::vector<Symbol> File::getSymbols() const {
  return _symbols;
}

std::string File::to_string() const {
  std::stringstream ss;

  ss << "ID: " << _id << std::endl;
  ss << "User IDs: " << userIdsToString() << std::endl;
  ss << "Symbols:" << std::endl << symbolsToString();

  return ss.str();
}
