#pragma once

#include <QMainWindow>
#include <QWidget>
#include <QString>

#include <memory>

#include "alert_messages.h"
#include "user.h"

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);

public slots:
  virtual void clear() = 0;
  void freeze();
  void unfreeze();

signals:
  void alert(Alert type, const QString &what);

protected:
  std::shared_ptr<User> _user;
};
