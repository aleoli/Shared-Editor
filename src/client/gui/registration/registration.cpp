#include "registration.h"
#include "../ui/ui_registration.h"

#include "utils.h"

Registration::Registration(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Registration)
{
    ui->setupUi(this);

    _widgetUsername = findChild<QLineEdit *>("ledit_uname");
    _widgetPassword  = findChild<QLineEdit *>("ledit_pwd");
    _widgetPasswordRepeat  = findChild<QLineEdit *>("lineEdit_cnfrmPwd");
    _widgetShowPasswords = findChild<QCheckBox *>("checkBox_showPwds");
    _widgetSignup = findChild<QPushButton *>("btn_signup");
    _widgetCancel = findChild<QPushButton *>("btn_cancel");

    connect(_widgetShowPasswords, &QCheckBox::stateChanged, this, &Registration::_showPasswords);
    connect(_widgetSignup, &QPushButton::clicked, this, &Registration::_signup);
    connect(_widgetCancel, &QPushButton::clicked, this, &Registration::_cancel);
}

Registration::~Registration()
{
    delete ui;
}

void Registration::_showPasswords(int state) {
  debug("Registration::_showPasswords");

  if(state) {
    _widgetPassword->setEchoMode(QLineEdit::Normal);
    _widgetPasswordRepeat->setEchoMode(QLineEdit::Normal);
  }
  else {
    _widgetPassword->setEchoMode(QLineEdit::Password);
    _widgetPasswordRepeat->setEchoMode(QLineEdit::Password);
  }
}

void Registration::_signup(bool checked) {
  auto username = _widgetUsername->text();
  auto pwd = _widgetPassword->text();
  auto pwdRepeat = _widgetPasswordRepeat->text();

  debug("Registration::_signup " + username + " " + pwd + " " + pwdRepeat);

  if(_checkFields(username, pwd, pwdRepeat)) {
    emit signup(username, pwd, pwdRepeat, std::nullopt); //TODO icon
  }
  else {
    emit alert(Alert::ERROR, INCORRECT_FIELDS);
  }
}

void Registration::_cancel(bool checked) {
  debug("Registration::_cancel");
  emit cancel();
}

bool Registration::_checkFields(const QString &username, const QString &password, const QString &pswRepeat) {
  //TODO migliora logica
  if(username.size() == 0 || password.size() == 0 || password != pswRepeat) {
    return false;
  }
  return true;
}
