#include "tmp.h"

#include <QTimer>

Tmp::Tmp(const SysConf& conf): QObject(nullptr) {
  this->_server = Server::get(conf.host, conf.port);
  this->_thread = new QThread{this};
  this->_server->moveToThread(this->_thread);
  QObject::connect(this->_thread, SIGNAL(started()), this->_server.get(), SLOT(connect()));
  QObject::connect(this->_thread, SIGNAL(started()), this, SLOT(started()));
  QObject::connect(this, SIGNAL(writeData(QByteArray)), this->_server.get(), SLOT(write(QByteArray)));
  QObject::connect(this->_server.get(), SIGNAL(disconnected()), this, SLOT(disconnected()));
  this->_thread->start();
}

void Tmp::started() {
  QTimer::singleShot(1000, this, SLOT(write()));
  QTimer::singleShot(2000, this->_server.get(), SLOT(disconnect()));
}

void Tmp::write() {
  emit this->writeData("Ciao!! Come stai?? Io bene :)");
}

void Tmp::disconnected() {
  this->_thread->quit();
  this->_thread->wait();
  emit this->quit();
}
