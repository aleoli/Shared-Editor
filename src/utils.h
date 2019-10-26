#pragma once

#include <QString>
#include <vector>
#include <QJsonArray>
#include <QJsonValue>

QString rndString(int randomStringLength = 16);

void error(QString str);
void warn(QString str);
void debug(QString str);
void info(QString str);

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
