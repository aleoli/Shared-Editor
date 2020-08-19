#pragma once

#include <QTextEdit>

class TextEdit : public QTextEdit {
  Q_OBJECT

public:
  explicit TextEdit(QWidget *parent = nullptr);

  void resizeEvent(QResizeEvent *event) override;
  void scrollContentsBy(int dx, int dy) override;

signals:
  void resized();
  void scrolled();
};
