#pragma once

#include "Symbol.h"

#include <vector>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QString>
#include <unordered_map>
#include "utils.h"

class File {
public:
  typedef struct {
    int clientId;
    QString username;
  } ClientInfo;

  File();
  explicit File(int id);
  File(int id, std::unordered_map<int, File::ClientInfo> clients, std::vector<Symbol> _symbols);
  explicit File(const QJsonObject &json);
  explicit File(QJsonObject &&json);

  static File fromJsonObject(const QJsonObject &json);
  static File fromJsonObject(QJsonObject &&json);
  static File fromQByteArray(const QByteArray &array);
  static File fromQByteArray(QByteArray &&array);
  QJsonObject toJsonObject() const;
  QByteArray toQByteArray() const;

  friend bool operator==(const File& lhs, const File& rhs);
  friend bool operator!=(const File& lhs, const File& rhs);
  friend bool operator==(const File::ClientInfo& lhs, const File::ClientInfo& rhs);

  int getId() const;
  std::unordered_map<int, File::ClientInfo> getClients() const;
  std::vector<Symbol> getSymbols() const;
  Symbol& symbolAt(int pos);
  std::pair<int, Symbol&> symbolById(SymbolId id);
  int getPosition(SymbolId id);
  int numSymbols() const;
  std::string to_string() const;
  std::string text() const;

  void addClient(int clientId, QString username);
  void removeClient(int clientId);

  //CRDT
  void localInsert(Symbol &sym, int pos);
  int remoteInsert(const Symbol &sym); // returns the position in which i inserted
  void localDelete(int pos);
  int remoteDelete(SymbolId id); // returns the position of the deleted element
  void remoteUpdate(const Symbol &sym);

private:
  void checkAndAssign(const QJsonObject &json);

  static void findPosition(int clientId, std::vector<Symbol::Identifier> v1,
    std::vector<Symbol::Identifier> v2, std::vector<Symbol::Identifier> &position,
    int level = 0);

  QJsonArray clientsToJsonArray() const;
  std::string clientsToString() const; //TODO vedi se rimuovere
  static std::unordered_map<int, File::ClientInfo> jsonArrayToclients(const QJsonArray &array);

  std::string symbolsToString() const; //TODO vedi se rimuovere

  int _id;
  std::unordered_map<int, ClientInfo> _clients;
  std::vector<Symbol> _symbols;
};
