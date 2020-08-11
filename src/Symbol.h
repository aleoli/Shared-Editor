#pragma once

#include "SymbolId.h"

#include <string>
#include <QChar>
#include <vector>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QTextCharFormat>

class Symbol {
public:
  class Identifier {
  public:
    Identifier();
    Identifier(int digit, int userId);
    explicit Identifier(const QJsonObject &json);
    explicit Identifier(QJsonObject &&json);
    Identifier(const Identifier&) = default;
    Identifier(Identifier &&) = default;

    Identifier& operator=(const Identifier& identifier) = default;
    Identifier& operator=(Identifier&& identifier) noexcept = default;

    static Identifier fromJsonObject(const QJsonObject &json);
    static Identifier fromJsonObject(QJsonObject &&json);
    [[nodiscard]] QJsonObject toJsonObject() const;

    friend bool operator==(const Identifier& lhs, const Identifier& rhs);
    friend bool operator<(const Identifier& lhs, const Identifier& rhs);

    [[nodiscard]] int getDigit() const;
    [[nodiscard]] int getUserId() const;
    [[nodiscard]] std::string to_string() const;

  private:
    void checkAndAssign(const QJsonObject &json);

    int _digit{}, _userId{};
  };

  Symbol();
  Symbol(SymbolId id, QChar chr);
  Symbol(SymbolId id, QChar chr, QTextCharFormat fmt);
  explicit Symbol(const QJsonObject &json, bool readPos = true);
  explicit Symbol(QJsonObject &&json, bool readPos = true);
  Symbol(const Symbol& s);
  Symbol(Symbol&& s) noexcept;

  Symbol& operator=(const Symbol& s);
  Symbol& operator=(Symbol&& s) noexcept;

  friend bool operator<(const Symbol& lhs, const Symbol& rhs);
  friend bool operator==(const Symbol& lhs, const Symbol& rhs);
  friend bool operator!=(const Symbol& lhs, const Symbol& rhs);

  static Symbol fromJsonObject(const QJsonObject &json, bool readPos = true);
  static Symbol fromJsonObject(QJsonObject &&json, bool readPos = true);
  [[nodiscard]] QJsonObject toJsonObject(bool writePos = true) const;

  static QJsonObject serializeFormat(const QTextCharFormat &fmt);
  static QTextCharFormat deserializeFormat(const QJsonObject &json);

  void setSymbolId(SymbolId id);
  [[nodiscard]] SymbolId getSymbolId() const;
  void setChar(QChar chr);
  [[nodiscard]] QChar getChar() const;
  void setPos(std::vector<Identifier> pos);
  [[nodiscard]] std::vector<Identifier> getPos() const;
  [[nodiscard]] std::string to_string() const;
  [[nodiscard]] std::string getFontInfo() const;

  void update(const Symbol &s);

  static bool compareFormats(const QTextCharFormat &fmt1, const QTextCharFormat &fmt2, bool ignoreBackground = false);
  [[nodiscard]] bool hasSameAttributes(const QChar &chr, const QTextCharFormat &fmt, bool ignoreBackground = false) const;

  void setFormat(QTextCharFormat fmt);
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
  void checkAndAssign(const QJsonObject &json, bool readPos = true);
  [[nodiscard]] std::string posToString() const; //TODO vedi se rimuovere

  static QBrush jsonArrayToBrush(const QJsonArray &array);
  static QJsonArray brushToJsonArray(const QBrush &b);
  static QFont jsonArrayToFont(const QJsonArray &array);
  static QJsonArray fontToJsonArray(const QFont &b);

  SymbolId _id;
  QChar _char;
  std::vector<Identifier> _pos;
  QTextCharFormat _fmt;
};
