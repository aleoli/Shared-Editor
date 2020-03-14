#pragma once

#include <QTextEdit>
#include "utils.h"

class QTextEditImpl : public QTextEdit {
  Q_OBJECT

public:
  explicit QTextEditImpl(QWidget *parent = nullptr) : QTextEdit(parent) {}

  void resizeEvent(QResizeEvent *event) override {
    emit resized();
    QTextEdit::resizeEvent(event);
  }

  void scrollContentsBy(int dx, int dy) override {
    emit scrolled();
    QTextEdit::scrollContentsBy(dx, dy);
  }

signals:
  void resized();
  void scrolled();
};
