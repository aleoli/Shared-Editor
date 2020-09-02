#pragma once

#include <QDialog>
#include <QIcon>
#include <QPushButton>
#include <QLabel>

#include <optional>

#include "FSElement.h"

namespace Ui {
class Details;
}

class FileInfo : public QDialog
{
  Q_OBJECT

public:
  explicit FileInfo(QWidget *parent, const FSElement::FileInfo& fileInfo);
  ~FileInfo();

  static bool show(QWidget *parent, const FSElement::FileInfo& fileInfo);

private:
  void _close(bool checked);

  Ui::Details *ui;

  QPushButton *_widgetClose;
};
