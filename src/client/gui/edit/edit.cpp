#include "edit.h"
#include "../ui/ui_edit.h"

#include "utils.h"
#include "image_utils.h"
#include "exceptions.h"
#include "icon_selector.h"

using namespace se_exceptions;

Edit::Edit(QWidget *parent) :
    MainWindow(parent),
    ui(new Ui::Edit),
    _iconSet(false)
{
    ui->setupUi(this);

    _widgetPassword  = findChild<QLineEdit *>("ledit_pwd");
    _widgetNewPassword  = findChild<QLineEdit *>("ledit_newPwd");
    _widgetNewPasswordRepeat  = findChild<QLineEdit *>("lineEdit_cnfrmPwd");
    _widgetShowPasswords = findChild<QCheckBox *>("checkBox_showPasswords");
    _widgetSave = findChild<QPushButton *>("btn_save");
    _widgetCancel = findChild<QPushButton *>("btn_cancel");
    _widgetDelete = findChild<QPushButton *>("btn_delete");
    _widgetIcon = findChild<QPushButton *>("btn_changeProfile");

    if(!_widgetPassword || !_widgetNewPassword || !_widgetNewPasswordRepeat ||
        !_widgetShowPasswords || !_widgetSave || !_widgetCancel ||
        !_widgetDelete || !_widgetIcon) {
          throw GuiException{"One or more widgets in Edit are null"};
    }

    connect(_widgetShowPasswords, &QCheckBox::stateChanged, this, &Edit::_showPasswords);
    connect(_widgetSave, &QPushButton::clicked, this, &Edit::_save);
    connect(_widgetCancel, &QPushButton::clicked, this, &Edit::_cancel);
    connect(_widgetDelete, &QPushButton::clicked, this, &Edit::_delete);
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
  //TODO show confirm dialog
  emit deleteAccount(_user->getToken());
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
