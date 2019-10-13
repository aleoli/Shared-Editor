#pragma once

#include <QJsonObject>
#include <QString>
#include <QJsonDocument>
#include <QFile>

#include <iostream>
#include "FifoMap.h"
#include "Message.h"
#include "def.h"

Message createMessage() {
  QJsonObject data;
  data["name"] = "newfile.txt";
  data["id_user"] = 5;

  return Message(Message::Type::FILE, static_cast<int>(Message::FileAction::CLOSE),
    Message::Status::RESPONSE, data);
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
  Message m1 = createMessage();

  std::cout << "PROVA STAMPA MESSAGGIO" << std::endl;
  std::cout << m1.to_string() << std::endl;

  Message m2(m1.toJsonObject());
  std::cout << "PROVA COPIA MESSAGGIO:" << std::endl;
  std::cout << m2.to_string() << std::endl;

  // test scrittura su disco e lettura
  writeMessage("m1.shed", m1);
  Message m3 = readMessage("m1.shed");
  std::cout << "PROVA SCRITTURA/LETTURA MESSAGE SU DISCO" << std::endl;
  std::cout << m3.to_string();
}
