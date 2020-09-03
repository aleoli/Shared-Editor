#include "text_edit.h"

#include "utils.h"

TextEdit::TextEdit(QWidget *parent) : QTextEdit(parent) {
  setStyleSheet("QWidget {background-color: white;}");
}

void TextEdit::resizeEvent(QResizeEvent *event) {
  emit resized();
  QTextEdit::resizeEvent(event);
}

void TextEdit::scrollContentsBy(int dx, int dy) {
  emit scrolled();
  QTextEdit::scrollContentsBy(dx, dy);
}

void TextEdit::keyPressEvent(QKeyEvent *e) {
  if( e->matches(QKeySequence::Undo) && document()->isUndoAvailable()) {
      emit undoredo();
  }
  else if( e->matches(QKeySequence::Redo) && document()->isRedoAvailable()) {
        emit undoredo();
    }

  QTextEdit::keyPressEvent(e);
}
