#pragma once

#include <QDialog>
#include <QIcon>
#include <QPushButton>

#include <optional>

namespace Ui {
class Confirm;
}

class Confirm : public QDialog
{
  Q_OBJECT

public:
  explicit Confirm(QWidget *parent = nullptr);
  ~Confirm();

  static bool show(QWidget *parent = nullptr);

private:
  void _confirm(bool checked);
  void _cancel(bool checked);

  Ui::Confirm *ui;

  QPushButton *_widgetConfirm, *_widgetCancel;
};
