//
// Created by Gianluca on 20/08/2019.
//

#include "Cursor.h"
#include <QTimer>
#include "utils.h"
#include <QString>

Cursor::Cursor(QTextEdit *parent, QColor color) : QLabel(parent), textEdit_(parent), _color(color) {
  //TODO rivedere questi parametri
  _color.setAlpha(192);
  setFixedWidth(2);

  setStyleSheet("background-color : " + _color.name(QColor::HexArgb) + ";");

  cursor_ = new QTextCursor(parent->document());
  updateCursorPosition(0);

  timer_ = new QTimer(this);
  QObject::connect(timer_, &QTimer::timeout, this, &Cursor::changeVisibility);
  timer_->start(500);
}

void Cursor::updateCursorPosition(int position) {
  cursor_->setPosition(position);
  updateCursorView();
}

void Cursor::updateCursorView() {
  auto rect = textEdit_->cursorRect(*cursor_);

  move(rect.x(), rect.y());
  setFixedHeight(rect.height());
}

void Cursor::changeVisibility() {
  isVisible_ ? setVisible(false) : setVisible(true);
  isVisible_ = !isVisible_;
}

void Cursor::insert(const Symbol &sym, int position) {
  cursor_->setPosition(position);

  cursor_->setCharFormat(sym.getFormat());
  cursor_->insertText(sym.getChar());

  updateCursorView();
}

void Cursor::remove(int position) {
  cursor_->setPosition(position);

  cursor_->deleteChar();

  updateCursorView();
}
