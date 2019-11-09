#include "login.h"
#include "ui_login.h"
#include <iostream>
#include <QMovie>
#include <QTimer>

Login::Login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);

    _username = findChild<QLineEdit *>("usernameForm");
    _psw = findChild<QLineEdit *>("pswForm");
    _btn = findChild<QPushButton *>("loginButton");

    initLoading();

    //QTimer::singleShot(5000, this, [this]() {unblock();});
}

Login::~Login()
{
    delete ui;
}

void Login::initLoading() {
    _movie = std::make_shared<QMovie>(":/imgs/loading");

    _loading = new QLabel(this);
    _loading->setMovie(_movie.get());
    _loading->setAlignment(Qt::AlignCenter);

    _loading->move(this->rect().center() - _loading->rect().center());
}

void Login::login() {
    disable();
    startLoading();

    emit loginRequest(_username->text(), _psw->text());
}

void Login::unblock() {
    stopLoading();
    enable();
}

void Login::startLoading() {
    _movie->start();
    _loading->show();
}

void Login::stopLoading() {
    _loading->hide();
    _movie->stop();
}

void Login::disable() {
    _btn->setEnabled(false);
    _username->setEnabled(false);
    _psw->setEnabled(false);
}

void Login::enable() {
    _btn->setEnabled(true);
    _username->setEnabled(true);
    _psw->setEnabled(true);
}

void Login::clear() {
    _username->clear();
    _psw->clear();
}
