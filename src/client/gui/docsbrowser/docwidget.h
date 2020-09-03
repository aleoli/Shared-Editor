#pragma once

#include <QWidget>
#include <QPushButton>
#include <QLabel>

#include "FSElement.h"

namespace Ui {
  class DocWidget;
}

class DocWidget: public QWidget {
Q_OBJECT

public:
  explicit DocWidget(FSElement element, QWidget *parent = nullptr);
  void paintEvent(QPaintEvent* e) override;

public slots:
  void clear();

private slots:
  void _openMenu(bool checked);

signals:
  void open(int fileId, const QString &name);
  void openMenu(bool isDir, const FSElement& element);

protected:
  void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
  Ui::DocWidget *ui;

  QLabel *_nameLabel, *_dateLabel;
  QPushButton *_optionsBtn;

  const FSElement _element;
};
