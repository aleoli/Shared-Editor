#include "confirm.h"

#include <utility>
#include "../ui/ui_dialog_confirm.h"

#include "exceptions.h"

using namespace se_exceptions;

Confirm::Confirm(QWidget *parent, const QString &windowTitle) :
  QDialog(parent),
  ui(new Ui::Confirm)
{
  ui->setupUi(this);

  _widgetConfirm  = ui->btn_yes;
  _widgetCancel  = ui->btn_no;

  connect(_widgetConfirm, &QPushButton::clicked, this, &Confirm::_confirm);
  connect(_widgetCancel, &QPushButton::clicked, this, &Confirm::_cancel);

  if(!windowTitle.isEmpty())
    setWindowTitle(windowTitle);
}

Confirm::~Confirm()
{
  delete ui;
}

bool Confirm::show(QWidget *parent, const QString &windowTitle) {
  Confirm dia(parent, windowTitle);
  return dia.exec();
}

void Confirm::_confirm(bool checked) {
  accept();
}

void Confirm::_cancel(bool checked) {
  reject();
}
