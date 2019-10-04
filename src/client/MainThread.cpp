#include "MainThread.h"

#include "exceptions.h"

using namespace se_exceptions;

MainThread::MainThread(QApplication &app) : _app(app) {}

void MainThread::start() {
  exit(_app.exec());
}

File& MainThread::getFile() {
  if(_file == nullptr) {
    throw FileNotReceivedException{"No file is currently loaded."};
  }

  return *_file;
}

void MainThread::closeFile() {
  _file.reset();
}

void MainThread::processRemoteMessage(const Message &m) {
  
}

void MainThread::setFile(const QJsonObject &file) {
  _file = std::make_shared<File>(file);
}
