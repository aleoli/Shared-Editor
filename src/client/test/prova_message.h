#pragma once

#include <QJsonObject>
#include <QString>
#include <QJsonDocument>
#include <QFile>

#include <iostream>
#include "FifoMap.h"
#include "Message.h"
#include "def.h"

#include "exceptions.h"

using namespace se_exceptions;

#define SAVE_BINARY 0

Message createMessage() {
  QJsonObject data;
  data["name"] = "newfile.txt";
  data["id_user"] = 5;

  return Message(Message::Type::FILE, static_cast<int>(Message::FileAction::CLOSE_FILE),
    false, Message::Status::RESPONSE, data);
}

void writeMessage(QString path, Message &m) {

  // scrivo su file
  QFile file(path);

  if(!file.open(QIODevice::WriteOnly)){
    std::cout << "cannot open file" << std::endl;
    exit(-1);
  }

  file.write(m.toQByteArray());
  file.close();
}

Message readMessage(QString path) {
  // leggo da file
  QFile file(path);

  if(!file.open(QIODevice::ReadOnly)){
    std::cout << "cannot open file" << std::endl;
    exit(-1);
  }

  auto data = file.readAll();
  file.close();

  return Message::fromQByteArray(data);
}

void prova_message() {
  std::cout << "Test classe Message" << std::endl;

  // creo messaggio a caso
  Message m1 = createMessage();

  std::cout << m1.to_string() << std::endl;

  // passaggio a QJsonObject e viceversa
  Message m2 = Message::fromJsonObject(m1.toJsonObject());

  if(m1 != m2) {
    throw TestException{"test copia fallito"};
  }

  // test scrittura su disco e lettura
  writeMessage("m1.shed", m1);
  Message m3 = readMessage("m1.shed");

  if(m1 != m3) {
    throw TestException{"test scrittura/lettura disco fallito"};
  }

  std::cout << "Test passato" << std::endl;
}
