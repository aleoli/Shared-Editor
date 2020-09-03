#pragma once

#include <vector>
#include <list>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QDateTime>

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

  bool isDifferent(const Paragraph &other);
  bool isDifferent(Qt::Alignment alignment);

  void localUpdate(Qt::Alignment alignment);
  void remoteUpdate(const Paragraph &other, const QDateTime &timestamp, int userId);

  static Paragraph fromJsonObject(const QJsonObject &json);
  static Paragraph fromJsonObject(QJsonObject &&json);
  [[nodiscard]] QJsonObject toJsonObject() const;

  static std::list<Paragraph> jsonArrayToParagraphs(const QJsonArray &array);
  static QJsonArray paragraphsToJsonArray(const std::list<Paragraph> &symbols);

  void setParagraphId(const ParagraphId &id);
  [[nodiscard]] ParagraphId getParagraphId() const;
  void setPos(const std::vector<Identifier> &pos);
  [[nodiscard]] std::vector<Identifier> getPos() const;
  void setAlignment(Qt::Alignment alignment);
  [[nodiscard]] Qt::Alignment getAlignment() const;
  [[nodiscard]] QDateTime getTimestamp() const;
  void setTimestamp(const QDateTime &time);
  bool isOlder(const QDateTime &time, int userId);

private:
  void checkAndAssign(const QJsonObject &json);

  ParagraphId _id;
  std::vector<Identifier> _pos;
  Qt::Alignment _alignment;
  QDateTime _timestamp;
  int _lastUser;
};
