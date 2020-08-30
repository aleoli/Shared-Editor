#pragma once

#include <QDialog>
#include <QIcon>
#include <QPushButton>
#include <QPlainTextEdit>

#include <optional>

namespace Ui {
class Message;
}

class Input : public QDialog
{
  Q_OBJECT

public:
  Input(QWidget *parent, const QString &title, const QString &text);
  ~Input();

  static std::optional<QString> show(QWidget *parent = nullptr, const QString &title = "Input", const QString &text = "", const QString &cancelText = "", const QString &confirmText = "");

private:
  void _confirm(bool checked);
  void _cancel(bool checked);

  Ui::Message *ui;

  QPushButton *_widgetConfirm, *_widgetCancel;
  QPlainTextEdit *_widgetText;
};
