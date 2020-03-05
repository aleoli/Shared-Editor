#include "record.h"
#include "ui_record.h"

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

/*QString Record::getRepeat_username() const
{
    return repeat_username;
}

void Record::setRepeat_username(const QString &value)
{
    repeat_username = value;
}*/

QString Record::getNickname() const
{
    return nickname;
}

void Record::setNickname(const QString &value)
{
    nickname = value;
}

QString Record::getRepeat_password() const
{
    return repeat_password;
}

void Record::setRepeat_password(const QString &value)
{
    repeat_password = value;
}

QString Record::getPassword() const
{
    return password;
}

void Record::setPassword(const QString &value)
{
    password = value;
}

QString Record::getUsername() const
{
    return username;
}

void Record::setUsername(const QString &value)
{
    username = value;
}

void Record::on_pushButton_signUp_clicked()
{
    setUsername(ui->lineEdit_nickname->text());//.toStdQString());
    //setRepeat_username(ui->lineEdit_email->text().toStdQString());
    //setNickname(ui->lineEdit_repeat_email->text().toStdQString());
    setPassword(ui->lineEdit_password->text());//.toStdQString());
    setRepeat_password(ui->lineEdit_repeat_password->text());//.toStdQString());
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


