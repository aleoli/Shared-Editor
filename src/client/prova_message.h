#pragma once

#include <QJsonObject>
#include <QString>
#include <QJsonDocument>
#include <QFile>

#include <iostream>
#include "FifoMap.h"
#include "Message.h"

#define SAVE_BINARY 0

Message createMessage() {
  QJsonObject data;
  data["name"] = "newfile.txt";
  data["id_user"] = 5;

  return Message(Message::Type::FILE, static_cast<int>(Message::FileAction::CLOSE_FILE),
    false, Message::Status::RESPONSE, data);
}

void writeMessage(QString path, Message &m) {
  // trasformo in QByteArray
  QJsonDocument doc(m.toJsonObject());

#if SAVE_BINARY
  auto array = doc.toBinaryData();
#else
  auto array = doc.toJson();
#endif

  // scrivo su file
  QFile file(path);

  if(!file.open(QIODevice::WriteOnly)){
    std::cout << "cannot open file" << std::endl;
  }

  file.write(array);
  file.close();
}

Message readMessage(QString path) {
  // leggo da file
  QFile file(path);

  if(!file.open(QIODevice::ReadOnly)){
    std::cout << "cannot open file" << std::endl;
  }

  auto data = file.readAll();
  file.close();

  // trasformo in oggetto Message
#if SAVE_BINARY
  auto doc = QJsonDocument::fromBinaryData(data);
#else
  auto doc = QJsonDocument::fromJson(data);
#endif

  auto jsonObject = doc.object();
  return Message::fromJsonObject(jsonObject);
}

void prova_message() {
  // creo messaggio a caso
  Message m = createMessage();

  // leggo messaggio da file (da socket è molto simile il procedimento)
  //Message m2 = readMessage("prova.shed");

  // passaggio a QJsonObject e viceversa
  auto json = m.toJsonObject();
  Message m3 = Message::fromJsonObject(json);

  // stampe varie
  std::cout << "type: " << static_cast<int>(m3.getType()) << std::endl;
  std::cout << "action: " << static_cast<int>(m3.getAction()) << std::endl;
  std::cout << "error: " << m3.getError() << std::endl;
  std::cout << "status: " << static_cast<int>(m3.getStatus()) << std::endl;

  auto data = m3.getData();

  std::cout << data["name"].toString().toStdString() << std::endl;
  std::cout << data["id_user"].toInt() << std::endl;

  // scrivo su file (su socket è molto simile)
  //writeMessage("prova.shed", m2);
}
