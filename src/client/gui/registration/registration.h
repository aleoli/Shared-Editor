#pragma once

#include <QMainWindow>

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

private:
    Ui::Registration *ui;
};
