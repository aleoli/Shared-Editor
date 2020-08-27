#include "input.h"

#include <utility>
#include "../ui/ui_dialog_text.h"

#include <QTextCursor>

#include "exceptions.h"

using namespace se_exceptions;

Input::Input(QWidget *parent, const QString &title, const QString &text) :
  QDialog(parent),
  ui(new Ui::Message)
{
  ui->setupUi(this);

  _widgetConfirm = ui->btn_yes;
  _widgetCancel = ui->btn_no;
  _widgetText = ui->text;

  connect(_widgetConfirm, &QPushButton::clicked, this, &Input::_confirm);
  connect(_widgetCancel, &QPushButton::clicked, this, &Input::_cancel);

  setWindowTitle(title);
  _widgetText->setPlainText(text);
  auto newCursor = _widgetText->textCursor();
  newCursor.movePosition(QTextCursor::End);
  _widgetText->setTextCursor(newCursor);
}

Input::~Input()
{
  delete ui;
}

std::optional<QString> Input::show(QWidget *parent, const QString &title, const QString &text) {
  Input dia(parent, title, text);

  if (dia.exec()) {
    return std::optional<QString>(dia._widgetText->toPlainText());
  }
  else {
    return std::nullopt;
  }
}

void Input::_confirm(bool checked) {
  accept();
}

void Input::_cancel(bool checked) {
  reject();
}
