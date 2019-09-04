#pragma once

#include "SymbolId.h"

#include <string>
#include <QChar>
#include <vector>
#include <QJsonObject>
#include <QJsonArray>

class Symbol {
  //TODO questione dei metadati (grassetto, corsivo...)
public:
  Symbol();
  Symbol(SymbolId id, QChar chr, std::vector<int> pos);
  explicit Symbol(QJsonObject &json);
  //TODO altri costruttori..

  static Symbol fromJsonObject(QJsonObject &json);
  QJsonObject toJsonObject();
  QJsonArray posToJsonArray();
  static std::vector<int> jsonArrayToPos(QJsonArray &array);

  SymbolId getSymbolId() const;
  QChar getChar() const;
  std::vector<int> getPos() const;

  //TODO altri metodi per settare i parametri

private:
  SymbolId _id;
  QChar _char;
  std::vector<int> _pos;
};
