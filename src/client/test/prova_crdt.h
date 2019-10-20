#include <iostream>

#include "Symbol.h"
#include "SymbolId.h"
#include "File.h"

void prova_crdt() {
  File f;
  std::cout << "Test algoritmo CRDT" << std::endl;

  //inserimento locale
  Symbol s1 {{1,0}, 'c'}, s2 {{1,1}, 'i'}, s3 {{1,2}, 'a'}, s4 {{1,3}, 'o'};

  f.localInsert(s1, 0); // c
  f.localInsert(s2, 1); // ci
  f.localInsert(s3, 2); // cia
  f.localInsert(s4, 3); // ciao

  // testo symbolById
  if(f.symbolById({1,2}).getChar() != 'a') {
    throw TestException{"localInsert ha inserito nella posizione errata"};
  }

  // simulo un inserimento remoto
  File f2 = f;
  Symbol s5 {{2,0}, 'w'};
  f2.localInsert(s5, 1); // cwiao
  f.remoteInsert(s5); // cwiao

  if(f != f2) {
    throw TestException{"remoteInsert ha inserito nella posizione errata"};
  }

  //cancellazione
  auto &sym = f2.symbolAt(1); // w
  auto id = sym.getSymbolId(); //occhio che se cancello da f2 poi perdo questa informazione
  f.localDelete(1); // ciao
  f2.remoteDelete(id); // ciao

  if(f.text() != "ciao") {
    throw TestException{"localDelete ha cancellato nella posizione errata"};
  }

  if(f != f2) {
    throw TestException{"remoteDelete ha cancellato nella posizione errata"};
  }

  //simulo inserimenti/cancellazioni errate
  try {
    Symbol s6{{1,4}, 'g'};
    f2.localInsert(s6, -3);
    throw TestException{"localInsert ha avuto successo (non doveva)"};
  }
  catch(FileSymbolsException e) {
  }

  try {
    Symbol s7{{1,5}, 'h'};
    f2.localInsert(s7, f.numSymbols() + 1);
    throw TestException{"localInsert ha avuto successo (non doveva)"};
  }
  catch(FileSymbolsException e) {
  }

  try {
    f2.localDelete(-1);
    throw TestException{"localDelete ha avuto successo (non doveva)"};
  }
  catch(FileSymbolsException e) {
  }

  try {
    f2.localDelete(f.numSymbols());
    throw TestException{"localDelete ha avuto successo (non doveva)"};
  }
  catch(FileSymbolsException e) {
  }

  std::cout << "Test passato" << std::endl;
}
