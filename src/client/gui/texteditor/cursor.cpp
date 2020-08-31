#include "cursor.h"
#include <QTimer>
#include <QString>

#include <utility>

#include "utils.h"

Cursor::Cursor(QTextEdit *parent, const QColor &color) : QLabel(parent), _textEdit(parent), _color(color), _isVisible(false), _isActive(false) {
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

Cursor::~Cursor() {
  _timer->stop();
  delete _timer;
  delete _cursor;
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

void Cursor::insert(const Symbol &sym, int position, const std::optional<QBrush> &backgroundColor) {
  _cursor->setPosition(position);
  insert(sym, backgroundColor);
}

void Cursor::insert(const Symbol &sym, const std::optional<QBrush> &backgroundColor) {
  auto fmt = sym.getFormat();

  if(backgroundColor) {
    fmt.setBackground(*backgroundColor);
  }

  _cursor->setCharFormat(fmt);
  _cursor->insertText(sym.getChar());
}

void Cursor::insert(const QString &text, QTextCharFormat &fmt, const std::optional<QBrush> &backgroundColor) {
  if(text.isEmpty()) return;

  if(backgroundColor) {
    fmt.setBackground(*backgroundColor);
  }

  _cursor->setCharFormat(fmt);
  _cursor->insertText(text);
}

void Cursor::moveForward(int position) {
  if(position == 0) return;
  _cursor->movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, position);
}

void Cursor::goStart() {
  _cursor->setPosition(0);
}

void Cursor::remove(int position) {
  _cursor->setPosition(position);

  _cursor->deleteChar();

  updateCursorView();
}
