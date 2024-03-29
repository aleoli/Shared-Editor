#include "edit.h"
#include "../ui/ui_edit.h"

#include "utils.h"
#include "image_utils.h"
#include "exceptions.h"
#include "icon_selector.h"
#include "confirm.h"

using namespace se_exceptions;

Edit::Edit(QWidget *parent) :
    MainWindow(parent),
    ui(new Ui::Edit),
    _iconSet(false)
{
  ui->setupUi(this);

  _widgetPassword  = ui->ledit_pwd;
  _widgetNewPassword  = ui->ledit_newPwd;
  _widgetNewPasswordRepeat  = ui->lineEdit_cnfrmPwd;
  _widgetShowPasswords = ui->checkBox_showPasswords;
  _widgetSave = ui->btn_save;
  _widgetCancel = ui->btn_cancel;
  _widgetDelete = ui->btn_delete;
  _widgetReset = ui->btn_reset;
  _widgetIcon = ui->btn_changeProfile;

  connect(_widgetShowPasswords, &QCheckBox::stateChanged, this, &Edit::_showPasswords);
  connect(_widgetSave, &QPushButton::clicked, this, &Edit::_save);
  connect(_widgetCancel, &QPushButton::clicked, this, &Edit::_cancel);
  connect(_widgetDelete, &QPushButton::clicked, this, &Edit::_delete);
  connect(_widgetReset, &QPushButton::clicked, this, &Edit::_reset);
  connect(_widgetIcon, &QPushButton::clicked, this, &Edit::_setIcon);
}

Edit::~Edit()
{
    delete ui;
}

void Edit::setIcon() {
  _widgetIcon->setIcon(_user->getIcon());
  _widgetIcon->setIconSize(_widgetIcon->size());
}

void Edit::clear() {
  _widgetPassword->clear();
  _widgetNewPassword->clear();
  _widgetNewPasswordRepeat->clear();
  _widgetShowPasswords->setCheckState(Qt::Unchecked);
  _setUserIcon();
  _iconSet = false;
}

void Edit::_showPasswords(int state) {
  debug("Edit::_showPasswords");

  if(state) {
    _widgetPassword->setEchoMode(QLineEdit::Normal);
    _widgetNewPassword->setEchoMode(QLineEdit::Normal);
    _widgetNewPasswordRepeat->setEchoMode(QLineEdit::Normal);
  }
  else {
    _widgetPassword->setEchoMode(QLineEdit::Password);
    _widgetNewPassword->setEchoMode(QLineEdit::Password);
    _widgetNewPasswordRepeat->setEchoMode(QLineEdit::Password);
  }
}

void Edit::_save(bool checked) {
  auto pwd = _widgetPassword->text();
  auto newPwd = _widgetNewPassword->text();
  auto newPwdRepeat = _widgetNewPasswordRepeat->text();

  debug("Edit::_save " + pwd + " " + newPwd + " " + newPwdRepeat + " " + QString::number(_iconSet));

  if(_checkFields(pwd, newPwd, newPwdRepeat)) {
    std::optional<QString> pwdOpt = std::nullopt;
    std::optional<QString> newPwdOpt = std::nullopt;
    std::optional<QString> iconOpt = std::nullopt;
    bool change = false;

    if(!pwd.isEmpty()) {
      pwdOpt = std::optional<QString>(std::move(pwd));
      newPwdOpt = std::optional<QString>(std::move(newPwd));
      change = true;
    }

    if(_iconSet) {
      iconOpt = std::optional<QString>(std::move(image_utils::encodeImage(_user->getTempIcon())));
      change = true;
    }

    if(change)
      emit save(_user->getToken(), pwdOpt, newPwdOpt, newPwdOpt, iconOpt);
    else
      emit alert(Alert::ERROR, NO_CHANGES);
  }
}

void Edit::_cancel(bool checked) {
  debug("Edit::_cancel");
  emit cancel();
}

void Edit::_delete(bool checked) {
  debug("Edit::_delete");

  if(Confirm::show(this))
    emit deleteAccount(_user->getToken());
}

void Edit::_reset(bool checked) {
  debug("Edit::_reset");

  _setDefaultIcon();
  _iconSet = true;
}

void Edit::_setIcon(bool checked) {
  debug("Edit::_setIcon");

  auto icon = IconSelector::show(this);

  if(icon) {
    _widgetIcon->setIcon(*icon);
    _widgetIcon->setIconSize(_widgetIcon->size());
    _user->setTempIcon(*icon);
    _iconSet = true;
  }
}

void Edit::_setUserIcon() {
  setIcon();
  _user->setTempIcon();
}

void Edit::_setDefaultIcon() {
  try {
    auto icon = image_utils::loadImage(DEFAULT_ICON);
    _widgetIcon->setIcon(icon);
    _user->setTempIcon(icon);
  }
  catch(...) {
    throw GuiException{"Failed to load predefined icon."};
  }
}

bool Edit::_checkFields(const QString &password, const QString &newPsw, const QString &newPswRepeat){
  if(password.isEmpty() != newPsw.isEmpty() || newPsw.isEmpty() != newPswRepeat.isEmpty()) {
    emit alert(Alert::ERROR, INCORRECT_FIELDS);
    return false;
  }

  if(!password.isEmpty() && !check_pass(newPsw)) {
    emit alert(Alert::ERROR, WRONG_PASSWORD);
    return false;
  }

  if(newPsw != newPswRepeat) {
    emit alert(Alert::ERROR, PSW_MISMATCH);
    return false;
  }

  if(!password.isEmpty() && password == newPsw) {
    emit alert(Alert::ERROR, SAME_PASSWORDS);
    return false;
  }

  return true;
}
