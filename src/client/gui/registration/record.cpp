#include "record.h"
#include "ui_record.h"
#include "string.h"

using namespace std;

Record::Record(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Record)
{
    ui->setupUi(this);

    ui->lineEdit_password->setEchoMode(QLineEdit::Password);
    ui->lineEdit_repeat_password->setEchoMode(QLineEdit::Password);
}

Record::~Record()
{
    delete ui;
}

void Record::on_pushButton_logIn_clicked()
{
    showLoginPage();
}

/*string Record::getRepeat_username() const
{
    return repeat_username;
}

void Record::setRepeat_username(const string &value)
{
    repeat_username = value;
}*/

string Record::getNickname() const
{
    return nickname;
}

void Record::setNickname(const string &value)
{
    nickname = value;
}

string Record::getRepeat_password() const
{
    return repeat_password;
}

void Record::setRepeat_password(const string &value)
{
    repeat_password = value;
}

string Record::getPassword() const
{
    return password;
}

void Record::setPassword(const string &value)
{
    password = value;
}

string Record::getUsername() const
{
    return username;
}

void Record::setUsername(const string &value)
{
    username = value;
}

void Record::on_pushButton_signUp_clicked()
{
    setUsername(ui->lineEdit_nickname->text().toStdString());
    //setRepeat_username(ui->lineEdit_email->text().toStdString());
    //setNickname(ui->lineEdit_repeat_email->text().toStdString());
    setPassword(ui->lineEdit_password->text().toStdString());
    setRepeat_password(ui->lineEdit_repeat_password->text().toStdString());
    emit record_try(this->getNickname(),this->getUsername(),this->getPassword(), this->getRepeat_password());
}

void Record::on_checkBox_2_toggled(bool checked)
{
    if(checked){
        ui->lineEdit_password->setEchoMode(QLineEdit::Normal);
        ui->lineEdit_repeat_password->setEchoMode(QLineEdit::Normal);
    }
    else {
        ui->lineEdit_password->setEchoMode(QLineEdit::Password);
        ui->lineEdit_repeat_password->setEchoMode(QLineEdit::Password);
    }
}


