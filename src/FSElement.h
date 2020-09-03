#pragma once

#include <QString>
#include <QJsonObject>
#include <QDateTime>

typedef struct SearchResult {
  int id;
  QString path;
  bool isDir;
} SearchResult;

class FSElement {
public:
  enum class Type {
    FILE,
    DIRECTORY
  };

  typedef struct FileInfo {
    QString name;
    QString path;
    QString owner;
    QDateTime created;
  } FileInfo;

  FSElement();
  FSElement(int id, int parentId, QString name, Type type, QDateTime creationDate);
  explicit FSElement(const QJsonObject &json);
  explicit FSElement(QJsonObject &&json);

  static FSElement fromJsonObject(const QJsonObject &json);
  static FSElement fromJsonObject(QJsonObject &&json);
  [[nodiscard]] QJsonObject toJsonObject() const;

  friend bool operator==(const FSElement& lhs, const FSElement& rhs);
  friend bool operator!=(const FSElement& lhs, const FSElement& rhs);

  [[nodiscard]] int getId() const;
  [[nodiscard]] int getParentId() const;
  void setParentId(int parentId);
  [[nodiscard]] QString getName() const;
  void setName(const QString &name);
  [[nodiscard]] Type getType() const;
  [[nodiscard]] QDateTime getCreationDate() const;
  [[nodiscard]] std::string to_string() const;

private:
  void checkAndAssign(const QJsonObject &json);

  int _id{};
  int _parentId{};
  QString _name;
  Type _type;
  QDateTime _creationDate{};
};
