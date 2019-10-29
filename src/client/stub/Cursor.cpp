//
// Created by Gianluca on 20/08/2019.
//

#include "Cursor.h"
#include <QTimer>
#include "utils.h"
#include <QString>

Cursor::Cursor(QTextEdit *parent, QColor color) : QLabel(parent), textEdit_(parent), _color(color) {
  //TODO rivedere questi parametri
  //_color.setAlpha(128);
  setFixedWidth(2);

  setStyleSheet("background-color : " + _color.name(QColor::HexArgb) + ";");

  cursor_ = new QTextCursor(parent->document());

  timer_ = new QTimer(this);
  QObject::connect(timer_, &QTimer::timeout, this, &Cursor::changeVisibility);
  timer_->start(500);
}

void Cursor::updateCursorPosition(int position) {
  cursor_->setPosition(position);
  updateCursorSize();
}

void Cursor::updateCursorSize() {
  auto rect = textEdit_->cursorRect(*cursor_);

  move(rect.x(), rect.y());
  setFixedHeight(rect.height());
}

void Cursor::changeVisibility() {
  isVisible_ ? setVisible(false) : setVisible(true);
  isVisible_ = !isVisible_;
}
