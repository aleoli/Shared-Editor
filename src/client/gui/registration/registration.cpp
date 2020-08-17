#include "registration.h"
#include "../ui/ui_registration.h"

#include "utils.h"
#include "image_utils.h"

#include <QFileDialog>


Registration::Registration(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Registration),
    _user(User::get())
{
    ui->setupUi(this);

    _widgetUsername = findChild<QLineEdit *>("ledit_uname");
    _widgetPassword  = findChild<QLineEdit *>("ledit_pwd");
    _widgetPasswordRepeat  = findChild<QLineEdit *>("lineEdit_cnfrmPwd");
    _widgetShowPasswords = findChild<QCheckBox *>("checkBox_showPwds");
    _widgetSignup = findChild<QPushButton *>("btn_signup");
    _widgetCancel = findChild<QPushButton *>("btn_cancel");
    _widgetIcon = findChild<QPushButton *>("btn_changeProfile");

    connect(_widgetShowPasswords, &QCheckBox::stateChanged, this, &Registration::_showPasswords);
    connect(_widgetSignup, &QPushButton::clicked, this, &Registration::_signup);
    connect(_widgetCancel, &QPushButton::clicked, this, &Registration::_cancel);
    connect(_widgetIcon, &QPushButton::clicked, this, &Registration::_setIcon);
}

Registration::~Registration()
{
    delete ui;
}

void Registration::clear() {
  _widgetUsername->clear();
  _widgetPassword->clear();
  _widgetPasswordRepeat->clear();
  _widgetShowPasswords->setCheckState(Qt::Unchecked);
  _setDefaultIcon();
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
    _user->setUsername(username);
    emit signup(username, pwd, pwdRepeat, image_utils::encodeImage(_user->getIcon()));
  }
}

void Registration::_cancel(bool checked) {
  debug("Registration::_cancel");
  emit cancel();
}

void Registration::_setIcon(bool checked) {
  debug("Registration::_setIcon");

  //TODO check sull'icona!
  auto filename = QFileDialog::getOpenFileName(this,
    tr("Open Image"), PROFILE_PICS_PATH, tr("Image Files (*.png *.jpg *.bmp)"));

    try {
      auto icon = image_utils::loadRoundedImage(filename);
      _widgetIcon->setIcon(icon);
      _widgetIcon->setIconSize(_widgetIcon->size());
      _user->setIcon(icon);
    }
    catch(...) {
      emit alert(Alert::ERROR, LOAD_ICON_FAILED);
    }
}

void Registration::_setDefaultIcon() {
  try {
    auto icon = image_utils::loadRoundedImage(DEFAULT_ICON);
    _widgetIcon->setIcon(icon);
    _user->setIcon(icon);
  }
  catch(...) {
    emit alert(Alert::ERROR, LOAD_ICON_FAILED);
  }
}

bool Registration::_checkFields(const QString &username, const QString &password, const QString &pswRepeat) {
  if(username.isEmpty() || password.isEmpty() || pswRepeat.isEmpty()) {
    emit alert(Alert::ERROR, INCORRECT_FIELDS);
    return false;
  }

  if(!check_pass(password)) {
    emit alert(Alert::ERROR, WRONG_PASSWORD);
    return false;
  }

  if(password != pswRepeat) {
    emit alert(Alert::ERROR, PSW_MISMATCH);
    return false;
  }

  return true;
}
