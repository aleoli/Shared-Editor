#include "guiwrapper.h"

#include <QTimer>

void GuiWrapper::testWindows() {
  info("TEST FINESTRE");
  info("Tra 15 secondi arriva una loginResponseReceived");
  info("Tra 30 secondi arriva un newFileResponseReceived");

  QTimer::singleShot(15000, this, [this]() {loginResponseReceived("questoeuntokenautentico", 33, nullptr, nullptr);});
  QTimer::singleShot(30000, this, [this]() {newFileResponseReceived(0);});
}

void GuiWrapper::testEditor() {
  info("TEST EDITOR");

  //TEST user connessi/disconnessi
  QTimer::singleShot(3000, this, [this]() {_textEdit->setUser(22, "kaka");});
  QTimer::singleShot(6000, this, [this]() {emit userConnectedQuery(0, 33, "thiago silva");});
  QTimer::singleShot(9000, this, [this]() {emit userConnectedQuery(0, 44, "oddo");});
  QTimer::singleShot(12000, this, [this]() {emit userDisconnectedQuery(0, 33);});

  //TEST setFile e refresh
  QTimer::singleShot(15000, this, [this]() {
    File f;
    Symbol s1 {{1,2}, 'c'}, s2({1,3}, 'i'), s3({1,4}, 'a'), s4({1,5}, 'o');
    f.localInsert(s1, 0);
    f.localInsert(s2, 1);
    f.localInsert(s3, 2);
    f.localInsert(s4, 3);
    _textEdit->setFile(f);});

  //TEST moveCursor
  QTimer::singleShot(18000, this, [this]() {emit remoteMoveQuery(0, 44, {1,4}, 2);});
}

void GuiWrapper::testCRDT() {
  debug("TEST CRDT");
  //connessione
  QTimer::singleShot(5000, this, [this]() {emit userConnectedQuery(0, 32, "thiago silva");});

  //aggiungo caratteri a caso
  QTimer::singleShot(8000, this, [this]() {
    QChar c('c');
    Symbol s{{32, 0}, c};
    File f = _textEdit->getFile();
    f.localInsert(s, 0); //per costruire il vettore pos!
    emit remoteInsertQuery(0, 32, std::vector<Symbol>{s});});

  QTimer::singleShot(9000, this, [this]() {
    QChar c('i');
    Symbol s{{32, 1}, c};
    File f = _textEdit->getFile();
    f.localInsert(s, 1); //per costruire il vettore pos!
    emit remoteInsertQuery(0, 32, std::vector<Symbol>{s});});

  QTimer::singleShot(10000, this, [this]() {
    QChar c('a');
    Symbol s{{32, 2}, c};
    File f = _textEdit->getFile();
    f.localInsert(s, 2); //per costruire il vettore pos!
    emit remoteInsertQuery(0, 32, std::vector<Symbol>{s});});

  QTimer::singleShot(11000, this, [this]() {
    QChar c('o');
    Symbol s{{32, 3}, c};
    File f = _textEdit->getFile();
    f.localInsert(s, 3); //per costruire il vettore pos!
    emit remoteInsertQuery(0, 32, std::vector<Symbol>{s});});
  /*
  QTimer::singleShot(12000, this, [this]() {
    QChar c('H');
    Symbol s{{32, 4}, c};
    File f = _textEdit->getFile();
    f.localInsert(s, 10); //per costruire il vettore pos!
    emit remoteInsertQuery(0, 32, std::vector<Symbol>{s});});*/

  QTimer::singleShot(13000, this, [this]() {
    emit remoteDeleteQuery(0, 32, std::vector<SymbolId>{{32,1}});});

  QTimer::singleShot(14000, this, [this]() {
    emit remoteDeleteQuery(0, 32, std::vector<SymbolId>{{33,10}});});

  QTimer::singleShot(15000, this, [this]() {
    emit remoteDeleteQuery(0, 32, std::vector<SymbolId>{{36,10}});});

  QTimer::singleShot(16000, this, [this]() {
    auto sym = _textEdit->getFile().symbolAt(1);
    sym.setChar('Z');
    emit remoteUpdateQuery(0, 32, std::vector<Symbol>{sym});});

  QTimer::singleShot(30000, this, [this]() {emit userDisconnectedQuery(0, 32);});
}
