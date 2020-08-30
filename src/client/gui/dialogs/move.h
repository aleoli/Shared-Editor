#pragma once

#include <QDialog>
#include <QIcon>
#include <QPushButton>
#include <QComboBox>

namespace Ui {
  class MoveFile;
}

class Move : public QDialog
{
  Q_OBJECT

public:
  Move(QWidget *parent, const QString &title);
  ~Move();

  static std::optional<int> show(const std::list<std::pair<QString, int>>& items, QWidget *parent = nullptr, const QString &title = "Input", const QString &cancelText = "", const QString &confirmText = "");

private:
  void _confirm(bool checked);
  void _cancel(bool checked);

  void _setValues(const std::list<std::pair<QString, int>>&);

  Ui::MoveFile *ui;

  QPushButton *_widgetConfirm, *_widgetCancel;
  QComboBox *_comboBox;
};