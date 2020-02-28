//
// Created by Gianluca on 07/08/2019.
//
#include "TextEdit.h"

#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <iostream>
#include <QTextDocument>
#include <QTextBlock>
#include <QFont>
#include <QDockWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QListView>
#include <vector>

#include "Symbol.h"
#include "utils.h"
#include "exceptions.h"

using namespace se_exceptions;

TextEdit::TextEdit(QWidget *parent)
        : QMainWindow(parent), _blockSignals(false)
{
  // QTextEdit sarà il nostro widget principale, contiene un box in cui inserire testo.
  // Inoltre avremo un menu, in cui ci saranno tutte le varie opzioni, e una toolbar che ne avrà un set
  _textEdit = new QTextEdit(this);
  setCentralWidget(_textEdit);

  // inizializzo tutte le varie azioni. Un'azione può essere "Nuovo File", "Copia", "Incolla", "Grassetto", ...
  // per ogni azione va associato uno slot (funzione che esegue qualcosa quando si vuole eseguire l'azione
  // (esempio quando clicco su "Grassetto" (azione) voglio che il testo diventi appunto in grassetto (slot)
  setupFileActions();
  setupEditActions();
  setupTextActions();

  // signal/slot aggiunta / rimozione / modifica caratteri
  QObject::connect(_textEdit->document(), &QTextDocument::contentsChange, this, &TextEdit::change);
  QObject::connect(_textEdit, &QTextEdit::cursorPositionChanged, this, &TextEdit::cursorChanged);

  // _dock right
  initDock();
}

void TextEdit::setInitialBackground() {
  QTextCursor cursor = _textEdit->textCursor();
  auto fmt = cursor.charFormat();
  fmt.setBackground(QColor("#00000000"));

  _textEdit->mergeCurrentCharFormat(fmt);
}

void TextEdit::setFile(const File &f, int charId) {
  _file = f;
  _user.charId = charId;

  //setto gli utenti connessi
  for(auto &client : f.getClients()) {
    remoteUser user;
    user.userId = client.second.clientId;
    user.username = client.second.username;
    user.color = _gen.getColor();
    user.cursor = new Cursor(_textEdit, user.color);

    user.item = new QListWidgetItem(user.username, _dock);
    user.item->setFlags(Qt::NoItemFlags);
    user.item->setForeground(user.color);

    //TODO check nella mappa per vedere che non ci sia già
    _users[client.second.clientId] = user;
  }

  refresh(true);
}

File &TextEdit::getFile() {
  return _file;
}

void TextEdit::setFileId(int id) {
  _fileId = id;
}

int TextEdit::getFileId() const {
  return _fileId;
}

void TextEdit::refresh(bool changeFile) {
  //salvo pos del cursore mio
  std::pair<SymbolId, int> pos;

  if(!changeFile)
    pos = saveCursorPosition(_textEdit->textCursor());

  //cancello
  _blockSignals = true;
  _textEdit->document()->clear();

  setInitialBackground();

  //refresh
  QTextCursor cursor{_textEdit->document()};
  for(auto &sym : _file.getSymbols()) {
    _blockSignals = true;
    cursor.setCharFormat(sym.getFormat());
    cursor.insertText(sym.getChar());
  }

  //ripristino posizione
  if(!changeFile) {
    _blockSignals = true;
    cursor.setPosition(getCursorPosition(pos.first, pos.second));
    _textEdit->setTextCursor(cursor);
  }

  _blockSignals = false;
}

void TextEdit::share() {
  emit getLinkQuery(_fileId);
}

void TextEdit::clear() {
  _blockSignals = true;

  _textEdit->document()->clear();
  _file.clear();

  for(auto &user : _users) {
    user.second.cursor->updateCursorView();
  }

  _blockSignals = false;
}

void TextEdit::setUser(int userId, QString username) {
  debug("User initialized with userId: " + QString::number(userId));

  _user.initialized = true;
  _user.userId = userId;
  _user.username = username;
  _user.charId = 0;

  if(_user.item != nullptr) {
    _dock->takeItem(_dock->row(_user.item));
    delete _user.item;
  }

  _user.item = new QListWidgetItem(username + " (Tu)");
  _user.item->setFlags(Qt::NoItemFlags);
  _user.item->setForeground(QColor("black"));
  _dock->insertItem(0, _user.item);
}

