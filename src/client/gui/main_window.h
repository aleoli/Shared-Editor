#pragma once

#include <QMainWindow>
#include <QWidget>
#include <QString>
#include <QLabel>
#include <QMovie>
#include <QCloseEvent>

#include <memory>

#include "alert_messages.h"
#include "user.h"

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  void closeEvent(QCloseEvent *bar) override;

public slots:
  virtual void clear() = 0;
  void freeze();
  void unfreeze();

signals:
  void alert(Alert type, const QString &what, const QString &title = "");
  void exit();

protected:
  std::shared_ptr<User> _user;

private:
  void initLoading();

  QLabel *_loading{};
  std::shared_ptr<QMovie> _movie;
};
