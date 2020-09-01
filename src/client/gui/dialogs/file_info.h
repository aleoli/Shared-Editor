#pragma once

#include <QDialog>
#include <QIcon>
#include <QPushButton>
#include <QLabel>

#include <optional>

namespace Ui {
class Details;
}

class FileInfo : public QDialog
{
  Q_OBJECT

public:
  explicit FileInfo(QWidget *parent);
  ~FileInfo();

  static bool show(QWidget *parent);

private:
  void _close(bool checked);

  Ui::Details *ui;

  QPushButton *_widgetClose;
  QLabel *_widgetName, *_widgetPath, *_widgetOwner, *_widgetModified, *_widgetCreated;
};
