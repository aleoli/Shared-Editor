#include "texteditor.h"

#include "utils.h"
#include "exceptions.h"

using namespace se_exceptions;

void TextEditor::_contentsChange(int pos, int removed, int added) {
  if(_blockSignals) return;

  int nsym = _file->numSymbols();

  // fix: segnalato un numero di caratteri rimossi maggiore dei caratteri presenti
  if(nsym < removed + pos) {
    //debug("ANOMALY detected!");
    int shift = removed - (nsym - pos);
    if(shift > 0) {
      removed -= shift;
      added -= shift;
    }
    //debug("New Values: removed: " + QString::number(removed) + " added: " + QString::number(added));
  }

  auto it = _file->iteratorAt(pos);
  switch(_checkOperation(pos,removed, added, it)) {
    case 0:
      debug("Phantom update");
      break;
    case 1:
      _handleDeleteInsert(pos, removed, added, it);
      break;
    case 2:
      _handleUpdate(pos, added, it);
      break;
  }
  _cursorPosition = pos + added;
}

// 0: phantom update | 1: delete/insert | 2: update
int TextEditor::_checkOperation(int pos, int removed, int added, std::list<Symbol>::iterator it) {
  if(pos < 0 || removed < 0 || added < 0) return 0;
  if(removed != added) return 1;
  if(_updateSyms || _updateAlignment) return 2;
  if(removed == 0) return 0;

  auto doc = _textEdit->document();
  QTextCursor cursor(doc);
  cursor.setPosition(pos);
  bool update = false;
  for(int i=0; i<removed; i++) {
    cursor.movePosition(QTextCursor::NextCharacter);
    auto chr = doc->characterAt(pos + i);
    auto fmt = cursor.charFormat();

    if(it->getChar() != chr) return 1;
    if(!Symbol::compareFormats(it->getFormat(), fmt, _highlighted)) update = true;

    it = std::next(it);
  }

  if(!update && _undoredo) {
    //update alignment
    _updateAlignment = true;
    _undoredo = false;
    return 2;
  }

  if(update && _undoredo) {
    //update symbols
    _updateSyms = true;
    _undoredo = false;
    return 2;
  }

  //phantom update
  return 0;
}

void TextEditor::_handleDeleteInsert(int pos, int removed, int added, std::list<Symbol>::iterator &it) {
  debug("TextEditor::_contentsChange | pos: " + QString::number(pos) + " removed: " + QString::number(removed) + " added: " + QString::number(added));

  if(removed > 0) _handleDelete(pos, removed, it);

  if(added > 0) {
    if(removed > 0) it = _file->iteratorAt(pos); // because it is not valid anymore
    _handleInsert(pos, added, it);
  }

  updateActions();
}

void TextEditor::_handleDelete(int pos, int removed, std::list<Symbol>::iterator &it) {
  std::list<Identifier> symRemoved, parRemoved;
  auto doc = _textEdit->document();

  for(int i=0; i<removed; i++) {
      try {
        auto id = it->getSymbolId();
        _file->localDelete(pos, &it);
        symRemoved.push_back(id);
      }
      catch(...) {
        throw TextEditorException("handleDelete: try to delete at an invalid position in File");
      }
  }

  // check alignment
  auto numPars = numParagraphs();
  if(numPars < _nblocks) {
    auto deleted = _nblocks - numPars;
    _nblocks = numPars;
    auto par = paragraphByPos(pos) + 1;
    auto it = _file->paragraphAt(par);

    for(int i=0; i<deleted; i++) {
      try {
        auto id = it->getParagraphId();
        _file->localDeleteParagraph(par, &it); //it is incremented by one
        parRemoved.push_back(id);
      }
      catch(...) {
        throw TextEditorException("handleDelete: try to delete a paragraph at an invalid position in File");
      }
    }
  }

  emit localDelete(_user->getToken(), _user->getFileId(), symRemoved, parRemoved);
}

