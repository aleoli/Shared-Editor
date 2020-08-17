#pragma once

#include <QMainWindow>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>

#include <optional>
#include <memory>

#include "alert_messages.h"
#include "user.h"

namespace Ui {
class Registration;
}

class Registration : public QMainWindow
{
    Q_OBJECT

public:
    explicit Registration(QWidget *parent = nullptr);
    ~Registration();

    void clear();

private slots:

signals:
  void signup(const QString &username, const QString &password, const QString &pswRepeat, const std::optional<QString> &icon);
  void cancel();
  void alert(Alert type, const QString &what);

private slots:
  void _showPasswords(int state);
  void _signup(bool checked);
  void _cancel(bool checked);
  void _setIcon(bool checked);
  void _setDefaultIcon();

private:
  bool _checkFields(const QString &username, const QString &password, const QString &pswRepeat);

  Ui::Registration *ui;
  QLineEdit *_widgetUsername, *_widgetPassword, *_widgetPasswordRepeat;
  QCheckBox *_widgetShowPasswords;
  QPushButton *_widgetSignup, *_widgetCancel, *_widgetIcon;

  std::shared_ptr<User> _user;
};
