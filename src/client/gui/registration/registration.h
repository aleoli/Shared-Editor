#pragma once

#include <QMainWindow>

#include "alert_messages.h"

namespace Ui {
class Registration;
}

class Registration : public QMainWindow
{
    Q_OBJECT

public:
    explicit Registration(QWidget *parent = nullptr);
    ~Registration();

private slots:

signals:
  void alert(Alert type, QString what);

private:
    Ui::Registration *ui;
};
