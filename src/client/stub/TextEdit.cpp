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
#include <QColorDialog>
#include <QApplication>

#include "Symbol.h"
#include "utils.h"
#include "exceptions.h"
#include "errordialog.h"

using namespace se_exceptions;

#ifdef Q_OS_MAC
const QString rsrcPath = ":/images/mac";
#else
const QString rsrcPath = ":/images/win";
#endif

TextEdit::TextEdit(QWidget *parent)
        : QMainWindow(parent), _blockSignals(false), _cursorPosition(0), _shareLink(std::nullopt)
{
  // QTextEdit sarà il nostro widget principale, contiene un box in cui inserire testo.
  // Inoltre avremo un menu, in cui ci saranno tutte le varie opzioni, e una toolbar che ne avrà un set
  _textEdit = new QTextEditImpl(this);
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
  QObject::connect(_textEdit, &QTextEdit::textChanged, this, &TextEdit::updateCursors);
  QObject::connect(_textEdit, &QTextEditImpl::resized, this, &TextEdit::updateCursors);
  QObject::connect(_textEdit, &QTextEditImpl::scrolled, this, &TextEdit::updateCursors);

  // _dock right
  initDock();
}

void TextEdit::setFile(const File &f, int charId) {
  if(!_user.initialized) {
    throw SE_Exception{"Utente non settato."};
  }

  _file = f;
  _shareLink = std::nullopt; //TODO quando sarà salvato nel file rimuovere
  _user.charId = charId;

  refresh(true);
}