void TextEdit::initDock() {
  auto *dockWidget = new QDockWidget(tr("Utenti connessi:"), this);
  dockWidget->setAllowedAreas(Qt::RightDockWidgetArea);
  addDockWidget(Qt::RightDockWidgetArea, dockWidget);

  _dock = new QListWidget(this);
  dockWidget->setWidget(_dock);
}

void TextEdit::setupFileActions() {
  // Azioni per i file (Nuovo file, salva...)

  // aggiunge toolbar, che sarebbe la barra in alto
  QToolBar *tb = addToolBar(tr("File Actions"));

  // aggiunge menu File
  QMenu *menu = menuBar()->addMenu(tr("&File"));

  const QIcon addLetterIcon(":/buttons/letter.png");
  QAction *add = menu->addAction(addLetterIcon,  tr("&Add"), this, &TextEdit::addLetter);
  tb->addAction(add);

  const QIcon getInfoIcon(":/buttons/info.png");
  QAction *info = menu->addAction(getInfoIcon,  tr("&Get infos"), this, &TextEdit::printTextFile);
  tb->addAction(info);

  const QIcon clearIcon(":/buttons/clear.png");
  QAction *clear = menu->addAction(clearIcon,  tr("&Clear document"), this, &TextEdit::clear);
  tb->addAction(clear);

  const QIcon refreshIcon(":/buttons/refresh.png");
  QAction *refresh = menu->addAction(refreshIcon,  tr("&Refresh document"), this, &TextEdit::refresh);
  tb->addAction(refresh);

  const QIcon shareIcon(":/buttons/share.png");
  QAction *share = menu->addAction(shareIcon,  tr("&Share document"), this, &TextEdit::share);
  tb->addAction(share);
}

void TextEdit::setupEditActions() {
  // Azioni per la modifica (Taglia, copia, incolla, undo, redo...)

  // aggiungo toolbar per le impostazioni di modifica (copia/incolla...) e anche una voce nel menu
  QToolBar *tb = addToolBar(tr("Edit Actions"));
  QMenu *menu = menuBar()->addMenu(tr("&Edit"));

  // AZIONE COPIA
  const QIcon copyIcon(":/buttons/copy.png");

  // in questo caso, QTextEdit ha già uno slot che fa la copia del testo in automatico, non è da implementare
  QAction *actionCopy = menu->addAction(copyIcon, tr("&Copy"), _textEdit, &QTextEdit::copy);
  actionCopy->setPriority(QAction::LowPriority);

  //imposto la shortcut da usare (che sarebbe CTRL+C)
  actionCopy->setShortcut(QKeySequence::Copy);
  tb->addAction(actionCopy);

  // AZIONE INCOLLA
  const QIcon pasteIcon(":/buttons/paste.png");

  // in questo caso, QTextEdit ha già uno slot che fa l'incolla del testo in automatico, non è da implementare
  QAction *actionPaste = menu->addAction(pasteIcon, tr("&Paste"), _textEdit, &QTextEdit::paste);
  actionPaste->setPriority(QAction::LowPriority);

  //imposto la shortcut da usare (che sarebbe CTRL+V)
  actionPaste->setShortcut(QKeySequence::Paste);
  tb->addAction(actionPaste);
}

void TextEdit::setupTextActions() {
  // Azioni per il testo (grassetto, corsivo, allineamento...)

  // aggiungo toolbar per le impostazioni del testo (grassetto, corsivo...)
  QToolBar *tb = addToolBar(tr("Format Actions"));
  QMenu *menu = menuBar()->addMenu(tr("F&ormat"));

  // AZIONE GRASSETTO
  // aggiungo un'azione per il grassetto, e ci associo uno slot (implementato da me)
  const QIcon boldIcon(":/buttons/bold.png");
  _actionTextBold = menu->addAction(boldIcon, tr("&Bold"), this, &TextEdit::textBold);
  // shortcut: CTRL+B
  _actionTextBold->setShortcut(Qt::CTRL + Qt::Key_B);
  _actionTextBold->setPriority(QAction::LowPriority);

  //questo è solo per dare il grassetto alla voce nel menu file.. non mette in grassetto il testo ovviamente
  QFont bold;
  bold.setBold(true);
  _actionTextBold->setFont(bold);

  tb->addAction(_actionTextBold);
  _actionTextBold->setCheckable(true);
}

