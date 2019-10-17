#pragma once

#include <iostream>
#include <QChar>

#include "FSElement.h"

void prova_fselement() {
  std::cout << "Test classe FSElement" << std::endl;

  FSElement fse1(1, 2, "file", FSElement::Type::FILE);

  std::cout << fse1.to_string() << std::endl;

  FSElement fse2(fse1.toJsonObject());

  if(fse1 != fse2) {
    throw TestException{"test copia FSElement fallito"};
  }

  std::cout << "Test passato" << std::endl;
}