void TextEditor::_handleInsert(int pos, int added, std::list<Symbol>::iterator &it) {
  auto doc = _textEdit->document();
  QTextCursor cursor(doc);
  std::list<Symbol> symAdded;
  std::list<Paragraph> parAdded, parUpdated;

  // check alignment
  auto numPars = numParagraphs();
  if(_nblocks < numPars) {
    auto added = numPars - _nblocks;
    _nblocks = numPars;
    auto parPos = paragraphByPos(pos) + 1;
    auto parIt = _file->paragraphAt(parPos);

    for(int i=0; i<added; i++) {
      try {
        Paragraph p{{_user->getUserId(), _user->getCharId()}, formatByBlock(parPos+i)}; //TODO new counter: paragraphId
        _file->localInsertParagraph(p, parPos+i, &parIt);
        parAdded.push_back(p);
      }
      catch(...) {
        throw TextEditorException("handleInsert: try to insert a paragraph at an invalid position in File");
      }
    }
  }

  // check symbols
  cursor.setPosition(pos);
  auto parPos = paragraphByPos(pos);
  auto parIt = _file->paragraphAt(parPos);
  int lastPar = -1;

  for(int i=0; i<added; i++) {
    cursor.movePosition(QTextCursor::NextCharacter); // must be moved BEFORE catching the correct format

    auto chr = doc->characterAt(pos + i);
    auto fmt = cursor.charFormat();

    if(chr == nullptr) {
      cursor.deletePreviousChar(); //TODO check. delete null characters
    }
    else {
      if(_highlighted) fmt.setBackground(_defColor);
      Symbol s{{_user->getUserId(), _user->getCharId()}, chr, fmt};
      _file->localInsert(s, pos+i, &it);
      symAdded.push_back(s);
    }

    // check if update paragraph happened
    auto curPar = paragraphByPos(pos + i);
    if(curPar != lastPar) {
      auto format = formatByBlock(curPar);
      auto res = _file->localUpdateParagraph(format, curPar, &parIt);
      if(res.second) parUpdated.push_back(*res.first);

      if(res.first != _file->getParagraphsEnd()) {
        _blockSignals = true;
        cursor.setBlockFormat(res.first->getPartialFormat());
        _blockSignals = false;
      }
      lastPar = curPar;
    }
  }

  if(_highlighted) {
    _partialRefresh(pos, added);
  }

  emit localInsert(_user->getToken(), _user->getFileId(), symAdded, parAdded);
  if(!parUpdated.empty()) {
    auto timestamp = QDateTime::currentDateTimeUtc();
    emit localUpdate(_user->getToken(), _user->getFileId(), std::list<Symbol>(), parUpdated, timestamp);
  }
}

void TextEditor::_handleUpdate(int pos, int added, std::list<Symbol>::iterator &it) {
  if(_updateSyms) {
    _handleSymbolUpdate(pos, added, it);
    _updateSyms = false;
  }
  else if(_updateAlignment) {
    _handleAlignmentUpdate(pos, added);
    _updateAlignment = false;
  }
  else {
    throw TextEditorException{"Don't know what to do"};
  }

  updateActions();
}

void TextEditor::_handleSymbolUpdate(int pos, int added, std::list<Symbol>::iterator &it) {
  debug("Symbol update");
  auto doc = _textEdit->document();
  QTextCursor cursor(doc);
  std::list<Symbol> symUpdated;

  cursor.setPosition(pos);
  for(int i=0; i<added; i++) {
    cursor.movePosition(QTextCursor::NextCharacter); // must be moved BEFORE catching the correct format

    auto fmt = cursor.charFormat();
    auto opt = _file->localUpdate(fmt, pos+i, _highlighted, &it);

    if(opt) symUpdated.push_back(**opt);
  }

  if(!symUpdated.empty()) {
    auto timestamp = QDateTime::currentDateTimeUtc();
    emit localUpdate(_user->getToken(), _user->getFileId(), symUpdated, std::list<Paragraph>(), timestamp);
  }
}

void TextEditor::_handleAlignmentUpdate(int pos, int added) {
  debug("Alignment update");
  std::list<Paragraph> parUpdated;

  int startPar = paragraphByPos(pos);
  int endPar = paragraphByPos(pos + added);
  if(endPar == startPar) endPar++;
  auto it = _file->paragraphAt(startPar);

  //debug("Start par: " + QString::number(startPar) + " end: " + QString::number(endPar));

  for(int i=startPar; i<=endPar; i++) {
    auto blockFmt = formatByBlock(i);
    auto res = _file->localUpdateParagraph(blockFmt, i, &it);

    if(res.second) parUpdated.push_back(*res.first);
  }

  //debug("Size: " + QString::number(parUpdated.size()));
  if(!parUpdated.empty()) {
    auto timestamp = QDateTime::currentDateTimeUtc();
    emit localUpdate(_user->getToken(), _user->getFileId(), std::list<Symbol>(), parUpdated, timestamp);
  }
}

