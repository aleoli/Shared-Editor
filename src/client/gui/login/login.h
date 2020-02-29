#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#include <QMainWindow>
#include "account.h"
#include "string.h"

class Manager;
class Account;

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
    string getPassword() const;
    string getUsername() const;
    void setManagerClass(Manager *m){this->m=m;}

private slots:
    void on_btn_loginButton_clicked();
    void on_btn_RegisterForm_clicked();

    void on_checkBox_showPassword_toggled(bool checked);

signals:
    void showRecordPage();
    void access(string username,string password);

private:
    Ui::Login *ui;
    string username;
    string password;
    Manager *m;

};

#endif // LOGIN_H
