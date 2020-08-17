#pragma once

#include <QMainWindow>

#include "alert_messages.h"

namespace Ui {
class Edit;
}

class Edit : public QMainWindow
{
    Q_OBJECT

public:
    explicit Edit(QWidget *parent = nullptr);
    ~Edit();

    void clear();

private slots:

signals:
  void alert(Alert type, const QString &what);

private:
    Ui::Edit *ui;
};
