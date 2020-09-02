#include "file_info.h"

#include <utility>
#include "../ui/ui_dialog_details.h"

#include "exceptions.h"

using namespace se_exceptions;

FileInfo::FileInfo(QWidget *parent, const FSElement::FileInfo& fileInfo) :
  QDialog(parent),
  ui(new Ui::Details)
{
  ui->setupUi(this);

  _widgetClose  = ui->btn_close;


  ui->field_fileName->setText(fileInfo.name);
  ui->field_path->setText(fileInfo.path);
  ui->field_owner->setText(fileInfo.owner);
  ui->field_creation->setText(fileInfo.created.toLocalTime().toString("dd.MM.yyyy hh:mm:ss"));

  connect(_widgetClose, &QPushButton::clicked, this, &FileInfo::_close);
}

FileInfo::~FileInfo()
{
  delete ui;
}

bool FileInfo::show(QWidget *parent, const FSElement::FileInfo& fileInfo) {
  FileInfo dia(parent, fileInfo);
  return dia.exec();
}

void FileInfo::_close(bool checked) {
  accept();
}
