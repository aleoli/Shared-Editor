#pragma once

#include "SymbolId.h"

#include <string>
#include <QChar>
#include <vector>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

class Symbol {
  //TODO questione dei metadati (grassetto, corsivo...)
public:
  Symbol();
  Symbol(SymbolId id, QChar chr, std::vector<int> pos);
  explicit Symbol(const QJsonObject &json);
  explicit Symbol(const QJsonObject &&json);
  //TODO altri costruttori..

  static Symbol fromJsonObject(const QJsonObject &json);
  static Symbol fromJsonObject(const QJsonObject &&json);
  QJsonObject toJsonObject() const;

  SymbolId getSymbolId() const;
  QChar getChar() const;
  std::vector<int> getPos() const;
  std::string to_string() const;

  //TODO altri metodi per settare i parametri
  //TODO booleani per i metadati esempio isBold() e via dicendo
  //TODO metodo per determinare posizione ?

private:
  void checkAndAssign(const QJsonValue &idValue, const QJsonValue &charValue, const QJsonValue &posValue);

  QJsonArray posToJsonArray() const;
  std::string posToString() const; //TODO vedi se rimuovere
  static std::vector<int> jsonArrayToPos(const QJsonArray &array);

  SymbolId _id;
  QChar _char;
  std::vector<int> _pos;
};
