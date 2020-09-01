#pragma once

#include <QLineEdit>
#include <QPushButton>

#include "main_window.h"
#include "alert_messages.h"

namespace Ui {
class Connect;
}

class Connect : public MainWindow
{
    Q_OBJECT

public:
    explicit Connect(QWidget *parent = nullptr);
    ~Connect();

signals:
  void openConnection(const QString &host, int port);
  void quit();

public slots:
  virtual void clear();
  void setDefaultAddress(const QString &host, int port);
  QString getHost() const;
  int getPort();

private slots:
  void _connect(bool checked);
  void _exit(bool checked);

private:

  Ui::Connect *ui;
  QLineEdit *_widgetAddress, *_widgetPort;
  QPushButton *_widgetConnect, *_widgetExit;
};
