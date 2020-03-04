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
#include <optional>

#include "Symbol.h"
#include "utils.h"
#include "exceptions.h"
#include "errordialog.h"

using namespace se_exceptions;

TextEdit::TextEdit(QWidget *parent)
        : QMainWindow(parent), _blockSignals(false), _cursorPosition(0), _shareLink(std::nullopt)
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
  QObject::connect(_textEdit, &QTextEdit::currentCharFormatChanged, this, &TextEdit::formatChanged);

  // _dock right
  initDock();
}

void TextEdit::setFile(const File &f, int charId) {
  _file = f;
  _shareLink = std::nullopt; //TODO quando sarà salvato nel file rimuovere
  _user.charId = charId;


  //setto gli utenti connessi
  for(auto &client : f.getClients()) {
    //TODO filtrare me stesso

    if (client.second.username == _user.username) continue; //TODO check userId dopo refactor

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
  _blockSignals = true;

  //salvo pos del cursore mio
  std::pair<SymbolId, int> pos;

  if(!changeFile)
    pos = saveCursorPosition(_textEdit->textCursor());

  //cancello
  _textEdit->document()->clear();

  //refresh
  QTextCursor cursor{_textEdit->document()};
  for(auto &sym : _file.getSymbols()) {
    cursor.setCharFormat(sym.getFormat());
    cursor.insertText(sym.getChar());
  }

  //ripristino posizione
  if(!changeFile) {
    cursor.setPosition(getCursorPosition(pos.first, pos.second));
    _textEdit->setTextCursor(cursor);
    _cursorPosition = pos.second;
  }

  _blockSignals = false;
}

void TextEdit::share() {
  if(!_shareLink) { //TODO quando sarà nella classe file cambiare
    emit getLinkQuery(_fileId);
  }
  else {
    showShareLink();
  }
}

void TextEdit::setShareLink(QString shareLink) {
  _shareLink = std::optional<QString>(shareLink);
}

void TextEdit::showShareLink() {
  if(!_shareLink) {
    error("chiamato showShareLink ma _shareLink non settato");
  }
  else {
    ErrorDialog::showDialog(this, *_shareLink); //TODO non errordialog ma dialog normale..
  }
}

void TextEdit::clear() {
  _blockSignals = true;

  _textEdit->document()->clear();
  _file.clear();

  for(auto &user : _users) {
    user.second.cursor->updateCursorView();
  }

  _cursorPosition = 0;
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

  int nsym = _file.numSymbols();
  debug("contentsChange triggered");
  debug("Num symbols before change: " + QString::number(nsym));
  debug("pos: " + QString::number(pos) + " removed: " + QString::number(removed) + " added: " + QString::number(added));

  // fix: a volte capita non so perchè questa cosa.
  // viene segnalato un numero di caratteri rimossi maggiore dei caratteri presenti
  if(nsym < removed + pos) {
    debug("ANOMALY detected!");
    int shift = removed - (nsym - pos);

    if(shift > 0) {
      removed -= shift;
      added -= shift;
    }

    debug("New Values: removed: " + QString::number(removed) + " added: " + QString::number(added));
  }

  //TODO se removed == added -> UPDATE
  //TODO si potrebbe fare anche una cosa più elaborata,
  //    tipo 2 rem e 3 added significa 2 update e 1 added, ma non so se vale la pena
  if(removed == added) {
    debug("Handling update");
    handleUpdate(pos, removed);
  }
  else {
    handleDelete(pos, removed);
    handleInsert(pos, added);
  }

  // update remote cursors
  updateCursors();
  // update _cursorPosition
  _cursorPosition = pos + added;
}

void TextEdit::handleUpdate(int pos, int nchars) {
  QTextCursor cursor(_textEdit->document());
  std::vector<Symbol> symUpdated;

  for(int i=0; i<nchars; i++) {
    auto chr = _textEdit->document()->characterAt(pos + i);
    auto fmt = cursor.charFormat();

    auto sym = _file.symbolAt(pos + i);
    sym.setFormat(fmt);
    sym.setChar(chr);

    symUpdated.push_back(sym);

    cursor.movePosition(QTextCursor::NextCharacter);
  }

  emit localUpdateQuery(_fileId, symUpdated);
}

void TextEdit::handleDelete(int pos, int removed) {
  std::vector<SymbolId> symRemoved;

  for(int i=0; i<removed; i++) {
      try {
        auto id = _file.symbolAt(pos).getSymbolId();
        _file.localDelete(pos);
        symRemoved.push_back(id);
      }
      catch(...) {
        throw SE_Exception("handleDelete: tentata rimozione di un carattere a una posizione non valida nel file");
      }
  }

  emit localDeleteQuery(_fileId, symRemoved);
}

void TextEdit::handleInsert(int pos, int added) {
  QTextCursor cursor(_textEdit->document());
  std::vector<Symbol> symAdded;

  for(int i=0; i<added; i++) {
    auto chr = _textEdit->document()->characterAt(pos + i);
    auto fmt = cursor.charFormat();

    if(chr == 0) {
      cursor.deleteChar(); //TODO check. delete null characters
    }
    else {
      Symbol s{{_user.userId, _user.charId++}, chr, fmt};
      //debug(QString::fromStdString(s.to_string()));

      _file.localInsert(s, pos+i);
      symAdded.push_back(s);

      cursor.movePosition(QTextCursor::NextCharacter);
    }
  }

  emit localInsertQuery(_fileId, symAdded);
}

void TextEdit::cursorChanged() {
  auto cursor = _textEdit->textCursor();

  if(_blockSignals) return;

  if(cursor.hasSelection()) return; //non mando nulla se sto selezionando

  // TODO a volte crasha a causa di contentsChange triggerati a cazzo
  try {
    auto pos = saveCursorPosition(cursor);

    if(_cursorPosition == pos.second) return; // ho già registrato questo spostamento, non mando nulla

    emit localMoveQuery(_fileId, pos.first, pos.second);

    debug("Cursore spostato in posizione " + QString::number(pos.second));
    debug("SymbolId alla sx: " + QString::fromStdString(pos.first.to_string()));

    // se sposto il cursore, devo modificare le azioni di conseguenza
    // esempio se mi sposto dove c'è il grassetto selezionato, deve esserci il pulsante cliccato
    _blockSignals = true;
    updateActions();
    _blockSignals = false;

  }
  catch (FileSymbolsException e) {
    warn("cursorChanged ha lanciato una FileSymbolsException");
    return;
  }
}

void TextEdit::formatChanged() {
  debug("Format changed");
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

  // reset value
  _blockSignals = false;
  _cursorPosition = 0;
  _gen.reset();
}

// SLOT messaggi ricevuti da remoto
//INFO il fileId qui diamo per scontato che sia corretto, ma andrebbe fatto un check
// Serve soprattutto nel caso in cui l'app supporti più files aperti
//INFO ipotizzo che i vettori non siano vuoti (check fatto nelle localXXX e lato server)

void TextEdit::remoteInsertQuery(int fileId, int clientId, std::vector<Symbol> symbols) {
  _blockSignals = true;
  int pos;

  //TODO non deve succedere, ma se il clientId non esiste crasha tutto.. idem delete e update
  auto cursor = _users.at(clientId).cursor;

  debug("Inserimento remoto di " + QString::number(symbols.size())
        + " caratteri dell'user " + QString::number(clientId));

  for(auto &sym : symbols) {
    pos = _file.remoteInsert(sym);
    cursor->insert(sym, pos);
  }

  updateCursors();

  _blockSignals = false;
}

void TextEdit::remoteDeleteQuery(int fileId, int clientId, std::vector<SymbolId> ids) {
  _blockSignals = true;

  int pos;
  auto cursor = _users.at(clientId).cursor;

  debug("Cancellazione remota di " + QString::number(ids.size())
        + " caratteri dell'user " + QString::number(clientId));

  for(auto &id : ids) {
    pos = _file.remoteDelete(id);
    if(pos != -1) cursor->remove(pos);
  }

  updateCursors();

  _blockSignals = false;
}

void TextEdit::remoteUpdateQuery(int fileId, int clientId, std::vector<Symbol> symbols) {
  _blockSignals = true;
  int pos;
  auto cursor = _users.at(clientId).cursor;

  debug("Update remoto di " + QString::number(symbols.size())
        + " caratteri dell'user " + QString::number(clientId));

  for(auto &sym : symbols) {
    pos = _file.remoteUpdate(sym);
    if(pos != -1) {
      cursor->remove(pos);
      cursor->insert(sym, pos);
    }
  }

  updateCursors();

  _blockSignals = false;
}

void TextEdit::updateCursors() {
  for(auto &user : _users) {
    user.second.cursor->updateCursorView();
  }
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
    id = _file.symbolAt(position-1).getSymbolId(); //TODO potenziale EXCEPTION
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
