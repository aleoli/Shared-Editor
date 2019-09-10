#pragma once

#include "SymbolId.h"

#include <string>
#include <QChar>
#include <vector>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QFont>
#include <QColor>

class Symbol {
public:
  Symbol();
  Symbol(QChar chr);
  Symbol(QChar chr, QFont font);
  //TODO non servono tutti di sicuro.
  Symbol(SymbolId id, QChar chr, std::vector<int> pos);
  Symbol(SymbolId id, QChar chr, std::vector<int> pos, QFont font);
  Symbol(SymbolId id, QChar chr, std::vector<int> pos, QFont font, QColor color);
  Symbol(SymbolId id, QChar chr, std::vector<int> pos, QFont font,
    QColor color, QColor backgroundColor);

  explicit Symbol(const QJsonObject &json);
  explicit Symbol(QJsonObject &&json);

  static Symbol fromJsonObject(const QJsonObject &json);
  static Symbol fromJsonObject(QJsonObject &&json);
  QJsonObject toJsonObject() const;

  SymbolId getSymbolId() const;
  QChar getChar() const;
  std::vector<int> getPos() const;
  std::string to_string() const;
  std::string getFontInfo() const;

  //TODO metodo per determinare posizione ?

  const QFont &getFont() const;
  void setBold(bool enable);
  bool isBold() const;
  void setSize(int size);
  int getSize() const;
  void setUnderline(bool enable);
  bool isUnderline() const;
  void setItalic(bool enable);
  bool isItalic() const;
  void setFamily(const QString &family);
  QString getFamily() const;
  void setColor(const QColor &color);
  QColor getColor() const;
  void setBackgroundColor(const QColor &color);
  QColor getBackgroundColor() const;

private:
  void checkAndAssign(const QJsonObject &json);

  QJsonArray posToJsonArray() const;
  std::string posToString() const; //TODO vedi se rimuovere
  static std::vector<int> jsonArrayToPos(const QJsonArray &array);

  SymbolId _id;
  QChar _char;
  std::vector<int> _pos;
  QFont _font;
  QColor _color;
  QColor _backgroundColor;
};
