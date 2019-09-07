#include "client.h"

#include <QDataStream>

#include <arpa/inet.h>

#include "session.h"

Client::Client(QTcpSocket *s, int id): QObject(nullptr) {
  this->id = id;
  this->socket = s;
  connect(this->socket, SIGNAL(readyRead()), SLOT(read()));
  connect(this->socket, SIGNAL(disconnected()), SLOT(_disconnected()));
}

Client::Client(Client &&c) {
  this->id = c.id;
  this->_session = c._session;
  c._session = nullptr;
  if(this->socket != nullptr) {
    QObject::disconnect(this->socket, SIGNAL(readyRead()), this, SLOT(read()));
    QObject::disconnect(this->socket, SIGNAL(disconnected()), this, SLOT(_disconnected()));
    this->socket->deleteLater();
    delete this->socket;
  }
  QObject::disconnect(c.socket, SIGNAL(readyRead()), &c, SLOT(read()));
  QObject::disconnect(c.socket, SIGNAL(disconnected()), &c, SLOT(_disconnected()));
  this->socket = c.socket;
  connect(this->socket, SIGNAL(readyRead()), SLOT(read()));
  connect(this->socket, SIGNAL(disconnected()), SLOT(_disconnected()));
  c.socket = nullptr;
}

Client::~Client() {
  if(this->_session != nullptr) {
    delete this->_session;
    this->_session = nullptr;
  }
  if(this->socket != nullptr) {
    QObject::disconnect(this->socket, SIGNAL(readyRead()), this, SLOT(read()));
    QObject::disconnect(this->socket, SIGNAL(disconnected()), this, SLOT(_disconnected()));
    this->socket->deleteLater();
    this->socket = nullptr;
  }
}

void Client::read() {
  if(this->in_size <= 0) {
    this->readSize();
  }
  if(this->in_size > 0) {
    this->readData();
  }
}

void Client::readSize() {
  if(this->socket->bytesAvailable() >= 4) {
    uint32_t tmp;
    memcpy(&tmp, this->socket->read(4).data(), 4);
    this->in_size = ntohl(tmp);
  }
}

void Client::readData() {
  while(this->socket->bytesAvailable() > 0) {
    this->in_buffer.append(this->socket->readAll());
    if(this->in_buffer.size() >= this->in_size) {
      this->in_size = 0;
      emit this->dataReady(this->in_buffer);
      // TODO: questi dati vanno mandati da qualche parte per il processamento
      std::string str{this->in_buffer.data()};
      this->in_buffer.clear();
      std::cout << str << std::endl;
    }
  }
}

void Client::send(QString msg) {
  if(this->socket->state() == QAbstractSocket::ConnectedState) {
    uint32_t size = htonl(msg.size());
    QByteArray arr;
    QDataStream stream(&arr, QIODevice::WriteOnly);
    stream << size;
    this->socket->write(arr+msg.toUtf8());
    this->socket->waitForBytesWritten();
  }
}

void Client::_disconnected() {
  emit this->disconnected(this->id);
}
