#pragma once

#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>

#include "main_window.h"
#include "alert_messages.h"

namespace Ui {
class Login;
}

class Login : public MainWindow
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = nullptr);
    ~Login();

public slots:
  void clear() override;

signals:
  void login(const QString &username, const QString &password);
  void signup();
  void alert(Alert type, const QString &what, const QString &title = LOGIN_ERROR);

private slots:
  void _showPassword(int state);
  void _login(bool checked);
  void _signup(bool checked);

private:
  bool _checkFields(const QString &username, const QString &password);

  Ui::Login *ui;
  QLineEdit *_widgetUsername, *_widgetPassword;
  QCheckBox *_widgetShowPassword;
  QPushButton *_widgetLogin, *_widgetSignup;
};
