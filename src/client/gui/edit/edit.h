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
class Edit;
}

class Edit : public QMainWindow
{
    Q_OBJECT

public:
    explicit Edit(QWidget *parent = nullptr);
    ~Edit();

    void clear();

public slots:
  void setIcon();

signals:
  void save(const QString &token, const std::optional<QString> &password, const std::optional<QString> &newPsw, const std::optional<QString> &newPswRepeat, const std::optional<QString> &icon);
  void cancel();
  void alert(Alert type, const QString &what);

private slots:
  void _showPasswords(int state);
  void _save(bool checked);
  void _cancel(bool checked);
  void _setIcon(bool checked);
  void _setUserIcon();

private:
  bool _checkFields(const QString &password, const QString &newPsw, const QString &newPswRepeat);

  Ui::Edit *ui;

  QLineEdit *_widgetPassword, *_widgetNewPassword, *_widgetNewPasswordRepeat;
  QCheckBox *_widgetShowPasswords;
  QPushButton *_widgetSave, *_widgetCancel, *_widgetIcon;

  std::shared_ptr<User> _user;
};
