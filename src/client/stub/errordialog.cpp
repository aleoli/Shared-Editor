#include "errordialog.h"

#include <utility>
#include "ui_errordialog.h"

ErrorDialog::ErrorDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::ErrorDialog)
{
  ui->setupUi(this);
}

ErrorDialog::~ErrorDialog()
{
  delete ui;
}

void ErrorDialog::setErrorMsg(QString msg) {
  emit setText(std::move(msg));
}

void ErrorDialog::showDialog(QWidget *parent, QString msg) {
  ErrorDialog dia(parent);
  dia.setErrorMsg(std::move(msg));
  dia.exec();
}
