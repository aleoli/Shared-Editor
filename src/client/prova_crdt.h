#include <iostream>
#include <vector>
#include <cmath>

#include "Symbol.h"
#include "SymbolId.h"

void findPosition(int clientId, std::vector<Symbol::Identifier> v1, std::vector<Symbol::Identifier> v2,
  std::vector<Symbol::Identifier> &position, int level = 0) {
    Symbol::Identifier pos1, pos2;

    if(!v1.empty()) pos1 = v1.at(0);
    else pos1 = {0, clientId};

    if(!v2.empty()) pos2 = v2.at(0);
    else pos2 = {static_cast<int>(pow(2, level) * 32), clientId};

    int digit1 = pos1.getDigit();
    int digit2 = pos2.getDigit();

    if(digit2 - digit1 > 1){
        //finished, found the position
        int digit = (digit2 + digit1) / 2; //TODO cambiare se vuoi
        position.emplace_back(digit, clientId);
        return;
    }

    else if(digit2 - digit1 == 1){
        //must go deeper
        position.push_back(pos1);

        if(!v1.empty()) v1.erase(v1.begin());
        v2.clear();

        findPosition(clientId, v1, v2, position, level + 1);
    }

    else if(digit2 == digit1) {
        //must go deeper
        int clientId1 = pos1.getClientId();
        int clientId2 = pos2.getClientId();

        if (clientId1 < clientId2) {
            position.push_back(pos1);

            if (!v1.empty()) v1.erase(v1.begin());
            v2.clear();

            findPosition(clientId, v1, v2, position, level + 1);
        } else if (clientId1 == clientId2) {
            position.push_back(pos1);

            if (!v1.empty()) v1.erase(v1.begin());
            if (!v2.empty()) v2.erase(v2.begin());

            findPosition(clientId, v1, v2, position, level + 1);
        } else {
            throw std::runtime_error("Error!!");
        }

    }
}

void localInsert(int clientId, std::vector<Symbol> &symbols, int index, Symbol &sym) {
    auto size = symbols.size();

    //TODO what to do if index is >= size of vector symbols?
    if(index > size) index = size;

    //create array position
    int previous = index > 0 ? index - 1 : -1;
    int next = index == size ? -1 : index;

    std::vector<Symbol::Identifier> v1, v2;

    if(previous != -1) {
        auto sym = symbols.at(previous);
        v1 = sym.getPos();
    }
    if(next != -1) {
        auto sym = symbols.at(next);
        v2 = sym.getPos();
    }

    std::vector<Symbol::Identifier> position;

    findPosition(clientId, v1, v2, position);

    sym.setPos(position);
    symbols.emplace(symbols.begin() + index, sym);
}

void stampaSymbols(const std::vector<Symbol> &symbols) {
  std::cout << "Symbols:" << std::endl;
  for(auto &sym : symbols)
    std::cout << sym.to_string() << std::endl;
}

void remoteInsert(std::vector<Symbol> &symbols, const Symbol &sym) {
  int i;

  for(i=0; i<symbols.size(); i++){
    Symbol &s = symbols[i];

    if(sym < s) break;
  }
  symbols.emplace(symbols.begin() + i, sym);
}

void prova_crdt() {
  std::vector<Symbol> symbols;

  Symbol sym1{{1,2}, 'c'}, sym2{{1,2}, 'i'}, sym3{{1,2}, 'a'}, sym4{{1,2}, 'o'};

  localInsert(1, symbols, 0, sym1);
  localInsert(1, symbols, 1, sym2);
  localInsert(1, symbols, 2, sym3);
  localInsert(1, symbols, 3, sym4);


  Symbol rem1{{2,2}, 'w'};
  rem1.setPos({{15, 2}});
  remoteInsert(symbols, rem1);

  Symbol sym5{{1,2}, 'x'};
  localInsert(1, symbols, 1, sym5);

  Symbol sym6{{1,2}, 'J'};
  localInsert(1, symbols, 1, sym6);

  stampaSymbols(symbols);
}