void TextEdit::setConnectedUsers() {
  //setto gli utenti connessi
  for(auto &user : _file.getUsers()) {
    if (user.second.userId == _user.userId) continue;

    debug("User ID: " + QString::number(user.second.userId));
    debug("isOnline: " + QString::number(user.second.online));

    addRemoteUser(user.second.userId, user.second.username, user.second.online);
  }
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

void TextEdit::actionRefresh() {
  refresh();
}

void TextEdit::refresh(bool changeFile, bool highlight) {
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
    auto format = sym.getFormat();

    if(highlight) {
      auto userId = sym.getSymbolId().getUserId();
      auto optcolor = getRemoteUserColor(userId);
      if(optcolor) {
        auto &color = *optcolor;
        color.setAlpha(128);
        format.setBackground(color);
      }
    }

    cursor.setCharFormat(format);
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

void TextEdit::addRemoteUser(int userId, const QString &username, bool isOnline) {
  remoteUser user;
  user.userId = userId;
  user.username = username;
  user.color = _gen.getColor();
  user.isOnline = isOnline;
  user.cursor = new Cursor(_textEdit, user.color);

  if(isOnline) {
    user.item = new QListWidgetItem(username, _dockOnline);
  }
  else {
    user.item = new QListWidgetItem(username, _dockOffline);

  }

  user.item->setFlags(Qt::NoItemFlags);
  user.item->setForeground(user.color);

  // userId should not be present in map, as it is checked by the caller of this function
  _users[userId] = user;
}

void TextEdit::setRemoteUserOnline(int userId) {
  auto &user = _users[userId];

  user.isOnline = true;
  _dockOffline->takeItem(_dockOffline->row(user.item));
  _dockOnline->addItem(user.item);
}

void TextEdit::remoteUserConnected(int userId, const QString &username) {
  if(_users.count(userId) == 0) {
    debug("This user just activated the link for this file");
    addRemoteUser(userId, username);
    _users[userId].cursor->show();
  }
  else {
    debug("User already known, just setting his name visible");
    setRemoteUserOnline(userId);
  }
}

void TextEdit::remoteUserDisconnected(int userId) {
  if(_users.count(userId) == 0) {
    error("User " + QString::number(userId) + " not present in the list of users!!");
    return;
  }

  auto &user = _users[userId];

  user.isOnline = false;
  _dockOnline->takeItem(_dockOnline->row(user.item));
  _dockOffline->addItem(user.item);
  //TODO reset cursor to first position
}

std::optional<QColor> TextEdit::getRemoteUserColor(int userId) {
  if(_user.userId == userId) {
    return std::nullopt;
  }

  if(_users.count(userId) == 0) {
    error("User " + QString::number(userId) + " not present in the list of users!!");
    return std::nullopt;
  }

  return std::optional<QColor>(_users[userId].color);
}

void TextEdit::setUser(int userId, QString username) {
  debug("User initialized with userId: " + QString::number(userId));

  _user.initialized = true;
  _user.userId = userId;
  _user.username = username;
  _user.charId = 0;

  if(_user.item != nullptr) {
    _dockOnline->takeItem(_dockOnline->row(_user.item));
    delete _user.item;
  }

  _user.item = new QListWidgetItem(username + " (Tu)");
  _user.item->setFlags(Qt::NoItemFlags);
  _user.item->setForeground(QColor("black"));
  _dockOnline->insertItem(0, _user.item);
}

void TextEdit::initDock() {
  auto *dockWidget = new QDockWidget(tr("Utenti online:"), this);
  dockWidget->setAllowedAreas(Qt::RightDockWidgetArea);
  addDockWidget(Qt::RightDockWidgetArea, dockWidget);

  auto *dockWidget2 = new QDockWidget(tr("Utenti offline:"), this);
  dockWidget2->setAllowedAreas(Qt::RightDockWidgetArea);
  addDockWidget(Qt::RightDockWidgetArea, dockWidget2);

  _dockOnline = new QListWidget(dockWidget);
  dockWidget->setWidget(_dockOnline);

  _dockOffline = new QListWidget(dockWidget2);
  dockWidget2->setWidget(_dockOffline);
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

  const QIcon refreshIcon(":/buttons/refresh.png");
  QAction *refresh = menu->addAction(refreshIcon,  tr("&Refresh document"), this, &TextEdit::actionRefresh);
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
  const QIcon boldIcon = QIcon::fromTheme("format-text-bold", QIcon(rsrcPath + "/textbold.png"));
  _actionTextBold = menu->addAction(boldIcon, tr("&Bold"), this, &TextEdit::textBold);
  _actionTextBold->setShortcut(Qt::CTRL + Qt::Key_B);
  _actionTextBold->setPriority(QAction::LowPriority);
  QFont bold;
  bold.setBold(true);
  _actionTextBold->setFont(bold);
  tb->addAction(_actionTextBold);
  _actionTextBold->setCheckable(true);


  // AZIONE CORSIVO
  const QIcon italicIcon = QIcon::fromTheme("format-text-italic", QIcon(rsrcPath + "/textitalic.png"));
  _actionTextItalic = menu->addAction(italicIcon, tr("&Italic"), this, &TextEdit::textItalic);
  _actionTextItalic->setPriority(QAction::LowPriority);
  _actionTextItalic->setShortcut(Qt::CTRL + Qt::Key_I);
  QFont italic;
  italic.setItalic(true);
  _actionTextItalic->setFont(italic);
  tb->addAction(_actionTextItalic);
  _actionTextItalic->setCheckable(true);


  // AZIONE SOTTOLINEATO
  const QIcon underlineIcon = QIcon::fromTheme("format-text-underline", QIcon(rsrcPath + "/textunder.png"));
  _actionTextUnderline = menu->addAction(underlineIcon, tr("&Underline"), this, &TextEdit::textUnderline);
  _actionTextUnderline->setShortcut(Qt::CTRL + Qt::Key_U);
  _actionTextUnderline->setPriority(QAction::LowPriority);
  QFont underline;
  underline.setUnderline(true);
  _actionTextUnderline->setFont(underline);
  tb->addAction(_actionTextUnderline);
  _actionTextUnderline->setCheckable(true);


  // AZIONE EVIDENZIA TESTO UTENTI
  const QIcon highlightIcon(":/buttons/colors.png");
  _actionTextHighlight = menu->addAction(highlightIcon,  tr("&Highlight text"), this, &TextEdit::textHighlight);
  tb->addAction(_actionTextHighlight);
  _actionTextHighlight->setCheckable(true);


  menu->addSeparator();


  // COLOR
  QPixmap pix(16, 16);
  pix.fill(Qt::black);
  _actionTextColor = menu->addAction(pix, tr("&Color..."), this, &TextEdit::textColor);
  tb->addAction(_actionTextColor);


  // BACKGROUND COLOR
  const QIcon backgroundColorIcon(":/buttons/fill.png");
  _actionTextBackgroundColor = menu->addAction(backgroundColorIcon, tr("&BackgroundColor..."), this, &TextEdit::textBackgroundColor);
  tb->addAction(_actionTextBackgroundColor);


  // FONT
  _comboFont = new QFontComboBox(tb);
  _comboFont->setEditable(false);
  tb->addWidget(_comboFont);
  connect(_comboFont, QOverload<const QString &>::of(&QComboBox::activated), this, &TextEdit::textFamily);


  // FONT SIZE
  _comboSize = new QComboBox(tb);
  _comboSize->setObjectName("comboSize");
  tb->addWidget(_comboSize);
  _comboSize->setEditable(true);

  const QList<int> standardSizes = QFontDatabase::standardSizes();
  for (int size : standardSizes)
      _comboSize->addItem(QString::number(size));
  _comboSize->setCurrentIndex(standardSizes.indexOf(QApplication::font().pointSize()));

  connect(_comboSize, QOverload<const QString &>::of(&QComboBox::activated), this, &TextEdit::textSize);
}

void TextEdit::textBold() {
  if(_blockSignals) return;

  debug("Premuto tasto grassetto");

  QTextCursor cursor = _textEdit->textCursor();
  auto fmt = cursor.charFormat();
  fmt.setFontWeight(_actionTextBold->isChecked() ? QFont::Bold : QFont::Normal);

  _textEdit->mergeCurrentCharFormat(fmt);
}

void TextEdit::textItalic() {
  if(_blockSignals) return;

  debug("Premuto tasto corsivo");

  QTextCursor cursor = _textEdit->textCursor();
  auto fmt = cursor.charFormat();
  fmt.setFontItalic(_actionTextItalic->isChecked() ? true : false);

  _textEdit->mergeCurrentCharFormat(fmt);
}

void TextEdit::textUnderline() {
  if(_blockSignals) return;

  debug("Premuto tasto sottolineato");

  QTextCursor cursor = _textEdit->textCursor();
  auto fmt = cursor.charFormat();
  fmt.setFontUnderline(_actionTextUnderline->isChecked() ? true : false);

  _textEdit->mergeCurrentCharFormat(fmt);
}

void TextEdit::textHighlight() {
  if (_actionTextHighlight->isChecked())
    enableTextHighlight();
  else
    disableTextHighlight();
}

void TextEdit::disableTextHighlight() {
  debug("Disable text highlight");
  refresh();
}

void TextEdit::enableTextHighlight() {
  debug("Enable text highlight");
  refresh(false, true);
}

void TextEdit::textColor() {
  if(_blockSignals) return;

  debug("Cambiato colore");
  QColor col = QColorDialog::getColor(_textEdit->textColor(), this, "Select color", QColorDialog::ShowAlphaChannel);
  if (!col.isValid())
      return;

  QTextCursor cursor = _textEdit->textCursor();
  auto fmt = cursor.charFormat();
  fmt.setForeground(col);

  _textEdit->mergeCurrentCharFormat(fmt);
}

void TextEdit::textBackgroundColor() {
  if(_blockSignals) return;

  debug("Cambiato colore sfondo");
  QTextCursor cursor = _textEdit->textCursor();
  auto fmt = cursor.charFormat();

  QColor col = QColorDialog::getColor(_textEdit->textColor(), this, "Select color", QColorDialog::ShowAlphaChannel);
  if (!col.isValid())
      return;

  fmt.setBackground(col);

  _textEdit->mergeCurrentCharFormat(fmt);
}

void TextEdit::textFamily(const QString &f) {
  if(_blockSignals) return;

  debug("Cambiato font");

  QTextCursor cursor = _textEdit->textCursor();
  auto fmt = cursor.charFormat();
  fmt.setFontFamily(f);

  _textEdit->mergeCurrentCharFormat(fmt);
  _textEdit->setFocus();
}

void TextEdit::textSize(const QString &p) {
  if(_blockSignals) return;

  debug("Cambiato font size");

  qreal pointSize = p.toFloat();
  if (pointSize > 0) {
    QTextCursor cursor = _textEdit->textCursor();
    auto fmt = cursor.charFormat();
    fmt.setFontPointSize(pointSize);

    _textEdit->mergeCurrentCharFormat(fmt);
  }

  _textEdit->setFocus();
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

    // update actions
    updateActions();
  }

  // update _cursorPosition
  _cursorPosition = pos + added;
}

void TextEdit::handleUpdate(int pos, int nchars) {
  if(nchars == 0) return;

  QTextCursor cursor(_textEdit->document());
  std::vector<Symbol> symUpdated;
  bool real = false;

  cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, pos);
  for(int i=0; i<nchars; i++) {
    cursor.movePosition(QTextCursor::NextCharacter); // must be moved BEFORE catching the correct format

    auto chr = _textEdit->document()->characterAt(pos + i);
    auto fmt = cursor.charFormat();

    auto &sym = _file.symbolAt(pos + i);

    if(!sym.hasSameAttributes(chr, fmt)) {
      sym.setFormat(fmt);
      sym.setChar(chr);
      real = true;
    }

    symUpdated.push_back(sym);
  }

  if(real) {
    emit localUpdateQuery(_fileId, symUpdated);
  }
  else {
    debug("Fake update");
  }
}

