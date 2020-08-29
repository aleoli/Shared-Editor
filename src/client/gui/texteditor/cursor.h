#pragma once

#include <QLabel>
#include <QTextCursor>
#include <QTextEdit>
#include <QColor>
#include <QBrush>

#include <optional>

#include "Symbol.h"

class Cursor : public QLabel {
  Q_OBJECT

public:
  Cursor(QTextEdit *parent, const QColor &color);
  ~Cursor();
  
  void updateCursorPosition(int position);
  void insert(const Symbol &sym, int position, const std::optional<QBrush> &backgroundColor = std::nullopt);
  void remove(int position);
  void show();

public slots:
  void updateCursorView();

private slots:
  void changeVisibility();

private:
  QTextEdit *_textEdit;
  QTextCursor *_cursor;
  QTimer *_timer;
  bool _isVisible;
  bool _isActive;
  QColor _color;
};