void TextEdit::textBold() {
  if(_blockSignals) return;

  debug("Premuto tasto grassetto");

  QTextCursor cursor = _textEdit->textCursor();
  auto fmt = cursor.charFormat();
  fmt.setFontWeight(_actionTextBold->isChecked() ? QFont::Bold : QFont::Normal);

  //setto in grassetto
  _textEdit->mergeCurrentCharFormat(fmt);
}

void TextEdit::addLetter() {
  Symbol sym(SymbolId(), 'a');
  sym.setBold(true);
  sym.setSize(20);
  sym.setUnderline(true);
  sym.setItalic(true);
  sym.setFamily("Helvetica");
  sym.setColor("red");
  sym.setBackgroundColor("#6600ff00");

  _textEdit->textCursor().insertText(sym.getChar(), sym.getFormat());
}

void TextEdit::printTextFile() {
  info(QString::fromStdString(_file.text()));
  //debug(QString::fromStdString(_file.to_string()));
}

void TextEdit::change(int pos, int removed, int added) {
  if(_blockSignals) return;

  // a volte capita non so perchè che viene triggerato a caso.
  if(_file.numSymbols() < removed) return;

  //TODO se removed == added -> UPDATE
  //TODO si potrebbe fare anche una cosa più elaborata,
  //    tipo 2 rem e 3 added significa 2 update e 1 added, ma non so se vale la pena

  debug("contentsChange: position: " + QString::number(pos) +
    " rimossi " + QString::number(removed) + " aggiunti " + QString::number(added));

  // rimozioni
  if(removed > 0) {
    std::vector<SymbolId> symRemoved;
    for(int i=0; i<removed; i++) {
      auto id = _file.symbolAt(pos).getSymbolId();
      _file.localDelete(pos);
      symRemoved.push_back(id);
    }
    emit localDeleteQuery(_fileId, symRemoved);
}

  // aggiunte
  if(added > 0) {
    QTextCursor cursor(_textEdit->document());

    cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, pos+1);

    std::vector<Symbol> symAdded;
    for(int i=0; i<added; i++) {
      auto chr = _textEdit->document()->characterAt(pos + i);
      auto fmt = cursor.charFormat();

      cursor.movePosition(QTextCursor::NextCharacter);
      Symbol s{{_user.userId, _user.charId++}, chr, fmt};

      //debug(QString::fromStdString(s.to_string()));

      _file.localInsert(s, pos+i);
      symAdded.push_back(s);
    }
    emit localInsertQuery(_fileId, symAdded);
  }

  for(auto &user : _users) {
    user.second.cursor->updateCursorView();
  }

  _blockSignals = true; // per bloccare lo slot cursorChanged
}

void TextEdit::cursorChanged() {
  if(_blockSignals) {
    _blockSignals = false;
    return;
  }

  auto cursor = _textEdit->textCursor();

  if(cursor.hasSelection()) return; //non mando nulla se sto selezionando

  auto pos = saveCursorPosition(cursor);

  emit localMoveQuery(_fileId, pos.first, pos.second);

  debug("Cursore spostato in posizione " + QString::number(pos.second));
  debug("SymbolId alla sx: " + QString::fromStdString(pos.first.to_string()));

  // se sposto il cursore, devo modificare le azioni di conseguenza
  // esempio se mi sposto dove c'è il grassetto selezionato, deve esserci il pulsante cliccato
  _blockSignals = true;
  updateActions();
  _blockSignals = false;
}

void TextEdit::updateActions() {
  //TODO rivedere qua se si può fare meglio, considerando che ci sono più pulsanti poi
  // (grassetto, corsivo..) ed è oneroso/brutto gestirli tutti così
  auto font = _textEdit->currentFont();
  if(font.bold() != _actionTextBold->isChecked()) {
    _actionTextBold->activate(QAction::Trigger);
  }
}

void TextEdit::closeEvent(QCloseEvent *event) {
  debug("Chiusura editor");

  emit closeFileQuery(_fileId);

  reset();

  QWidget::closeEvent(event);
}

