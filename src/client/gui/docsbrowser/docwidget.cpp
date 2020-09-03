#include "docwidget.h"
#include "../ui/ui_docwidget.h"

#include <QPixmap>
#include <QPainter>

#include "utils.h"

DocWidget::DocWidget(FSElement element, QWidget *parent): QWidget(parent), ui(new Ui::DocWidget), _element(std::move(element)) {
  ui->setupUi(this);

  _nameLabel = ui->label_dName;
  _nameLabel->setText(this->_element.getName());

  _dateLabel = ui->label_date;
  _dateLabel->setText(this->_element.getCreationDate().toLocalTime().toString("dd-MM-yyyy hh:mm"));

  _optionsBtn = ui->btn_options;
  QObject::connect(_optionsBtn, &QPushButton::clicked, this, &DocWidget::_openMenu);
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
  emit this->open(this->_element.getId(), this->_element.getName());
}

void DocWidget::_openMenu(bool checked) {
  emit this->openMenu(this->_element.getType() == FSElement::Type::DIRECTORY, this->_element);
}
