#pragma once

#include <QDialog>
#include <QIcon>
#include <QPushButton>
#include <QLabel>

#include <optional>

namespace Ui {
class Info;
}

class Info : public QDialog
{
  Q_OBJECT

public:
  explicit Info(QWidget *parent, const QString &windowTitle, const QString &title, const QString &msg);
  ~Info();

  static bool show(QWidget *parent, const QString &windowTitle, const QString &title, const QString &msg);

private:
  void _confirm(bool checked);
  void _copy(bool checked);

  Ui::Info *ui;

  QPushButton *_widgetConfirm, *_widgetCopy;
  QLabel *_widgetTitle, *_widgetText;
};
