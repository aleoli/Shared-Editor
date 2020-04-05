//
// Created by Gianluca on 20/08/2019.
//

#ifndef PROVA_QTEXTEDIT_CURSOR_H
#define PROVA_QTEXTEDIT_CURSOR_H

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
  Cursor(QTextEdit *parent, QColor color);
  void updateCursorPosition(int position);
  void insert(const Symbol &sym, int position, std::optional<QBrush> backgroundColor = std::nullopt);
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


#endif //PROVA_QTEXTEDIT_CURSOR_H
