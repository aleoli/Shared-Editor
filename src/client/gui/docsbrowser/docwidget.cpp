#include "docwidget.h"
#include "../ui/ui_docwidget.h"

#include <QPixmap>
#include <QPainter>

#include "utils.h"

DocWidget::DocWidget(FSElement element, QWidget *parent): QWidget(parent), ui(new Ui::DocWidget), _element(std::move(element)) {
  ui->setupUi(this);

  _nameLabel = findChild<QLabel *>("label_dName");
  _nameLabel->setText(this->_element.getName());

  _dateLabel = findChild<QLabel *>("label_date");
  _dateLabel->setText("01/01/1970");

  _optionsBtn = findChild<QPushButton *>("btn_options");
}

void DocWidget::paintEvent(QPaintEvent* e) {
  QPainter painter(this);
  QPixmap bkgnd(":/res/file.png");
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

void DocWidget::clear() {
}

void DocWidget::mouseDoubleClickEvent(QMouseEvent *event) {
  QWidget::mouseDoubleClickEvent(event);
  emit this->open(this->_element.getId());
}
