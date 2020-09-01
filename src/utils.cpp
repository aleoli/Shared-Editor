#include "utils.h"
#include "sets.h"

#include <iostream>
#include <QJsonDocument>
#include <QJsonObject>

#define RST  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

#define FRED(x) KRED x RST
#define FGRN(x) KGRN x RST
#define FYEL(x) KYEL x RST
#define FBLU(x) KBLU x RST
#define FMAG(x) KMAG x RST
#define FCYN(x) KCYN x RST
#define FWHT(x) KWHT x RST

#define BOLD(x) "\x1B[1m" x RST
#define UNDL(x) "\x1B[4m" x RST

QString rndString(int randomStringLength) {
  const QString possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");

  QString randomString;
  for(int i=0; i<randomStringLength; ++i) {
    int index = qrand() % possibleCharacters.length();
    QChar nextChar = possibleCharacters.at(index);
    randomString.append(nextChar);
  }
  return randomString;
}

void error(const QString& str) {
  if(Sets::get()->getLogLevel() >= Sets::LogLevel::ERROR) {
#if defined(__APPLE__) || defined(__linux__)
    std::cerr << BOLD(FRED("[error] ")) << str.toStdString() << std::endl;
#else
    std::cerr << "[error] " << str.toStdString() << std::endl;
#endif
  }
}

void warn(const QString& str) {
  if(Sets::get()->getLogLevel() >= Sets::LogLevel::WARN) {
#if defined(__APPLE__) || defined(__linux__)
    std::cerr << BOLD(FYEL("[warn] ")) << str.toStdString() << std::endl;
#else
    std::cerr << "[warn] " << str.toStdString() << std::endl;
#endif
  }
}

void debug(const QString& str) {
  if(Sets::get()->getLogLevel() >= Sets::LogLevel::DEBUG) {
#if defined(__APPLE__) || defined(__linux__)
    std::cout << BOLD(FBLU("[debug] ")) << str.toStdString() << std::endl;
#else
    std::cout << "[debug] " << str.toStdString() << std::endl;
#endif
  }
}

void info(const QString& str) {
  if(Sets::get()->getLogLevel() >= Sets::LogLevel::INFO) {
#if defined(__APPLE__) || defined(__linux__)
    std::cout << BOLD(FGRN("[info] ")) << str.toStdString() << std::endl;
#else
    std::cout << "[info] " << str.toStdString() << std::endl;
#endif
  }
}

bool check_pass(const QString& pass) {
  // NOTE: the password has to have at least an Uppercase char, a Lowercase char, a number, and length are to be equal or more than 6
  if(pass.length() < 6) {
    return false;
  }
  bool upper = false, lower = false, number = false;
  for(const auto& ch: pass) {
    if(ch.isUpper()) {
      upper = true;
    }
    if(ch.isLower()) {
      lower = true;
    }
    if(ch.isNumber()) {
      number = true;
    }
    if(upper && lower && number) {
      return true;
    }
  }
  return false;
}

QString jsonToString(const QJsonObject &json) {
  QJsonDocument doc(json);
  return doc.toJson();
}
