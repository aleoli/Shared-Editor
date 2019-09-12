#include "tmp.h"

#include <QTimer>

Tmp::Tmp(): QObject(nullptr) {
  this->_server = Server::get();
  this->_thread = new QThread{this};
  this->_server->moveToThread(this->_thread);
  QObject::connect(this->_thread, SIGNAL(started()), this->_server.get(), SLOT(connect()));
  QObject::connect(this->_thread, SIGNAL(started()), this, SLOT(started()));
  QObject::connect(this, SIGNAL(writeData(QByteArray)), this->_server.get(), SLOT(write(QByteArray)));
  this->_thread->start();
}

void Tmp::started() {
  QTimer::singleShot(1000, this, SLOT(write()));
}

void Tmp::write() {
  emit this->writeData("Ciao!! Come stai?? Io bene :)");
}
