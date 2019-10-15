#pragma once

#include <iostream>
#include <QChar>

#include "FSElement.h"

void prova_fselement() {
  FSElement fse1(1, 2, "file", FSElement::Type::FILE);

  std::cout << "PROVA STAMPA ELEMENTO" << std::endl;
  std::cout << fse1.to_string() << std::endl;

  FSElement fse2(fse1.toJsonObject());
  std::cout << "PROVA COPIA ELEMENTO:" << std::endl;
  std::cout << fse2.to_string() << std::endl;
}
