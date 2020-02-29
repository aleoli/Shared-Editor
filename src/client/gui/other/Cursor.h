//
// Created by Gianluca on 20/08/2019.
//
#pragma once
#ifndef PROVA_QTEXTEDIT_CURSOR_H
#define PROVA_QTEXTEDIT_CURSOR_H

#include <QLabel>
#include <QTextCursor>
#include <QTextEdit>
#include <QColor>

#include "Symbol.h"

class Cursor : public QLabel {
  Q_OBJECT

public:
  Cursor(QTextEdit *parent, QColor color);
  void updateCursorPosition(int position);
  void insert(const Symbol &sym, int position);
  void remove(int position);

public slots:
  void updateCursorView();

private slots:
  void changeVisibility();

private:
  QTextEdit *textEdit_;
  QTextCursor *cursor_;
  QTimer *timer_;
  bool isVisible_ = true;
  QColor _color;
};


#endif //PROVA_QTEXTEDIT_CURSOR_H
