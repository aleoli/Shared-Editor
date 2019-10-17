#pragma once

#include <iostream>
#include "File.h"
#include "Symbol.h"
#include <QChar>
#include "def.h"

void writeFile(QString path, File &f) {
  // scrivo su file
  QFile file(path);

  if(!file.open(QIODevice::WriteOnly)){
    std::cout << "cannot open file" << std::endl;
    exit(-1);
  }

  file.write(f.toQByteArray());
  file.close();
}

File readFile(QString path) {
  // leggo da file
  QFile file(path);

  if(!file.open(QIODevice::ReadOnly)){
    std::cout << "cannot open file" << std::endl;
    exit(-1);
  }

  auto data = file.readAll();
  file.close();

  return File::fromQByteArray(data);
}

void prova_file() {
  // test Symbol
  std::cout << "Test classe Symbol" << std::endl;

  Symbol s1({1,2}, 'y');
  s1.setPos({{1,1}, {2,1}, {3,1}});
  s1.setBold(true);
  s1.setSize(20);
  s1.setUnderline(true);
  s1.setFamily("Helvetica");
  s1.setColor("red");
  s1.setBackgroundColor("#6600ff00");

  std::cout << s1.to_string() << std::endl;

  Symbol s2(s1.toJsonObject());
  if(s1 != s2) {
    throw TestException{"test copia symbol fallito"};
  }

  std::cout << "Test passato" << std::endl;

  // test File
  std::cout << "Test classe File" << std::endl;

  Symbol s3({3,4}, 'x');
  Symbol s4({5,6}, 'w');


  File f1(0);
  f1.localInsert(s2, 0);
  f1.localInsert(s3, 1);
  f1.localInsert(s4, 1);
  f1.addClient(1, "bob");
  f1.addClient(2, "ale");
  f1.addClient(3, "klajdi");
  f1.addClient(4, "erich");

  std::cout << f1.to_string() << std::endl;

  // chiamate che devono generano eccezioni
  try {
    f1.addClient(1, "pippo");
    throw TestException{"test addClient fallito"};
  }
  catch(...) {}

  try {
    f1.removeClient(5);
    throw TestException{"test removeClient fallito"};
  }
  catch(...) {}

  f1.removeClient(2);

  File f2(f1.toJsonObject());
  if(f1 != f2) {
    throw TestException{"test copia file fallito"};
  }

  // test scrittura su disco e lettura
  writeFile("f1.shed", f1);
  File f3 = readFile("f1.shed");
  if(f1 != f3) {
    throw TestException{"test lettura/scrittura disco fallito"};
  }

  std::cout << "Test passato" << std::endl;
}
