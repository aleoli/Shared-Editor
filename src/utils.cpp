#include "utils.h"
#include "sets.h"

#include <iostream>

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

void error(QString str) {
  if(Sets::get()->getLogLevel() >= Sets::LogLevel::ERROR) {
    std::cout << str.toStdString() << std::endl;
  }
}

void warn(QString str) {
  if(Sets::get()->getLogLevel() >= Sets::LogLevel::WARN) {
    std::cout << str.toStdString() << std::endl;
  }
}

void debug(QString str) {
  if(Sets::get()->getLogLevel() >= Sets::LogLevel::DEBUG) {
    std::cout << str.toStdString() << std::endl;
  }
}

void info(QString str) {
  if(Sets::get()->getLogLevel() >= Sets::LogLevel::INFO) {
    std::cout << str.toStdString() << std::endl;
  }
}