void TextEditor::_partialRefresh(int pos, int added) {
  _blockSignals = true;

  auto doc = _textEdit->document();
  QTextCursor cursor(doc);
  cursor.setPosition(pos);
  QTextCharFormat fmt;
  QString text = "";

  for(int i=0; i<added; i++) {
    cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor); // must be moved BEFORE catching the correct format
    auto chr = doc->characterAt(pos + i);
    auto format = cursor.charFormat();

    if(format != fmt) {
      if(!text.isEmpty()) {
        fmt.setBackground(_defColor);
        cursor.removeSelectedText();
        cursor.setCharFormat(fmt);
        cursor.insertText(text);
        pos -= text.size();
        text.clear();
      }

      fmt = format;
    }

    text.append(chr);
  }

  if(!text.isEmpty()) {
    fmt.setBackground(_defColor);
    cursor.removeSelectedText();
    cursor.setCharFormat(fmt);
    cursor.insertText(text);
  }

  _blockSignals = false;
}


void TextEditor::_cursorChanged() {
  if(_blockSignals) return;

  auto cursor = _textEdit->textCursor();
  if(cursor.hasSelection()) return; //non mando nulla se sto selezionando

  // se crasha vuol dire che c'è un problema da qualche parte da risolvere
  try {
    if(_cursorPosition == cursor.position()) return; // ho già registrato questo spostamento, non mando nulla

    debug("TextEditor::_cursorChanged");
    auto pos = saveCursorPosition(cursor);
    emit localMove(_user->getToken(), _user->getFileId(), pos.first, pos.second);

    //debug("Cursore spostato in posizione " + QString::number(pos.second));
    //debug("SymbolId alla sx: " + QString::fromStdString(pos.first.to_string()));

    updateActions();
    _cursorPosition = pos.second;
  }
  catch (FileSymbolsException& e) {
    error("TextEditor::_cursorChanged exception: TO SOLVE!");
  }
}

void TextEditor::_updateCursors() {
  for(auto &user : _users) {
    user.second->getCursor()->updateCursorView();
  }
}

// Messages from server

void TextEditor::remoteInsert(int fileId, int userId, const std::list<Symbol>& symbols, const std::list<Paragraph> &paragraphs) {
  _blockSignals = true;
  int pos = 0;
  auto backgroundColor = _highlighted ? std::optional<QColor>(getUserColorHighlight(userId)) : std::nullopt;

  if(_users.count(userId) == 0) {
    throw TextEditorException{"User " + QString::number(userId) + " not present in the list of users!!"};
  }

  debug("Remote insert of  " + QString::number(symbols.size()) + " chars from user " + QString::number(userId));

  auto cursor = _users[userId]->getCursor();
  auto it = _file->getSymbolsStart();
  int tempPos = 0;
  cursor->goTo(0);
  QString text = "";
  QTextCharFormat fmt;
  for(auto &sym : symbols) {
    auto newpos = _file->remoteInsert(sym, &it, pos);
    auto symFmt = sym.getFormat();

    // se i caratteri inseriti non sono consecutivi o hanno formato diverso
    if(newpos - pos != 1 || symFmt != fmt) {
      cursor->insert(text, fmt, backgroundColor);
      cursor->goTo(newpos);
      text.clear();
    }

    text.append(sym.getChar());
    pos = newpos;
    fmt = symFmt;
  }
  if(!text.isEmpty()) {
    cursor->insert(text, fmt, backgroundColor);
  }

  _remoteInsertParagraphs(paragraphs);

  cursor->updateCursorView();
  cursor->show();
  _blockSignals = false;
}

void TextEditor::remoteDelete(int fileId, int userId, const std::list<Identifier>& ids, const std::list<Identifier> &paragraphs) {
  _blockSignals = true;
  int pos = 0;

  if(_users.count(userId) == 0) {
    throw TextEditorException{"User " + QString::number(userId) + " not present in the list of users!!"};
  }

  debug("Remote delete of " + QString::number(ids.size())
        + " chars from user " + QString::number(userId));

  auto cursor = _users[userId]->getCursor();
  cursor->clearSelection();
  auto it = _file->getSymbolsStart();
  int tempPos = 0;
  cursor->goTo(0);
  for(auto &id : ids) {
    pos = _file->remoteDelete(id, &it, tempPos);
    if(pos == -1) continue;

    if(pos != tempPos) {
      cursor->removeSelected();
      cursor->goTo(pos);
      tempPos = pos;
    }

    cursor->selectNext();
  }
  cursor->removeSelected();

  _remoteDeleteParagraphs(paragraphs);
  cursor->updateCursorView();
  cursor->show();
  _blockSignals = false;
}

