//
// Created by Gianluca on 20/08/2019.
//

#include "Cursor.h"
#include <QTimer>
#include "utils.h"
#include <QString>

Cursor::Cursor(QTextEdit *parent, QColor color) : QLabel(parent), _textEdit(parent), _color(color), _isVisible(false), _isActive(false) {
  //TODO rivedere questi parametri
  _color.setAlpha(192);
  setFixedWidth(2);

  setStyleSheet("background-color : " + _color.name(QColor::HexArgb) + ";");

  _cursor = new QTextCursor(parent->document());
  updateCursorPosition(0);
  setVisible(false);

  _timer = new QTimer(this);
  QObject::connect(_timer, &QTimer::timeout, this, &Cursor::changeVisibility);
}

void Cursor::show() {
  if(!_isActive) {
    changeVisibility();
    _timer->start(500);
    _isActive = true;
  }
}

void Cursor::updateCursorPosition(int position) {
  _cursor->setPosition(position);
  updateCursorView();
}

void Cursor::updateCursorView() {
  auto rect = _textEdit->cursorRect(*_cursor);
  move(rect.x(), rect.y());
  setFixedHeight(rect.height());
}

void Cursor::changeVisibility() {
  _isVisible ? setVisible(false) : setVisible(true);
  _isVisible = !_isVisible;
}

void Cursor::insert(const Symbol &sym, int position, std::optional<QBrush> backgroundColor) {
  _cursor->setPosition(position);

  auto fmt = sym.getFormat();

  if(backgroundColor) {
    fmt.setBackground(*backgroundColor);
  }

  _cursor->setCharFormat(fmt);
  _cursor->insertText(sym.getChar());

  updateCursorView();
}

void Cursor::remove(int position) {
  _cursor->setPosition(position);

  _cursor->deleteChar();

  updateCursorView();
}
