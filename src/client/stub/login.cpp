#include "login.h"
#include "ui_login.h"
#include <iostream>
#include <QMovie>
#include <QTimer>
#include "utils.h"
#include "errordialog.h"

Login::Login(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::Login)
{
  ui->setupUi(this);

  _username = findChild<QLineEdit *>("usernameForm");
  _psw = findChild<QLineEdit *>("pswForm");
  _btnLogin = findChild<QPushButton *>("loginButton");
  _btnNew = findChild<QPushButton *>("newButton");

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
  auto username = _username->text();
  auto psw = _psw->text();

  if(username.isEmpty() || psw.isEmpty()) {
    debug("Premuto btnLogin. Errore form");
    ErrorDialog::showDialog(this, "Completa i campi!");
    return;
  }

  disable();
  startLoading();

  debug("Premuto btnLogin. Username: " + username + " Password: " + psw);

  emit loginQuery(username, psw);
}

void Login::newUser() {
  auto username = _username->text();
  auto psw = _psw->text();

  if(username.isEmpty() || psw.isEmpty()) {
    debug("Premuto btnNew. Errore form");
    ErrorDialog::showDialog(this, "Completa i campi!");
    return;
  }

  disable();
  startLoading();

  debug("Premuto btnNew. Username: " + username + " Password: " + psw);

  emit newUserQuery(username, psw, psw, std::nullopt);
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
  _btnLogin->setEnabled(false);
  _btnNew->setEnabled(false);
  _username->setEnabled(false);
  _psw->setEnabled(false);
}

void Login::enable() {
  _btnLogin->setEnabled(true);
  _btnNew->setEnabled(true);
  _username->setEnabled(true);
  _psw->setEnabled(true);
}

void Login::clear() {
  _username->clear();
  _psw->clear();
}
