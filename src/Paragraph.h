#pragma once

#include <vector>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

#include "Identifier.h"
typedef Identifier ParagraphId;

class Paragraph {
public:
  Paragraph();
  Paragraph(ParagraphId id);
  Paragraph(ParagraphId id, Qt::Alignment alignment);
  explicit Paragraph(const QJsonObject &json);
  explicit Paragraph(QJsonObject &&json);
  Paragraph(const Paragraph& s);
  Paragraph(Paragraph&& s) noexcept;

  Paragraph& operator=(const Paragraph& s);
  Paragraph& operator=(Paragraph&& s) noexcept;

  friend bool operator<(const Paragraph& lhs, const Paragraph& rhs);
  friend bool operator==(const Paragraph& lhs, const Paragraph& rhs);
  friend bool operator!=(const Paragraph& lhs, const Paragraph& rhs);

  static Paragraph fromJsonObject(const QJsonObject &json);
  static Paragraph fromJsonObject(QJsonObject &&json);
  [[nodiscard]] QJsonObject toJsonObject() const;

  void setParagraphId(const ParagraphId &id);
  [[nodiscard]] ParagraphId getParagraphId() const;
  void setPos(const std::vector<Identifier> &pos);
  [[nodiscard]] std::vector<Identifier> getPos() const;
  void setAlignment(Qt::Alignment alignment);
  [[nodiscard]] Qt::Alignment getAlignment() const;

private:
  void checkAndAssign(const QJsonObject &json);

  ParagraphId _id;
  std::vector<Identifier> _pos;
  Qt::Alignment _alignment;
};
