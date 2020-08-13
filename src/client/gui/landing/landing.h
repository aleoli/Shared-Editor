#pragma once

#include <QMainWindow>

#include "alert_messages.h"

namespace Ui {
class Landing;
}

class Landing : public QMainWindow
{
    Q_OBJECT

public:
    explicit Landing(QWidget *parent = nullptr);
    ~Landing();

private slots:

signals:
  void alert(Alert type, QString what);

private:
    Ui::Landing *ui;
};
