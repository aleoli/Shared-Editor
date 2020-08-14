#pragma once

#include <QMainWindow>

#include "alert_messages.h"
#include "user.h"

#include <QPushButton>

#include <memory>

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
  void alert(Alert type, QString what);
  void logout(QString token);

private slots:
  void _logout(bool checked);

private:
    Ui::DocsBrowser *ui;

    QPushButton *_widgetLogout;

    std::shared_ptr<User> _user;
};
