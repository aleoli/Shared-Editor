#include "login.h"
#include "../ui/ui_login.h"

#include "utils.h"
#include "user.h"

Login::Login(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);

    _widgetUsername = findChild<QLineEdit *>("ledit_uname");
    _widgetPassword  = findChild<QLineEdit *>("ledit_pwd");
    _widgetShowPassword = findChild<QCheckBox *>("checkBox_showPwd");
    _widgetLogin = findChild<QPushButton *>("btn_login");
    _widgetSignup = findChild<QPushButton *>("btn_signup");

    connect(_widgetShowPassword, &QCheckBox::stateChanged, this, &Login::_showPassword);
    connect(_widgetLogin, &QPushButton::clicked, this, &Login::_login);
    connect(_widgetSignup, &QPushButton::clicked, this, &Login::_signup);
}

Login::~Login()
{
    delete ui;
}

void Login::_showPassword(int state) {
  debug("Login::_showPassword");

  if(state) {
    _widgetPassword->setEchoMode(QLineEdit::Normal);
  }
  else {
    _widgetPassword->setEchoMode(QLineEdit::Password);
  }
}

void Login::_login(bool checked) {
  auto username = _widgetUsername->text();
  auto pwd = _widgetPassword->text();

  debug("Login::_login " + username + " " + pwd);

  if(_checkFields(username, pwd)) {
    User::get()->setUsername(username);
    emit login(username, pwd);
  }
  else {
    emit alert(Alert::ERROR, INCORRECT_FIELDS);
  }
}

void Login::_signup(bool checked) {
  debug("Login::_signup");
  emit signup();
}

bool Login::_checkFields(const QString &username, const QString &password) {
  //TODO migliora logica
  if(username.size() == 0 || password.size() == 0) {
    return false;
  }

  return true;
}
