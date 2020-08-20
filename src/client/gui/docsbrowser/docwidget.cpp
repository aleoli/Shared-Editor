#include "docwidget.h"
#include "../ui/ui_docwidget.h"

#include <QPixmap>
#include <QPainter>

#include "utils.h"

DocWidget::DocWidget(FSElement element, QWidget *parent): QWidget(parent), ui(new Ui::DocWidget), _element(std::move(element)) {
  ui->setupUi(this);

  auto nameLabel = findChild<QLabel *>("label_dName");
  nameLabel->setText(this->_element.getName());
}

void DocWidget::paintEvent(QPaintEvent* e) {
    QPainter painter(this);
    QPixmap bkgnd(":/res/file.png");
    painter.drawPixmap( 0, 0, bkgnd.scaled(size(), Qt::IgnoreAspectRatio));
    QWidget::paintEvent(e);
}

void DocWidget::clear() {
}

void DocWidget::mouseDoubleClickEvent(QMouseEvent *event) {
  QWidget::mouseDoubleClickEvent(event);
  emit this->open(this->_element.getId());
}
