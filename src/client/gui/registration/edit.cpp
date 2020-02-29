#include "edit.h"
#include "ui_edit.h"

Edit::Edit(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Edit)
{
    ui->setupUi(this);

    ui->lineEdit_actualPassword->setEchoMode(QLineEdit::Password);
    ui->lineEdit_newPassword->setEchoMode(QLineEdit::Password);
    ui->lineEdit_repeatPassword->setEchoMode(QLineEdit::Password);
}

Edit::~Edit()
{
    delete ui;
}

void Edit::on_checkBox_showPasswords_toggled(bool checked)
{
    if(checked){
        ui->lineEdit_actualPassword->setEchoMode(QLineEdit::Normal);
        ui->lineEdit_newPassword->setEchoMode(QLineEdit::Normal);
        ui->lineEdit_repeatPassword->setEchoMode(QLineEdit::Normal);
    }
    else{
        ui->lineEdit_actualPassword->setEchoMode(QLineEdit::Password);
        ui->lineEdit_newPassword->setEchoMode(QLineEdit::Password);
        ui->lineEdit_repeatPassword->setEchoMode(QLineEdit::Password);
    }
}
