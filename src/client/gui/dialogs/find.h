#pragma once

#include <QDialog>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>

namespace Ui {
class Find;
}

class Find : public QDialog
{
  Q_OBJECT

public:
  explicit Find(QWidget *parent, QTextEdit *textEdit);
  ~Find();

  void clear();

private:
  void _search(bool checked);
  void _cancel(bool checked);

  Ui::Find *ui;

  QPushButton *_widgetNext, *_widgetCancel, *_widgetSearch;
  QCheckBox *_widgetBackwards, *_widgetCase, *_widgetWhole;
  QLineEdit *_widgetText;
  QLabel *_widgetMatches;
  QTextEdit *_textEdit;
};
