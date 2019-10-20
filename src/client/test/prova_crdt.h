#include <iostream>

#include "Symbol.h"
#include "SymbolId.h"
#include "File.h"

void prova_crdt() {
  File f;
  int pos;
  std::cout << "Test algoritmo CRDT" << std::endl;

  //inserimento locale
  Symbol s1 {{1,0}, 'c'}, s2 {{1,1}, 'i'}, s3 {{1,2}, 'a'}, s4 {{1,3}, 'o'};

  f.localInsert(s1, 0); // c
  f.localInsert(s2, 1); // ci
  f.localInsert(s3, 2); // cia
  f.localInsert(s4, 3); // ciao

  // testo symbolById
  auto pair = f.symbolById({1,2});
  if(pair.first != 2 || pair.second.getChar() != 'a') {
    throw TestException{"localInsert ha inserito nella posizione errata"};
  }

  // simulo un inserimento remoto
  File f2 = f;
  Symbol s5 {{2,0}, 'w'};
  f2.localInsert(s5, 1); // cwiao

  Symbol s6 ({1,4}, 'n');
  f.localInsert(s6, 0); // nciao
  pos = f.remoteInsert(s5); // ncwiao

  if(pos != 2) {
    throw TestException{"remoteInsert ha fallito"};
  }

  pos = f2.remoteInsert(s6); // ncwiao
  if(pos != 0) {
    throw TestException{"remoteInsert ha fallito"};
  }

  if(f != f2) {
    throw TestException{"remoteInsert ha fallito"};
  }

  //cancellazione
  auto &sym = f2.symbolAt(2); // w
  auto id = sym.getSymbolId(); //occhio che se cancello da f2 poi perdo questa informazione
  f.localDelete(1); // nciao
  f2.localDelete(1); // nciao
  pos = f2.remoteDelete(id); // nciao

  if(pos != -1) {
    throw TestException{"remoteDelete ha fallito"};
  }

  if(f.text() != "nciao") {
    throw TestException{"localDelete ha fallito"};
  }

  if(f != f2) {
    throw TestException{"remoteDelete ha fallito"};
  }

  //simulo inserimenti/cancellazioni errate
  try {
    Symbol s6{{1,4}, 'g'};
    f2.localInsert(s6, -3);
    throw TestException{"localInsert ha avuto successo (non doveva)"};
  }
  catch(...) {
  }

  try {
    Symbol s7{{1,5}, 'h'};
    f2.localInsert(s7, f.numSymbols() + 1);
    throw TestException{"localInsert ha avuto successo (non doveva)"};
  }
  catch(...) {
  }

  try {
    f2.localDelete(-1);
    throw TestException{"localDelete ha avuto successo (non doveva)"};
  }
  catch(...) {
  }

  try {
    f2.localDelete(f.numSymbols());
    throw TestException{"localDelete ha avuto successo (non doveva)"};
  }
  catch(...) {
  }

  std::cout << "Test passato" << std::endl;
}
