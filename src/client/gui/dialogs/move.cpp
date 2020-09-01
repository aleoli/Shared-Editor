#include "move.h"

#include <utility>
#include "../ui/ui_dialog_move.h"

#include <QTextCursor>
#include <utils.h>

#include "exceptions.h"

using namespace se_exceptions;

Move::Move(QWidget *parent, const QString &title) :
        QDialog(parent),
        ui(new Ui::MoveFile)
{
  ui->setupUi(this);

  _widgetConfirm = ui->btn_confirm;
  _widgetCancel = ui->btn_cancel;
  _comboBox = ui->comboBox_dst;

  connect(_widgetConfirm, &QPushButton::clicked, this, &Move::_confirm);
  connect(_widgetCancel, &QPushButton::clicked, this, &Move::_cancel);

  setWindowTitle(title);
}

Move::~Move()
{
  delete ui;
}

std::optional<int> Move::show(const std::list<std::pair<QString, int>>& items, QWidget *parent, const QString &title, const QString &cancelText, const QString &confirmText) {
  Move dia(parent, title);

  if(cancelText != "") {
    dia._widgetCancel->setText(cancelText);
  }
  if(confirmText != "") {
    dia._widgetConfirm->setText(confirmText);
  }

  dia._setValues(items);

  if (dia.exec()) {
    return std::optional<int>(dia._comboBox->itemData(dia._comboBox->currentIndex()).toInt());
  }
  else {
    return std::nullopt;
  }
}

void Move::_confirm(bool checked) {
  accept();
}

void Move::_cancel(bool checked) {
  reject();
}

void Move::_setValues(const std::list<std::pair<QString, int>>& items) {
  for(auto& item: items) {
    debug("DIRS:" + item.first);
    this->_comboBox->addItem(item.first, item.second);
  }
}
