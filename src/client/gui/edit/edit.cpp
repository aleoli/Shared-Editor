#include "edit.h"
#include "../ui/ui_edit.h"

#include <QFileDialog>

#include "utils.h"
#include "image_utils.h"

Edit::Edit(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Edit),
    _user(User::get())
{
    ui->setupUi(this);

    _widgetPassword  = findChild<QLineEdit *>("lineEdit_pwd");
    _widgetNewPassword  = findChild<QLineEdit *>("lineEdit_newPwd");
    _widgetNewPasswordRepeat  = findChild<QLineEdit *>("lineEdit_cnfrmPwd");
    _widgetShowPasswords = findChild<QCheckBox *>("checkBox_showPasswords");
    _widgetSave = findChild<QPushButton *>("btn_save");
    _widgetCancel = findChild<QPushButton *>("btn_cancel");
    _widgetIcon = findChild<QPushButton *>("btn_changeProfile");

    connect(_widgetShowPasswords, &QCheckBox::stateChanged, this, &Edit::_showPasswords);
    connect(_widgetSave, &QPushButton::clicked, this, &Edit::_save);
    connect(_widgetCancel, &QPushButton::clicked, this, &Edit::_cancel);
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

  debug("Edit::_save " + pwd + " " + newPwd + " " + newPwdRepeat);

  if(_checkFields(pwd, newPwd, newPwdRepeat)) {
    if(!pwd.isEmpty())
      emit save(_user->getToken(), std::optional<QString>(std::move(pwd)), std::optional<QString>(std::move(newPwd)), std::optional<QString>(std::move(newPwdRepeat)), std::optional<QString>(std::move(image_utils::encodeImage(_user->getTempIcon()))));
    else
      emit save(_user->getToken(), std::nullopt, std::nullopt, std::nullopt, std::optional<QString>(std::move(image_utils::encodeImage(_user->getTempIcon()))));
  }
}

void Edit::_cancel(bool checked) {
  debug("Edit::_cancel");
  emit cancel();
}

void Edit::_setIcon(bool checked) {
  debug("Edit::_setIcon");

  auto filename = QFileDialog::getOpenFileName(this,
    tr("Open Image"), PROFILE_PICS_PATH, tr("Image Files (*.png *.jpg *.bmp)"));

    try {
      auto icon = image_utils::loadRoundedImage(filename);
      _widgetIcon->setIcon(icon);
      _widgetIcon->setIconSize(_widgetIcon->size());
      _user->setTempIcon(icon);
    }
    catch(...) {
      emit alert(Alert::ERROR, LOAD_ICON_FAILED);
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

  return true;
}
