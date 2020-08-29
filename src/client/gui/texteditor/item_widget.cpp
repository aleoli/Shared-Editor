#include "item_widget.h"

#include "exceptions.h"

using namespace se_exceptions;

ItemWidget::ItemWidget() : _list(nullptr) {
  _item = new QListWidgetItem("");
  _item->setFlags(Qt::NoItemFlags);
}

void ItemWidget::insert(int pos, QListWidget *list) {
  if(_list)
    throw GuiException{"ItemWidget::insert: already inserted"};

  list->insertItem(pos, _item);
  list->setItemWidget(_item, this);

  _list = list;
}

void ItemWidget::add(QListWidget *list) {
  if(_list)
    throw GuiException{"ItemWidget::add: already inserted"};

  list->addItem(_item);
  list->setItemWidget(_item, this);

  _list = list;
}

void ItemWidget::remove() {
  if(_list)
    _list->removeItemWidget(_item);

  delete _item;
  delete this;
}

QListWidgetItem *ItemWidget::getItem() const {
  return _item;
}
