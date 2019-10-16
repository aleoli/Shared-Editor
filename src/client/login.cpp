#include "login.h"
#include "ui_login.h"
#include "account.h"
#include "string.h"
#include <QObject>

using namespace std;
bool rightAccess(string u, string p);
Account getAccountFromServer(string u,string p);

Login::Login(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);
    //connect(this->btn_loginButton, &QPushButton::clicked, this, on_btn_loginButton_clicked());
}

Login::~Login()
{
    delete ui;
}

void Login::on_btn_loginButton_clicked()
{
    /*if(verifidUser()) {
        adviseMainWindowToChangePage();
    }*/
    string userIdOrEmail = ui->ledit_loginUsername->text().toStdString();
    string password = ui->ledit_loginPassword->text().toStdString();
    if(rightAccess(userIdOrEmail,password)){ // explained below
        //*user = getAccountFromServer(userIdOrEmail,password);    // explained below
        emit login_successful();
    }
}

bool rightAccess(string u, string p){
    //ask server if exists username or email equals "u" with password(encrypted maybe) "p"
    return true;
    return false;
}

Account getAccountFromServer(string u,string p){
    // local account is created to -> modify infos about, such as change email or password and maybe
    // to check if user is logged in (ex mainwindow -> timer(no account) -> showLoginPage();)
    Account *user = new Account();
    return *user;
}
Account Login::getUser(){
    return *user;
}
void Login::on_btn_RegisterForm_clicked()
{
    emit showRecordPage();
}
