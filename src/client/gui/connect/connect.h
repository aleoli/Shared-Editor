#pragma once

#include <QLineEdit>
#include <QPushButton>

#include "main_window.h"
#include "alert_messages.h"
#include "sys.h"

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
  void openConnection(const QString &host, int port, int ms = CONNECT_TIME_LIMIT);
  void quit();

public slots:
  void clear() override;
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
