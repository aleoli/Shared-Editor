#include "login.h"
#include "ui_login.h"
//#include "QString.h"
#include <QObject>
#include <iostream>

class Account;

using namespace std;

//bool rightAccess(QString u, QString p);
//Account getAccountFromServer(QString u,QString p);

Login::Login(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);
}

Login::~Login()
{
    delete ui;
}

void Login::on_btn_loginButton_clicked()
{
    username = ui->ledit_loginUsername->text();//.toStdQString();
    password = ui->ledit_loginPassword->text();//.toStdQString();
    emit access(username,password);
}

void Login::on_btn_RegisterForm_clicked()
{
    emit showRecordPage();
}

QString Login::getUsername() const
{
    return username;
}

QString Login::getPassword() const
{
    return password;
}

void Login::on_checkBox_showPassword_toggled(bool checked)
{
    if(checked){
        ui->ledit_loginPassword->setEchoMode(QLineEdit::Normal);
    }
    else {
        ui->ledit_loginPassword->setEchoMode(QLineEdit::Password);
    }
}
