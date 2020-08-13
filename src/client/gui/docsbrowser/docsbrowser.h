#pragma once

#include <QMainWindow>

#include "alert_messages.h"

namespace Ui {
class DocsBrowser;
}

class DocsBrowser : public QMainWindow
{
    Q_OBJECT

public:
    explicit DocsBrowser(QWidget *parent = nullptr);
    ~DocsBrowser();

private slots:

signals:
  void alert(Alert type, QString what);

private:
    Ui::DocsBrowser *ui;
};
