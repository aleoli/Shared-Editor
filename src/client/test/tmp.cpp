#include "tmp.h"

#include <iostream>

#include <QTimer>
#include <QJsonDocument>

#include "def.h"
#include "Message.h"
#include "../message_manager.h"

Tmp::Tmp(const SysConf& conf): QObject(nullptr) {
  this->_server = Server::get(conf.host, conf.port);
  this->_thread = new QThread{this};
  this->_server->moveToThread(this->_thread);
  auto mm = MessageManager::get();
  QObject::connect(this->_thread, SIGNAL(started()), this->_server.get(), SLOT(connect()));
  QObject::connect(this->_thread, SIGNAL(started()), this, SLOT(started()));
  QObject::connect(this, SIGNAL(writeData(QByteArray)), this->_server.get(), SLOT(write(QByteArray)));
  QObject::connect(this->_server.get(), SIGNAL(disconnected()), this, SLOT(disconnected()));
  QObject::connect(this->_server.get(), SIGNAL(dataReady(QByteArray)), mm.get(), SLOT(process_data(QByteArray)));
  QObject::connect(mm.get(), SIGNAL(connection_error()), this->_server.get(), SLOT(disconnect()));
  QObject::connect(mm.get(), SIGNAL(send_data(QByteArray)), this->_server.get(), SLOT(write(QByteArray)));
  this->_thread->start();
}

void Tmp::started() {
  QTimer::singleShot(1000, this, SLOT(write()));
  QTimer::singleShot(2000, this->_server.get(), SLOT(disconnect()));
}

void Tmp::write() {
  Message msg{};
  QJsonDocument res_doc(msg.toJsonObject());
#if SAVE_BINARY
  auto array = res_doc.toBinaryData();
#else
  auto array = res_doc.toJson(QJsonDocument::Compact);
#endif
  emit this->writeData(array);
}

void Tmp::disconnected() {
  this->_thread->quit();
  this->_thread->wait();
  emit this->quit();
}