void TextEdit::showEvent(QShowEvent* event) {
  QWidget::showEvent(event);

  if(!_user.initialized) {
    throw SE_Exception{"Utente non settato."};
  }
}

void TextEdit::reset() {
  //TODO vedi se si può trovare un modo migliore per fare ciò

  //elimino caratteri
  clear();

  //elimino users
  for(auto &user : _users) {
    delete user.second.cursor;
    _dock->takeItem(_dock->row(user.second.item));
    delete user.second.item;
  }
  _users.clear();

  _blockSignals = false;
  _gen.reset();
}

// SLOT messaggi ricevuti da remoto
//INFO il fileId qui diamo per scontato che sia corretto, ma andrebbe fatto un check
// Serve soprattutto nel caso in cui l'app supporti più files aperti
//INFO ipotizzo che i vettori non siano vuoti (check fatto nelle localXXX e lato server)

void TextEdit::remoteInsertQuery(int fileId, int clientId, std::vector<Symbol> symbols) {
  int pos;
  auto cursor = _users.at(clientId).cursor;

  debug("Inserimento remoto di " + QString::number(symbols.size())
        + " caratteri dell'user " + QString::number(clientId));

  for(auto &sym : symbols) {
    _blockSignals = true;
    pos = _file.remoteInsert(sym);
    cursor->insert(sym, pos);
  }
  _blockSignals = false;
}

void TextEdit::remoteDeleteQuery(int fileId, int clientId, std::vector<SymbolId> ids) {
  int pos;
  auto cursor = _users.at(clientId).cursor;

  debug("Cancellazione remota di " + QString::number(ids.size())
        + " caratteri dell'user " + QString::number(clientId));

  for(auto &id : ids) {
    _blockSignals = true;
    pos = _file.remoteDelete(id);
    if(pos != -1) cursor->remove(pos);
  }
  _blockSignals = false;
}

void TextEdit::remoteUpdateQuery(int fileId, int clientId, std::vector<Symbol> symbols) {
  int pos;
  auto cursor = _users.at(clientId).cursor;

  debug("Update remoto di " + QString::number(symbols.size())
        + " caratteri dell'user " + QString::number(clientId));

  for(auto &sym : symbols) {
    pos = _file.remoteUpdate(sym);
    if(pos != -1) {
      _blockSignals = true;
      cursor->remove(pos);
      _blockSignals = true;
      cursor->insert(sym, pos);
    }
  }
  _blockSignals = false;
}

void TextEdit::userConnectedQuery(int fileId, int clientId, QString username) {
  info("New remote user: " + username + " " + QString::number(clientId));

  remoteUser user;
  user.userId = clientId;
  user.username = username;
  user.color = _gen.getColor();
  user.cursor = new Cursor(_textEdit, user.color);

  user.item = new QListWidgetItem(username, _dock);
  user.item->setFlags(Qt::NoItemFlags);
  user.item->setForeground(user.color);

  //TODO check nella mappa per vedere che non ci sia già
  _users[clientId] = user;
}

void TextEdit::userDisconnectedQuery(int fileId, int clientId) {
  info("User " + QString::number(clientId) + " disconnected");

  //TODO check nella mappa per vedere che ci sia
  auto user = _users[clientId];

  delete user.cursor;
  _dock->takeItem(_dock->row(user.item));
  delete user.item;

  _users.erase(clientId);
}

void TextEdit::remoteMoveQuery(int fileId, int clientId, SymbolId symbolId, int cursorPosition) {
  debug("User: " + QString::number(clientId) + " moved cursor");

  //TODO check nella mappa per vedere che ci sia
  auto user = _users[clientId];
  user.cursor->updateCursorPosition(getCursorPosition(symbolId, cursorPosition));
}

std::pair<SymbolId, int> TextEdit::saveCursorPosition(const QTextCursor &cursor) {
  int position = cursor.position();
  SymbolId id;

  if(position == 0) {
    id = {-1,-1}; //TODO magari si può fare meglio
  }
  else {
    id = _file.symbolAt(position-1).getSymbolId();
  }

  return {id, position};
}

int TextEdit::getCursorPosition(SymbolId id, int position) {
  int pos;

  try {
    pos = _file.symbolById(id).first + 1;
  }
  catch(FileSymbolsException e) {
    pos = position;
  }

  return pos;
}
