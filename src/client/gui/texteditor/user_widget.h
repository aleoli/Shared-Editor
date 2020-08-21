#pragma once

#include <QWidget>
#include <QPushButton>

namespace Ui {
class ConnectedUser;
}

class UserWidget : public QWidget
{
    Q_OBJECT

public:
    explicit UserWidget(QWidget *parent = nullptr);
    ~UserWidget();

private slots:

private:
  Ui::ConnectedUser *ui;
};
