#include "file_info.h"

#include <utility>
#include "../ui/ui_dialog_details.h"

#include "exceptions.h"

using namespace se_exceptions;

FileInfo::FileInfo(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::Details)
{
  ui->setupUi(this);

  _widgetClose  = ui->btn_close;

  connect(_widgetClose, &QPushButton::clicked, this, &FileInfo::_close);
}

FileInfo::~FileInfo()
{
  delete ui;
}

bool FileInfo::show(QWidget *parent) {
  FileInfo dia(parent);
  return dia.exec();
}

void FileInfo::_close(bool checked) {
  accept();
}
