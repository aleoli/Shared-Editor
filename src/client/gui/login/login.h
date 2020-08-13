#pragma once

#include <QMainWindow>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>

#include "alert_messages.h"

namespace Ui {
class Login;
}

class Login : public QMainWindow
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = nullptr);
    ~Login();

signals:
  void login(QString username, QString password);
  void signup();
  void alert(Alert type, QString what);

private:
  void _showPassword(int state);
  void _login(bool checked);
  void _signup(bool checked);

  bool _checkFields(const QString &username, const QString &password);

  Ui::Login *ui;
  QLineEdit *_widgetUsername, *_widgetPassword;
  QCheckBox *_widgetShowPassword;
  QPushButton *_widgetLogin, *_widgetSignup;
};
