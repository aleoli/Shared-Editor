#include "docwidget_folder.h"
#include "../ui/ui_docwidget_folder.h"

#include <QLabel>
#include <utility>

#include "utils.h"

DocWidgetFolder::DocWidgetFolder(FSElement element, QWidget *parent): QWidget(parent), ui(new Ui::DocWidgetFolder), _element(std::move(element)) {
  ui->setupUi(this);

  auto nameLabel = findChild<QLabel *>("label_dName");
  nameLabel->setText(this->_element.getName());
}

void DocWidgetFolder::clear() {
}

void DocWidgetFolder::mouseDoubleClickEvent(QMouseEvent *event) {
  QWidget::mouseDoubleClickEvent(event);
  emit this->open(this->_element.getId());
}
