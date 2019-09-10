#include "client.h"

#include <QDataStream>

#include <arpa/inet.h>

#include "session.h"

Client::Client(QTcpSocket *s, int id): QObject(nullptr) {
  this->_id = id;
  this->_socket = s;
  connect(this->_socket, SIGNAL(readyRead()), SLOT(read()));
  connect(this->_socket, SIGNAL(disconnected()), SLOT(_disconnected()));
}

Client::Client(Client &&c) {
  this->_id = c._id;
  this->_session = c._session;
  c._session = nullptr;
  if(this->_socket != nullptr) {
    QObject::disconnect(this->_socket, SIGNAL(readyRead()), this, SLOT(read()));
    QObject::disconnect(this->_socket, SIGNAL(disconnected()), this, SLOT(_disconnected()));
    this->_socket->deleteLater();
    delete this->_socket;
  }
  QObject::disconnect(c._socket, SIGNAL(readyRead()), &c, SLOT(read()));
  QObject::disconnect(c._socket, SIGNAL(disconnected()), &c, SLOT(_disconnected()));
  this->_socket = c._socket;
  connect(this->_socket, SIGNAL(readyRead()), SLOT(read()));
  connect(this->_socket, SIGNAL(disconnected()), SLOT(_disconnected()));
  c._socket = nullptr;
  this->_in_size = c._in_size;
  c._in_size = 0;
  this->_in_buffer = std::move(c._in_buffer);
}

Client::~Client() {
  if(this->_session != nullptr) {
    delete this->_session;
    this->_session = nullptr;
  }
  if(this->_socket != nullptr) {
    QObject::disconnect(this->_socket, SIGNAL(readyRead()), this, SLOT(read()));
    QObject::disconnect(this->_socket, SIGNAL(disconnected()), this, SLOT(_disconnected()));
    this->_socket->deleteLater();
    this->_socket = nullptr;
  }
}

void Client::read() {
  if(this->_in_size <= 0) {
    this->readSize();
  }
  if(this->_in_size > 0) {
    this->readData();
  }
}

void Client::readSize() {
  if(this->_socket->bytesAvailable() >= 4) {
    uint32_t tmp;
    memcpy(&tmp, this->_socket->read(4).data(), 4);
    this->_in_size = ntohl(tmp);
  }
}

void Client::readData() {
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
      // TODO: questi dati vanno mandati da qualche parte per il processamento
      std::string str{this->_in_buffer.data()};
      this->_in_buffer.clear();
      std::cout << str << std::endl;
    }
  }
}

void Client::send(QByteArray msg) {
  if(this->_socket->state() == QAbstractSocket::ConnectedState) {
    uint32_t size = htonl(msg.size());
    QByteArray arr;
    QDataStream stream(&arr, QIODevice::WriteOnly);
    stream << size;
    this->_socket->write(arr+msg);
    this->_socket->waitForBytesWritten();
  }
}

void Client::_disconnected() {
  emit this->disconnected(this->_id);
}
