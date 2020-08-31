#pragma once

#include <string>
#include <QJsonObject>

class SymbolId {
public:
  SymbolId();
  SymbolId(int userId, int charId);
  explicit SymbolId(const QJsonObject &json);
  explicit SymbolId(QJsonObject &&json);
  SymbolId(const SymbolId&) = default;
  SymbolId(SymbolId&&) = default;

  SymbolId& operator=(const SymbolId&) = default;
  SymbolId& operator=(SymbolId&&) = default;

  friend bool operator<(const SymbolId& lhs, const SymbolId& rhs);
  friend bool operator==(const SymbolId& lhs, const SymbolId& rhs);
  friend bool operator!=(const SymbolId& lhs, const SymbolId& rhs);

  static SymbolId fromJsonObject(const QJsonObject &json);
  static SymbolId fromJsonObject(QJsonObject &&json);
  [[nodiscard]] QJsonObject toJsonObject() const;

  [[nodiscard]] int getUserId() const;
  [[nodiscard]] int getCharId() const;
  [[nodiscard]] std::string to_string() const;

private:
  void checkAndAssign(const QJsonObject &json);

  int _userId{}, _charId{};
};
