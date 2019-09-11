#include <iostream>

#include "Symbol.h"
#include "SymbolId.h"
#include "File.h"

void prova_crdt() {
  File f;

  //inserimento locale
  Symbol s1 {{1,0}, 'c'}, s2 {{1,1}, 'i'}, s3 {{1,2}, 'a'}, s4 {{1,3}, 'o'};

  f.localInsert(s1, 0);
  f.localInsert(s2, 1);
  f.localInsert(s3, 2);
  f.localInsert(s4, 3);

  std::cout << "Inserimento locale:" << std::endl;
  std::cout << f.text() << std::endl;


  // simulo un inserimento remoto
  File f2 = f;
  Symbol s5 {{2,0}, 'w'};
  f2.localInsert(s5, 1); //qui dentro sarà anche inizializzato il vettore pos
  f.remoteInsert(s5);

  std::cout << "Inserimento remoto:" << std::endl;
  std::cout << f.text() << std::endl;

  //cancellazione locale
  f.localDelete(1);
  std::cout << "Cancellazione locale:" << std::endl;
  std::cout << f.text() << std::endl;

  //simulo cancellazione remota
  f2 = f;
  auto &sym = f2.symbolAt(1);
  auto id = sym.getSymbolId(); //occhio che se cancello da f2 poi perdo questa informazione
  f2.localDelete(1);
  f.remoteDelete(id);

  std::cout << "Cancellazione remota:" << std::endl;
  std::cout << f.text() << std::endl;

  //simulo inserimenti/cancellazioni errate
  std::cout << "Prova inserimenti/cancellazioni errate" << std::endl;
  try {
    f2 = f;
    Symbol s6{{1,4}, 'g'};
    f2.localInsert(s6, -3);
    std::cout << "Errato: mi aspettavo una eccezione" << std::endl;
  }
  catch(...) {
    std::cout << "Corretto" << std::endl;
  }

  try {
    f2 = f;
    Symbol s7{{1,5}, 'h'};
    f2.localInsert(s7, f.numSymbols() + 1);
    std::cout << "Errato: mi aspettavo una eccezione" << std::endl;
  }
  catch(...) {
    std::cout << "Corretto" << std::endl;
  }

  try {
    f2 = f;
    f2.localDelete(-1);
    std::cout << "Errato: mi aspettavo una eccezione" << std::endl;
  }
  catch(...) {
    std::cout << "Corretto" << std::endl;
  }

  try {
    f2 = f;
    f2.localDelete(f.numSymbols());
    std::cout << "Errato: mi aspettavo una eccezione" << std::endl;
  }
  catch(...) {
    std::cout << "Corretto" << std::endl;
  }
}
