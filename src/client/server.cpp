#include "server.h"

#include <iostream>

#include <QDataStream>
#include <QtEndian>

std::shared_ptr<Server> Server::instance = nullptr;

Server::Server() {
  this->_socket = new QTcpSocket(this);
  QObject::connect(this->_socket, SIGNAL(readyRead()), this, SLOT(read()));
  QObject::connect(this->_socket, SIGNAL(connected()), this, SIGNAL(connected()));
  QObject::connect(this->_socket, SIGNAL(disconnected()), this, SIGNAL(disconnected()));
}

std::shared_ptr<Server> Server::get() {
  if(instance == nullptr) {
    instance.reset(new Server);
  }
  return instance;
}

void Server::connect(const QString &host, int port) {
  this->_socket->connectToHost(host, port);
}

void Server::abort() {
  this->_socket->abort();
}

void Server::disconnect() {
  this->_socket->disconnect();
  this->_socket->disconnectFromHost();
  this->_socket->deleteLater();
  emit this->disconnected();
}

void Server::write(const QByteArray &data) {
  QByteArray data2;
  quint32 size = data.size();
  QDataStream ds(&data2, QIODevice::WriteOnly);
  ds.setByteOrder(QDataStream::BigEndian);
  ds << size;
  data2 += data;

  if(this->_socket->state() == QAbstractSocket::ConnectedState) {
    this->_socket->write(data2);
    if(!this->_socket->waitForBytesWritten()) {
      emit this->connection_error();
      return;
    }
  }
}

void Server::read() {
  if(this->_in_size <= 0) {
    this->readSize();
  }
  if(this->_in_size > 0) {
    this->readData();
  }
}

void Server::readSize() {
  if(this->_socket->bytesAvailable() >= 4) {
    QByteArray data = this->_socket->read(4);
    QDataStream ds(data);
    ds.setByteOrder(QDataStream::BigEndian);
    ds >> this->_in_size;
  }
}

void Server::readData() {
  while(this->_socket->bytesAvailable() > 0) {
    auto read_n = qMin(this->_socket->bytesAvailable(), (qint64) (this->_in_size - this->_in_buffer.size()));
    if(read_n <= 0) {
      if(this->_socket->bytesAvailable() > 0) {
        this->read();
      }
      return;
    }
    this->_in_buffer.append(this->_socket->read(read_n));
    if(this->_in_buffer.size() >= this->_in_size) {
      this->_in_size = 0;
      emit this->dataReady(this->_in_buffer);
      this->_in_buffer.clear();
    }
  }
}
