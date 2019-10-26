#pragma once

#include <QString>
#include <QJsonObject>

class FSElement {
public:
  enum class Type {
    FILE,
    DIRECTORY
  };

  FSElement();
  FSElement(int id, int parentId, QString name, Type type);
  explicit FSElement(const QJsonObject &json);
  explicit FSElement(QJsonObject &&json);

  static FSElement fromJsonObject(const QJsonObject &json);
  static FSElement fromJsonObject(QJsonObject &&json);
  QJsonObject toJsonObject() const;

  friend bool operator==(const FSElement& lhs, const FSElement& rhs);
  friend bool operator!=(const FSElement& lhs, const FSElement& rhs);

  int getId() const;
  int getParentId() const;
  void setParentId(int parentId);
  QString getName() const;
  void setName(const QString &name);
  Type getType() const;
  std::string to_string() const;

private:
  void checkAndAssign(const QJsonObject &json);

  int _id;
  int _parentId;
  QString _name;
  Type _type;
};