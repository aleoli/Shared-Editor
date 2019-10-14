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
  Symbol sym({1,2}, 'y');
  sym.setPos({{1,1}, {2,1}, {3,1}});
  sym.setBold(true);
  sym.setSize(20);
  sym.setUnderline(true);
  sym.setFamily("Helvetica");
  sym.setColor("red");
  sym.setBackgroundColor("#6600ff00");

  std::cout << "PROVA STAMPA SIMBOLO" << std::endl;
  std::cout << sym.to_string() << std::endl;

  Symbol s2(sym.toJsonObject());
  std::cout << "PROVA COPIA SIMBOLO:" << std::endl;
  std::cout << s2.to_string() << std::endl;

  // test File
  Symbol s3({3,4}, 'x');
  Symbol s4({5,6}, 'w');

  std::vector<Symbol> symbols{sym, s3, s4};
  std::vector<int> ids{1,2,3,4,5,6,7,8,9,0};

  File f1(0, ids, symbols);
  std::cout << "PROVA STAMPA FILE" << std::endl;
  std::cout << f1.to_string();

  File f2(f1.toJsonObject());
  std::cout << "PROVA COPIA FILE:" << std::endl;
  std::cout << f2.to_string();

  // test scrittura su disco e lettura
  writeFile("f1.shed", f1);
  File f3 = readFile("f1.shed");
  std::cout << "PROVA SCRITTURA/LETTURA FILE SU DISCO" << std::endl;
  std::cout << f3.to_string();
}
