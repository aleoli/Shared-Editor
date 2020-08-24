#include "menu_options.h"
#include "../ui/ui_texteditor_options.h"

#include "utils.h"
#include "exceptions.h"

using namespace se_exceptions;

OptionsWidget::OptionsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Options)
{
  ui->setupUi(this);

  _widgetShare = findChild<QPushButton *>("btn_share");
  _widgetComments = findChild<QPushButton *>("btn_comments");
  _widgetConnected = findChild<QPushButton *>("btn_connected");
  _widgetUser = findChild<QPushButton *>("btn_logout");
  _widgetRename = findChild<QLineEdit *>("ledit_rename");

  if(!_widgetShare || !_widgetComments || !_widgetConnected || !_widgetUser ||
    !_widgetRename) {
    throw GuiException{"One or more widgets in UserWidget are null"};
  }

  connect(_widgetShare, &QPushButton::clicked, this, &OptionsWidget::_share);
  connect(_widgetComments, &QPushButton::clicked, this, &OptionsWidget::_comments);
  connect(_widgetConnected, &QPushButton::clicked, this, &OptionsWidget::_connected);
  connect(_widgetUser, &QPushButton::clicked, this, &OptionsWidget::_user);
  connect(_widgetRename, &QLineEdit::returnPressed, this, &OptionsWidget::_rename);
}

OptionsWidget::~OptionsWidget()
{
    delete ui;
}

void OptionsWidget::_share(bool checked) {
  debug("UserWidget::_share");
  emit share();
}

void OptionsWidget::_comments(bool checked) {
  debug("UserWidget::_comments");
  emit comments(checked);
}

void OptionsWidget::_connected(bool checked) {
  debug("UserWidget::_connected");
  emit connected(checked);
}

void OptionsWidget::_user(bool checked) {
  debug("UserWidget::_user");
  emit user();
}

void OptionsWidget::_rename() {
  debug("UserWidget::_rename");
  emit rename(_widgetRename->text());
}

void OptionsWidget::setFileName(const QString &name) {
  _widgetRename->setText(name);
}

void OptionsWidget::setShowCommentsChecked(bool checked) {
  _widgetComments->setChecked(checked);
}

void OptionsWidget::setShowConnectedChecked(bool checked) {
  _widgetConnected->setChecked(checked);
}
