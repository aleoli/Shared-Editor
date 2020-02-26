#pragma once

#include <QJsonObject>
#include <QString>
#include <QJsonDocument>
#include <QFile>
#include <optional>

#include <iostream>
#include "FifoMap.h"
#include "Message.h"
#include "def.h"

#include "exceptions.h"

using namespace se_exceptions;

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

void testDataGetters() {
  Message m(Message::Type::FILE, 4, Message::Status::QUERY);
  QJsonObject obj;
  obj["pizza"] = 4;
  auto array = QJsonArray{1,2,3};

  m.setValue("int", 5);
  m.setValue("string", "ciao");
  m.setValue("obj", obj);
  m.setValue("arr", array);

  //test errors
  try {
    m.setValue("int", 24);
    throw TestException{"setValue fallito"};
  }
  catch(MessageDataException e) {}

  if(m.getInt("int") != 5) {
    throw TestException{"getInt fallito"};
  }

  if(m.getString("string") != "ciao") {
    throw TestException{"getString fallito"};
  }

  if(m.getObject("obj") != obj) {
    throw TestException{"getObject fallito"};
  }

  if(m.getArray("arr") != array) {
    throw TestException{"getArray fallito"};
  }

  try {
    m.getInt("aaa");
    throw TestException{"getInt fallito"};
  }
  catch(MessageDataException e) {}

  try {
    m.getString("aaa");
    throw TestException{"getString fallito"};
  }
  catch(MessageDataException e) {}

  try {
    m.getObject("aaa");
    throw TestException{"getObject fallito"};
  }
  catch(MessageDataException e) {}

  try {
    m.getArray("aaa");
    throw TestException{"getArray fallito"};
  }
  catch(MessageDataException e) {}

  try {
    m.getInt("string");
    throw TestException{"getInt fallito"};
  }
  catch(MessageDataException e) {}

  try {
    m.getString("int");
    throw TestException{"getString fallito"};
  }
  catch(MessageDataException e) {}

  try {
    m.getObject("arr");
    throw TestException{"getObject fallito"};
  }
  catch(MessageDataException e) {}

  try {
    m.getArray("obj");
    throw TestException{"getArray fallito"};
  }
  catch(MessageDataException e) {}

}

void testDataGettersOpt() {
  Message m(Message::Type::FILE, 4, Message::Status::QUERY);
  QJsonObject obj;
  obj["pizza"] = 4;
  auto array = QJsonArray{1,2,3};

  m.setValue("int", 5);
  m.setValue("string", "ciao");
  m.setValue("obj", obj);
  m.setValue("arr", array);

  if(!m.getIntOpt("int")) {
    throw TestException{"testDataGettersOpt fallito"};
  }

  if(m.getIntOpt("aaa")) {
    throw TestException{"testDataGettersOpt fallito"};
  }

  if(*m.getIntOpt("int") != 5) {
    throw TestException{"testDataGettersOpt fallito"};
  }

  if(!m.getStringOpt("string")) {
    throw TestException{"testDataGettersOpt fallito"};
  }

  if(m.getStringOpt("aaa")) {
    throw TestException{"testDataGettersOpt fallito"};
  }

  if(*m.getStringOpt("string") != "ciao") {
    throw TestException{"testDataGettersOpt fallito"};
  }

  if(!m.getObjectOpt("obj")) {
    throw TestException{"testDataGettersOpt fallito"};
  }

  if(m.getObjectOpt("aaa")) {
    throw TestException{"testDataGettersOpt fallito"};
  }

  if(*m.getObjectOpt("obj") != obj) {
    throw TestException{"testDataGettersOpt fallito"};
  }

  if(!m.getArrayOpt("arr")) {
    throw TestException{"testDataGettersOpt fallito"};
  }

  if(m.getArrayOpt("aaa")) {
    throw TestException{"testDataGettersOpt fallito"};
  }

  if(*m.getArrayOpt("arr") != array) {
    throw TestException{"testDataGettersOpt fallito"};
  }
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

  testDataGetters();

  testDataGettersOpt();

  std::cout << "Test passato" << std::endl;
}
