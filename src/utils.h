#pragma once

#include <QString>
#include <vector>
#include <list>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonObject>

QString rndString(int randomStringLength = 16);

void error(const QString& str);
void warn(const QString& str);
void debug(const QString& str);
void info(const QString& str);

bool check_pass(const QString& pass);

// for debugging
QString jsonToString(const QJsonObject &json);

namespace utils {
  template<typename T>
  std::vector<T> jsonArrayToVector(const QJsonArray &json) {
    std::vector<T> vect;

    for(auto &&element : json) {
      vect.emplace_back(element.toObject());
    }

    return vect;
  }

  template<typename T>
  QJsonArray vectorToJsonArray(const std::vector<T> &vect) {
    QJsonArray json;

    for(auto &&element : vect) {
      json.push_back(QJsonValue(element.toJsonObject()));
    }

    return json;
  }

  template<typename T>
  std::list<T> jsonArrayToList(const QJsonArray &json) {
    std::list<T> vect;

    for(auto &&element : json) {
      vect.emplace_back(element.toObject());
    }

    return vect;
  }

  template<typename T>
  QJsonArray listToJsonArray(const std::list<T> &vect) {
    QJsonArray json;

    for(auto &&element : vect) {
      json.push_back(QJsonValue(element.toJsonObject()));
    }

    return json;
  }
}