void TextEdit::handleDelete(int pos, int removed) {
  if(removed == 0) return;

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
  if(added == 0) return;

  QTextCursor cursor(_textEdit->document());
  std::vector<Symbol> symAdded;

  cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, pos);
  for(int i=0; i<added; i++) {
    cursor.movePosition(QTextCursor::NextCharacter); // must be moved BEFORE catching the correct format

    auto chr = _textEdit->document()->characterAt(pos + i);
    auto fmt = cursor.charFormat();

    if(chr == 0) {
      cursor.deletePreviousChar(); //TODO check. delete null characters
    }
    else {
      Symbol s{{_user.userId, _user.charId++}, chr, fmt};
      //debug(QString::fromStdString(s.to_string()));

      _file.localInsert(s, pos+i);
      symAdded.push_back(s);
    }
  }

  emit localInsertQuery(_fileId, symAdded);
}

void TextEdit::cursorChanged() {
  //debug("cursorChanged triggered");
  if(_blockSignals) return;

  auto cursor = _textEdit->textCursor();

  if(cursor.hasSelection()) return; //non mando nulla se sto selezionando

  // TODO se crasha vuol dire che c'è un problema da qualche parte da risolvere
  try {
    auto pos = saveCursorPosition(cursor);

    if(_cursorPosition == pos.second) return; // ho già registrato questo spostamento, non mando nulla

    emit localMoveQuery(_fileId, pos.first, pos.second);

    debug("Cursore spostato in posizione " + QString::number(pos.second));
    debug("SymbolId alla sx: " + QString::fromStdString(pos.first.to_string()));

    // se sposto il cursore, devo modificare le azioni di conseguenza
    // esempio se mi sposto dove c'è il grassetto selezionato, deve esserci il pulsante cliccato
    updateActions();
    _cursorPosition = pos.second;
  }
  catch (FileSymbolsException e) {
    warn("cursorChanged ha lanciato una FileSymbolsException");
  }
}

