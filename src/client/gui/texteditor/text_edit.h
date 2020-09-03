#pragma once

#include <QTextEdit>
#include <QKeyEvent>

class TextEdit : public QTextEdit {
  Q_OBJECT

public:
  explicit TextEdit(QWidget *parent = nullptr);

  void resizeEvent(QResizeEvent *event) override;
  void keyPressEvent(QKeyEvent *e) override;
  void scrollContentsBy(int dx, int dy) override;

signals:
  void resized();
  void scrolled();
  void undoredo();
};
