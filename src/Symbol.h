#pragma once

#include <string>
#include <QChar>
#include <vector>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QTextCharFormat>

#include "Identifier.h"
typedef Identifier SymbolId;

class Symbol {
public:
  Symbol();
  Symbol(SymbolId id, QChar chr);
  Symbol(SymbolId id, QChar chr, QTextCharFormat fmt);
  explicit Symbol(const QJsonObject &json, QFont *font = nullptr, QBrush *col = nullptr, QBrush *bac = nullptr);
  explicit Symbol(QJsonObject &&json, QFont *font = nullptr, QBrush *col = nullptr, QBrush *bac = nullptr);
  Symbol(const Symbol& s);
  Symbol(Symbol&& s) noexcept;

  Symbol& operator=(const Symbol& s);
  Symbol& operator=(Symbol&& s) noexcept;

  friend bool operator<(const Symbol& lhs, const Symbol& rhs);
  friend bool operator==(const Symbol& lhs, const Symbol& rhs);
  friend bool operator!=(const Symbol& lhs, const Symbol& rhs);

  static Symbol fromJsonObject(const QJsonObject &json, QFont *font = nullptr, QBrush *col = nullptr, QBrush *bac = nullptr);
  static Symbol fromJsonObject(QJsonObject &&json, QFont *font = nullptr, QBrush *col = nullptr, QBrush *bac = nullptr);
  [[nodiscard]] QJsonObject toJsonObject(QFont *font = nullptr, QBrush *col = nullptr, QBrush *bac = nullptr) const;

  static std::list<Symbol> jsonArrayToSymbols(const QJsonArray &array);
  static QJsonArray symbolsToJsonArray(const std::list<Symbol> &symbols);

  static QJsonObject serializeFormat(const QTextCharFormat &format, QFont *font = nullptr, QBrush *col = nullptr, QBrush *bac = nullptr);
  static QTextCharFormat deserializeFormat(const QJsonObject &json, QFont *font = nullptr, QBrush *col = nullptr, QBrush *bac = nullptr);

  void setSymbolId(const SymbolId &id);
  [[nodiscard]] SymbolId getSymbolId() const;
  void setChar(const QChar &chr);
  [[nodiscard]] QChar getChar() const;
  void setPos(const std::vector<Identifier> &pos);
  [[nodiscard]] std::vector<Identifier> getPos() const;
  [[nodiscard]] std::string to_string() const;
  [[nodiscard]] std::string getFontInfo() const;

  void update(const Symbol &s);

  static bool compareFormats(const QTextCharFormat &fmt1, const QTextCharFormat &fmt2, bool ignoreBackground = false);
  [[nodiscard]] bool hasSameAttributes(const QChar &chr, const QTextCharFormat &fmt, bool ignoreBackground = false) const;

  void setFormat(const QTextCharFormat &fmt);
  [[nodiscard]] QTextCharFormat getFormat() const;
  void setFont(const QFont& font);
  [[nodiscard]] QFont getFont() const;
  void setBold(bool enable);
  [[nodiscard]] bool isBold() const;
  void setSize(int size);
  [[nodiscard]] int getSize() const;
  void setUnderline(bool enable);
  [[nodiscard]] bool isUnderline() const;
  void setItalic(bool enable);
  [[nodiscard]] bool isItalic() const;
  void setFamily(const QString &family);
  [[nodiscard]] QString getFamily() const;
  void setColor(const QColor &color);
  [[nodiscard]] QColor getColor() const;
  void setBackgroundColor(const QColor &color);
  [[nodiscard]] QColor getBackgroundColor() const;

private:
  void checkAndAssign(const QJsonObject &json, QFont *font = nullptr, QBrush *col = nullptr, QBrush *bac = nullptr);
  [[nodiscard]] std::string posToString() const; //TODO vedi se rimuovere

  static QColor fixColor(const QBrush &brush, bool isBackground);
  static QBrush stringToBrush(const QString &string);
  static QString brushToString(const QBrush &b, bool isBackground);
  static QFont jsonArrayToFont(const QJsonArray &array);
  static QJsonArray fontToJsonArray(const QFont &b);
  static QFont stringToFont(const QString &string);
  static QString fontToString(const QFont &b);

  SymbolId _id;
  QChar _char;
  std::vector<Identifier> _pos;
  QTextCharFormat _fmt;
};
