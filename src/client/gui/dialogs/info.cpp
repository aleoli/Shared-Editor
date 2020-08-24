#include "info.h"

#include <utility>
#include "../ui/ui_dialog_info.h"

#include <QApplication>
#include <QClipboard>

#include "exceptions.h"

using namespace se_exceptions;

Info::Info(QWidget *parent, const QString &windowTitle, const QString &title, const QString &msg) :
  QDialog(parent),
  ui(new Ui::Info)
{
  ui->setupUi(this);

  _widgetConfirm  = findChild<QPushButton *>("btn_yes");
  _widgetCopy  = findChild<QPushButton *>("btn_no");
  _widgetTitle  = findChild<QLabel *>("label_title");
  _widgetText  = findChild<QLabel *>("text");


  if(!_widgetConfirm || !_widgetCopy || !_widgetTitle || !_widgetText) {
        throw GuiException{"One or more widgets in Copy are null"};
  }

  _widgetTitle->setText(title);
  _widgetText->setText(msg);
  setWindowTitle(windowTitle);

  connect(_widgetConfirm, &QPushButton::clicked, this, &Info::_confirm);
  connect(_widgetCopy, &QPushButton::clicked, this, &Info::_copy);
}

Info::~Info()
{
  delete ui;
}

bool Info::show(QWidget *parent, const QString &windowTitle, const QString &title, const QString &msg) {
  Info dia(parent, windowTitle, title, msg);
  return dia.exec();
}

void Info::_confirm(bool checked) {
  accept();
}

void Info::_copy(bool checked) {
  QClipboard *clipboard = QApplication::clipboard();
  clipboard->setText(_widgetText->text());
}
