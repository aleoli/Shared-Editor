#pragma once

#include <QMainWindow>

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

private:
    Ui::Landing *ui;
};
