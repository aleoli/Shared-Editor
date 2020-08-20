#include "confirm.h"

#include <utility>
#include "../ui/ui_dialog_confirm.h"

#include "exceptions.h"

using namespace se_exceptions;

Confirm::Confirm(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::Confirm)
{
  ui->setupUi(this);

  _widgetConfirm  = findChild<QPushButton *>("btn_yes");
  _widgetCancel  = findChild<QPushButton *>("btn_no");

  if(!_widgetConfirm || !_widgetCancel) {
        throw GuiException{"One or more widgets in Confirm are null"};
  }

  connect(_widgetConfirm, &QPushButton::clicked, this, &Confirm::_confirm);
  connect(_widgetCancel, &QPushButton::clicked, this, &Confirm::_cancel);
}

Confirm::~Confirm()
{
  delete ui;
}

bool Confirm::show(QWidget *parent) {
  Confirm dia(parent);
  return dia.exec();
}

void Confirm::_confirm(bool checked) {
  accept();
}

void Confirm::_cancel(bool checked) {
  reject();
}
