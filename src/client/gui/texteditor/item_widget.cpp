#include "item_widget.h"

ItemWidget::ItemWidget() {
  _item = new QListWidgetItem("");
  _item->setFlags(Qt::NoItemFlags);
}

void ItemWidget::insert(int pos, QListWidget *list) {
  list->insertItem(pos, _item);
  list->setItemWidget(_item, this);
}

void ItemWidget::add(QListWidget *list) {
  list->addItem(_item);
  list->setItemWidget(_item, this);
}

void ItemWidget::remove(QListWidget *list) {
  list->takeItem(list->row(_item));
}

QListWidgetItem *ItemWidget::getItem() const {
  return _item;
}