void TextEdit::formatChanged() {
  //debug("Format changed");
}

void TextEdit::updateActions() {
  _blockSignals = true;

  auto fmt = _textEdit->currentCharFormat();
  _actionTextBold->setChecked(fmt.font().bold());
  _actionTextItalic->setChecked(fmt.fontItalic());
  _actionTextUnderline->setChecked(fmt.fontUnderline());

  QPixmap pix(16, 16);
  pix.fill(fmt.foreground().color());
  _actionTextColor->setIcon(pix);

  _comboFont->setCurrentText(fmt.font().family());
  _comboSize->setCurrentIndex(QFontDatabase::standardSizes().indexOf(fmt.font().pointSize()));

  _blockSignals = false;
}

void TextEdit::closeEvent(QCloseEvent *event) {
  debug("Chiusura editor");

  emit closeFileQuery(_fileId);

  reset();

  QWidget::closeEvent(event);
}

void TextEdit::reset() {
  //TODO vedi se si può trovare un modo migliore per fare ciò

  //elimino caratteri
  clear();

  //elimino users
  for(auto &user : _users) {
    delete user.second.cursor;

  }
  _users.clear();

  //clear dock widgets
  auto me = _dockOnline->takeItem(0);
  _dockOnline->clear();
  _dockOffline->clear();
  _dockOnline->addItem(me);

  // reset value
  _blockSignals = false;
  _cursorPosition = 0;
  _gen.reset();

  //TODO reset actions (pulsanti non checked etc)
}

