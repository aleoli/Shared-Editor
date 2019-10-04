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
  class Identifier {
  public:
    Identifier();
    Identifier(int digit, int clientId);
    explicit Identifier(const QJsonObject &json);
    explicit Identifier(QJsonObject &&json);

    static Identifier fromJsonObject(const QJsonObject &json);
    static Identifier fromJsonObject(QJsonObject &&json);
    QJsonObject toJsonObject() const;

    friend bool operator==(const Identifier& lhs, const Identifier& rhs);
    friend bool operator<(const Identifier& lhs, const Identifier& rhs);

    int getDigit() const;
    int getClientId() const;
    std::string to_string() const;

  private:
    void checkAndAssign(const QJsonObject &json);

    int _digit, _clientId;
  };

  Symbol();
  Symbol(SymbolId id, QChar chr);
  Symbol(SymbolId id, QChar chr, QFont font, QColor color, QColor backgroundColor);
  explicit Symbol(const QJsonObject &json);
  explicit Symbol(QJsonObject &&json);

  friend bool operator<(const Symbol& lhs, const Symbol& rhs);

  static Symbol fromJsonObject(const QJsonObject &json);
  static Symbol fromJsonObject(QJsonObject &&json);
  QJsonObject toJsonObject() const;

  void setSymbolId(SymbolId id);
  SymbolId getSymbolId() const;
  void setChar(QChar chr);
  QChar getChar() const;
  void setPos(std::vector<Identifier> pos);
  std::vector<Identifier> getPos() const;
  std::string to_string() const;
  std::string getFontInfo() const;

  void setFont(QFont font);
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
  static std::vector<Identifier> jsonArrayToPos(const QJsonArray &array);

  SymbolId _id;
  QChar _char;
  std::vector<Identifier> _pos;
  QFont _font;
  QColor _color;
  QColor _backgroundColor = QColor("#00000000");
};
