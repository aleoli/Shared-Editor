#include "FSElement.h"
#include <QString>
#include <QJsonArray>
#include <vector>
#include "exceptions.h"
#include "utils.h"
#include "Symbol.h"

using namespace se_exceptions;
using namespace utils;


void prova_jsonarraytovector() {
  {
    FSElement f1(0,1,"pippo", FSElement::Type::FILE);
    FSElement f2(0,1,"pluto", FSElement::Type::DIRECTORY);
    FSElement f3(0,1,"paperino", FSElement::Type::FILE);

    QJsonArray a {f1.toJsonObject(), f2.toJsonObject(), f3.toJsonObject()};

    auto vect = jsonArrayToVector<FSElement>(a);

    if(vect.size() != 3) {
      throw TestException{"fallito test jsonArrayToVector"};
    }
    if(vect.at(0) != f1) {
      throw TestException{"fallito test jsonArrayToVector"};
    }
    if(vect.at(1) != f2) {
      throw TestException{"fallito test jsonArrayToVector"};
    }
    if(vect.at(2) != f3) {
      throw TestException{"fallito test jsonArrayToVector"};
    }

  }
  {
    Symbol s1({1,2}, 'y');
    Symbol s2({2,4}, 'c');

    QJsonArray a {s1.toJsonObject(), s2.toJsonObject()};

    auto vect = jsonArrayToVector<Symbol>(a);

    if(vect.size() != 2) {
      throw TestException{"fallito test jsonArrayToVector"};
    }
    if(vect.at(0) != s1) {
      throw TestException{"fallito test jsonArrayToVector"};
    }
    if(vect.at(1) != s2) {
      throw TestException{"fallito test jsonArrayToVector"};
    }

  }

  info("Test jsonArrayToVector passato");
}

void prova_vectortojsonarray() {
  {
    FSElement f1(0,1,"pippo", FSElement::Type::FILE);
    FSElement f2(0,1,"pluto", FSElement::Type::DIRECTORY);
    FSElement f3(0,1,"paperino", FSElement::Type::FILE);

    std::vector<FSElement> vect{f1,f2,f3};
    QJsonArray a1 {f1.toJsonObject(), f2.toJsonObject(), f3.toJsonObject()};

    auto a2 = vectorToJsonArray(vect);

    if(a1 != a2) {
      throw TestException{"fallito test vectorToJsonArray"};
    }

  }
  {
    Symbol s1({1,2}, 'y');
    Symbol s2({2,4}, 'c');

    std::vector<Symbol> vect{s1,s2};
    QJsonArray a1 {s1.toJsonObject(), s2.toJsonObject()};

    auto a2 = vectorToJsonArray(vect);

    if(a1 != a2) {
      throw TestException{"fallito test vectorToJsonArray"};
    }

  }

  info("Test vectorToJsonArray passato");
}

void prova_misc() {
  prova_jsonarraytovector();
  prova_vectortojsonarray();
}
