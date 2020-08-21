#include "docwidget_folder.h"
#include "../ui/ui_docwidget_folder.h"

#include <QLabel>
#include <utility>
#include <QPixmap>
#include <QPainter>

#include "utils.h"

DocWidgetFolder::DocWidgetFolder(FSElement element, QWidget *parent): QWidget(parent), ui(new Ui::DocWidgetFolder), _element(std::move(element)) {
  ui->setupUi(this);

  _nameLabel = findChild<QLabel *>("label_dName");
  _nameLabel->setText(this->_element.getName());

  _dateLabel = findChild<QLabel *>("label_date");
  _dateLabel->setText("01/01/1970");

  _optionsBtn = findChild<QPushButton *>("btn_options");
}

void DocWidgetFolder::paintEvent(QPaintEvent* e) {
  QPainter painter(this);
  QPixmap bkgnd(":/res/folder.png");
  painter.drawPixmap( 0, 0, bkgnd.scaled(size(), Qt::IgnoreAspectRatio));

  _nameLabel->setStyleSheet("background-color: rgba(0,0,0,0%); border-image: none; text-align: right; color: white;");
  auto font = _nameLabel->font();
  font.setPointSize(14);
  _nameLabel->setFont(font);
  _nameLabel->setAlignment(Qt::AlignRight);

  _dateLabel->setStyleSheet("background-color: rgba(0,0,0,0); border-image: none; text-align: right; color: white;");
  font = _dateLabel->font();
  font.setPointSize(14);
  _dateLabel->setFont(font);
  _dateLabel->setAlignment(Qt::AlignRight);

  _optionsBtn->setStyleSheet("border-width: 0px; border-style: solid; background-color: rgba(0,0,0,0); border-image: none;");

  QWidget::paintEvent(e);
}

void DocWidgetFolder::clear() {
}

void DocWidgetFolder::mouseDoubleClickEvent(QMouseEvent *event) {
  QWidget::mouseDoubleClickEvent(event);
  emit this->open(this->_element.getId());
}
