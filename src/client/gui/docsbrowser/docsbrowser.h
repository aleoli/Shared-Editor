#pragma once

#include <QMainWindow>

#include "alert_messages.h"
#include "user.h"

#include <QPushButton>

#include <memory>
#include <optional>

namespace Ui {
class DocsBrowser;
}

class DocsBrowser : public QMainWindow
{
    Q_OBJECT

public:
    explicit DocsBrowser(QWidget *parent = nullptr);
    ~DocsBrowser();

    void clear();

public slots:
    void setIcon();

signals:
  void alert(Alert type, const QString &what);
  void logout(const QString &token);
  void editAccount();
  void newFile(const QString &token, const QString &name, const std::optional<int> &dirId = std::nullopt);

private slots:
  void _account(bool checked);
  void _newFile(bool checked);
  void _logout(bool checked);

private:
    Ui::DocsBrowser *ui;

    QPushButton *_widgetAccount, *_widgetNewFile;
    QAction *_actionLogout;

    std::shared_ptr<User> _user;
};
