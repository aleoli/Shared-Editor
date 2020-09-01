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

  if(!_isFakeUpdate(pos,removed, added)) {
    debug("TextEditor::_contentsChange | pos: " + QString::number(pos) + " removed: " + QString::number(removed) + " added: " + QString::number(added));

    _handleDelete(pos, removed);
    _handleInsert(pos, added);
    updateActions();
  }
  else {
    debug("Phantom update");
  }

  _cursorPosition = pos + added;
}

bool TextEditor::_isFakeUpdate(int pos, int removed, int added) {
  if(removed != added) return false;
  if(removed == 0) return true;

  QTextCursor cursor(_textEdit->document());
  cursor.setPosition(pos);
  auto it = _file->iteratorAt(pos);

  for(int i=0; i<removed; i++) {
    cursor.movePosition(QTextCursor::NextCharacter);
    auto chr = _textEdit->document()->characterAt(pos + i);
    auto fmt = cursor.charFormat();

    if(!it->hasSameAttributes(chr, fmt, _highlighted)) {
      return false;
    }

    it = std::next(it);
  }

  return true;
}

void TextEditor::_handleDelete(int pos, int removed) {
  if(removed == 0) return;

  std::vector<SymbolId> symRemoved;
  auto it = _file->iteratorAt(pos);

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

  emit localDelete(_user->getToken(), _user->getFileId(), symRemoved);
}

void TextEditor::_handleInsert(int pos, int added) {
  if(added == 0) return;

  QTextCursor cursor(_textEdit->document());
  std::vector<Symbol> symAdded;

  cursor.setPosition(pos);
  auto it = _file->iteratorAt(pos);
  for(int i=0; i<added; i++) {
    cursor.movePosition(QTextCursor::NextCharacter); // must be moved BEFORE catching the correct format

    auto chr = _textEdit->document()->characterAt(pos + i);
    auto fmt = cursor.charFormat();

    if(chr == nullptr) {
      cursor.deletePreviousChar(); //TODO check. delete null characters
    }
    else {
      // mettendo qua il check dell'highlight cambio anche il simbolo interno nel File
      // così sono sicuro che il background è trasparente (es ho inserito a destra di un carattere inserito
      // da un altro, il background sarà di quel colore erroneamente)
      if(_highlighted) {
        // set transparent background
        _blockSignals = true;
        cursor.deletePreviousChar();
        fmt.setBackground(_defColor);
        cursor.insertText(chr, fmt);
        _blockSignals = false;
      }

      Symbol s{{_user->getUserId(), _user->getCharId()}, chr, fmt};
      //debug(QString::fromStdString(s.to_string()));

      _file->localInsert(s, pos+i, &it);
      symAdded.push_back(s);
    }
  }

  emit localInsert(_user->getToken(), _user->getFileId(), symAdded);
}


void TextEditor::_cursorChanged() {
  if(_blockSignals) return;

  auto cursor = _textEdit->textCursor();
  if(cursor.hasSelection()) return; //non mando nulla se sto selezionando

  // se crasha vuol dire che c'è un problema da qualche parte da risolvere
  try {
    auto pos = saveCursorPosition(cursor);
    if(_cursorPosition == pos.second) return; // ho già registrato questo spostamento, non mando nulla

    debug("TextEditor::_cursorChanged");
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

void TextEditor::remoteInsert(int fileId, int userId, const std::vector<Symbol>& symbols) {
  _blockSignals = true;
  int pos = -1;
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
    pos = _file->remoteInsert(sym, &it, pos);
    auto movePos = pos - tempPos;
    auto symFmt = sym.getFormat();

    if(movePos != 0 || symFmt != fmt) {
      cursor->insert(text, fmt, backgroundColor);
      cursor->moveForward(movePos);
      text.clear();
    }

    text.append(sym.getChar());
    tempPos = pos + 1;
    fmt = symFmt;
  }
  if(!text.isEmpty()) {
    cursor->insert(text, fmt, backgroundColor);
  }

  cursor->updateCursorView();
  cursor->show();
  _blockSignals = false;
}

void TextEditor::remoteDelete(int fileId, int userId, const std::vector<SymbolId>& ids) {
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
    pos = _file->remoteDelete(id, &it, pos);
    if(pos == -1) continue;

    if(pos != tempPos) {
      cursor->removeSelected();
      cursor->goTo(pos);
      tempPos = pos;
    }

    cursor->selectNext();
  }

  cursor->removeSelected();
  cursor->updateCursorView();
  cursor->show();
  _blockSignals = false;
}

void TextEditor::remoteUpdate(int fileId, int userId, const std::vector<Symbol>& symbols) {
  throw TextEditorException{"Update not used in this version!"};
  _blockSignals = true;
  int pos;
  auto backgroundColor = _highlighted ? std::optional<QColor>(getUserColorHighlight(userId)) : std::nullopt;

  if(_users.count(userId) == 0) {
    throw TextEditorException{"User " + QString::number(userId) + " not present in the list of users!!"};
  }

  debug("Remote update of " + QString::number(symbols.size())
        + " chars from user " + QString::number(userId));

  auto cursor = _users[userId]->getCursor();
  for(auto &sym : symbols) {
    pos = _file->remoteUpdate(sym);
    if(pos != -1) {
      cursor->remove(pos);
      cursor->insert(sym, pos, backgroundColor);
    }
  }

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
