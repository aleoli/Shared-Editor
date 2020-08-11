#include "client.h"

#include <QDataStream>

#include "session.h"

Client::Client(QTcpSocket *s, quint64 id): QObject(nullptr) {
  this->_id = id;
  this->_socket = s;
  this->_socket->setParent(this);
  connect(this->_socket, SIGNAL(readyRead()), SLOT(read()));
  connect(this->_socket, SIGNAL(disconnected()), SLOT(_disconnected()));
}

Client::Client(Client &&c) noexcept {
  this->_id = c._id;
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
    QByteArray data = this->_socket->read(4);
    QDataStream ds(data);
    ds.setByteOrder(QDataStream::BigEndian);
    ds >> this->_in_size;
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
      emit this->dataReady(this->_id, this->_in_buffer);
      this->_in_buffer.clear();
    }
  }
}

void Client::send(quint64 client_id, const QByteArray& msg) {
  if(client_id == this->_id && this->_socket->state() == QAbstractSocket::ConnectedState) {
    QByteArray data;
    quint32 size = msg.size();
    QDataStream ds(&data, QIODevice::WriteOnly);
    ds.setByteOrder(QDataStream::BigEndian);
    ds << size;
    data += msg;

    this->_socket->write(data);
    this->_socket->waitForBytesWritten();
  }
}

void Client::_disconnected() {
  emit this->disconnected(this->_id);
}

void Client::disconnect(quint64 id) {
	if(this->_id == id) {
		this->_socket->disconnect();
		this->_disconnected();
	}
}
