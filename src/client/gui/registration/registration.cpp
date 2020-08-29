#include "registration.h"
#include "../ui/ui_registration.h"

#include "utils.h"
#include "image_utils.h"
#include "exceptions.h"
#include "icon_selector.h"

using namespace se_exceptions;

Registration::Registration(QWidget *parent) :
    MainWindow(parent),
    ui(new Ui::Registration), _iconSet(false)
{
  ui->setupUi(this);

  _widgetUsername = ui->ledit_uname;
  _widgetPassword  = ui->ledit_pwd;
  _widgetPasswordRepeat  = ui->lineEdit_cnfrmPwd;
  _widgetShowPasswords = ui->checkBox_showPwds;
  _widgetSignup = ui->btn_signup;
  _widgetCancel = ui->btn_cancel;
  _widgetReset = ui->btn_reset;
  _widgetIcon = ui->btn_changeProfile;

  connect(_widgetShowPasswords, &QCheckBox::stateChanged, this, &Registration::_showPasswords);
  connect(_widgetSignup, &QPushButton::clicked, this, &Registration::_signup);
  connect(_widgetCancel, &QPushButton::clicked, this, &Registration::_cancel);
  connect(_widgetReset, &QPushButton::clicked, this, &Registration::_reset);
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
  _iconSet = false;
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

  debug("Registration::_signup " + username + " " + pwd + " " + pwdRepeat + " " + QString::number(_iconSet));

  if(_checkFields(username, pwd, pwdRepeat)) {
    _user->setUsername(username);

    std::optional<QString> icon = std::nullopt;
    if(_iconSet)
      icon = std::optional<QString>(std::move(image_utils::encodeImage(_user->getIcon())));

    emit signup(username, pwd, pwdRepeat, icon);
  }
}

void Registration::_cancel(bool checked) {
  debug("Registration::_cancel");
  emit cancel();
}

void Registration::_reset(bool checked) {
  debug("Registration::_reset");

  _setDefaultIcon();
  _iconSet = false;
}

void Registration::_setIcon(bool checked) {
  debug("Registration::_setIcon");

  auto icon = IconSelector::show(this);

  if(icon) {
    _widgetIcon->setIcon(*icon);
    _widgetIcon->setIconSize(_widgetIcon->size());
    _user->setIcon(*icon);
    _iconSet = true;
  }
}

void Registration::_setDefaultIcon() {
  try {
    auto icon = image_utils::loadImage(DEFAULT_ICON);
    _widgetIcon->setIcon(icon);
    _user->setIcon(icon);
  }
  catch(...) {
    throw GuiException{"Failed to load predefined icon."};
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
