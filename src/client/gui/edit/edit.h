#pragma once

#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>

#include <optional>

#include "main_window.h"
#include "alert_messages.h"
#include "user.h"

namespace Ui {
class Edit;
}

class Edit : public MainWindow
{
    Q_OBJECT

public:
    explicit Edit(QWidget *parent = nullptr);
    ~Edit();

public slots:
    virtual void clear();

public slots:
  void setIcon();

signals:
  void save(const QString &token, const std::optional<QString> &password, const std::optional<QString> &newPsw, const std::optional<QString> &newPswRepeat, const std::optional<QString> &icon);
  void cancel();
  void deleteAccount(const QString &token);

private slots:
  void _showPasswords(int state);
  void _save(bool checked);
  void _cancel(bool checked);
  void _delete(bool checked);
  void _setIcon(bool checked);
  void _setUserIcon();

private:
  bool _checkFields(const QString &password, const QString &newPsw, const QString &newPswRepeat);

  Ui::Edit *ui;

  QLineEdit *_widgetPassword, *_widgetNewPassword, *_widgetNewPasswordRepeat;
  QCheckBox *_widgetShowPasswords;
  QPushButton *_widgetSave, *_widgetCancel, *_widgetDelete, *_widgetIcon;

  bool _iconSet;
};