// SLOT messaggi ricevuti da remoto
//INFO il fileId qui diamo per scontato che sia corretto, ma andrebbe fatto un check
// Serve soprattutto nel caso in cui l'app supporti più files aperti
//INFO ipotizzo che i vettori non siano vuoti (check fatto nelle localXXX e lato server)

void TextEdit::remoteInsertQuery(int fileId, int userId, std::vector<Symbol> symbols) {
  _blockSignals = true;
  int pos;

  //TODO non deve succedere, ma se il userId non esiste crasha tutto.. idem delete e update
  debug("userid: " + QString::number(userId));
  auto cursor = _users.at(userId).cursor;

  debug("Inserimento remoto di " + QString::number(symbols.size())
        + " caratteri dell'user " + QString::number(userId));

  for(auto &sym : symbols) {
    pos = _file.remoteInsert(sym);
    cursor->insert(sym, pos);
  }

  cursor->show();
  _blockSignals = false;
}

void TextEdit::remoteDeleteQuery(int fileId, int userId, std::vector<SymbolId> ids) {
  _blockSignals = true;

  int pos;
  auto cursor = _users.at(userId).cursor;

  debug("Cancellazione remota di " + QString::number(ids.size())
        + " caratteri dell'user " + QString::number(userId));

  for(auto &id : ids) {
    pos = _file.remoteDelete(id);
    if(pos != -1) cursor->remove(pos);
  }

  cursor->show();
  _blockSignals = false;
}

void TextEdit::remoteUpdateQuery(int fileId, int userId, std::vector<Symbol> symbols) {
  _blockSignals = true;
  int pos;
  auto cursor = _users.at(userId).cursor;

  debug("Update remoto di " + QString::number(symbols.size())
        + " caratteri dell'user " + QString::number(userId));

  for(auto &sym : symbols) {
    pos = _file.remoteUpdate(sym);
    if(pos != -1) {
      cursor->remove(pos);
      cursor->insert(sym, pos);
    }
  }

  cursor->show();
  _blockSignals = false;
}

void TextEdit::updateCursors() {
  for(auto &user : _users) {
    user.second.cursor->updateCursorView();
  }
}

void TextEdit::userConnectedQuery(int fileId, int userId, QString username) {
  info("New remote user: " + username + " " + QString::number(userId));

  remoteUserConnected(userId, username);
}

void TextEdit::userDisconnectedQuery(int fileId, int userId) {
  info("User " + QString::number(userId) + " disconnected");

  remoteUserDisconnected(userId);
}

void TextEdit::remoteMoveQuery(int fileId, int userId, SymbolId symbolId, int cursorPosition) {
  debug("User: " + QString::number(userId) + " moved cursor");

  //TODO check nella mappa per vedere che ci sia
  if(_users.count(userId) == 0) {
    error("User " + QString::number(userId) + " not present in the list of users!!");
    return;
  }

  auto cursor = _users[userId].cursor;
  cursor->updateCursorPosition(getCursorPosition(symbolId, cursorPosition));
  cursor->show();
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
