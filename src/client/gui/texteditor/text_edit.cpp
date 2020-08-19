#include "text_edit.h"

TextEdit::TextEdit(QWidget *parent) : QTextEdit(parent) {}

void TextEdit::resizeEvent(QResizeEvent *event) {
  emit resized();
  QTextEdit::resizeEvent(event);
}

void TextEdit::scrollContentsBy(int dx, int dy) {
  emit scrolled();
  QTextEdit::scrollContentsBy(dx, dy);
}
