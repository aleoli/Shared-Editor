#pragma once

#include <QMainWindow>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>

#include <optional>

#include "alert_messages.h"

namespace Ui {
class Registration;
}

class Registration : public QMainWindow
{
    Q_OBJECT

public:
    explicit Registration(QWidget *parent = nullptr);
    ~Registration();

private slots:

signals:
  void signup(QString username, QString password, QString pswRepeat, std::optional<QString> icon);
  void cancel();
  void alert(Alert type, QString what);

private:
  void _showPasswords(int state);
  void _signup(bool checked);
  void _cancel(bool checked);

  bool _checkFields(const QString &username, const QString &password, const QString &pswRepeat);

  Ui::Registration *ui;
  QLineEdit *_widgetUsername, *_widgetPassword, *_widgetPasswordRepeat;
  QCheckBox *_widgetShowPasswords;
  QPushButton *_widgetSignup, *_widgetCancel;
};