void TextEditor::remoteUpdate(int fileId, int userId, const std::list<Symbol>& symbols, const std::list<Paragraph> &paragraphs, const QDateTime &timestamp) {
  _blockSignals = true;
  int pos = 0;
  auto backgroundColor = getUserColorHighlight(userId);

  if(_users.count(userId) == 0) {
    throw TextEditorException{"User " + QString::number(userId) + " not present in the list of users!!"};
  }

  debug("Remote update of " + QString::number(symbols.size())
        + " chars from user " + QString::number(userId));

  auto cursor = _users[userId]->getCursor();
  auto it = _file->getSymbolsStart();
  QTextCharFormat fmt;
  cursor->goTo(0);
  for(auto &sym : symbols) {
    int newpos = _file->remoteUpdate(sym, userId, timestamp, &it, pos); //pos of current char
    auto newfmt = it->getFormat();
    if(newpos == -1) continue;

    if(newpos - pos != 0 || !Symbol::compareFormats(fmt, newfmt)) {
      if(_highlighted) fmt.setBackground(getUserColorHighlight(it->getSymbolId().getFirst()));
      cursor->mergeCharFormat(fmt);
      cursor->clearSelection();
      cursor->goTo(newpos);
    }

    cursor->selectNext();
    std::advance(it, 1);
    pos = newpos + 1;
    fmt = newfmt;
  }

  if(cursor->hasSelection()) {
    if(_highlighted) fmt.setBackground(backgroundColor);
    cursor->mergeCharFormat(fmt);
    cursor->clearSelection();
  }

  _remoteUpdateParagraphs(paragraphs, userId, timestamp);
  cursor->updateCursorView();
  cursor->show();
  _blockSignals = false;
}

void TextEditor::remoteMove(int fileId, int userId, const SymbolId &symbolId, int cursorPosition) {
  debug("User: " + QString::number(userId) + " moved cursor");

  if(_users.count(userId) == 0) {
    throw TextEditorException{"User " + QString::number(userId) + " not present in the list of users!!"};
  }

  auto cursor = _users[userId]->getCursor();
  cursor->updateCursorPosition(getCursorPosition(symbolId, cursorPosition));
  cursor->show();
}

// paragraphs

int TextEditor::paragraphByPos(int pos) {
  return _textEdit->document()->findBlock(pos).blockNumber();
}

int TextEditor::numParagraphs() {
  return _textEdit->document()->blockCount();
}

QTextBlockFormat TextEditor::formatByPos(int pos) {
  return _textEdit->document()->findBlock(pos).blockFormat();
}

QTextBlockFormat TextEditor::formatByBlock(int blockNumber) {
  return _textEdit->document()->findBlockByNumber(blockNumber).blockFormat();
}

void TextEditor::setFormatInBlock(int blockNumber, const QTextBlockFormat &fmt) {
  auto cursor = QTextCursor(_textEdit->document());
  cursor.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor, blockNumber);
  cursor.mergeBlockFormat(fmt);
}

void TextEditor::_remoteInsertParagraphs(const std::list<Paragraph> &paragraphs) {
  int pos = 0;
  auto it = _file->getParagraphsStart();

  for(auto &par : paragraphs) {
    pos = _file->remoteInsertParagraph(par, &it, pos);
    setFormatInBlock(pos, par.getFormat());
  }
}

void TextEditor::_remoteDeleteParagraphs(const std::list<ParagraphId> &paragraphs) {
  auto it = _file->getParagraphsStart();

  for(auto &par : paragraphs) {
    _file->remoteDeleteParagraph(par, &it, 0);
  }
}

void TextEditor::_remoteUpdateParagraphs(const std::list<Paragraph> &paragraphs, int userId, const QDateTime &timestamp) {
  int pos = 0;
  auto it = _file->getParagraphsStart();

  for(auto &par : paragraphs) {
    int newpos = _file->remoteUpdateParagraph(par, userId, timestamp, &it, pos);
    if(newpos == -1) continue;

    setFormatInBlock(newpos, par.getFormat());
    pos = newpos + 1;
    std::advance(it, 1);
  }
}
