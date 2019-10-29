//
// Created by Gianluca on 20/08/2019.
//

#ifndef PROVA_QTEXTEDIT_CURSOR_H
#define PROVA_QTEXTEDIT_CURSOR_H

#include <QLabel>
#include <QTextCursor>
#include <QTextEdit>
#include <QColor>

class Cursor : public QLabel {
  Q_OBJECT

public:
  Cursor(QTextEdit *parent, QColor color);
  void updateCursorPosition(int position);

public slots:
  void updateCursorSize();

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
