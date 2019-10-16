#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#include <QMainWindow>
#include "account.h"

using namespace std;

namespace Ui {
class Login;
}

class Login : public QWidget
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = nullptr);
    ~Login();
    Account getUser();

private slots:
    void on_btn_loginButton_clicked();
    void on_btn_RegisterForm_clicked();

signals:
    void login_successful();
    void showRecordPage();

private:
    Ui::Login *ui;
    Account *user;

};

#endif // LOGIN_H
